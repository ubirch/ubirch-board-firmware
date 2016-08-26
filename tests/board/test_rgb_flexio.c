#include <fsl_port.h>
#include <fsl_flexio_uart.h>
#include <fsl_debug_console.h>
#include <arm_math.h>
#include "support.h"

#define WS2812B_CLOCK_FREQ    800000U
#define FLEXIO_SRC_CLOCK      kCLOCK_CoreSysClk
#define FLEXIO_SRC_CLOCK_FRQ  CLOCK_GetFreq(FLEXIO_SRC_CLOCK)
#define FLEXIO_CLOCK_DIVIDER  (FLEXIO_SRC_CLOCK_FRQ/WS2812B_CLOCK_FREQ)
#define FLEXIO_SHIFT_BITS(n)  ((((n) * 2 - 1) << 8) | ((FLEXIO_CLOCK_DIVIDER / 2) - 1))

#define FLEXIO_SHIFTER     0
#define FIO_CLOCK       0
#define FLEXIO_0_TIMER     1
#define FLEXIO_1_TIMER     2

#define FLEXIO_SHIFTER_PIN 2
#define FLEXIO_CLOCK_PIN   3
#define FLEXIO_DATA_PIN 20

#define LEDS 8

// calculate count from nanoseconds
// (!) can't use USEC_TO_COUNT with floats as the the value is cast to uint64_t)
#define NSEC_TO_COUNT(ns, frq)  (USEC_TO_COUNT(ns, (frq))/1000U)

void init_flexio() {
  // signal 0 and 1 high/low counter values,
  const uint32_t T0H = (uint32_t) NSEC_TO_COUNT(375, FLEXIO_SRC_CLOCK_FRQ) - 1;
  const uint32_t T0L = (uint32_t) NSEC_TO_COUNT(875, FLEXIO_SRC_CLOCK_FRQ) - 1;
  const uint32_t T1H = (uint32_t) NSEC_TO_COUNT(750, FLEXIO_SRC_CLOCK_FRQ) - 1;
  const uint32_t T1L = (uint32_t) NSEC_TO_COUNT(500, FLEXIO_SRC_CLOCK_FRQ) - 1;

  PRINTF("T0H=%d T0L=%d T1H=%d T1L=%d\r\n", T0H, T0L, T1H, T1L);

  CLOCK_SetFlexio0Clock(FLEXIO_SRC_CLOCK);
  CLOCK_EnableClock(kCLOCK_Flexio0);

  FLEXIO_Reset(FLEXIO0);

  // shifter
  const flexio_shifter_config_t shifter_config = {
    .inputSource = kFLEXIO_ShifterInputFromPin,
    .shifterStop = kFLEXIO_ShifterStopBitDisable,
    .shifterStart = kFLEXIO_ShifterStartBitDisabledLoadDataOnShift,
    .timerSelect = FIO_CLOCK,
    .timerPolarity = kFLEXIO_ShifterTimerPolarityOnPositive,
    .pinConfig = kFLEXIO_PinConfigOutput,
    .pinSelect = FLEXIO_SHIFTER_PIN,
    .pinPolarity = kFLEXIO_PinActiveHigh,
    .shifterMode = kFLEXIO_ShifterModeTransmit
  };
  FLEXIO_SetShifterConfig(FLEXIO0, FLEXIO_SHIFTER, &shifter_config);

  // clock generator
  const flexio_timer_config_t timer_clock_config = {
    .timerCompare = FLEXIO_SHIFT_BITS(24),
    .timerOutput = kFLEXIO_TimerOutputZeroNotAffectedByReset,
    .timerDecrement = kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput,
    .timerReset = kFLEXIO_TimerResetNever,
    .timerDisable = kFLEXIO_TimerDisableOnTimerCompare,
    .timerEnable = kFLEXIO_TimerEnableOnTriggerHigh,
    .timerStop = kFLEXIO_TimerStopBitDisabled,
    .timerStart = kFLEXIO_TimerStartBitDisabled,
    .triggerSelect = FLEXIO_TIMER_TRIGGER_SEL_SHIFTnSTAT(FLEXIO_SHIFTER),
    .triggerPolarity = kFLEXIO_TimerTriggerPolarityActiveLow,
    .triggerSource = kFLEXIO_TimerTriggerSourceInternal,
    .pinConfig = kFLEXIO_PinConfigOutput,
    .pinSelect = FLEXIO_CLOCK_PIN,
    .pinPolarity = kFLEXIO_PinActiveHigh,
    .timerMode = kFLEXIO_TimerModeDual8BitBaudBit,
  };
  FLEXIO_SetTimerConfig(FLEXIO0, FIO_CLOCK, &timer_clock_config);

  // timer to generate the 0 wave form (short pulse), triggered by clock
  const flexio_timer_config_t timer_t0_config = {
    .timerCompare = (T0L << 8) | T0H,
    .timerOutput = kFLEXIO_TimerOutputOneNotAffectedByReset,
    .timerDecrement = kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput,
    .timerReset = kFLEXIO_TimerResetNever,
    .timerDisable = kFLEXIO_TimerDisableOnTimerCompare,
    .timerEnable = kFLEXIO_TimerEnableOnTriggerRisingEdge,
    .timerStop = kFLEXIO_TimerStopBitDisabled,
    .timerStart = kFLEXIO_TimerStartBitDisabled,
    .triggerSelect = FLEXIO_TIMER_TRIGGER_SEL_PININPUT(FLEXIO_CLOCK_PIN),
    .triggerPolarity = kFLEXIO_TimerTriggerPolarityActiveHigh,
    .triggerSource = kFLEXIO_TimerTriggerSourceInternal,
    .pinConfig = kFLEXIO_PinConfigOutput,
    .pinSelect = FLEXIO_DATA_PIN,
    .pinPolarity = kFLEXIO_PinActiveHigh,
    .timerMode = kFLEXIO_TimerModeDual8BitPWM,
  };
  FLEXIO_SetTimerConfig(FLEXIO0, FLEXIO_0_TIMER, &timer_t0_config);

  // timer to generate the 1 wave form (long pulse), triggered by shifter
  const flexio_timer_config_t timer_t1_config = {
    .timerCompare = (T1L << 8) | T1H,
    .timerOutput = kFLEXIO_TimerOutputOneNotAffectedByReset,
    .timerDecrement = kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput,
    .timerReset = kFLEXIO_TimerResetNever,
    .timerDisable = kFLEXIO_TimerDisableOnTriggerFallingEdge,
    .timerEnable = kFLEXIO_TimerEnableOnTriggerRisingEdge,
    .timerStop = kFLEXIO_TimerStopBitDisabled,
    .timerStart = kFLEXIO_TimerStartBitDisabled,
    .triggerSelect = FLEXIO_TIMER_TRIGGER_SEL_PININPUT(FLEXIO_SHIFTER_PIN),
    .triggerPolarity = kFLEXIO_TimerTriggerPolarityActiveHigh,
    .triggerSource = kFLEXIO_TimerTriggerSourceInternal,
    .pinConfig = kFLEXIO_PinConfigOutput,
    .pinSelect = FLEXIO_DATA_PIN,
    .pinPolarity = kFLEXIO_PinActiveHigh,
    .timerMode = kFLEXIO_TimerModeDual8BitPWM,
  };
  FLEXIO_SetTimerConfig(FLEXIO0, FLEXIO_1_TIMER, &timer_t1_config);

  FLEXIO_Enable(FLEXIO0, true);

  //NVIC_EnableIRQ(FLEXIO0_IRQn);
}

void transmit(uint32_t *leds, int n) {
  FLEXIO0->TIMCMP[FIO_CLOCK] = FLEXIO_SHIFT_BITS(24);
  while (n--) {
    while (!(FLEXIO_GetShifterStatusFlags(FLEXIO0) & (1U << FLEXIO_SHIFTER))) {}
    // if this is the last junk of data, shift a single bit more than necessary to drive signal low
    if (!n) FLEXIO0->TIMCMP[FIO_CLOCK] = FLEXIO_SHIFT_BITS(25);
    FLEXIO0->TIMSTAT |= FLEXIO_TIMSTAT_TSF(1U << FIO_CLOCK);
    // we use the bit swapped shift buffer to send data
    FLEXIO0->SHIFTBUFBIS[0] = (*leds++) << 8;
  }
}

void test_rgb_flexio(void) {
  if (!yesno("RGB (FlexIO) test?")) return;

  CLOCK_EnableClock(kCLOCK_PortA);
  CLOCK_EnableClock(kCLOCK_PortB);

  PORT_SetPinMux(PORTB, 2U, kPORT_MuxAlt7);
  PORT_SetPinMux(PORTB, 3U, kPORT_MuxAlt7);
  PORT_SetPinMux(PORTB, 10U, kPORT_MuxAlt7);
  PORT_SetPinMux(PORTA, 14U, kPORT_MuxAlt5);

  init_flexio();

  // colors off
  uint32_t colors[LEDS] = {0, 0, 0, 0, 0, 0, 0, 0};
  transmit(colors, LEDS);
  delay(10);


  transmit(colors, LEDS);
  for(int k = 0; k < 10; k++) {
    for (int i = 0; i < LEDS; i++) {
      memset(colors, 0, sizeof(colors));
      colors[i] = 0x005500;
      transmit(colors, LEDS);
      delay(40);
    }
    for (int i = LEDS - 1; i >= 0; i--) {
      memset(colors, 0, sizeof(colors));
      colors[i] = 0x005500;
      transmit(colors, LEDS);
      delay(40);
    }
    delay(1);
  }
  delay(1000);

  // do some rainbow coloring...
  int phase = 0;
  int length = 8;
  int center = 128, width = 127;
  float frequency = PI * 2 / length;

  while (true) {
    for (int i = 0; i < length; ++i) {
      for(int l = 0; l < 8; l++) {
        uint8_t r = (uint8_t) (sin(frequency * (i+l) + 0 + phase) * width + center);
        uint8_t g = (uint8_t) (sin(frequency * (i+l) + 2 + phase) * width + center);
        uint8_t b = (uint8_t) (sin(frequency * (i+l) + 4 + phase) * width + center);
        colors[l] = g << 16 | r << 8 | b;
      }

      transmit(colors, 8);

      delay(100);
    }
  }
}
