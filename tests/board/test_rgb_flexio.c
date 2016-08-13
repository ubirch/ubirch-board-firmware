#include <fsl_port.h>
#include <fsl_flexio_uart.h>
#include <fsl_debug_console.h>
#include <ubirch/timer.h>
#include <arm_math.h>
#include <math.h>
#include "support.h"

#define WS2812B_PROT_CLOCK_FREQ    800000U
#define FLEXIO_SRC_CLOCK      kCLOCK_CoreSysClk
#define FLEXIO_SRC_CLOCK_FRQ  CLOCK_GetFreq(FLEXIO_SRC_CLOCK)
#define FLEXIO_CLOCK_DIVIDER  (FLEXIO_SRC_CLOCK_FRQ/WS2812B_PROT_CLOCK_FREQ)


void init_flexio() {
  CLOCK_SetFlexio0Clock(FLEXIO_SRC_CLOCK);
  CLOCK_EnableClock(kCLOCK_Flexio0);

  /* Reset FLEXIO before configuration. */
  FLEXIO_Reset(FLEXIO0);

  FLEXIO0->CTRL = 0;

  // shifter
  FLEXIO0->SHIFTCTL[0] = 0;
  FLEXIO0->SHIFTCFG[0] = FLEXIO_SHIFTCFG_INSRC(0) |
                         FLEXIO_SHIFTCFG_SSTOP(kFLEXIO_ShifterStopBitDisable) |
                         FLEXIO_SHIFTCFG_SSTART(kFLEXIO_ShifterStartBitDisabledLoadDataOnShift);
  FLEXIO0->SHIFTCTL[0] = FLEXIO_SHIFTCTL_TIMSEL(0) |
                         FLEXIO_SHIFTCTL_TIMPOL(kFLEXIO_ShifterTimerPolarityOnPositive) |
                         FLEXIO_SHIFTCTL_PINCFG(kFLEXIO_PinConfigOutput) |
                         FLEXIO_SHIFTCTL_PINSEL(18) |
                         FLEXIO_SHIFTCTL_PINPOL(kFLEXIO_PinActiveHigh) |
                         FLEXIO_SHIFTCTL_SMOD(kFLEXIO_ShifterModeTransmit);


  // clock generator
  FLEXIO0->TIMCTL[0] = 0;
  FLEXIO0->TIMCMP[0] = ((25 * 2 - 1) << 8) | ((FLEXIO_CLOCK_DIVIDER / 2) - 1);
  FLEXIO0->TIMCFG[0] = FLEXIO_TIMCFG_TIMOUT(kFLEXIO_TimerOutputZeroNotAffectedByReset) |
                       FLEXIO_TIMCFG_TIMDEC(kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput) |
                       FLEXIO_TIMCFG_TIMRST(kFLEXIO_TimerResetNever) |
                       FLEXIO_TIMCFG_TIMDIS(kFLEXIO_TimerDisableOnTimerCompare) |
                       FLEXIO_TIMCFG_TIMENA(kFLEXIO_TimerEnableOnTriggerHigh) |
                       FLEXIO_TIMCFG_TSTOP(kFLEXIO_TimerStopBitDisabled) |
                       FLEXIO_TIMCFG_TSTART(kFLEXIO_TimerStartBitDisabled);
  FLEXIO0->TIMCTL[0] = FLEXIO_TIMCTL_TRGSEL(0 << 2 | 0b01) | // shifter 0 status flag
                       FLEXIO_TIMCTL_TRGPOL(kFLEXIO_TimerTriggerPolarityActiveLow) |
                       FLEXIO_TIMCTL_TRGSRC(kFLEXIO_TimerTriggerSourceInternal) |
                       FLEXIO_TIMCTL_PINCFG(kFLEXIO_PinConfigOutput) |
                       FLEXIO_TIMCTL_PINSEL(19) |
                       FLEXIO_TIMCTL_PINPOL(kFLEXIO_PinActiveHigh) |
                       FLEXIO_TIMCTL_TIMOD(kFLEXIO_TimerModeDual8BitBaudBit);

  PRINTF("CLK = %x\r\n", USEC_TO_COUNT(0.375, FLEXIO_SRC_CLOCK_FRQ));
  uint32_t T0H = (uint32_t) (0.375 * FLEXIO_SRC_CLOCK_FRQ / 1000000U);
  uint32_t T0L = (uint32_t) (0.875 * FLEXIO_SRC_CLOCK_FRQ / 1000000U);
  uint32_t T1H = (uint32_t) (0.75 * FLEXIO_SRC_CLOCK_FRQ / 1000000U);
  uint32_t T1L = (uint32_t) (0.50 * FLEXIO_SRC_CLOCK_FRQ / 1000000U);

  PRINTF("T0H=%d T0L=%d T1H=%d T1L=%d\r\n", T0H, T0L, T1H, T1L);

  FLEXIO0->TIMCTL[1] = 0;
  FLEXIO0->TIMCMP[1] = ((T0L - 1) << 8) | (T0H - 1);
  PRINTF("T1 %04x\r\n", FLEXIO0->TIMCMP[1]);

  FLEXIO0->TIMCFG[1] = FLEXIO_TIMCFG_TIMOUT(kFLEXIO_TimerOutputOneNotAffectedByReset) |
                       FLEXIO_TIMCFG_TIMDEC(kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput) |
                       FLEXIO_TIMCFG_TIMRST(kFLEXIO_TimerResetNever) |
                       FLEXIO_TIMCFG_TIMDIS(kFLEXIO_TimerDisableOnTimerCompare) |
                       FLEXIO_TIMCFG_TIMENA(kFLEXIO_TimerEnableOnTriggerRisingEdge) |
                       FLEXIO_TIMCFG_TSTOP(kFLEXIO_TimerStopBitDisabled) |
                       FLEXIO_TIMCFG_TSTART(kFLEXIO_TimerStartBitDisabled);
  FLEXIO0->TIMCTL[1] = FLEXIO_TIMCTL_TRGSEL(9 << 2 | 0b10) | // pin 19 (9*2+1) from clock generator
                       FLEXIO_TIMCTL_TRGPOL(kFLEXIO_TimerTriggerPolarityActiveHigh) |
                       FLEXIO_TIMCTL_TRGSRC(kFLEXIO_TimerTriggerSourceInternal) |
                       FLEXIO_TIMCTL_PINCFG(kFLEXIO_PinConfigOutput) |
                       FLEXIO_TIMCTL_PINSEL(20) |
                       FLEXIO_TIMCTL_PINPOL(kFLEXIO_PinActiveHigh) |
                       FLEXIO_TIMCTL_TIMOD(kFLEXIO_TimerModeDual8BitPWM);

  FLEXIO0->TIMCTL[2] = 0;
  FLEXIO0->TIMCMP[2] = ((T1L - 1) << 8) | (T1H - 1);
  PRINTF("T2 %04x\r\n", FLEXIO0->TIMCMP[2]);

  FLEXIO0->TIMCFG[2] = FLEXIO_TIMCFG_TIMOUT(kFLEXIO_TimerOutputOneNotAffectedByReset) |
                       FLEXIO_TIMCFG_TIMDEC(kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput) |
                       FLEXIO_TIMCFG_TIMRST(kFLEXIO_TimerResetNever) |
                       FLEXIO_TIMCFG_TIMDIS(kFLEXIO_TimerDisableOnTriggerFallingEdge) |
                       FLEXIO_TIMCFG_TIMENA(kFLEXIO_TimerEnableOnTriggerRisingEdge) |
                       FLEXIO_TIMCFG_TSTOP(kFLEXIO_TimerStopBitDisabled) |
                       FLEXIO_TIMCFG_TSTART(kFLEXIO_TimerStartBitDisabled);
  FLEXIO0->TIMCTL[2] = FLEXIO_TIMCTL_TRGSEL(9 << 2 | 0b00) | // pin 18 (9*2) - from shifter
                       FLEXIO_TIMCTL_TRGPOL(kFLEXIO_TimerTriggerPolarityActiveHigh) |
                       FLEXIO_TIMCTL_TRGSRC(kFLEXIO_TimerTriggerSourceInternal) |
                       FLEXIO_TIMCTL_PINCFG(kFLEXIO_PinConfigOutput) |
                       FLEXIO_TIMCTL_PINSEL(20) |
                       FLEXIO_TIMCTL_PINPOL(kFLEXIO_PinActiveHigh) |
                       FLEXIO_TIMCTL_TIMOD(kFLEXIO_TimerModeDual8BitPWM);

  FLEXIO0->CTRL = FLEXIO_CTRL_FLEXEN(1);

}

void test_rgb_uart(void) {
  if (!yesno("RGB (UART) test?")) return;

  CLOCK_EnableClock(kCLOCK_PortA);
  /* Initialize FlexIO pins below */
  PORT_SetPinMux(PORTA, 12U, kPORT_MuxAlt5);
  PORT_SetPinMux(PORTA, 13U, kPORT_MuxAlt5);
  PORT_SetPinMux(PORTA, 14U, kPORT_MuxAlt5);


  init_flexio();

  int phase = 0;
  int length = 128;
  int center = 128, width = 127;
  float frequency = PI * 2 / length;

  while (true) {
    for (int i = 0; i < length; ++i) {
      uint8_t red = (uint8_t) (sin(frequency * i + 2 + phase) * width + center);
      uint8_t green = (uint8_t) (sin(frequency * i + 2 + phase) * width + center);
      uint8_t blue = (uint8_t) (sin(frequency * i + 4 + phase) * width + center);

      // BBRRBB00
      uint32_t color = blue << 24 | red << 16 | green << 8;
      while (FLEXIO0->TIMSTAT & FLEXIO_TIMSTAT_TSF(0)) {}
      FLEXIO0->TIMSTAT |= FLEXIO_TIMSTAT_TSF(0);
      FLEXIO0->SHIFTBUFBIS[0] = color;
      delay(50);
    }
  }
}
