#include <fsl_lptmr.h>
#include <fsl_ftm.h>
#include <fsl_debug_console.h>
#include <fsl_port.h>
#include <ubirch/timer.h>
#include <fsl_edma.h>
#include <fsl_dmamux.h>
#include <malloc.h>
#include <ubirch/dbgutil.h>
#include <MK82F25615.h>
#include <fsl_tsi_v4.h>
#include "support.h"


#define TCH_ELECTRODE   0U
#define FTM_CHANNELS    3
#define RGB_LEDS        2
#define RGB_COLOR_BITS  24

#define OUT1  ((1U << 5))

static uint8_t BUFF[RGB_LEDS * RGB_COLOR_BITS] = {
  // RGB LED 1
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // GREEN
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // RED
  OUT1, OUT1, OUT1, OUT1, OUT1, OUT1, OUT1, OUT1, // BLUE
  // RGB LED 2
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // GREEN
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // RED
  OUT1, OUT1, OUT1, OUT1, OUT1, OUT1, OUT1, OUT1, // BLUE
};

volatile static bool dma_done = false;
static const uint8_t ONE = OUT1;

void EDMA_Callback(edma_handle_t *handle, void *userData, bool transferDone, uint32_t tcds) {
  (void) userData;
  (void) tcds;

  dma_done = handle->channel == 2 && transferDone;
}

void ftm_init(void) {
  ftm_config_t ftm_config;
  ftm_chnl_pwm_signal_param_t ftm_chnl_config[FTM_CHANNELS];

  CLOCK_EnableClock(kCLOCK_PortC);
  PORT_SetPinMux(PORTC, 1U, kPORT_MuxAlt4);
  PORT_SetPinMux(PORTC, 2U, kPORT_MuxAlt4);
  PORT_SetPinMux(PORTC, 3U, kPORT_MuxAlt4);

  ftm_chnl_config[0].chnlNumber = kFTM_Chnl_0;
  ftm_chnl_config[0].level = kFTM_HighTrue;
  ftm_chnl_config[0].dutyCyclePercent = 21;
  ftm_chnl_config[0].firstEdgeDelayPercent = 0U;

  ftm_chnl_config[1].chnlNumber = kFTM_Chnl_1;
  ftm_chnl_config[1].level = kFTM_HighTrue;
  ftm_chnl_config[1].dutyCyclePercent = 56;
  ftm_chnl_config[1].firstEdgeDelayPercent = 0U;

  ftm_chnl_config[2].chnlNumber = kFTM_Chnl_2;
  ftm_chnl_config[2].level = kFTM_HighTrue;
  ftm_chnl_config[2].dutyCyclePercent = 86;
  ftm_chnl_config[2].firstEdgeDelayPercent = 0U;

  // initialize FTM
  EnableIRQ(FTM0_IRQn);
  FTM_GetDefaultConfig(&ftm_config);

  FTM_Init(FTM0, &ftm_config);
  FTM_SetWriteProtection(FTM0, false);
  FTM_DisableInterrupts(FTM0, kFTM_TimeOverflowInterruptEnable | kFTM_FaultInterruptEnable);
  FTM_EnableInterrupts(FTM0, kFTM_Chnl0InterruptEnable | kFTM_Chnl1InterruptEnable | kFTM_Chnl2InterruptEnable);
  FTM0->CONTROLS[kFTM_Chnl_0].CnSC |= FTM_CnSC_DMA(1);
  FTM0->CONTROLS[kFTM_Chnl_1].CnSC |= FTM_CnSC_DMA(1);
  FTM0->CONTROLS[kFTM_Chnl_2].CnSC |= FTM_CnSC_DMA(1);

  FTM_SetupPwm(FTM0, ftm_chnl_config, FTM_CHANNELS, kFTM_EdgeAlignedPwm, 1600000U, CLOCK_GetFreq(kCLOCK_CoreSysClk));
}

void ftm_reset(void) {

}

void dma_init() {
  edma_config_t edma_config;

  // initialize EDMA
  EDMA_GetDefaultConfig(&edma_config);
  edma_config.enableHaltOnError = false;
  edma_config.enableRoundRobinArbitration = true;
  EDMA_Init(DMA0, &edma_config);

  DMAMUX_Init(DMAMUX0);
  for (uint32_t c = 0; c < 3; c++) {
    DMAMUX_EnableChannel(DMAMUX0, c);
    DMAMUX_SetSource(DMAMUX0, c, (uint8_t) (kDmaRequestMux0FTM0Channel0 + c));
    EDMA_SetModulo(DMA0, c, kEDMA_ModuloDisable, kEDMA_ModuloDisable);
  }
}


void cycle_colors(uint8_t *buffer) {
  uint8_t tmp[8];

  // store first row
  memcpy(tmp, buffer, 8);
  memcpy(buffer, buffer + 8, 16);
  memcpy(buffer + 16, tmp, 8);
}

void switch_color() {
  edma_transfer_config_t transfer[3];
  memset(&transfer, 0, sizeof(transfer));

  edma_tcd_t __aligned(32) tcd0;
  edma_tcd_t __aligned(32) tcd1;
  edma_tcd_t __aligned(32) tcd2;

  edma_handle_t handle[3];
  memset(handle, 0, sizeof(handle));

  EDMA_InstallTCDMemory(&handle[0], &tcd0, 1);
  EDMA_InstallTCDMemory(&handle[1], &tcd1, 1);
  EDMA_InstallTCDMemory(&handle[2], &tcd2, 1);

  for(uint32_t c = 0; c < 3; c++) {
    EDMA_CreateHandle(&handle[c], DMA0, c);
  }

  EDMA_PrepareTransfer(&transfer[0], (void *) &ONE, 1, (void *) &(GPIOD->PSOR), 1, 1,  RGB_COLOR_BITS, kEDMA_MemoryToPeripheral);
  EDMA_PrepareTransfer(&transfer[1], (void *) BUFF, 1, (void *) &(GPIOD->PDOR), 1, 1,  RGB_COLOR_BITS, kEDMA_MemoryToPeripheral);
  EDMA_PrepareTransfer(&transfer[2], (void *) &ONE, 1, (void *) &(GPIOD->PCOR), 1, 1,  RGB_COLOR_BITS, kEDMA_MemoryToPeripheral);

  transfer[0].srcOffset = 0;
  transfer[2].srcOffset = 0;
  EDMA_SubmitTransfer(&handle[0], &transfer[0]);
  EDMA_SubmitTransfer(&handle[1], &transfer[1]);
  EDMA_SetCallback(&handle[2], EDMA_Callback, NULL);
  EDMA_SubmitTransfer(&handle[2], &transfer[2]);

  for(uint32_t c = 0; c < 3; c++) {
    EDMA_StartTransfer(&handle[c]);
  }

  dma_done = false;


  FTM0->CNT = 0;
  FTM_ClearStatusFlags(FTM0, kFTM_TimeOverflowFlag);
  FTM_StartTimer(FTM0, kFTM_SystemClock);
  do {} while (!dma_done);
  FTM_StopTimer(FTM0);
  FTM_ClearStatusFlags(FTM0, kFTM_TimeOverflowFlag);

  for (uint32_t c = 0; c < 3; c++) {
    EDMA_StopTransfer(&handle[c]);
    EDMA_AbortTransfer(&handle[c]);
  }
}

tsi_calibration_data_t buffer;
volatile bool touched = false;

void TSI0_IRQHandler(void) {
  /* Clear flags */
  TSI_ClearStatusFlags(TSI0, kTSI_EndOfScanFlag);
  TSI_ClearStatusFlags(TSI0, kTSI_OutOfRangeFlag);

  if (TSI_GetMeasuredChannelNumber(TSI0) == TCH_ELECTRODE) {
    if (TSI_GetCounter(TSI0) > (buffer.calibratedData[TCH_ELECTRODE] + 100U)) {
      touched = true;
    }
  }
}

void touch_init() {
  CLOCK_EnableClock(kCLOCK_PortB);
  PORT_SetPinMux(PORTB, 0U, kPORT_PinDisabledOrAnalog);

  tsi_config_t tsiConfig_normal = {0};
  lptmr_config_t lptmrConfig;
  memset((void *) &lptmrConfig, 0, sizeof(lptmrConfig));

  TSI_GetNormalModeDefaultConfig(&tsiConfig_normal);
  TSI_Init(TSI0, &tsiConfig_normal);
  //TSI_SetNumberOfScans(TSI0,5);
  //TSI_SetLowThreshold(TSI0, 3900);
  TSI_SetElectrodeChargeCurrent(TSI0, kTSI_ExtOscChargeCurrent_1uA);
  TSI_SetElectrodeSeriesResistor(TSI0, kTSI_SeriesResistance_32k);
  TSI_SetNumberOfScans(TSI0, kTSI_ConsecutiveScansNumber_32time);

  NVIC_EnableIRQ(TSI0_IRQn);
  TSI_EnableModule(TSI0, true); /* Enable module */

  memset((void *) &buffer, 0, sizeof(buffer));
  TSI_Calibrate(TSI0, &buffer);

  TSI_EnableModule(TSI0, false);
  TSI_EnableHardwareTriggerScan(TSI0, true);
  TSI_EnableInterrupts(TSI0, kTSI_GlobalInterruptEnable);
  TSI_EnableInterrupts(TSI0, kTSI_EndOfScanInterruptEnable);
  TSI_ClearStatusFlags(TSI0, kTSI_EndOfScanFlag);

  TSI_SetMeasuredChannelNumber(TSI0, TCH_ELECTRODE);
  TSI_EnableModule(TSI0, true);

  LPTMR_GetDefaultConfig(&lptmrConfig);
  LPTMR_Init(LPTMR0, &lptmrConfig);
  LPTMR_SetTimerPeriod(LPTMR0, (uint16_t) USEC_TO_COUNT(500000U, CLOCK_GetFreq(kCLOCK_LpoClk)));
  LPTMR_StartTimer(LPTMR0);
}


void test_rgb(void) {
  if (!yesno("RGB test?")) return;

  // initialize RGB LED output port
  CLOCK_EnableClock(kCLOCK_PortD);
  PORT_SetPinMux(PORTD, 5U, kPORT_MuxAsGpio);
  gpio_pin_config_t OUTPUT = {kGPIO_DigitalOutput, false};
  GPIO_PinInit(GPIOD, 5U, &OUTPUT);

  touch_init();

  while (true) {
    cycle_colors(BUFF);
    cycle_colors(BUFF + 24);

    ftm_init();
    dma_init();
    switch_color();
    DMAMUX_Deinit(DMAMUX0);
    EDMA_Deinit(DMA0);
    FTM_Deinit(FTM0);

    while (!touched) { delay(1); }
    touched = false;
  }
}
