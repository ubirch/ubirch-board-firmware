#include <fsl_port.h>
#include <fsl_gpio.h>
#include <fsl_debug_console.h>
#include <fsl_sai.h>
#include <ubirch/dbgutil.h>
#include <ubirch/timer.h>
#include "tosdcard.h"

#define RX_SAI_IRQ I2S0_Rx_IRQn
#define new_I2S0_Rx_DriverIRQHandler I2S0_Rx_IRQHandler

#ifndef FSL_FEATURE_SAI_FIFO_COUNT
  #define FSL_FEATURE_SAI_FIFO_COUNT
#endif
bool isFinished = false;

sai_handle_t rxHandle = {0};

sai_config_t config;
sai_transfer_format_t format;

static uint32_t temp[3 * 1024];

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
  SAI_TransferRxSetFormat(I2S0, &rxHandle, &format, CLOCK_GetCoreSysClkFreq(), format.masterClockHz);
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

void SAI_Transfer_Rx_Handle_IRQ(I2S_Type *base, sai_handle_t *handle)
{
  assert(handle);

  uint8_t *buffer = handle->saiQueue[handle->queueDriver].data;
  uint8_t dataSize = handle->bitWidth / 8U;

  /* Handle Error */
  if (base->RCSR & I2S_RCSR_FEF_MASK)
  {
    /* Clear FIFO error flag to continue transfer */
    SAI_RxClearStatusFlags(base, kSAI_FIFOErrorFlag);
    PRINTF("Clear the FIFO Error flag\r\n");
    /* Call the callback */
    if (handle->callback)
    {
      (handle->callback)(base, handle, kStatus_SAI_RxError, handle->userData);
    }
  }

/* Handle transfer */
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
  if (base->RCSR & I2S_RCSR_FRF_MASK)
  {
    PRINTF("Handle the transfer FIFO_Count\r\n");
    /* Judge if the data need to transmit is less than space */
    uint8_t size = MIN((handle->saiQueue[handle->queueDriver].dataSize), (handle->watermark * dataSize));

    /* Copy the data from sai buffer to FIFO */
    SAI_ReadNonBlocking(base, handle->channel, handle->bitWidth, buffer, size);

    /* Update the internal counter */
    handle->saiQueue[handle->queueDriver].dataSize -= size;
    handle->saiQueue[handle->queueDriver].data += size;
  }
#else
  if (base->RCSR & I2S_RCSR_FWF_MASK)
  {
    PRINTF("Handle the transfer No FIFO count\r\n");

    uint8_t size = MIN((handle->saiQueue[handle->queueDriver].dataSize), dataSize);

    SAI_ReadNonBlocking(base, handle->channel, handle->bitWidth, buffer, size);

    /* Update internal state */
    handle->saiQueue[handle->queueDriver].dataSize -= size;
    handle->saiQueue[handle->queueDriver].data += size;
  }
#endif /* FSL_FEATURE_SAI_FIFO_COUNT */

  /* If finished a blcok, call the callback function */
  if (handle->saiQueue[handle->queueDriver].dataSize == 0U)
  {
    PRINTF("Block transfer is over\r\n");

    memset(&handle->saiQueue[handle->queueDriver], 0, sizeof(sai_transfer_t));
    handle->queueDriver = (handle->queueDriver + 1) % SAI_XFER_QUEUE_SIZE;
    if (handle->callback)
    {
      (handle->callback)(base, handle, kStatus_SAI_RxIdle, handle->userData);
    }
  }

  /* If all data finished, just stop the transfer */
  if (handle->saiQueue[handle->queueDriver].data == NULL)
  {
    SAI_TransferAbortReceive(base, handle);
  }
  PRINTF("Lets copy buffer to temp\r\n");
  memcpy(temp, buffer, sizeof(temp));
}

void new_I2S0_Rx_DriverIRQHandler(void)
{
  assert(&rxHandle);
  SAI_Transfer_Rx_Handle_IRQ(I2S0, &rxHandle);
}

int main (void)
{
  size_t count = 0;
  uint8_t the_size =0;

  sai_transfer_t xfer;

  xfer.data = (uint8_t *)temp;
  xfer.dataSize = sizeof(temp);

  board_init();
  board_console_init(BOARD_DEBUG_BAUD);

  sai_init(config);

  PRINTF("Listening...\r\n");

  EnableIRQ(RX_SAI_IRQ);

  PRINTF("SAI non-blocking status: %d\r\n", SAI_TransferReceiveNonBlocking(I2S0, &rxHandle, &xfer));
//  SAI_RxEnableInterrupts(I2S0, kSAI_FIFOErrorInterruptEnable | kSAI_FIFORequestInterruptEnable);
//  SAI_RxEnable(I2S0, true);

  GPIO_WritePinOutput(GPIOA, 18U, true);
  delay(1000);

  while (isFinished != true) {
//    uint32_t hex_val = SAI_RxGetStatusFlag(I2S0);
//    PRINTF("The status is %x :: %u\r\n", hex_val, hex_val);
  }
  GPIO_WritePinOutput(GPIOA, 18U, false);

  PRINTF("Previous status     : %d\r\n", SAI_TransferGetReceiveCount(I2S0, &rxHandle, &count));
  PRINTF("Received data count : %d\r\n", count);
  PRINTF("FINISHED.\r\n");
  PRINTF("Size is %d\r\n", the_size);
  dbg_xxd("AUDIO", (uint8_t *)temp, the_size);

  while(true)
  {
    delay(10000);
  }
  return 0;
}
