/*
 * SIM800H HTTP operations.
 *
 * @author Matthias L. Jugel
 * @date 2016-04-09
 *
 * @copyright &copy; 2015 ubirch GmbH (https://ubirch.com)
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
#include "ubirch/modem.h"
#include "sim800h_debug.h"

int modem_http_prepare(const char *url, uint32_t timeout) {
  timer_set_timeout(timeout * 1000);

  modem_send("AT+HTTPTERM");
  modem_expect_OK(timeout);

  modem_send("AT+HTTPINIT");
  if (!modem_expect_OK(uTimer_Remaining)) return 1000;

  modem_send("AT+HTTPPARA=\"CID\",1");
  if (!modem_expect_OK(uTimer_Remaining)) return 1101;

  modem_send("AT+HTTPPARA=\"UA\",\"" BOARD "\"");
  if (!modem_expect_OK(uTimer_Remaining)) return 1102;

  modem_send("AT+HTTPPARA=\"REDIR\",1");
  if (!modem_expect_OK(uTimer_Remaining)) return 1103;

  modem_send("AT+HTTPPARA=\"URL\",\"%s\"", url);
  if (!modem_expect_OK(uTimer_Remaining)) return 1110;

  return 0;
}

int modem_http(modem_http_method_t method, size_t *res_size, uint32_t timeout) {
  timer_set_timeout(timeout * 1000);
  int bearer, status;

  modem_send("AT+HTTPACTION=%d", method);
  if (!modem_expect_OK(uTimer_Remaining)) return 1004;

  modem_expect_scan("+HTTPACTION: %d,%d,%d", uTimer_Remaining, &bearer, &status, res_size);

  return status;
}

size_t modem_http_write(const uint8_t *buffer, size_t size, uint32_t timeout) {
  if(size <= 0) return 0;

  timer_set_timeout(timeout * 1000);

  modem_send("AT+HTTPDATA=%d,%d", size, timer_timeout_remaining()/1000);
  modem_expect("DOWNLOAD", uTimer_Remaining);

  CIODUMP(buffer, size);
  modem_write(buffer, size);

  if (!modem_expect_OK(uTimer_Remaining)) return 0;

  return size;
}

size_t modem_http_read(uint8_t *buffer, uint32_t start, size_t size, uint32_t timeout) {
  if(size <= 0) return 0;

  timer_set_timeout(timeout * 1000);
  size_t available;

  modem_send("AT+HTTPREAD=%d,%d", start, size);
  modem_expect_scan("+HTTPREAD: %lu", timeout, &available);

  size_t idx = modem_read_binary(buffer, available, uTimer_Remaining);
  if (!modem_expect_OK(uTimer_Remaining)) return 0;

  CIODUMP(buffer, idx);

  return idx;
}

int modem_http_get(const char *url, size_t *res_size, uint32_t timeout) {
  timer_set_timeout(timeout * 1000);

  int status = modem_http_prepare(url, timeout);
  if (status) return status;

  return modem_http(HTTP_GET, res_size, uTimer_Remaining);
}

int modem_http_post(const char *url, size_t *res_size, uint8_t *request, size_t req_size, uint32_t timeout) {
  timer_set_timeout(timeout * 1000);

  int status = modem_http_prepare(url, timeout);
  if(status) return status;

  modem_http_write(request, req_size, uTimer_Remaining);

  return modem_http(HTTP_POST, res_size, uTimer_Remaining);
}


