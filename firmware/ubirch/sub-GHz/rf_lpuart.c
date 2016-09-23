//
// Created by Niranjan Rao on 9/19/16.
//

#include "rf_lpuart.h"
#include <board.h>

static uint8_t uart2_read_buffer[UART2_BUFFER_SIZE];


//void uart2_init(const rf_lpuart_config_t *rf_lpuart_config) {
//
//void sub-ghz_init(void) {
//
//  // make sure init needs any arg inout like config_t
//  // we sshould define a config_t for the sub-ghz uart thingy with tx, rx, freq, ect
//
//  uart2_init(); //the uart init should take config_t as input
//
//}

/* TODO: we need simple send() and receive() functions to tx/ rx data b/w MCU - transceiver */
/* TODO: we need simple send() and receive() functions to tx/ rx data b/w MCU - transceiver */
//void  rf_send(the data to send);
//
//int rf_read(maybe the config);



void rf_init(rf_config_t *rf_config) {
  // initialize Board UART pins
  CLOCK_EnableClock(rf_config->lpuart_port_clock);
  PORT_SetPinMux(rf_config->port, rf_config->rx_pin, rf_config->rx_mux);
  PORT_SetPinMux(rf_config->port, rf_config->tx_pin, rf_config->tx_mux);

  LPUART_Init(rf_config->lpuart, rf_config->lpuart_config,
                 rf_config->lpuart_port_clock_frequency);
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

void rf_send(const uint8_t *buffer, size_t size)
{
  LPUART_WriteBlocking(rf_config->lpuart, buffer, size);
}

status_t rf_read(uint8_t *buffer, uint8_t length)
{
  uint8_t *buffer = uart2_read_buffer;

  status_t lpuart_read_status;
  lpuart_read_status = LPUART_ReadBlocking(rf_config->lpuart, buffer, length);

  return lpuart_read_status;
}

