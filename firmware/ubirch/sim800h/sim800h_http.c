/*
 * SIM800H HTTP operations.
 *
 * @author Matthias L. Jugel
 * @date 2016-04-09
 *
 * Copyright 2016 ubirch GmbH (https://ubirch.com)
 *
 * ```
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ```
 */

#include <stddef.h>
#include <stdint.h>
#include <ubirch/timer.h>
#include "ubirch/sim800h.h"
#include "sim800h_debug.h"

int sim800h_http_prepare(const char *url, uint32_t timeout) {
  uint32_t deadline = timer_read() + timeout * 1000;

  sim800h_send("AT+HTTPTERM");
  sim800h_expect_OK(timeout);

  sim800h_send("AT+HTTPINIT");
  if (!sim800h_expect_OK(REMAINING(deadline))) return 1000;

  sim800h_send("AT+HTTPPARA=\"CID\",1");
  if (!sim800h_expect_OK(REMAINING(deadline))) return 1101;

  sim800h_send("AT+HTTPPARA=\"UA\",\"ubirch#1 r0.2\"");
  if (!sim800h_expect_OK(REMAINING(deadline))) return 1102;

  sim800h_send("AT+HTTPPARA=\"REDIR\",1");
  if (!sim800h_expect_OK(REMAINING(deadline))) return 1103;

  sim800h_send("AT+HTTPPARA=\"URL\",\"%s\"", url);
  if (!sim800h_expect_OK(REMAINING(deadline))) return 1110;

  return 0;
}

int sim800h_http(sim800h_http_method_t method, size_t *res_size, uint32_t timeout) {
  uint32_t deadline = timer_read() + timeout * 1000;
  int bearer, status;

  sim800h_send("AT+HTTPACTION=%d", method);
  if (!sim800h_expect_OK(REMAINING(deadline))) return 1004;

  sim800h_expect_scan("+HTTPACTION: %d,%d,%d", REMAINING(deadline), &bearer, &status, res_size);

  return status;
}

size_t sim800h_http_write(const uint8_t *buffer, size_t size, uint32_t timeout) {
  if(size <= 0) return 0;

  uint32_t deadline = timer_read() + timeout * 1000;

  sim800h_send("AT+HTTPDATA=%d,%d", size, timeout);
  sim800h_expect("DOWNLOAD", REMAINING(deadline));

  CIODUMP(buffer, size);
  sim800h_write(buffer, size);

  if (!sim800h_expect_OK(REMAINING(deadline))) return 0;

  return size;
}

size_t sim800h_http_read(uint8_t *buffer, uint32_t start, size_t size, uint32_t timeout) {
  if(size <= 0) return 0;

  uint32_t deadline = timer_read() + timeout * 1000;
  size_t available;

  sim800h_send("AT+HTTPREAD=%d,%d", start, size);
  sim800h_expect_scan("+HTTPREAD: %lu", timeout, &available);

  size_t idx = sim800h_read_binary(buffer, available, REMAINING(deadline));
  if (!sim800h_expect_OK(REMAINING(deadline))) return 0;

  CIODUMP(buffer, idx);

  return idx;
}

int sim800h_http_get(const char *url, size_t *res_size, uint32_t timeout) {
  uint32_t deadline = timer_read() + timeout * 1000;

  int status = sim800h_http_prepare(url, timeout);
  if (status) return status;

  return sim800h_http(HTTP_GET, res_size, REMAINING(deadline));
}

int sim800h_http_post(const char *url, size_t *res_size, uint8_t *request, size_t req_size, uint32_t timeout) {
  uint32_t deadline = timer_read() + timeout * 1000;

  int status = sim800h_http_prepare(url, timeout);
  if(status) return status;

  sim800h_http_write(request, req_size, REMAINING(deadline));

  return sim800h_http(HTTP_POST, res_size, REMAINING(deadline));
}


