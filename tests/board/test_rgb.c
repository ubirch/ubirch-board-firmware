/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_flexio_uart.h"
#include "../../firmware/ubirch/timer.h"

#define BOARD_FLEXIO_BASE FLEXIO0
#define FLEXIO_UART_TX_PIN 11U
#define FLEXIO_UART_RX_PIN 12U
#define FLEXIO_CLOCK_FREQUENCY 12000000U

#define RX_RING_BUFFER_SIZE 20U

/*!
 * @brief Main function
 */
void test_ftm(void) {
  flexio_uart_config_t config;

  CLOCK_EnableClock(kCLOCK_PortA);
  /* Initialize FlexIO pins below */
  PORT_SetPinMux(PORTA, 1U, kPORT_MuxAlt5);
  PORT_SetPinMux(PORTA, 2U, kPORT_MuxAlt5);

  CLOCK_SetFlexio0Clock(2U);

  /*
   * config.enableUart = true;
   * config.enableInDoze = false;
   * config.enableInDebug = true;
   * config.enableFastAccess = false;
   * config.baudRate_Bps = 115200U;
   * config.bitCountPerChar = kFLEXIO_UART_8BitsPerChar;
   */
  FLEXIO_UART_GetDefaultConfig(&config);
  config.baudRate_Bps = 1200;
  config.enableUart = true;

  FLEXIO_UART_Type uartDev;
  uartDev.flexioBase = BOARD_FLEXIO_BASE;
  uartDev.TxPinIndex = FLEXIO_UART_TX_PIN;
  uartDev.RxPinIndex = FLEXIO_UART_RX_PIN;
  uartDev.shifterIndex[0] = 0U;
  uartDev.shifterIndex[1] = 1U;
  uartDev.timerIndex[0] = 0U;
  uartDev.timerIndex[1] = 1U;

  FLEXIO_UART_Init(&uartDev, &config, FLEXIO_CLOCK_FREQUENCY);

  uint8_t txbuff[] = {0b11111111, 0b00000000};


  while (1)
  {
    FLEXIO_UART_WriteBlocking(&uartDev, txbuff, 2);
  }
}
