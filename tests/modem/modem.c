/**
 * Identify the board.
 *
 * Outputs the UUID and the IMEI (if available)
 *
 * @author Matthias L. Jugel
 * @date 2016-10-04
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
#include <stdlib.h>
#include <ubirch/timer.h>
#include <ubirch/device.h>
#include <ubirch/modem.h>
#include <ubirch/m66/m66_tcp.h>
#include "../config.h"

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

  uint32_t uuid[4];
  device_uuid(uuid);
  printf("UUID: %08lX-%04lX-%04lX-%04lX-%04lX%08lX\r\n",
         uuid[0],                     // 8
         uuid[1] >> 16,               // 4
         uuid[1] & 0xFFFF,            // 4
         uuid[2] >> 16,               // 4
         uuid[2] & 0xFFFF, uuid[3]);  // 4+8

  // Initialize modem
  modem_init();

  if (!modem_enable())
  {
    printf("failed to enable modem\r\n");
    return false;
  }

  // Register to the network
  if (!modem_register(6 * 5000))
  {
    printf("failed to register\r\n");
    return false;
  }

  if (!modem_mqtt_connect( CELL_APN, CELL_USER, CELL_PWD, 20 * 5000))
  {
    printf("unable to connect \r\n");
    return 0;
  }

  const char send_data[] = {"GET / HTTP/1.1\r\n\r\n"};
  if (!modem_mqtt_send(send_data, (uint8_t)strlen(send_data)))
  {
    printf("failed to send\r\n");
    return 0;
  }

  modem_mqtt_close(1000);

  while (true) {
    delay(1000);
  };
}
