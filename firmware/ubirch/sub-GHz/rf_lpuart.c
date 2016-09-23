//
// Created by Niranjan Rao on 9/19/16.
//

#include "rf_lpuart.h"
#include <board.h>

//static uint8_t uart2_read_buffer[UART2_BUFFER_SIZE];

void rf_init(rf_config_t *rf_config) {
  // initialize Board UART pins
  CLOCK_EnableClock(rf_config->lpuart_port_clock);
  PORT_SetPinMux(rf_config->port, rf_config->rx_pin, rf_config->rx_mux);
  PORT_SetPinMux(rf_config->port, rf_config->tx_pin, rf_config->tx_mux);

//  CLOCK_GetFreq()
  LPUART_Init(rf_config->lpuart, (lpuart_config_t *)&rf_config->lpuart_config, CLOCK_GetPllFllSelClkFreq);
//                 rf_config->lpuart_port_clock_frequency);
  LPUART_EnableRx(rf_config->lpuart, true);
  LPUART_EnableTx(rf_config->lpuart , true);

  LPUART_EnableInterrupts(rf_config->lpuart, kLPUART_RxDataRegFullInterruptEnable);
  EnableIRQ(rf_config->lpuart_IRQ);
}


// Do we need deinit
 // void rf_deinit()
 // {
 //   ;
 // }

void rf_send(rf_config_t *rf_config, const uint8_t *buffer, size_t size)
{
  LPUART_WriteBlocking(rf_config->lpuart, buffer, size);
}

int32_t rf_read(rf_config_t *rf_config, uint8_t *buffer, size_t length)
{
  status_t lpuart_read_status;
  lpuart_read_status = LPUART_ReadBlocking(rf_config->lpuart, buffer, length);

  return lpuart_read_status;
}

