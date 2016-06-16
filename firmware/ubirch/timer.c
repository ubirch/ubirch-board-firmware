/*
 * ubirch#1 timer driver code.
 *
 * Found some inspiration here:
 * https://github.com/mbedmicro/mbed/blob/master/libraries/mbed/targets/hal/TARGET_Freescale/TARGET_KSDK2_MCUS/TARGET_K64F/us_ticker.c
 *
 * TODO may make sense to move this into the board code and provide a programmatic interface.
 *
 * @author Matthias L. Jugel
 * @date 2016-04-06
 *
 * Copyright 2016 ubirch GmbH (https://ubirch.com)
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
#include <fsl_pit.h>
#include "timer.h"

static bool initialized = false;

void BOARD_TIMER_HANDLER() {
  // clear interrupt flag
  PIT_ClearStatusFlags(BOARD_TIMER, kPIT_Chnl_3, kPIT_TimerFlag);
  PIT_DisableInterrupts(BOARD_TIMER, kPIT_Chnl_3, kPIT_TimerInterruptEnable);

  // stop the timer
  PIT_StopTimer(BOARD_TIMER, kPIT_Chnl_3);
  PIT_StopTimer(BOARD_TIMER, kPIT_Chnl_2);

  // create continue event
  __SEV();
}

void timer_init() {
  if (initialized) return;
  initialized = true;

  pit_config_t pitConfig;
  PIT_GetDefaultConfig(&pitConfig);
  PIT_Init(BOARD_TIMER, &pitConfig);

  PIT_SetTimerPeriod(BOARD_TIMER, kPIT_Chnl_0, (uint32_t) USEC_TO_COUNT(1U, CLOCK_GetFreq(kCLOCK_BusClk)) - 1);
  PIT_SetTimerPeriod(BOARD_TIMER, kPIT_Chnl_1, 0xFFFFFFFF);
  PIT_SetTimerChainMode(BOARD_TIMER, kPIT_Chnl_1, true);

  PIT_StartTimer(BOARD_TIMER, kPIT_Chnl_0);
  PIT_StartTimer(BOARD_TIMER, kPIT_Chnl_1);

  PIT_SetTimerPeriod(BOARD_TIMER, kPIT_Chnl_2, (uint32_t) USEC_TO_COUNT(1U, CLOCK_GetFreq(kCLOCK_BusClk)) - 1);
  PIT_SetTimerPeriod(BOARD_TIMER, kPIT_Chnl_3, 0xFFFFFFFF);
  PIT_SetTimerChainMode(BOARD_TIMER, kPIT_Chnl_3, true);

  EnableIRQ(BOARD_TIMER_IRQ);
}

uint32_t timer_read() {
  if (!initialized) timer_init();
  return ~(PIT_GetCurrentTimerCount(BOARD_TIMER, kPIT_Chnl_1));
}

// TODO: handle longer than 71 minute interrupt times (chaining both timers)
void timer_set_interrupt(uint32_t us) {
  if (!initialized) timer_init();

  PIT_StopTimer(BOARD_TIMER, kPIT_Chnl_3);
  PIT_StopTimer(BOARD_TIMER, kPIT_Chnl_2);
  PIT_SetTimerPeriod(BOARD_TIMER, kPIT_Chnl_3, (uint32_t) us);
  PIT_EnableInterrupts(BOARD_TIMER, kPIT_Chnl_3, kPIT_TimerInterruptEnable);
  PIT_StartTimer(BOARD_TIMER, kPIT_Chnl_3);
  PIT_StartTimer(BOARD_TIMER, kPIT_Chnl_2);
}

void timer_set_timeout(uint32_t us) {
  if (us) timer_set_interrupt(us);
}

uint32_t timer_timeout_remaining() {
  if (!initialized) timer_init();

  if (PIT_GetEnabledInterrupts(PIT, kPIT_Chnl_3) & kPIT_TimerInterruptEnable)
    return PIT_GetCurrentTimerCount(PIT, kPIT_Chnl_3);
  return 0;
}

void delay_us(uint32_t us) {
  // save the current timeout, so delay() does not interfere
  uint32_t remaining = timer_timeout_remaining();

  timer_set_timeout(us);
  while (timer_timeout_remaining()) { __WFE(); }

  // set timeout to the value before delay and minus this delay
  if (remaining > us) timer_set_timeout(remaining - us);
}

void delay(uint32_t ms) {
  if (ms > (uTimer_MaxTimeout - 1) / 1000) return;

  uint32_t us_delay = ms * 1000;
  delay_us(us_delay);
}

