//
// Created by dothraki on 9/19/16.
//

#ifndef UBIRCH_FIRMWARE_UART2_H
#define UBIRCH_FIRMWARE_UART2_H

#include <drivers/fsl_lpuart.h>

#define UART2_BUFFER_SIZE 32 // set this to the sub-GHz chip buffer size

lpuart_config_t lpuart_config_default =
{
  .baudRate_Bps = 115200,
  .parityMode = kLPUART_ParityDisabled,
  .stopBitCount = kLPUART_OneStopBit,
  .enableTx = true,
  .enableRx = true,
};

void uart2_init();

// void uart2_deinit(...);

void uart2_write(const uint8_t *buffer, size_t size);
uint8_t *uart2_read(size_t length);

//void uart2_error()


#endif //UBIRCH_FIRMWARE_UART2_H
