#include <fsl_port.h>
#include <fsl_gpio.h>
#include <fsl_debug_console.h>
#include <fsl_sai.h>
#include <ubirch/dbgutil.h>
#include <ubirch/timer.h>
#include <ubirch/ws2812b.h>
#include "tosdcard.h"

bool isFinished = false;
uint32_t temp[3 * 2048];
uint8_t buffer_write[255];
sai_handle_t rxHandle = {0};

static sai_config_t config;
sai_transfer_format_t format;
sai_transfer_t xfer;

static void callback(I2S_Type *base, sai_handle_t *handle, status_t status, void *userData)
{
  if (kStatus_SAI_RxIdle == status)
  {
    isFinished = true;
  }
  else
  {
    PRINTF("STATUS is %x\r\n", status);
  }
}

void sai_init(sai_config_t config)
{
  PRINTF("Enable I2S pins...\r\n");
  CLOCK_EnableClock(kCLOCK_PortA);

  const gpio_pin_config_t OUTTRUE = {kGPIO_DigitalOutput, true};
  const gpio_pin_config_t IN = {kGPIO_DigitalInput, false};

  // DM_DO (Data) and // DM_WS (FS, Word/Frame Sync)
  port_pin_config_t pin_config = {0};

  pin_config.pullSelect = kPORT_PullDown;
  pin_config.mux = kPORT_MuxAlt6;

  PORT_SetPinConfig(PORTA, 15U, &pin_config);
  PORT_SetPinConfig(PORTA, 16U, &pin_config);

//  PORT_SetPinMux(PORTA, 16U, kPORT_MuxAlt6);    // WS
  PORT_SetPinMux(PORTA, 15U, kPORT_MuxAlt6);    // DM_DO
  PORT_SetPinMux(PORTA, 16U, kPORT_MuxAlt6);  // DM_WS
  PORT_SetPinMux(PORTA, 17U, kPORT_MuxAlt6);  // DM_CLK (MCLK/BCLK)

  PORT_SetPinMux(PORTA, 18U, kPORT_MuxAsGpio);  // DM_SEL
  GPIO_PinInit(BOARD_CELL_PIN_GPIO, BOARD_CELL_STATUS_PIN, &IN);


  const gpio_pin_config_t OUT = {kGPIO_DigitalOutput, 0};
//  GPIO_PinInit(GPIOA, 16U, &OUT);
  GPIO_PinInit(GPIOA, 18U, &OUT);

  PRINTF("Configuring SAI...\r\n");

  SAI_RxGetDefaultConfig(&config);
  config.protocol = kSAI_BusI2S;
  config.syncMode = kSAI_ModeAsync;
  SAI_RxInit(I2S0, &config);

  /* Configure the audio format */
  format.bitWidth = kSAI_WordWidth24bits;
  format.channel = 0U;
  format.sampleRate_Hz = kSAI_SampleRate32KHz;
  format.masterClockHz = 64U * format.sampleRate_Hz;
  format.protocol = config.protocol;
  format.stereo = kSAI_Stereo;
  format.watermark = FSL_FEATURE_SAI_FIFO_COUNT / 2U;

  SAI_TransferRxCreateHandle(I2S0, &rxHandle, callback, NULL);
//  SAI_RxSetFormat
  SAI_TransferRxSetFormat(I2S0, &rxHandle, &format, CLOCK_GetCoreSysClkFreq(), format.masterClockHz);
}

bool on = true;
volatile uint32_t milliseconds = 0;

void SysTick_Handler() {
  milliseconds++;
  if (milliseconds % 1000 == 0) on = !on;
  BOARD_LED0(on);
}

void error(char *msg) {
  PRINTF("ERROR: %s\r\n", msg);
  while(true) {}
}

int main (void)
{
  board_init();
  board_console_init(BOARD_DEBUG_BAUD);

//  delay(1000);
//  ws2812b_send(0x000011, 2);
//  delay(1000);

  SysTick_Config(BOARD_SYSTICK_100MS / 10);

//  init_sdhc_pins();

  sai_init(config);

  PRINTF("Listening...\r\n");
  GPIO_WritePinOutput(GPIOA, 18U, true);

//  delay(2000);
//  GPIO_WritePinOutput(GPIOA, 16U, true);
//  delay(2000);

  xfer.data = (uint8_t *) temp;
  xfer.dataSize = sizeof(temp);
//  xfer.data = buffer_write;
//  xfer.dataSize = sizeof(buffer_write);
//  PRINTF("FINISHED.\r\nDATA size: %d", xfer.dataSize);



//  PRINTF("%x\r\n", SAI_TransferReceiveNonBlocking(I2S0, &rxHandle, &xfer));
  SAI_ReadBlocking(I2S0, 0, kSAI_WordWidth24bits, xfer.data, xfer.dataSize);
  dbg_xxd("AUDIO", xfer.data, xfer.dataSize);
//  pcm__sdhc(buffer_write);
//  delay(2000);

//  GPIO_WritePinOutput(GPIOA, 16U, false);
  GPIO_WritePinOutput(GPIOA, 18U, false);

  while (isFinished != true) {}

  PRINTF("FINISHED.\r\n"); //DATA size: %d\r\n", xfer.dataSize);
  PRINTF("DATA is %x\r\n", buffer_write[2]);
  dbg_xxd("AUDIO", buffer_write, xfer.dataSize);

  while(true)
  {
    delay(10000);
  }

  return 0;
}
