/**
 * Test the Firmware code.
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
#include "test.h"

int test_i2c();

int test_timer();

int test_rtc();

#if defined(BOARD_FRDM_KL82Z) || defined(BOARD_FRDM_K82F)
#  define LED BOARD_LED1
#elif defined(BOARD_UBIRCH_1R02)
#  define LED BOARD_LED0
#else
#  define LED(...)
#endif

volatile bool on = true;

void SysTick_Handler() {
  test_1ms_ticker++;
  if (test_1ms_ticker % 100 == 0) on = !on;
  LED(on);
}

int main(void) {
  board_init();
  board_console_init(BOARD_DEBUG_BAUD);

  // set the ticker to 1ms for comparison rates
  SysTick_Config(BOARD_SYSTICK_1MS);

  PRINTF("Testing Board and Firmware: " BOARD "\r\n\r\n");

  TEST("I2C", test_i2c());
  TEST("TIMER", test_timer());
  TEST("RTC", test_rtc());

  PRINTF("Test finished.\r\n");

  SysTick_Config(BOARD_SYSTICK_100MS);

  return 0;
}
