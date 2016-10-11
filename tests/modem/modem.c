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
#include <ubirch/m66/m66_tcp.h>
#include "config.h"

bool on = true;
volatile uint32_t milliseconds = 0;

void SysTick_Handler() {
  milliseconds++;
  if (milliseconds % 1000 == 0) on = !on;
  BOARD_LED0(on);
}


int main(void) {
  // INITIALIZATION
  board_init();

  // INITIALIZE CONSOLE (Tests debug uart pins!)
  board_console_init(BOARD_DEBUG_BAUD);

  // 100ms led blink, only works if setup for LED was correct
  SysTick_Config(BOARD_SYSTICK_100MS / 10);

  // Initialize modem
  modem_init();

  if (!modem_enable())
  {
    PRINTF("failed to enable modem\r\n");
    return false;
  }

  // Register to the network
  if (!modem_register(6 * 5000))
  {
    PRINTF("failed to register\r\n");
    return false;
  }

  if (!modem_tcp_connect(CELL_APN, CELL_USER, CELL_PWD, 20 * 5000))
  {
    PRINTF("unable to connect \r\n");
    return 0;
  }

  const char send_data[] = "GET / HTTP/1.1\r\n\r\n";
  if (!modem_tcp_send(send_data, (uint8_t) strlen(send_data)))
  {
    PRINTF("failed to send\r\n");
    return 0;
  }

  modem_tcp_close(1000);

  while (true) {
    delay(1000);
  };
}
