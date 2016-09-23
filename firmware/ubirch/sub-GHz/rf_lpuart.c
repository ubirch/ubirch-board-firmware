//
// Created by Niranjan Rao on 9/19/16.
//

#include "rf_lpuart.h"
#include <board.h>

//static uint8_t uart2_read_buffer[UART2_BUFFER_SIZE];
static rf_config_t *new_rf_config;

void rf_init(rf_config_t *rf_config) {
  // initialize Board UART pins
  CLOCK_EnableClock(rf_config->lpuart_port_clock);
  PORT_SetPinMux(rf_config->port, rf_config->rx_pin, rf_config->rx_mux);
  PORT_SetPinMux(rf_config->port, rf_config->tx_pin, rf_config->tx_mux);

  uint32_t port_clock_freq = CLOCK_GetPllFllSelClkFreq();
  //TODO: We need to find a better way to define the clock frequency
  LPUART_Init(rf_config->lpuart, (lpuart_config_t *)&rf_config->lpuart_config,
              port_clock_freq);
  LPUART_EnableRx(rf_config->lpuart, true);
  LPUART_EnableTx(rf_config->lpuart, true);

  LPUART_EnableInterrupts(rf_config->lpuart, kLPUART_RxDataRegFullInterruptEnable);
  EnableIRQ(rf_config->lpuart_IRQ);
  new_rf_config = rf_config;
}


// Do we need deinit
 // void rf_deinit()
 // {
 //   ;
 // }

void rf_send(rf_config_t *new_rf_config, const uint8_t *buffer, size_t size)
{
  LPUART_WriteBlocking(new_rf_config->lpuart, buffer, size);
}

int32_t rf_read(rf_config_t *new_rf_config, uint8_t *buffer, size_t length)
{
  status_t lpuart_read_status;
  lpuart_read_status = LPUART_ReadBlocking(new_rf_config->lpuart, buffer, length);

  return lpuart_read_status;
}

