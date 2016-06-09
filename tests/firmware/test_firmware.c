/**
 * Test the Firmware code.
 *
 * @author Matthias L. Jugel
 * @date 2016-05-09
 *
 * Copyright 2016 ubirch GmbH (https://ubirch.com)
 *
 * == LICENSE ==
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
 */

#include <stdint.h>
#include <board.h>
#include "test.h"

int test_i2c();
int test_timer();
int test_rtc();

void SysTick_Handler() {
  test_100ms_ticker++;
  BOARD_LED0((test_100ms_ticker % 100) < 10);
}

int main(void) {
  board_init();
  board_console_init(BOARD_DEBUG_BAUD);
  SysTick_Config(BOARD_SYSTICK_100MS);

  PRINTF("Testing Board and Firmware: " BOARD "\r\n\r\n");

  TEST("I2C", test_i2c());
  TEST("TIMER", test_timer());
  TEST("RTC", test_rtc());

  PRINTF("Test finished.\r\n");

  return 0;
}
