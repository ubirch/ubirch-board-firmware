#include <fsl_port.h>
#include <fsl_gpio.h>
#include <fsl_debug_console.h>
#include <fsl_sai.h>
#include <ubirch/dbgutil.h>
#include <ubirch/timer.h>
#include "tosdcard.h"

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

static void SAI_ReadNonBlocking(I2S_Type *base, uint32_t channel, uint32_t bitWidth, uint8_t *buffer, uint32_t size)
{
  uint32_t i = 0;
  uint8_t j = 0;
  uint8_t bytesPerWord = bitWidth / 8U;
  uint32_t data = 0;

  for (i = 0; i < size / bytesPerWord; i++)
  {
    data = base->RDR[channel];
    for (j = 0; j < bytesPerWord; j++)
    {
      *buffer = (data >> (8U * j)) & 0xFF;
      buffer++;
    }
  }
}


void SAI_TxIRQHandler(I2S_Type *base, uint32_t channel, uint32_t bitWidth, uint8_t *buffer, uint32_t size)
{

  uint8_t i = 0;
  uint8_t j = 0;
  uint32_t data = 0;
  uint32_t temp = 0;

  /* Clear the FIFO error flag */
  if (SAI_TxGetStatusFlag(I2S0) & kSAI_FIFOErrorFlag)
  {
    SAI_TxClearStatusFlags(I2S0, kSAI_FIFOErrorFlag);
  }

  if (SAI_TxGetStatusFlag(I2S0) & kSAI_FIFOWarningFlag)
  {
    for (i = 0; i < FSL_FEATURE_SAI_FIFO_COUNT; i++)
    {
      SAI_ReadNonBlocking(I2S0, channel, bitWidth, buffer, size);
      data = 0;
      for (j = 0; j < format.bitWidth / 8U; j++)
      {
        temp = (uint32_t)(music[g_index]);
        data |= (temp << (8U * j));
        g_index++;
      }
      SAI_WriteData(I2S0, format.channel, data);
    }
  }

  if (g_index >= MUSIC_LEN)
  {
    isFinished = true;
    SAI_TxDisableInterrupts(I2S0, kSAI_FIFOWarningInterruptEnable | kSAI_FIFOErrorInterruptEnable);
    SAI_TxEnable(I2S0, false);
  }
}


status_t xSAI_TransferReceiveNonBlocking(I2S_Type *base, sai_handle_t *handle, sai_transfer_t *xfer)
{
  assert(handle);

  /* Check if the queue is full */
  if (handle->saiQueue[handle->queueUser].data)
  {
    return kStatus_SAI_QueueFull;
  }

  /* Add into queue */
  handle->transferSize[handle->queueUser] = xfer->dataSize;
  handle->saiQueue[handle->queueUser].data = xfer->data;
  handle->saiQueue[handle->queueUser].dataSize = xfer->dataSize;
  handle->queueUser = (handle->queueUser + 1) % SAI_XFER_QUEUE_SIZE;

  /* Set state to busy */
  handle->state = 0x0U;

/* Enable interrupt */
//#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
//  /* Use FIFO request interrupt and fifo error*/
  SAI_RxEnableInterrupts(base, kSAI_FIFOErrorInterruptEnable
                               | kSAI_FIFORequestInterruptEnable
                               | kSAI_WordStartInterruptEnable);
//#else
//  SAI_RxEnableInterrupts(base, kSAI_FIFOErrorInterruptEnable | kSAI_FIFOWarningInterruptEnable);
//#endif /* FSL_FEATURE_SAI_FIFO_COUNT */

  /* Enable Rx transfer */
  SAI_RxEnable(base, true);

  return kStatus_Success;
}


int main (void)
{
  board_init();
  board_console_init(BOARD_DEBUG_BAUD);

  sai_init(config);

  init_sdhc_pins();

  sai_handle_t *handle = &rxHandle;

  uint8_t buffer[3 * 2048]; // = handle->saiQueue[handle->queueDriver].data;
  uint8_t dataSize = handle->bitWidth / 8U;
  uint8_t the_size =0;
//  while(true)
//  {
  PRINTF("Listening...\r\n");

  xfer.data = (uint8_t *) temp;
  xfer.dataSize = sizeof(temp);

//  GPIO_WritePinOutput(GPIOA, 18U, true);

  PRINTF("%x\r\n", xSAI_TransferReceiveNonBlocking(I2S0, &rxHandle, &xfer));

  while (isFinished != true) {}

  if (I2S0->RCSR & I2S_RCSR_FWF_MASK)
  {
    uint8_t size = MIN((handle->saiQueue[handle->queueDriver].dataSize), dataSize);
    PRINTF("first size is %d\r\n", size);
    SAI_ReadNonBlocking(I2S0, handle->channel, handle->bitWidth, buffer, size);

    the_size = size;
    /* Update internal state */
    handle->saiQueue[handle->queueDriver].dataSize -= size;
    handle->saiQueue[handle->queueDriver].data += size;
  }

//  GPIO_WritePinOutput(GPIOA, 18U, false);


  PRINTF("FINISHED.\r\n");


  PRINTF("Size is %d\r\n", the_size);
//  dbg_xxd("AUDIO", xfer.data, xfer.dataSize);
  dbg_xxd("AUDIO", buffer, the_size);
  PRINTF("Done \r\n\r\n\r\n");

  PRINTF("Now copying stuff into SD card\r\n");
  pcm__sdhc(buffer);

  while(true)
  {
    delay(10000);
  }

  return 0;
}
