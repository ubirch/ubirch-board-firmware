/**
 * @brief ubirch#1 rtc driver code.
 *
 * @author Matthias L. Jugel
 * @date 2016-04-07
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
#include "rtc.h"

static rtc_datetime_handler_t _handler = NULL;

/*!
 * @brief ISR for alarm interrupt.
 * Switches off the alarm after the event and calls the handler if
 * one is attached.
 */
void RTC_IRQHandler(void) {
  if (RTC_GetStatusFlags(BOARD_RTC) & kRTC_AlarmFlag) {
    RTC_ClearStatusFlags(BOARD_RTC, kRTC_AlarmFlag);
    RTC_DisableInterrupts(BOARD_RTC, kRTC_AlarmInterruptEnable);
    if (_handler != NULL) {
      rtc_datetime_t datetime;
      RTC_GetDatetime(RTC, &datetime);
      _handler(&datetime);
    }
  }
}

void rtc_init() {
  rtc_config_t rtcConfig;
  RTC_GetDefaultConfig(&rtcConfig);
  RTC_Init(BOARD_RTC, &rtcConfig);
  BOARD_RTC->CR |= BOARD_RTC_CLOCK;
  EnableIRQ(BOARD_RTC_IRQ);
}

void rtc_set(const rtc_datetime_t *datetime) {
  RTC_StopTimer(BOARD_RTC);
  RTC_SetDatetime(BOARD_RTC, datetime);
  RTC_StartTimer(BOARD_RTC);
}

void rtc_get(rtc_datetime_t *datetime) {
  RTC_GetDatetime(BOARD_RTC, datetime);
}

void rtc_set_alarm(const rtc_datetime_t *datetime) {
  RTC_EnableInterrupts(BOARD_RTC, kRTC_AlarmInterruptEnable);
  RTC_SetAlarm(BOARD_RTC, datetime);
}

void rtc_get_alarm(rtc_datetime_t *datetime) {
  RTC_GetAlarm(BOARD_RTC, datetime);
}

void rtc_set_alarm_in(const uint32_t seconds) {
  BOARD_RTC->TAR = BOARD_RTC->TSR + seconds;
  RTC_EnableInterrupts(BOARD_RTC, kRTC_AlarmInterruptEnable);
}

void rtc_attach(rtc_datetime_handler_t handler) {
  __disable_irq();
  _handler = handler;
  __enable_irq();
}

extern void rtc_detach();
