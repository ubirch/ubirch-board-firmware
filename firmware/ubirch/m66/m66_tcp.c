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

bool modem_tcp_connect(const char *host, uint16_t port, uint32_t timeout) {
  timer_set_timeout(timeout * 1000);

// This is used to establish TCP connection  - server / client
//  modem_send("AT+QIMUX=0");
//  if (!modem_expect_OK(uTimer_Remaining)) return false;

//  Set QIMODE to 1, to establish TCP connection in transparent mode
//  modem_send("AT+QIMODE=0");
//  if (!modem_expect_OK(uTimer_Remaining)) return false;
//  CSTDEBUG("MUX and MODE selected \r\n");

  modem_send("AT+QIDNSIP=1");
  if (!modem_expect_OK(uTimer_Remaining)) return false;

//     Add header to the received data
//     we need these AT commands to configure the way we receive the O/P
//     we also need this so that we can easily diff b/w received data and
//     the echo / URC replies or errors
//    AT+QINDI=1; //set type of mode when receiving the data
//    AT+QIHEAD=1;  // add info IPD<len>: before the received data
//    AT+QISHOWRA=1; // display IP address and port of the sender
//    AT+QISHOWPT=1; // show transmission layer protocol type, TCP or UDP

  modem_send("AT+QINDI=1");
  if (!modem_expect_OK(uTimer_Remaining)) return false;

  modem_send("AT+QIOPEN=\"TCP\",\"%s\",\"%d\"", host, port);
  if (!modem_expect_OK(uTimer_Remaining)) return false;
  if (!modem_expect("CONNECT OK", uTimer_Remaining)) return false;

  return true;
}

bool modem_tcp_check_ack(uint32_t timeout) {
  // this is just to check if the tcp connection is alive
  uint16_t sent, acked, nacked;
  do {
    modem_send("AT+QISACK");
    modem_expect_scan("+QISACK: %d, %d, %d", uTimer_Remaining, &sent, &acked, &nacked);
    if (!modem_expect_OK(uTimer_Remaining)) return false;
  } while (sent != 0);
  return true;
}

bool modem_tcp_send(const uint8_t *buffer, uint8_t size, uint32_t timeout) {
  timer_set_timeout(timeout * 1000);

  if(!modem_tcp_check_ack(uTimer_Remaining)) return false;

  modem_send("AT+QISEND=%d", size);
  // wait for prompt: '\r\n>' (3 bytes, last must be '>')
  uint8_t prompt[3] = {0, 0, 0};
  if (!(modem_read_binary(prompt, 3, uTimer_Remaining) && prompt[2] == '>')) {
    CSTDEBUG("error waiting for prompt\r\n");
    CIODUMP(prompt, 3);
    return false;
  }

  modem_send((const char *) buffer, size);
  if (!modem_expect("SEND OK", 300)) return false;

  return true;
}

size_t modem_tcp_receive(uint8_t *buffer, size_t size, uint32_t timeout) {
  timer_set_timeout(timeout * 1000);

  // we can only handle 1500 bytes at once
  if(size > 1500) return 0;

  unsigned int sent, acked, nacked;
  do {
    modem_send("AT+QISACK");
    modem_expect_scan("+QISACK: %u, %u, %u", uTimer_Remaining, &sent, &acked, &nacked);
    if (!modem_expect_OK(uTimer_Remaining)) return false;
  } while (sent != acked);

  // wait for the receive notification
  int id, sc, sid;
  modem_expect_scan("+QIRDI: %d,%d,%d", uTimer_Remaining, &id, &sc, &sid);
  modem_send("AT+QIRD=%d,%d,%d,%u", id, sc, sid, size);

  uint16_t ip[4], port, expected;
  if (modem_expect_scan("+QIRD: %u.%u.%u.%u:%u,TCP,%u", uTimer_Remaining,
                         &ip[0], &ip[1], &ip[2], &ip[3], &port, &expected) != 6) return 0;
  size_t received = modem_read_binary(buffer, expected, uTimer_Remaining);
  if (!modem_expect_OK(uTimer_Remaining)) return 0;
  modem_expect("CLOSED", 100);

  return received;
}

bool modem_tcp_close(uint32_t timeout) {
  timer_set_timeout(timeout * 1000);

  modem_send("AT+QICLOSE");
  if (!modem_expect_OK(uTimer_Remaining)) {
    CSTDEBUG("failed to close TCP connection\r\n");
    return false;
  }

  return true;
}
