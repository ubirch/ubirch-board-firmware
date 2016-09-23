//
// Created by Niranjan Rao on 9/19/16.
//

#ifndef UBIRCH_FIRMWARE_UART2_H
#define UBIRCH_FIRMWARE_UART2_H

#include <drivers/fsl_lpuart.h>

#include <board.h>

#define UART2_BUFFER_SIZE 32 // set this to the sub-GHz chip buffer size

/*!
 * RF LPUART Configuration
 */
typedef struct {
    lpuart_config_t  lpuart_config;
    PORT_Type        *port;
    clock_name_t     lpuart_clock_src;           // or rename it to cloccksrc
    clock_ip_name_t  lpuart_port_clock;          // uart_clock_port uart_clock_port_freq
    LPUART_Type      *lpuart;
    uint32_t         tx_pin;
    uint32_t         rx_pin;
    port_mux_t       tx_mux;
    port_mux_t       rx_mux;
    IRQn_Type        lpuart_IRQ;

} rf_config_t;

// Here we set the default configuration for LPUART to LPUART2 config
rf_config_t rf_config_default = {

  .lpuart_config = {
    .baudRate_Bps = 115200,
    .parityMode   = kLPUART_ParityDisabled,
    .enableRx     = true,
    .enableTx     = true,
  },

  .port                        = BOARD_UART2_PORT,
  .lpuart_port_clock           = BOARD_UART2_PORT_CLOCK,
  .lpuart                      = BOARD_UART2,
  .tx_pin                      = BOARD_UART2_TX_PIN,
  .tx_mux                      = BOARD_UART2_TX_ALT,
  .rx_pin                      = BOARD_UART2_RX_PIN,
  .rx_mux                      = BOARD_UART2_RX_ALT,
  .lpuart_IRQ                  = BOARD_UART2_IRQ,
};

void rf_init(rf_config_t *rf_config);
//void rf_deinit(...);

void rf_send(rf_config_t *rf_config, const uint8_t *buffer, size_t size);
int32_t rf_read(rf_config_t *rf_config, uint8_t *buffer, size_t length);

//void uart2_error()
#endif //UBIRCH_FIRMWARE_UART2_H
