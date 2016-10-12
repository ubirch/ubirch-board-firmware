/*
 * M66 TCP/IP operations.
 * Simple driver with send and receive functions
 *
 * @author Niranjan H. Rao
 * @date 2016-10-07
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
#include "m66_tcp.h"
#include "m66_core.h"
#include "m66_ops.h"
#include "m66_parser.h"
#include "ubirch/timer.h"
#include "m66_debug.h"
#include <stdio.h>

bool modem_tcp_connect(const char *apn, const char *user, const char *password, uint32_t timeout) {
  timer_set_timeout(timeout * 1000);

  if (!modem_gprs_attach(apn, user, password, uTimer_Remaining)) return false;

// This is used to establish TCP connection  - server / client
//  modem_send("AT+QIMUX=0");
//  if (!modem_expect_OK(uTimer_Remaining)) return false;

//  Set QIMODE to 1, to establish TCP connection in transparent mode
//  modem_send("AT+QIMODE=0");
//  if (!modem_expect_OK(uTimer_Remaining)) return false;
//  CSTDEBUG("MUX and MODE selected \r\n");

  modem_send("AT+QIDNSIP=1");
  if (!modem_expect_OK(uTimer_Remaining)) return false;
  CSTDEBUG("DNS and not IP\r\n");

//     Add header to the received data
//     we need these AT commands to configure the way we receive the O/P
//     we also need this so that we can easily diff b/w received data and
//     the echo / URC replies or errors
//    AT+QINDI=1; //set type of mode when receiving the data
//    AT+QIHEAD=1;  // add info IPD<len>: before the received data
//    AT+QISHOWRA=1; // display IP address and port of the sender
//    AT+QISHOWPT=1; // show transmission layer protocol type, TCP or UDP

  modem_send("AT+QIOPEN=\"TCP\",\"api.ubirch.com\",\"80\"");
  if (!modem_expect_OK(uTimer_Remaining)) return false;
  if (!modem_expect("CONNECT OK", uTimer_Remaining)) return false;

  return true;
}


bool modem_tcp_send(const char *data, uint8_t len)
{
  // this is just to check if the tcp connection is alive
  modem_send("AT+QISACK");
  if (modem_expect("ERROR", 100)) return false;

  modem_send("AT+QISEND=%d", len);

  if (modem_expect("> ", 300))
  {
    modem_send(data);
  }
  else
  {
    CSTDEBUG("we did not receive < \r\n");
    return false;
  }

  if (!modem_expect("SEND OK", 300)) return false;

  static char rx_buffer[] = {0};
  static int rx_buffer_len = 0;

  rx_buffer_len = modem_readline(rx_buffer, MQTT_READ_BUFFER, 2000);
  if (rx_buffer_len)
  {
    CSTDEBUG("No data received\r\n");
    return 0;
  }
  else
  {
    CIODUMP(rx_buffer, rx_buffer_len);
    CSTDEBUG("Total bytes received: %d\r\n", rx_buffer_len);
  }

  return true;
}

void modem_tcp_close(uint32_t timeout)
{
  modem_send("AT+QICLOSE");
  if (!modem_expect_OK(10 * timeout))
  {
    CSTDEBUG("Failed to close the TCP connection\r\n");
  }

  modem_send("AT+QIDEACT");
  if (!modem_expect_OK(10 * timeout))
  {
    CSTDEBUG("Failed to deactivate PDP context, disabling modem\r\n");
  }

  modem_disable();
}
