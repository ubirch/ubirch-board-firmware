/**
 * Timer test.
 *
 * Tries to test that the timer functions work.
 *
 * @author Matthias L. Jugel
 * @date 2016-05-08
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

#include <board.h>
#include <stdio.h>
#include <ubirch/timer.h>

extern uint32_t test_1ms_ticker;

void test_timer_runs(void) {
  const uint32_t interval = SystemCoreClock / 10 - 1;

  uint32_t timer_start = timer_read();
  uint32_t start = test_1ms_ticker;
  for(uint32_t i = interval; i > 0; i--);
  uint32_t ticker_elapsed = test_1ms_ticker - start;
  uint32_t timer_elapsed = timer_read() - timer_start;

  // test that timer elapsed at least 100000us
  assert(timer_elapsed > 100000);
  // test that the ticker has elapsed about 1000ms
  assert(ticker_elapsed <= 1005);
}


void test_delay(void) {
  uint32_t start = test_1ms_ticker;
  delay(1000);
  uint32_t elapsed = test_1ms_ticker - start;

  // test that the elapsed time is 100 (100ms precision)
  assert(elapsed >= 1000);
}

void test_timeout() {
  int counter = 0;

  const uint32_t interval = 1000 * 1000;
  timer_timeout(interval);

  uint32_t timer_start = timer_read();
  while (timer_timeout_remaining()) { __WFI(); counter++; }
  uint32_t elapsed =  (timer_read() - timer_start) / 1000;

  // assert that we didn't just busy loop through the delay, we have an interrupt every ms
  assert(counter <= 1010);

  // assert the timing is 1000 (elapsed time between start and end)
  assert(elapsed >= 1000 && elapsed <= 1010);
}

int test_timer(void) {
  timer_init();
  assert(timer_read() > 0);

  test_timer_runs();
  test_timeout();
  test_delay();

  return 0;
}
