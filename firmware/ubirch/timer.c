/*!
 * @brief ubirch#1 timer driver code.
 *
 * Based on https://github.com/mbedmicro/mbed/blob/master/libraries/mbed/targets/hal/TARGET_Freescale/TARGET_KSDK2_MCUS/TARGET_K64F/us_ticker.c
 *
 * @author Matthias L. Jugel
 * @date 2016-04-06
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
#include <fsl_pit.h>
#include "timer.h"

static bool initialized = false;

void PIT3_IRQHandler() {
  PIT_ClearStatusFlags(PIT, kPIT_Chnl_3, kPIT_TimerFlag);
  PIT_DisableInterrupts(PIT, kPIT_Chnl_3, kPIT_TimerInterruptEnable);
  __SEV();
}

void timer_init() {
  pit_config_t pitConfig;
  PIT_GetDefaultConfig(&pitConfig);
  PIT_Init(PIT, &pitConfig);

  PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, (uint32_t) USEC_TO_COUNT(1U, CLOCK_GetFreq(kCLOCK_BusClk)) - 1);
  PIT_SetTimerPeriod(PIT, kPIT_Chnl_1, 0xFFFFFFFF);
  PIT_SetTimerChainMode(PIT, kPIT_Chnl_1, true);

  PIT_StartTimer(PIT, kPIT_Chnl_0);
  PIT_StartTimer(PIT, kPIT_Chnl_1);

  PIT_SetTimerPeriod(PIT, kPIT_Chnl_2, (uint32_t) USEC_TO_COUNT(1U, CLOCK_GetFreq(kCLOCK_BusClk)) - 1);
  PIT_SetTimerPeriod(PIT, kPIT_Chnl_3, 0xFFFFFFFF);
  PIT_SetTimerChainMode(PIT, kPIT_Chnl_3, true);
  EnableIRQ(PIT3_IRQn);
}

uint32_t timer_read() {
  if (!initialized) timer_init();
  return ~(PIT_GetCurrentTimerCount(PIT, kPIT_Chnl_1));
}

uint32_t timer_schedule(uint32_t timestamp) {
  int delta = (int) (timestamp - timer_read());
  if (delta <= 0) {
    // This event was in the past.
    // Set the interrupt as pending, but don't process it here.
    // This prevents a recursive loop under heavy load
    // which can lead to a stack overflow.
    NVIC_SetPendingIRQ(PIT3_IRQn);
  } else {

    PIT_StopTimer(PIT, kPIT_Chnl_3);
    PIT_StopTimer(PIT, kPIT_Chnl_2);
    PIT_SetTimerPeriod(PIT, kPIT_Chnl_3, (uint32_t) delta);
    PIT_EnableInterrupts(PIT, kPIT_Chnl_3, kPIT_TimerInterruptEnable);
    PIT_StartTimer(PIT, kPIT_Chnl_3);
    PIT_StartTimer(PIT, kPIT_Chnl_2);
  }
  return timestamp;
}

extern uint32_t timer_schedule_in(uint32_t us);

extern void delay(uint32_t ms);
