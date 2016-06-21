/**
 * Real Time Clock test.
 *
 * Tries to test that the RTC functions work.
 *
 * @author Matthias L. Jugel
 * @date 2016-05-09
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
#include <ubirch/rtc.h>
#include <ubirch/timer.h>
#include "test.h"

extern uint32_t test_1ms_ticker;

static const rtc_datetime_t fixdate = {
  .year = 2016,
  .month = 5,
  .day = 9,
  .hour = 0,
  .minute = 0,
  .second = 0
};

void test_rtc_advances() {
  rtc_set(&fixdate);
  PRINTF("- (0s) %04hd-%02hd-%02hd %02hd:%02hd:%02hd\r\n",
         fixdate.year, fixdate.month, fixdate.day, fixdate.hour, fixdate.minute, fixdate.second);

  delay(5000);

  rtc_datetime_t datetime;
  rtc_get(&datetime);

  PRINTF("- (5s) %04hd-%02hd-%02hd %02hd:%02hd:%02hd\r\n",
         datetime.year, datetime.month, datetime.day, datetime.hour, datetime.minute, datetime.second);
  ASSERT_TRUE(datetime.second >= 4 && datetime.second <= 6);
}

volatile bool rtc_interrupt_happened = false;

void rtc_interrupt(rtc_datetime_t *datetime) {
  PRINTF("- (IR) %04hd-%02hd-%02hd %02hd:%02hd:%02hd\r\n",
         datetime->year, datetime->month, datetime->day, datetime->hour, datetime->minute, datetime->second);
  rtc_interrupt_happened = true;
}

void test_rtc_alarm() {
  rtc_interrupt_happened = false;

  rtc_set(&fixdate);
  rtc_datetime_t datetime;
  memcpy(&datetime, &fixdate, sizeof(rtc_datetime_t));

  rtc_attach(rtc_interrupt);
  datetime.second += 2;
  rtc_set_alarm(&datetime);
  delay(3000);
  rtc_detach();

  ASSERT_TRUE(rtc_interrupt_happened);
}

void test_rtc_alarm_in() {
  rtc_interrupt_happened = false;

  rtc_set(&fixdate);
  rtc_datetime_t datetime;
  memcpy(&datetime, &fixdate, sizeof(rtc_datetime_t));

  rtc_attach(rtc_interrupt);
  datetime.second += 2;
  rtc_set_alarm_in(2);
  delay(3000);
  rtc_detach();

  ASSERT_TRUE(rtc_interrupt_happened);
}


int test_rtc(void) {
  rtc_init();

  test_rtc_advances();
  test_rtc_alarm();
  test_rtc_alarm_in();

  return 0;
}
