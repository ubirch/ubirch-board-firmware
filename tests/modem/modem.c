/**
 * Establish a TCP connection - Send / Receive packets.
 *
 *
 * @author Niranjan H. Rao
 * @date 2016-10-07
 *
 * @copyright &copy; 2016 ubirch GmbH (https://ubirch.com)
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

#include <stdint.h>
#include <board.h>
#include <stdio.h>
#include <ubirch/timer.h>
#include <ubirch/modem.h>
#include <ubirch/dbgutil.h>
#include "config.h"

bool on = true;
volatile uint32_t milliseconds = 0;

void SysTick_Handler() {
  milliseconds++;
  if (milliseconds % 1000 == 0) on = !on;
  BOARD_LED0(on);
}

void error(char *msg) {
  PRINTF("ERROR: %s\r\n", msg);
  while(true) {}
}


int main(void) {
  board_init(BOARD_MODE_RUN);
  board_console_init(BOARD_DEBUG_BAUD);

  SysTick_Config(BOARD_SYSTICK_100MS / 10);

  // Initialize modem
  modem_init();

  if (!modem_enable()) error("modem enable");
  if (!modem_register(60000)) error("network register");
  if (!modem_gprs_attach(CELL_APN, CELL_USER, CELL_PWD, 30000)) error("GPRS attach");

  double lat = 0.0, lon = 0.0;
  rtc_datetime_t date;
  modem_location(&lat, &lon, &date, 30000);

  PRINTF("%04hd-%02hd-%02hd %02hd:%02hd:%02hd\r\n",
         date.year, date.month, date.day, date.hour, date.minute, date.second);

  PRINTF("Location: lat=%f, lon=%f\r\n", lat, lon);

  if (!modem_tcp_connect("api.ubirch.com", 80, 5000)) error("TCP connect");

  const char *send_data = "GET / HTTP/1.1\r\n\r\n";
  if (!modem_tcp_send((const uint8_t *) send_data, (uint8_t) strlen(send_data), 30000))
    error("simulated HTTP GET");

  uint8_t buffer[8192];
  size_t received = modem_tcp_receive(buffer, 1500, 10000);
  PRINTF("received %d bytes\r\n", received);

  dbg_dump("RCV", buffer, received);

  modem_tcp_close(1000);

  modem_disable();

  while (true) {
    delay(1000);
  }
}
