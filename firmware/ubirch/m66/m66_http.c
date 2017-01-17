/*
 * M66 HTTP operations.
 *
 * @author Matthias L. Jugel,
 * @date 2016-10-14
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
#include <stdio.h>
#include <stdlib.h>
#include "ubirch/modem.h"
#include "m66_debug.h"

// marker for the last downloaded data if content-length was known
// necessary, because the RAM:file will have extra data prepended and appended
// that we need to ignore
static uint16_t file_start = 0;
static uint16_t file_length = 0;

size_t modem_http_read_raw(uint8_t *buffer, uint32_t start, size_t size, uint32_t timeout) {
  if (size <= 0) return 0;

  timer_set_timeout(timeout * 1000);

  int handle = 0;
  modem_send("AT+QFOPEN=\"RAM:HTTP.BIN\",2");
  if (!modem_expect_scan("+QFOPEN: %d", uTimer_Remaining, &handle)) return 0;
  if (!handle) return 0;

  modem_send("AT+QFSEEK=%d,%d,%d", handle, start, 0);
  if (!modem_expect_OK(uTimer_Remaining)) {
    CSTDEBUG("GSM ERR  !! file seek failed\r\n");
    modem_send("AT+QFCLOSE=%d", handle);
    modem_expect_OK(uTimer_Remaining);
    return 0;
  }

  size_t available = 0;
  modem_send("AT+QFREAD=%d,%d", handle, size);
  if (!modem_expect_scan("CONNECT %d", uTimer_Remaining, &available)) {
    CSTDEBUG("GSM INFO !! file read failed\r\n");
    modem_send("AT+QFCLOSE=%d", handle);
    modem_expect_OK(uTimer_Remaining);
    return 0;
  }

  size_t received = modem_read_binary(buffer, available, uTimer_Remaining);

  modem_send("AT+QFCLOSE=%d", handle);
  if (!modem_expect_OK(uTimer_Remaining)) return 0;

  return received;
}

int modem_http_prepare(const char *url, uint32_t timeout) {
  timer_set_timeout(timeout * 1000);

// TODO: find a way to set HTTP headers
//  modem_send("AT+HTTPPARA=\"UA\",\"" BOARD "\"");
//  if (!modem_expect_OK(uTimer_Remaining)) return 1102;

  modem_send("AT+QFDEL=\"RAM:HTTP.BIN\"");
  modem_expect_OK(uTimer_Remaining);

  modem_send("AT+QHTTPURL=%d,%d", strlen(url), timer_timeout_remaining() / 1000000);
  if (!modem_expect("CONNECT", uTimer_Remaining)) return 2001;

  modem_send(url);
  if (!modem_expect_OK(uTimer_Remaining)) return 2002;

  return 0;
}

int modem_http(http_method_t method, size_t *res_size, uint32_t timeout) {
  int status = 0;
  // if we get a hint for the size and the Content-Length: header is not sent by
  // the remote server, we set the max size to 512k. The length will be ignored if
  // Content-Length is sent. Make sure you plan enough timeout to receiv all data.
  int max_download = *res_size ? *res_size : 524288; // expected download size
  int wait_time = (int) (timer_timeout_remaining() / 1000000); // timeout

  // save download into RAM of M66, max_download bytes and some wait_time
  modem_send("AT+QHTTPDL=\"RAM:HTTP.BIN\",%d,%d", max_download, wait_time);
  if (!modem_expect_OK(uTimer_Remaining)) return 2004;

  // restart the timeout from here, just in case we don't get a Content-Length
  // give it some 5s extra to handle the data
  timer_set_timeout((timeout + 5000) * 1000);

  int download_size, content_size;
  // TODO: we can't handle unknown content length right now (res_size: -1)
  if (modem_expect_scan("+QHTTPDL: %d,%d,%d", uTimer_Remaining,
                        &download_size, &content_size, &status) < 3)
    return 2005;

  // check if we ran into a timeout waiting for data, respond with amount
  // of data received and set status to 0
  if (content_size == -1) {
    if (status == 3826) status = 0;
    // we need to find the file start and the length as the actual file in
    // M66 RAM has a length prefix and an end marker we need to ignore
    uint8_t tmp[8] = {0};
    modem_http_read_raw(tmp, 0, 7, 1000);
    CIODUMP(tmp, 7);
    file_length = (uint16_t) strtol((const char *) tmp, NULL, 16);
    for (file_start = 0; file_start < 7;) if (tmp[file_start++] == '\n') break;
    *res_size = file_length;
  } else {
    file_start = 0;
    file_length = (uint16_t) content_size;
    *res_size = (size_t) content_size;
  }

  return status;
}

size_t modem_http_write(const uint8_t *buffer, size_t size, uint32_t timeout) {
  if (size <= 0) return 0;

  timer_set_timeout(timeout * 1000);

  CIODUMP(buffer, size);
  modem_write(buffer, size);

  if (!modem_expect_OK(uTimer_Remaining)) return 0;

  return size;
}

size_t modem_http_read(uint8_t *buffer, uint32_t start, size_t size, uint32_t timeout) {
  if (size <= 0) return 0;

  size_t real_size = (start + size) > file_length ? file_length - (start + start) : size;

  return modem_http_read_raw(buffer, file_start + start, real_size, timeout);
}

int modem_http_get(const char *url, size_t *res_size, uint32_t timeout) {
  int error = modem_http_prepare(url, timeout);
  if (error) return error;

  modem_send("AT+QHTTPGET=%d", timer_timeout_remaining() / 1000000);
  if (!modem_expect_OK(uTimer_Remaining)) return 2003;

  return modem_http(HTTP_GET, res_size, timeout);
}

int modem_http_post(const char *url, size_t *res_size, uint8_t *request, size_t req_size, uint32_t timeout) {
  timer_set_timeout(timeout * 1000);

  int status = modem_http_prepare(url, timeout);
  if (status) return status;

  uint32_t wait_time = timer_timeout_remaining() / 1000000;
  modem_send("AT+QHTTPPOST=%d,%d,%d", req_size, wait_time, 65535);
  if (!modem_expect("CONNECT", uTimer_Remaining)) return 2003;

  size_t sent = modem_http_write(request, req_size, uTimer_Remaining);
  if (sent != req_size) return 2004;

  return modem_http(HTTP_POST, res_size, timeout);
}
