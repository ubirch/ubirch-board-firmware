#include <fsl_port.h>
#include <fsl_flexio_uart.h>
#include <ubirch/timer.h>
#include <arm_math.h>
#include "support.h"

#define WS2812B_PROT_CLOCK_FREQ    800000U
#define FLEXIO_SRC_CLOCK      kCLOCK_CoreSysClk
#define FLEXIO_SRC_CLOCK_FRQ  CLOCK_GetFreq(FLEXIO_SRC_CLOCK)
#define FLEXIO_CLOCK_DIVIDER  (FLEXIO_SRC_CLOCK_FRQ/WS2812B_PROT_CLOCK_FREQ)


#define FIO_SHIFTER     0
#define FIO_CLOCK       0
#define FIO_0_TIMER     1
#define FIO_1_TIMER     2

#define FIO_SHIFTER_PIN 2
#define FIO_CLOCK_PIN   3
#define WS2812B_DIN_PIN 20


void init_flexio() {
  // signal 0 and 1 high/low counter values,
  const uint32_t T0H = (uint32_t) (0.375 * FLEXIO_SRC_CLOCK_FRQ / 1000000U);
  const uint32_t T0L = (uint32_t) (0.875 * FLEXIO_SRC_CLOCK_FRQ / 1000000U);
  const uint32_t T1H = (uint32_t) (0.75 * FLEXIO_SRC_CLOCK_FRQ / 1000000U);
  const uint32_t T1L = (uint32_t) (0.50 * FLEXIO_SRC_CLOCK_FRQ / 1000000U);
  const uint32_t LTC = (uint32_t) (50 * FLEXIO_SRC_CLOCK_FRQ / 1000000U);

  CLOCK_SetFlexio0Clock(FLEXIO_SRC_CLOCK);
  CLOCK_EnableClock(kCLOCK_Flexio0);

  FLEXIO_Reset(FLEXIO0);

  FLEXIO0->CTRL = 0;

  // shifter
  FLEXIO0->SHIFTCTL[FIO_SHIFTER] = 0;
  FLEXIO0->SHIFTCFG[FIO_SHIFTER] = FLEXIO_SHIFTCFG_INSRC(0) |
                                   FLEXIO_SHIFTCFG_SSTOP(kFLEXIO_ShifterStopBitDisable) |
                                   FLEXIO_SHIFTCFG_SSTART(kFLEXIO_ShifterStartBitDisabledLoadDataOnShift);
  FLEXIO0->SHIFTCTL[FIO_SHIFTER] = FLEXIO_SHIFTCTL_TIMSEL(FIO_CLOCK) |
                                   FLEXIO_SHIFTCTL_TIMPOL(kFLEXIO_ShifterTimerPolarityOnPositive) |
                                   FLEXIO_SHIFTCTL_PINCFG(kFLEXIO_PinConfigOutput) |
                                   FLEXIO_SHIFTCTL_PINSEL(FIO_SHIFTER_PIN) |
                                   FLEXIO_SHIFTCTL_PINPOL(kFLEXIO_PinActiveHigh) |
                                   FLEXIO_SHIFTCTL_SMOD(kFLEXIO_ShifterModeTransmit);

  // clock generator
  FLEXIO0->TIMCTL[FIO_CLOCK] = 0;
  FLEXIO0->TIMCMP[FIO_CLOCK] = ((24 * 2 - 1) << 8) | ((FLEXIO_CLOCK_DIVIDER / 2) - 1);
  FLEXIO0->TIMCFG[FIO_CLOCK] =
    FLEXIO_TIMCFG_TIMOUT(kFLEXIO_TimerOutputZeroNotAffectedByReset) |
    FLEXIO_TIMCFG_TIMDEC(kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput) |
    FLEXIO_TIMCFG_TIMRST(kFLEXIO_TimerResetNever) |
    FLEXIO_TIMCFG_TIMDIS(kFLEXIO_TimerDisableOnTimerCompare) |
    FLEXIO_TIMCFG_TIMENA(kFLEXIO_TimerEnableOnTriggerHigh) |
    FLEXIO_TIMCFG_TSTOP(kFLEXIO_TimerStopBitDisabled) |
    FLEXIO_TIMCFG_TSTART(kFLEXIO_TimerStartBitDisabled);
  FLEXIO0->TIMCTL[FIO_CLOCK] =
    FLEXIO_TIMCTL_TRGSEL(FLEXIO_TIMER_TRIGGER_SEL_SHIFTnSTAT(FIO_SHIFTER)) | // shifter 0 status flag
    FLEXIO_TIMCTL_TRGPOL(kFLEXIO_TimerTriggerPolarityActiveLow) |
    FLEXIO_TIMCTL_TRGSRC(kFLEXIO_TimerTriggerSourceInternal) |
    FLEXIO_TIMCTL_PINCFG(kFLEXIO_PinConfigOutput) |
    FLEXIO_TIMCTL_PINSEL(FIO_CLOCK_PIN) |
    FLEXIO_TIMCTL_PINPOL(kFLEXIO_PinActiveHigh) |
    FLEXIO_TIMCTL_TIMOD(kFLEXIO_TimerModeDual8BitBaudBit);

  // timer to generate the 0 wave form (short pulse), triggered by clock
  FLEXIO0->TIMCTL[FIO_0_TIMER] = 0;
  FLEXIO0->TIMCMP[FIO_0_TIMER] = ((T0L - 1) << 8) | (T0H - 1);
  FLEXIO0->TIMCFG[FIO_0_TIMER] =
    FLEXIO_TIMCFG_TIMOUT(kFLEXIO_TimerOutputOneNotAffectedByReset) |
    FLEXIO_TIMCFG_TIMDEC(kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput) |
    FLEXIO_TIMCFG_TIMRST(kFLEXIO_TimerResetNever) |
    FLEXIO_TIMCFG_TIMDIS(kFLEXIO_TimerDisableOnTimerCompare) |
    FLEXIO_TIMCFG_TIMENA(kFLEXIO_TimerEnableOnTriggerRisingEdge) |
    FLEXIO_TIMCFG_TSTOP(kFLEXIO_TimerStopBitDisabled) |
    FLEXIO_TIMCFG_TSTART(kFLEXIO_TimerStartBitDisabled);
  FLEXIO0->TIMCTL[FIO_0_TIMER] =
    FLEXIO_TIMCTL_TRGSEL(FLEXIO_TIMER_TRIGGER_SEL_PININPUT(FIO_CLOCK_PIN)) | // pin 19 (9*2+1) from clock
    FLEXIO_TIMCTL_TRGPOL(kFLEXIO_TimerTriggerPolarityActiveHigh) |
    FLEXIO_TIMCTL_TRGSRC(kFLEXIO_TimerTriggerSourceInternal) |
    FLEXIO_TIMCTL_PINCFG(kFLEXIO_PinConfigOutput) |
    FLEXIO_TIMCTL_PINSEL(WS2812B_DIN_PIN) |
    FLEXIO_TIMCTL_PINPOL(kFLEXIO_PinActiveHigh) |
    FLEXIO_TIMCTL_TIMOD(kFLEXIO_TimerModeDual8BitPWM);

  // timer to generate the 1 wave form (long pulse), triggered by shifter
  FLEXIO0->TIMCTL[FIO_1_TIMER] = 0;
  FLEXIO0->TIMCMP[FIO_1_TIMER] = ((T1L - 1) << 8) | (T1H - 1);
  FLEXIO0->TIMCFG[FIO_1_TIMER] =
    FLEXIO_TIMCFG_TIMOUT(kFLEXIO_TimerOutputOneNotAffectedByReset) |
    FLEXIO_TIMCFG_TIMDEC(kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput) |
    FLEXIO_TIMCFG_TIMRST(kFLEXIO_TimerResetNever) |
    FLEXIO_TIMCFG_TIMDIS(kFLEXIO_TimerDisableOnTriggerFallingEdge) |
    FLEXIO_TIMCFG_TIMENA(kFLEXIO_TimerEnableOnTriggerRisingEdge) |
    FLEXIO_TIMCFG_TSTOP(kFLEXIO_TimerStopBitDisabled) |
    FLEXIO_TIMCFG_TSTART(kFLEXIO_TimerStartBitDisabled);
  FLEXIO0->TIMCTL[FIO_1_TIMER] =
    FLEXIO_TIMCTL_TRGSEL(FLEXIO_TIMER_TRIGGER_SEL_PININPUT(FIO_SHIFTER_PIN)) | // pin 18 (9*2) - from shifter
    FLEXIO_TIMCTL_TRGPOL(kFLEXIO_TimerTriggerPolarityActiveHigh) |
    FLEXIO_TIMCTL_TRGSRC(kFLEXIO_TimerTriggerSourceInternal) |
    FLEXIO_TIMCTL_PINCFG(kFLEXIO_PinConfigOutput) |
    FLEXIO_TIMCTL_PINSEL(WS2812B_DIN_PIN) |
    FLEXIO_TIMCTL_PINPOL(kFLEXIO_PinActiveHigh) |
    FLEXIO_TIMCTL_TIMOD(kFLEXIO_TimerModeDual8BitPWM);

  FLEXIO0->CTRL = FLEXIO_CTRL_FLEXEN(1);

  //NVIC_EnableIRQ(FLEXIO0_IRQn);
}

void transmit(uint32_t *leds, int n) {
  FLEXIO0->TIMCMP[FIO_CLOCK] = ((24 * 2 - 1) << 8) | ((FLEXIO_CLOCK_DIVIDER / 2) - 1);
  while (n--) {
    while (!(FLEXIO_GetShifterStatusFlags(FLEXIO0) & (1U << FIO_SHIFTER))) {}
    if (!n) FLEXIO0->TIMCMP[FIO_CLOCK] = ((25 * 2 - 1) << 8) | ((FLEXIO_CLOCK_DIVIDER / 2) - 1);
    FLEXIO0->TIMSTAT |= FLEXIO_TIMSTAT_TSF(1U << FIO_CLOCK);
    // we use the bit swapped shift buffer to send data
    FLEXIO0->SHIFTBUFBIS[0] = (*leds++) << 8;
  }
}

void test_rgb_uart(void) {
  if (!yesno("RGB (UART) test?")) return;

  CLOCK_EnableClock(kCLOCK_PortA);
  CLOCK_EnableClock(kCLOCK_PortB);

  PORT_SetPinMux(PORTB, 2U, kPORT_MuxAlt7);
  PORT_SetPinMux(PORTB, 3U, kPORT_MuxAlt7);
  PORT_SetPinMux(PORTB, 10U, kPORT_MuxAlt7);
  PORT_SetPinMux(PORTA, 14U, kPORT_MuxAlt5);

  init_flexio();

  // colors off
  uint32_t colors[2] = {0, 0};
  transmit(colors, 2);

  // swap blue and green (left/right)
  colors[0] = 0x550000;
  colors[1] = 0x000055;
  transmit(colors, 2);
  for (int i = 0; i < 5; i++) {
    delay(500);
    uint32_t tmp = colors[0];
    colors[0] = colors[1];
    colors[1] = tmp;
    transmit(colors, 2);
  }

  delay(1000);

  int phase = 0;
  int length = 128;
  int center = 128, width = 127;
  float frequency = PI * 2 / length;

  while (true) {
    for (int i = 0; i < length; ++i) {
      uint8_t r0 = (uint8_t) (sin(frequency * i + 0 + phase) * width + center);
      uint8_t g0 = (uint8_t) (sin(frequency * i + 2 + phase) * width + center);
      uint8_t b0 = (uint8_t) (sin(frequency * i + 4 + phase) * width + center);
      // double frequency (second LED)
      uint8_t r1 = (uint8_t) (sin(frequency * 2 * i + 0 + phase) * width + center);
      uint8_t g1 = (uint8_t) (sin(frequency * 2 * i + 2 * phase) * width + center);
      uint8_t b1 = (uint8_t) (sin(frequency * 2 * i + 4 * phase) * width + center);


      colors[0] = g0 << 16 | r0 << 8 | b0;
      colors[1] = g1 << 16 | r1 << 8 | b1;

      transmit(colors, 2);

      delay(20);
    }
  }
}
