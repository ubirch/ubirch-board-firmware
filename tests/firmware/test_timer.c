/**
 * Timer test.
 *
 * Tries to test that the timer functions work.
 *
 * @author Matthias L. Jugel
 * @date 2016-05-08
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

#include <board.h>
#include <stdio.h>
#include <ubirch/timer.h>
#include "test.h"

extern uint32_t test_1ms_ticker;

void test_timer_runs(void) {
  const uint32_t interval = SystemCoreClock / 10 - 1;

  uint32_t timer_start = timer_read();
  uint32_t start = test_1ms_ticker;
  for(uint32_t i = interval; i > 0; i--) __asm__ __volatile__("");
  uint32_t ticker_elapsed = test_1ms_ticker - start;
  uint32_t timer_elapsed = timer_read() - timer_start;

  PRINTF("- timer=%d, ticker=%d\r\n", timer_elapsed, ticker_elapsed);

  // test that timer elapsed at least 100000us
  ASSERT_GREATER(timer_elapsed, 100000);
  // test that the ticker has elapsed about 1000ms
  ASSERT_LESS(ticker_elapsed, 1005);
}


void test_delay(void) {
  uint32_t start = test_1ms_ticker;
  delay(1000);
  uint32_t elapsed = test_1ms_ticker - start;

  PRINTF("- start=%d, elapsed=%d, remaining=%d\r\n", start, elapsed, timer_timeout_remaining());

  // test that the elapsed time is 1000ms (precision of the timer is 1ms)
  ASSERT_TRUE(elapsed >= 1000);
}

void test_timeout() {
  int counter = 0;

  const uint32_t interval = 1000 * 1000;
  timer_set_timeout(interval);

  uint32_t timer_start = timer_read();
  while (timer_timeout_remaining()) { __WFI(); counter++; }
  uint32_t elapsed =  (timer_read() - timer_start) / 1000;

  PRINTF("- start=%d, elapsed=%d, remaining=%d\r\n", timer_start, elapsed, timer_timeout_remaining());

  // assert that we didn't just busy loop through the delay, we have an interrupt every ms
  ASSERT_TRUE(counter <= 1010);

  // assert the timing is 1000 (elapsed time between start and end)
  ASSERT_TRUE(elapsed >= 1000 && elapsed <= 1010);
}

int test_timer(void) {
  timer_init();
  ASSERT_TRUE(timer_read() > 0);

  test_timer_runs();
  test_timeout();
  test_delay();

  return 0;
}
