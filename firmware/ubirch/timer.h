/*!
 * @file
 * @brief timer driver code.
 *
 * Driver for timer related tasks.
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
#ifndef _UBIRCH_TIMER_H_
#define _UBIRCH_TIMER_H_

#include <fsl_pit.h>
#include <stdint.h>
#include <board.h>

#define TIMEOUT 0

/*!
 * @brief Initialize the timer.
 */
void timer_init(void);

/*!
 * @brief Read current timer value.
 *
 * Will initialialize the timer if it has not been initialized.
 *
 * @return the current timestamp
 */
uint32_t timer_read(void);

/*!
 * @brief Schedule a timer interrupt in the future (relative).
 *
 * Schedules a timer interrupt in the given number of microseconds.
 * The maximum time of the interval given is just over 71 minutes.
 * If the interval is too large, no interrupt will be scheduled.
 *
 * @param us a relative time interval in us (less than UINT32_MAX)
 */
void timer_set_interrupt(uint32_t us);

static inline void timer_timeout(uint32_t us) {
  assert((PIT_GetEnabledInterrupts(PIT, kPIT_Chnl_3) & kPIT_TimerInterruptEnable) == 0);
  timer_set_interrupt(us);
}

static inline uint32_t timer_timeout_remaining(void) {
  if (PIT_GetEnabledInterrupts(PIT, kPIT_Chnl_3) & kPIT_TimerInterruptEnable)
    return PIT_GetCurrentTimerCount(PIT, kPIT_Chnl_3);
  return 0;
}

/*!
 * @brief Delay execution for a certain amount of milliseconds.
 *
 * This function will try to go into a low power mode (__WFE()) until the
 * end of the delay. It will schedule an interrupt in the future
 * and under optimal conditions only wake up when the interrupt
 * is triggered. If other events preempt this it checks the
 * time and continues.
 *
 * The maximum delay interval is just over 71 minutes, longer intervals
 * will return immediately, effectively not delaying!
 *
 * @param ms the milliseconds to delay execution
 */
void delay(uint32_t ms);

#endif // _UBIRCH_TIMER_H_
