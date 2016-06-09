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

extern uint32_t test_100ms_ticker;

void test_delay(void) {
  uint32_t start = test_100ms_ticker;
  delay(1000);

  // test that the elapsed time is 100 (100ms precision)
  uint32_t elapsed = test_100ms_ticker - start;
  assert(elapsed >= 100);
}

void test_schedule() {
  int counter = 0;

  uint32_t target = timer_schedule_in(1000 * 1000);
  // do the waiting as in delay()
  uint32_t start = timer_read();
  while (timer_read() < target) { __WFE(); counter++; }
  uint32_t elapsed =  (timer_read() - start) / 1000;

  // assert that we didn't just busy loop through the delay
  assert(counter < 1000);

  // assert the timing is 1000 (elapsed time between start and end)
  assert(elapsed >= 1000);
}

int test_timer(void) {
  timer_init();
  assert(timer_read() > 0);

  test_delay();
  test_schedule();

  return 0;
}
