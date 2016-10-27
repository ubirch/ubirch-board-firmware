#include <fsl_port.h>
#include <fsl_gpio.h>
#include <fsl_debug_console.h>
#include <fsl_sai.h>
#include <ubirch/dbgutil.h>
#include <ubirch/timer.h>
#include "tosdcard.h"

#define RX_SAI_IRQ I2S0_Rx_IRQn
#define SAI_RxIRQHandler I2S0_Rx_IRQHandler

bool isFinished = false;
uint32_t temp[2000];
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
}


/* Initialize the SAI / I2S interface */
void sai_init(sai_config_t config)
{
  PRINTF("Enable I2S pins...\r\n");

  CLOCK_EnableClock(kCLOCK_PortA);

  // DM_DO (Data) and // DM_WS (FS, Word/Frame Sync)
  port_pin_config_t pin_config = {0};

  pin_config.pullSelect = kPORT_PullDown;
  pin_config.mux = kPORT_MuxAlt6;

  PORT_SetPinConfig(PORTA, 15U, &pin_config);  //DO
  PORT_SetPinConfig(PORTA, 16U, &pin_config);  //WS

  PORT_SetPinMux(PORTA, 17U, kPORT_MuxAlt6);  // DM_CLK (MCLK/BCLK)

  PORT_SetPinMux(PORTA, 18U, kPORT_MuxAsGpio);  // DM_SEL

  const gpio_pin_config_t OUT = {kGPIO_DigitalOutput, 0};
  GPIO_PinInit(GPIOA, 18U, &OUT);

  PRINTF("Configuring SAI...\r\n");

  SAI_RxGetDefaultConfig(&config);
  config.protocol = kSAI_BusI2S;
  config.syncMode = kSAI_ModeSync;
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
  SAI_TransferRxSetFormat(I2S0, &rxHandle, &format, CLOCK_GetCoreSysClkFreq(), format.masterClockHz);
}

void error(char *msg) {
  PRINTF("ERROR: %s\r\n", msg);
  while(true) {}
}

/* The IRQ handler to read the messages*/
void SAI_RxIRQHandler(I2S_Type *base, uint32_t channel, uint32_t bitWidth, uint32_t *buffer, uint32_t size)
{
  uint8_t i = 0;
  uint8_t j = 0;
  uint32_t data = 0;
  uint8_t bytesPerWord = bitWidth / 8U;

  size_t g_index = 0;

  PRINTF("IRQ handler\r\n");
  /* Clear the FIFO error flag */
  if (SAI_RxGetStatusFlag(I2S0) & kSAI_FIFOErrorFlag)
  {
    SAI_RxClearStatusFlags(I2S0, kSAI_FIFOErrorFlag);
  }

  if (SAI_RxGetStatusFlag(I2S0) & kSAI_FIFOWarningFlag)
  {
    for (i = 0; i < FSL_FEATURE_SAI_FIFO_COUNT; i++)
    {
      for (i = 0; i < size / bytesPerWord; i++)
      {
        data = base->RDR[channel];
        for (j = 0; j < bytesPerWord; j++)
        {
          *buffer = (data >> (8U * j)) & 0xFF;
          buffer++;
          g_index++;
        }
      }
    }
  }

  isFinished = true;

  if (g_index >= size)
  {
    isFinished = true;
    SAI_RxDisableInterrupts(I2S0, kSAI_FIFOWarningInterruptEnable | kSAI_FIFOErrorInterruptEnable);
    SAI_RxEnable(I2S0, false);
  }
}

int main (void)
{
  board_init();
  board_console_init(BOARD_DEBUG_BAUD);

  sai_init(config);

  init_sdhc_pins();

  uint32_t buffer[3 * 2048]; // = handle->saiQueue[handle->queueDriver].data;
//  uint8_t dataSize = rxHandle.bitWidth / 8U;
  uint8_t the_size =0;

  PRINTF("Listening...\r\n");

  xfer.data = (uint8_t *) temp;
  xfer.dataSize = sizeof(temp);

  GPIO_WritePinOutput(GPIOA, 18U, true);

  if (rxHandle.saiQueue[rxHandle.queueUser].data)
  {
    PRINTF("Returning %d\r\n", kStatus_SAI_QueueFull);
    return kStatus_SAI_QueueFull;
  }

  EnableIRQ(RX_SAI_IRQ);
  SAI_RxEnableInterrupts(I2S0, kSAI_FIFOWarningInterruptEnable);
//  kSAI_WordStartInterruptEnable); //   |
//                               kSAI_FIFORequestInterruptEnable |
//                               kSAI_FIFOErrorInterruptEnable   |
//                               kSAI_FIFOWarningInterruptEnable);
  SAI_RxEnable(I2S0, true);

  while (isFinished != true) {
    uint32_t hex_val = SAI_RxGetStatusFlag(I2S0);
    PRINTF("The status %x  %u ", hex_val, hex_val);
  }

  GPIO_WritePinOutput(GPIOA, 18U, false);

  PRINTF("FINISHED.\r\n");
  PRINTF("Size is %d\r\n", the_size);
  dbg_xxd("AUDIO", (uint8_t *)buffer, the_size);

  PRINTF("Now copying stuff into SD card\r\n");
  pcm__sdhc((uint8_t *)buffer);

  while(true)
  {
    delay(10000);
  }

  return 0;
}
