/*
 * ubirch#1 RGB LED driver code (for WS2812b compatible LED strips).
 *
 * @author Matthias L. Jugel
 * @date 2016-04-06
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

#include <fsl_port.h>
#include <fsl_flexio.h>
#include "ws2812b.h"

#define WS2812B_CLOCK_FREQ 800000U
#define WS2812B_RGB_BITS   24

#define FLEXIO_CLOCK_DIVIDER(freq)  ((freq)/WS2812B_CLOCK_FREQ)
#define FLEXIO_SHIFT_BITS_MASK      (0x0000FF00)
#define FLEXIO_SHIFT_BITS(n)        (((n) * 2 - 1) << 8)

// calculate count from nanoseconds
#define NSEC_TO_COUNT(ns, freq)  (((uint64_t)(ns) * (uint64_t)(freq))/1000000000UL)

static uint8_t shifter_idx, clk_timer_idx;

extern uint32_t rgb(uint8_t r, uint8_t g, uint8_t b);

void ws2812b_init(ws2812b_config_t *config) {
  // get frequency of the flexio source clock
  const clock_name_t flexio_src_clk = (clock_name_t) ((SIM->SOPT2 & SIM_SOPT2_FLEXIOSRC_MASK) >> SIM_SOPT2_FLEXIOSRC_SHIFT);
  const uint32_t flexio_src_clk_freq = CLOCK_GetFreq(flexio_src_clk);

  // signal 0 and 1 high/low counter values,
  const uint32_t T0H = (uint32_t) NSEC_TO_COUNT(375, flexio_src_clk_freq) - 1;
  const uint32_t T1H = (uint32_t) NSEC_TO_COUNT(750, flexio_src_clk_freq) - 1;
  const uint32_t T0L = (uint32_t) NSEC_TO_COUNT(800, flexio_src_clk_freq) - 1;
  const uint32_t T1L = (uint32_t) NSEC_TO_COUNT(500, flexio_src_clk_freq) - 1;

  PRINTF("T0H=%d T1H=%d T0L=%d T1L=%d\r\n", T0H, T1H, T0L, T1L);

  // shifter configuration, take clock timer as input and output in flexio pin
  const flexio_shifter_config_t shifter_config = {
    .inputSource = kFLEXIO_ShifterInputFromPin,
    .shifterStop = kFLEXIO_ShifterStopBitDisable,
    .shifterStart = kFLEXIO_ShifterStartBitDisabledLoadDataOnShift,
    .timerSelect = config->flexio_clk_timer,
    .timerPolarity = kFLEXIO_ShifterTimerPolarityOnPositive,
    .pinConfig = kFLEXIO_PinConfigOutput,
    .pinSelect = config->flexio_shifter_pin,
    .pinPolarity = kFLEXIO_PinActiveHigh,
    .shifterMode = kFLEXIO_ShifterModeTransmit
  };
  shifter_idx = config->flexio_shifter;
  FLEXIO_SetShifterConfig(FLEXIO0, config->flexio_shifter, &shifter_config);

  // clock generator, generates the 800kHz clock signal in sync with the shifter, output in flexio timer pin
  const flexio_timer_config_t timer_clock_config = {
    .timerCompare = FLEXIO_SHIFT_BITS(WS2812B_RGB_BITS) | ((FLEXIO_CLOCK_DIVIDER(flexio_src_clk_freq) / 2) - 1),
    .timerOutput = kFLEXIO_TimerOutputZeroNotAffectedByReset,
    .timerDecrement = kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput,
    .timerReset = kFLEXIO_TimerResetNever,
    .timerDisable = kFLEXIO_TimerDisableOnTimerCompare,
    .timerEnable = kFLEXIO_TimerEnableOnTriggerHigh,
    .timerStop = kFLEXIO_TimerStopBitDisabled,
    .timerStart = kFLEXIO_TimerStartBitDisabled,
    .triggerSelect = FLEXIO_TIMER_TRIGGER_SEL_SHIFTnSTAT(config->flexio_shifter),
    .triggerPolarity = kFLEXIO_TimerTriggerPolarityActiveLow,
    .triggerSource = kFLEXIO_TimerTriggerSourceInternal,
    .pinConfig = kFLEXIO_PinConfigOutput,
    .pinSelect = config->flexio_clk_timer_pin,
    .pinPolarity = kFLEXIO_PinActiveHigh,
    .timerMode = kFLEXIO_TimerModeDual8BitBaudBit,
  };
  // store a reference to the TIMCMP index for changing the amount of bits send
  clk_timer_idx = config->flexio_clk_timer;
  FLEXIO_SetTimerConfig(FLEXIO0, config->flexio_clk_timer, &timer_clock_config);

  // timer to generate the 0 signal (0.35us high, 0.9us low, short pulse), triggered by clock
  const flexio_timer_config_t timer_t0_config = {
    .timerCompare = (T0L << 8) | T0H,
    .timerOutput = kFLEXIO_TimerOutputOneNotAffectedByReset,
    .timerDecrement = kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput,
    .timerReset = kFLEXIO_TimerResetNever,
    .timerDisable = kFLEXIO_TimerDisableOnTimerCompare,
    .timerEnable = kFLEXIO_TimerEnableOnTriggerRisingEdge,
    .timerStop = kFLEXIO_TimerStopBitDisabled,
    .timerStart = kFLEXIO_TimerStartBitDisabled,
    .triggerSelect = FLEXIO_TIMER_TRIGGER_SEL_PININPUT(config->flexio_clk_timer_pin),
    .triggerPolarity = kFLEXIO_TimerTriggerPolarityActiveHigh,
    .triggerSource = kFLEXIO_TimerTriggerSourceInternal,
    .pinConfig = kFLEXIO_PinConfigOutput,
    .pinSelect = config->flexio_data_pin,
    .pinPolarity = kFLEXIO_PinActiveHigh,
    .timerMode = kFLEXIO_TimerModeDual8BitPWM,
  };
  FLEXIO_SetTimerConfig(FLEXIO0, config->flexio_t0_timer, &timer_t0_config);

  // timer to generate the 1 signal (0.9us high, 0.35us low, long pulse), triggered by shifter
  const flexio_timer_config_t timer_t1_config = {
    .timerCompare = (T1L << 8) | T1H,
    .timerOutput = kFLEXIO_TimerOutputOneNotAffectedByReset,
    .timerDecrement = kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput,
    .timerReset = kFLEXIO_TimerResetNever,
    .timerDisable = kFLEXIO_TimerDisableOnTriggerFallingEdge,
    .timerEnable = kFLEXIO_TimerEnableOnTriggerRisingEdge,
    .timerStop = kFLEXIO_TimerStopBitDisabled,
    .timerStart = kFLEXIO_TimerStartBitDisabled,
    .triggerSelect = FLEXIO_TIMER_TRIGGER_SEL_PININPUT(config->flexio_shifter_pin),
    .triggerPolarity = kFLEXIO_TimerTriggerPolarityActiveHigh,
    .triggerSource = kFLEXIO_TimerTriggerSourceInternal,
    .pinConfig = kFLEXIO_PinConfigOutput,
    .pinSelect = config->flexio_data_pin,
    .pinPolarity = kFLEXIO_PinActiveHigh,
    .timerMode = kFLEXIO_TimerModeDual8BitPWM,
  };
  FLEXIO_SetTimerConfig(FLEXIO0, config->flexio_t1_timer, &timer_t1_config);

  // ensure flexio is enabled
  FLEXIO_Enable(FLEXIO0, true);
  //FLEXIO_EnableShifterStatusDMA(FLEXIO0, config->flexio_shifter, true);
}

void ws2812b_send(uint32_t *data, unsigned int num_leds) {
  // set the shifted bits to the number of bits per LED (24)
  volatile uint32_t *TIMCMP = &(FLEXIO0->TIMCMP[clk_timer_idx]);
  *TIMCMP = (*TIMCMP & ~FLEXIO_SHIFT_BITS_MASK) | FLEXIO_SHIFT_BITS(WS2812B_RGB_BITS);

  while (num_leds--) {
    // wait for the shifter to finish
    while (!(FLEXIO_GetShifterStatusFlags(FLEXIO0) & (1U << shifter_idx))) {}

    // if this is the last junk of data, shift a single bit more than necessary to drive signal low
    if (!num_leds) *TIMCMP = (*TIMCMP & ~FLEXIO_SHIFT_BITS_MASK) | FLEXIO_SHIFT_BITS(WS2812B_RGB_BITS);

    // set shifter operational and put data in the  swapped shift buffer to send
    FLEXIO0->TIMSTAT |= FLEXIO_TIMSTAT_TSF(1U << clk_timer_idx);
    FLEXIO0->SHIFTBUFBIS[0] = (*data++) << 8;
  }
}
