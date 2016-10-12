/*
 * M66 HTTP operations.
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
#include "m66_debug.h"

int modem_http_prepare(const char *url, uint32_t timeout) {
  timer_set_timeout(timeout * 1000);

/*  modem_enable();
  modem_register();
  modem_gprs_attach();*/

  modem_send("AT+HTTPTERM");
  modem_expect_OK(timeout);

  modem_send("AT+HTTPINIT");
  if (!modem_expect_OK(uTimer_Remaining)) return 1000;

  modem_send("AT+HTTPPARA=\"CID\",1");
  if (!modem_expect_OK(uTimer_Remaining)) return 1101;

  modem_send("AT+HTTPPARA=\"UA\",\"ubirch#1 r0.2\"");
  if (!modem_expect_OK(uTimer_Remaining)) return 1102;

  modem_send("AT+HTTPPARA=\"REDIR\",1");
  if (!modem_expect_OK(uTimer_Remaining)) return 1103;

  modem_send("AT+HTTPPARA=\"URL\",\"%s\"", url);
  if (!modem_expect_OK(uTimer_Remaining)) return 1110;

  return 0;
}

int modem_http(http_method_t method, size_t *res_size, uint32_t timeout) {
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

  modem_send("AT+HTTPDATA=%d,%d", size, timer_timeout_remaining() / 1000);
  modem_expect("DOWNLOAD", uTimer_Remaining);

  CIODUMP(buffer, size);
  modem_write(buffer, size);

  if (!modem_expect_OK(uTimer_Remaining)) return 0;

  return size;
}

//default is 10240 bytessuper
size_t modem_http_read(uint8_t *buffer, uint32_t timeout) {

  timer_set_timeout(timeout * 1000);
  size_t idx, available = 0;
  available = 2322;

  modem_send("AT+QHTTPREAD=30"); //, uTimer_Remaining);
  if (modem_expect("CONNECT", uTimer_Remaining))
  {
    modem_expect_OK(2000);
    idx = modem_readline((char *)buffer, available, uTimer_Remaining);
    CIODUMP(buffer, idx);

    if (!modem_expect_OK(uTimer_Remaining)) return 0;
  }

  return idx;
}

uint32_t modem_http_dl_file(const char *file_name, uint32_t timeout)
{
  timer_set_timeout(timeout * 1000);

  static uint32_t dl_len, content_len = 0;
  static uint16_t error_code = 0;

  // Here the default download file size is 10240 Bytes
  // To give a size, enter the size after the filename
  modem_send("AT+QHTTPDL=\"RAM:%s.txt\"", file_name);
  modem_expect_scan("+QHTTPDL:%d,%d,%d", uTimer_Remaining, &dl_len, &content_len, &error_code);
  if (!modem_expect_OK(uTimer_Remaining)) return 0;

  return dl_len;
}

int modem_http_get(const char *url, uint32_t timeout) {
  timer_set_timeout(timeout * 1000);

  bool url_ok = false;
  modem_send("AT+QHTTPURL=%d,%d", strlen(url), uTimer_Remaining);
  if (!modem_expect("CONNECT", uTimer_Remaining))
  {
    CIODEBUG("HTTP (25) -> 'Failed to receive CONNECT'\r\n");
  }
  else
  {
    modem_send(url);
    if (!modem_expect_OK(uTimer_Remaining))
    {
      PRINTF("Failed to connect to the server");
      return false;
    }
    else
    {
      url_ok = true;
    }
  }

  if (url_ok)
  {
    modem_send("AT+QHTTPGET=%d", uTimer_Remaining);
    if (!modem_expect_OK(uTimer_Remaining))
    {
      char *get_response;
      modem_readline(get_response, 20, uTimer_Remaining);
      CIODEBUG("HTTP (%02d) -> '%s'\r\n", strlen(get_response), get_response);
      return false;
    }
    return true;
  }
}

int modem_http_post(const char *url, size_t *res_size, uint8_t *request, size_t req_size, uint32_t timeout) {
  timer_set_timeout(timeout * 1000);

  int status = modem_http_prepare(url, timeout);
  if(status) return status;

  modem_http_write(request, req_size, uTimer_Remaining);

  return modem_http(HTTP_POST, res_size, uTimer_Remaining);
}


uint32_t http_file_open(const char *file_name, uint8_t rw_mode, uint32_t timeout)
{
  timer_set_timeout(timeout * 1000);
  uint32_t file_handle = 0;
  modem_send("AT+QFOPEN=\"%s\",%d", file_name, rw_mode);
  modem_expect_scan("+QFOPEN:%d", &file_handle);
  if (!modem_expect_OK(uTimer_Remaining)) return false;
  return file_handle;
}

int http_file_read(const char *read_buffer, uint32_t file_handle, uint16_t len)
{
  modem_send("AT+QFREAD=%d,%d", file_handle, len);
  size_t  data_len= modem_read_binary((uint8_t *)read_buffer, len, uTimer_Remaining);

  if (!modem_expect_OK(uTimer_Remaining)) return false;

  return data_len;
}
bool http_file_close(int file_handle)
{
  modem_send("AT+QFCLOSE=%d", file_handle);
  if (!modem_expect_OK(2000)) return false;
  return true;
}
