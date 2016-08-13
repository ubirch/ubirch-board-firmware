#include <ubirch/dbgutil.h>
#include <fsl_port.h>
#include <fsl_gpio.h>
#include <fsl_debug_console.h>
#include <fsl_sai.h>
#include <fsl_sai_edma.h>
#include <ubirch/timer.h>

bool isFinished = false;
uint32_t temp[3 * 2048];
sai_handle_t rxHandle = {0};

static void callback(I2S_Type *base, sai_handle_t *handle, status_t status, void *userData) {
  isFinished = true;
}

void test_i2s(void) {
  sai_config_t config;
  sai_transfer_format_t format;
  sai_transfer_t xfer;

  PRINTF("Enable I2S pins...\r\n");
  CLOCK_EnableClock(kCLOCK_PortA);

  // DM_DO (Data) and // DM_WS (FS, Word/Frame Sync)
  port_pin_config_t pin_config = {0};
  pin_config.pullSelect = kPORT_PullDown;
  pin_config.mux = kPORT_MuxAlt6;
  PORT_SetPinConfig(PORTA, 15U, &pin_config);
  PORT_SetPinConfig(PORTA, 16U, &pin_config);

  PORT_SetPinMux(PORTA, 17U, kPORT_MuxAlt6);    // DM_CLK (MCLK/BCLK)
  PORT_SetPinMux(PORTA, 18U, kPORT_MuxAsGpio);  // DM_SEL

  const gpio_pin_config_t OUT = {kGPIO_DigitalOutput, 0};
  GPIO_PinInit(GPIOA, 18U, &OUT);

  PRINTF("Configuring SAI...\r\n");
  SAI_RxGetDefaultConfig(&config);
  config.protocol = kSAI_BusI2S;
  SAI_RxInit(I2S0, &config);

  /* Configure the audio format */
  format.bitWidth = kSAI_WordWidth24bits;
  format.channel = 0U;
  format.sampleRate_Hz = kSAI_SampleRate32KHz;
  format.masterClockHz = 64U * format.sampleRate_Hz;
  format.protocol = config.protocol;
  format.stereo = kSAI_Stereo;
  format.watermark = FSL_FEATURE_SAI_FIFO_COUNT / 2U;

  PRINTF("Listening...\r\n");
  GPIO_WritePinOutput(GPIOA, 18U, true);

  SAI_TransferRxCreateHandle(I2S0, &rxHandle, callback, NULL);
  SAI_TransferRxSetFormat(I2S0, &rxHandle, &format, CLOCK_GetCoreSysClkFreq(), format.masterClockHz);
  xfer.data = (uint8_t *) temp;
  xfer.dataSize = sizeof(temp);
  PRINTF("%x\r\n", SAI_TransferReceiveNonBlocking(I2S0, &rxHandle, &xfer));

  while (isFinished != true) {}

  GPIO_WritePinOutput(GPIOA, 18U, false);
  PRINTF("FINISHED.\r\n");

  dbg_xxd("AUDIO", (const uint8_t *) temp, xfer.dataSize);
}
