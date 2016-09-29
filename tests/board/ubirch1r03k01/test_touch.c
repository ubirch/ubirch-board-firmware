#include <stdint.h>
#include <fsl_gpio.h>
#include <fsl_port.h>
#include <fsl_tsi_v4.h>
#include <fsl_debug_console.h>
#include "support.h"
#include <fsl_lptmr.h>

tsi_calibration_data_t buffer;

/* Get source clock for LPTMR driver */
#define LPTMR_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_LpoClk)

#define BOARD_TSI_ELECTRODE_1 0U
#define BOARD_TSI_ELECTRODE_2 10U

#define PAD_TSI_ELECTRODE_1_NAME "SENSE_3"
#define PAD_TSI_ELECTRODE_2_NAME "SENSE_2"

volatile bool touched = false;

void TSI0_IRQHandler(void) {
  /* Clear flags */
  TSI_ClearStatusFlags(TSI0, kTSI_EndOfScanFlag);
  TSI_ClearStatusFlags(TSI0, kTSI_OutOfRangeFlag);

  if (TSI_GetMeasuredChannelNumber(TSI0) == BOARD_TSI_ELECTRODE_1) {
    if (TSI_GetCounter(TSI0) > (buffer.calibratedData[BOARD_TSI_ELECTRODE_1] + 100U)) {
      touched = true;
      PRINTF("TOUCH");
    }
    PRINTF("\e[KChannel %d (%d)\r", BOARD_TSI_ELECTRODE_1, TSI_GetCounter(TSI0));
  }

  if (TSI_GetMeasuredChannelNumber(TSI0) == BOARD_TSI_ELECTRODE_2) {
    if (TSI_GetCounter(TSI0) > (buffer.calibratedData[BOARD_TSI_ELECTRODE_2] + 100U)) {
      touched = true;
      PRINTF("TOUCH");
    }
    PRINTF("\e[KChannel %d (%d)\r", BOARD_TSI_ELECTRODE_2, TSI_GetCounter(TSI0));
  }
}


void test_touch() {
  if (!yesno("Touch test?")) return;

  /* Initialize Touch pad pins below */
  /* Ungate the port clock */
  CLOCK_EnableClock(kCLOCK_PortB);
  /* Affects PORTB_PCR18 register */
  PORT_SetPinMux(PORTB, 0U, kPORT_PinDisabledOrAnalog);
  /* Affects PORTB_PCR19 register */
  PORT_SetPinMux(PORTB, 17U, kPORT_PinDisabledOrAnalog);


  volatile uint32_t i = 0;
  tsi_config_t tsiConfig_normal = {0};
  lptmr_config_t lptmrConfig;
  memset((void *) &lptmrConfig, 0, sizeof(lptmrConfig));

  LPTMR_GetDefaultConfig(&lptmrConfig);

  /* TSI default hardware configuration for normal mode */
  TSI_GetNormalModeDefaultConfig(&tsiConfig_normal);

  /* Initialize the LPTMR */
  LPTMR_Init(LPTMR0, &lptmrConfig);
  /* Initialize the TSI */
  TSI_Init(TSI0, &tsiConfig_normal);
  //TSI_SetNumberOfScans(TSI0,5);


  //TSI_SetLowThreshold(TSI0, 3900);
  TSI_SetElectrodeChargeCurrent(TSI0,kTSI_ExtOscChargeCurrent_1uA);
  TSI_SetElectrodeSeriesResistor(TSI0,kTSI_SeriesResistance_32k);
  TSI_SetNumberOfScans(TSI0,kTSI_ConsecutiveScansNumber_32time);
  /* Set timer period */
  LPTMR_SetTimerPeriod(LPTMR0, (uint16_t) USEC_TO_COUNT(500000U, LPTMR_SOURCE_CLOCK));

  NVIC_EnableIRQ(TSI0_IRQn);
  TSI_EnableModule(TSI0, true); /* Enable module */

  PRINTF("\r\nTSI_V4 Normal_mode Example Start!\r\n");

  memset((void *) &buffer, 0, sizeof(buffer));
  TSI_Calibrate(TSI0, &buffer);

  /* Print calibrated counter values */
  for (i = 0U; i < FSL_FEATURE_TSI_CHANNEL_COUNT; i++) {
    PRINTF("Calibrated counters for channel %d is: %d \r\n", i, buffer.calibratedData[i]);
  }

  /********** TEST HARDWARE TRIGGER SCAN ********/
  PRINTF("\r\nNOW, Test the hardware trigger scan method!\r\n");
  PRINTF("After running, touch pad %s, you will see LED shines.\r\n", PAD_TSI_ELECTRODE_1_NAME);
  TSI_EnableModule(TSI0, false);
  TSI_EnableHardwareTriggerScan(TSI0, true);
  TSI_EnableInterrupts(TSI0, kTSI_GlobalInterruptEnable);
  TSI_EnableInterrupts(TSI0, kTSI_EndOfScanInterruptEnable);
  TSI_ClearStatusFlags(TSI0, kTSI_EndOfScanFlag);

  TSI_SetMeasuredChannelNumber(TSI0, BOARD_TSI_ELECTRODE_2); /* Select TSI_CHANNEL_1 as detecting electrode. */
  TSI_EnableModule(TSI0, true);
  LPTMR_StartTimer(LPTMR0); /* Start LPTMR triggering */

  while(!touched);
}
