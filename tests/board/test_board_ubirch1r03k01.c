/**
 * Test individual components of the board.
 *
 * Blinks at 100ms interval while working and 1s interval when finished successfully.
 * Also prints messages do debug output.
 *
 * @author Matthias L. Jugel
 * @date 2016-05-09
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
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <ubirch/sim800h.h>
#include "config.h"
#include "support.h"

#if defined(BOARD_FRDM_KL82Z) || defined(BOARD_FRDM_K82F)
#  define LED BOARD_LED1
#elif defined(BOARD_UBIRCH_1R02) || defined(BOARD_UBIRCH_1R03K01)
#  define LED BOARD_LED0
#else
#  define LED(...)
#endif


volatile static int state = 100;
volatile static bool on = false;
volatile static int cnt = 0;

systick_callback_t callback;

void SysTick_Handler() {
  if (++cnt % state == 0) on = !on;
  if (callback != NULL) {
    callback(on);
  }
  LED(on);
}

int main(void) {
  // INITIALIZATION
  board_init();
  // INITIAILIZE CONSOLE (Tests debug uart pins!)
  board_console_init(BOARD_DEBUG_BAUD);

  // 100ms led blink, only works if setup for LED was correct
  SysTick_Config(BOARD_SYSTICK_100MS);

//  PRINTF(BOARD "\r\n");
//  enter("Press Enter to test debug console input: ");
//  PRINTF("OK\r\n");

  if(enable_test_audio) test_audio();
  if(enable_test_quectel) test_quectel();
  if(enable_test_gpio) test_gpio();
  if(enable_test_rgb) test_rgb();
  if(enable_test_touch) test_touch();

  PRINTF("DONE\r\n");
  while (true);
}
