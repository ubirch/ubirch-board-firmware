/**
 * Test the behaviour of the MODEM upon power enable.
 *
 * @author Matthias L. Jugel
 * @date 2017-01-10
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

int main(void) {
  board_init(BOARD_MODE_RUN);
  board_console_init(BOARD_DEBUG_BAUD);

  // Initialize modem
  modem_init();

  PRINTF("press return to start...");
  int c;
  while ((c = GETCHAR()) != '\r' && c != '\n') PRINTF("%02x", c);

  for(int i = 0; i < 5; i++) {
    PRINTF("%d M66 ON\r\n", i);
    modem_enable();
    PRINTF("%d M66 OFF\r\n", i);
    modem_disable();
  }

  while (true) {
    delay(1000);
  }
}
