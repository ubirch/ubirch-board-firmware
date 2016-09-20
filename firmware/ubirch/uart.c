//
// Created by Niranjan Rao on 9/19/16.
//

#include "uart.h"
#include <drivers/fsl_lpuart.h>
#include <board.h>

static uint8_t uart2_read_buffer[UART2_BUFFER_SIZE];


void uart2_init() {
//  const gpio_pin_config_t OUTTRUE = {kGPIO_DigitalOutput, true};
//  const gpio_pin_config_t IN = {kGPIO_DigitalInput, false};

  // initialize Board UART pins
  CLOCK_EnableClock(BOARD_UART2_PORT_CLOCK);
  PORT_SetPinMux(BOARD_UART2_PORT, BOARD_UART2_TX_PIN, BOARD_UART2_TX_ALT);
  PORT_SetPinMux(BOARD_UART2_PORT, BOARD_UART2_RX_PIN, BOARD_UART2_RX_ALT);

  // configure uart driver connected to the sub-GHz transceiver
  lpuart_config_t lpuart_config;
  LPUART_GetDefaultConfig(&lpuart_config);
  lpuart_config.baudRate_Bps = 115200;
  lpuart_config.parityMode = kLPUART_ParityDisabled;
  lpuart_config.stopBitCount = kLPUART_OneStopBit;

  LPUART_Init(BOARD_UART2, &lpuart_config, BOARD_UART2_PORT_CLOCK_FREQ);
  LPUART_EnableRx(BOARD_UART2, true);
  LPUART_EnableTx(BOARD_UART2, true);

  LPUART_EnableInterrupts(BOARD_UART2, kLPUART_RxDataRegFullInterruptEnable);
  EnableIRQ(BOARD_UART2_IRQ);
}

// void uart2_deinit()
// {
//   ;
// }

void uart2_write(const uint8_t *buffer, size_t size)
{
  LPUART_WriteBlocking(BOARD_UART2, buffer, size);
}

uint8_t *uart2_read(size_t length)
{
  uint8_t *buffer = uart2_read_buffer;
  status_t uart2_read_status;
  uart2_read_status = LPUART_ReadBlocking(BOARD_UART2, buffer, length);

  if (!uart2_read_status)
  {
//    debug_print("there is nothing to read\r\n");
    return 0;
  }

  return buffer;
}

// void uart2_error(..)
// {

// }