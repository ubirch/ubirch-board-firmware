/*
 * M66 HTTP operations.
 *
 * @author Matthias L. Jugel
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
#include "ubirch/modem.h"
#include "m66_debug.h"


char file_name[] = "RAM:file.bin";

int modem_http_prepare(const char *url, uint32_t timeout) {
  timer_set_timeout(timeout * 1000);

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

/*
File Name: Name for the downloaded file
Length: Size file to be downloaded, only for RAM files; default is 10240
Wait Time: time in seconds, it closes the http connection when timeout
DL_SIZE
 CONTENT lENGTH
 ERRORCODE
*/
int modem_http_file_dl(const char *file_name, uint32_t timeout)
{
  timer_set_timeout(timeout * 1000);

  int dl_size, content_len, dl_error_code = 0;

  modem_send("AT+QHTTPDL=\"%s\"", file_name);

  if (!modem_expect_OK(5 * 1000))
  {
    CSTDEBUG("HTTP --- Failed to download the file\r\n");
    return 0;
  }

  if (modem_expect_scan("+QHTTPDL: %d,%d,%d", 10 * 5000, &dl_size, &content_len, &dl_error_code))
  {
    if (dl_error_code)
    {
      CSTDEBUG("HTTP --- Error downloading file: %d\r\n", dl_error_code);
      return 0;
    }

    if (content_len == -1)
    {
      CSTDEBUG("HTTP --- Content length unknown\r\n");
      return 0;
    }

    if (!dl_size)
    {
      CSTDEBUG("HTTP --- Downloaded ZERO bytes\r\n");
      return 0;
    }

    CSTDEBUG("HTTP --- dl_size = %d, content_len = %d, error_code = %d\r\n", dl_size, content_len, dl_error_code);
  }

  return dl_size;
}

/* File Name: Name of the file to be opened
 * Mode: there are three modes
    0 - creat file if doesn exist, open. file type RW
    1 - if file exists clears it and creats new file
    2 - if the file exists open it, it is read-only
  * Length: Max length of the file, Used only for RAM file 10240 is default value
  * File Handle: Handle for the file to be operated
  */
int modem_http_file_open(const char *file_name, uint8_t rw_mode, uint32_t timeout)
{
  timer_set_timeout(timeout * 1000);

  int file_handle =0;

  modem_send("AT+QFOPEN=\"%s\",%d", file_name, rw_mode);

  if (!modem_expect_scan("+QFOPEN: %d", uTimer_Remaining, &file_handle))
  {
    CSTDEBUG("HTTP --- No File handle received\r\n");
    return 0;
  }

  CSTDEBUG("HTTP --- File handle number %d\r\n", file_handle);

  return file_handle;
}

size_t modem_http_file_read(uint8_t *read_buffer, int file_handle, size_t len, uint32_t timeout)
{
  timer_set_timeout(timeout * 1000);

  modem_send("AT+QFREAD=%d,%d", file_handle, len);

  int new_len = 0;
  if (!modem_expect_scan("CONNECT %d", uTimer_Remaining, &new_len)) return 0;

  size_t data_len = modem_read_binary(read_buffer, len, uTimer_Remaining);

  if(!modem_expect_OK(uTimer_Remaining))
  {
    CSTDEBUG("HTTP --- Read file failed\r\n");
    return  0;
  }

  if (data_len < 0) return 0;

  return data_len;
}

bool modem_http_file_close(int file_handle, uint32_t timeout)
{
  timer_set_timeout(timeout * 1000);

  modem_send("AT+QFCLOSE=%d", file_handle);
  if (!modem_expect_OK(uTimer_Remaining)) return false;
  return true;
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

size_t modem_http_read(uint8_t *buffer, uint32_t start, size_t size, uint32_t timeout) {
  if(size <= 0) return 0;

  timer_set_timeout(timeout * 1000);

  int file_handle = 0;
  size_t  data_len = 0;
  bool close_file = false;

  file_handle = modem_http_file_open(file_name, 0, uTimer_Remaining);

  if (!file_handle) return 0;

  /*
   *  Position -> Pointer movement mode
   *  0 -> File begining | Default value
   *  1 -> Current position of the pointer
   *  2 -> File end
   *  */
  modem_send("AT+QFSEEK=%d,%d,%d", file_handle, start, 0);
  if (!modem_expect_OK(uTimer_Remaining))
  {
    CSTDEBUG("HTTP --- Failed ot seek the file position\r\n");
    return 0;
  }

  data_len = modem_http_file_read(buffer, file_handle, size, uTimer_Remaining);
  if (!data_len) return 0;

  close_file = modem_http_file_close(file_handle, uTimer_Remaining);
  if (!close_file) return 0;

  return data_len;
}

int modem_http_get(const char *url, size_t *res_size, uint32_t timeout)
{
  timer_set_timeout(timeout * 1000);

  int dl_size = 0;

  modem_send("AT+QHTTPURL=%d,%d", strlen(url), timer_timeout_remaining() / 1000);

  if (!modem_expect("CONNECT", uTimer_Remaining))
  {
    CSTDEBUG("HTTP --- Failed to receive CONNECT\r\n");
    return false;
  }

  modem_send(url);

  if (!modem_expect_OK(uTimer_Remaining))
  {
    CSTDEBUG("HTTP --- Failed to connect to the server");
    return 0;
  }

  modem_send("AT+QHTTPGET=%d", timer_timeout_remaining() / 1000);
  if (!modem_expect_OK(uTimer_Remaining)) return 0;

  dl_size = modem_http_file_dl(file_name, uTimer_Remaining);

  if (!dl_size) return 0;

  return dl_size;
}

int modem_http_post(const char *url, size_t *res_size, uint8_t *request, size_t req_size, uint32_t timeout) {
  timer_set_timeout(timeout * 1000);

  int status = modem_http_prepare(url, timeout);
  if(status) return status;

  modem_http_write(request, req_size, uTimer_Remaining);

  return modem_http(HTTP_POST, res_size, uTimer_Remaining);
}
