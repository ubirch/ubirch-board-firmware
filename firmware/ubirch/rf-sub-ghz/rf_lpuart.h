/*!
 * @file
 * @brief RF Low Power UART Driver code (for CC430 sub-GHz Transceiver MCU).
 *
 * This driver drives serial communication between main MCU and the RF module.
 *
 * @author Niranjan H. Rao
 * @date 2016-09-23
 *
 * @copyright &copy; 2015 ubirch GmbH (https://ubirch.com)
 *
 * ```
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ```
 */

#ifndef UBIRCH_FIRMWARE_RF_H
#define UBIRCH_FIRMWARE_RF_H

#include <fsl_lpuart.h>
#include <board.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UART2_BUFFER_SIZE 32                     //!< Set this to the sub-GHz chip buffer size

/*!
 * LPUART Configuration for RF module (CC430 in this case)
 */
typedef struct {
    lpuart_config_t  lpuart_config;              //!< LPUART config struct
    PORT_Type        *port;                      //!< LPUART port type
    clock_name_t     lpuart_clock_src;           //!< LPUART Clock source name
    clock_ip_name_t  lpuart_port_clock;          //!< LPUART Port Clock frequency
    LPUART_Type      *lpuart;                    //!< LPUART base type
    uint32_t         tx_pin;                     //!< LPUART TX Pin
    uint32_t         rx_pin;                     //!< LPUART RX Pin
    port_mux_t       tx_mux;                     //!< LPUART Chip-Specific TX Pin Mux selection
    port_mux_t       rx_mux;                     //!< LPUART Chip-Specific TX Pin Mux selection
    IRQn_Type        lpuart_IRQ;                 //!< LPUART IRQ type

} rf_config_t;

#if defined(BOARD_LPUART2_PORT)

/*!
 * Default RF LPUART configuration for Ubirch boards
 */
rf_config_t rf_config_default = {

  .lpuart_config = {
    .baudRate_Bps = 115200,
    .parityMode   = kLPUART_ParityDisabled,
    .enableRx     = true,
    .enableTx     = true,
  },

  .port                        = BOARD_LPUART2_PORT,
  .lpuart_port_clock           = BOARD_LPUART2_PORT_CLOCK,
  .lpuart                      = BOARD_LPUART2,
  .tx_pin                      = BOARD_LPUART2_TX_PIN,
  .tx_mux                      = BOARD_LPUART2_TX_ALT,
  .rx_pin                      = BOARD_LPUART2_RX_PIN,
  .rx_mux                      = BOARD_LPUART2_RX_ALT,
  .lpuart_IRQ                  = BOARD_LPUART2_IRQ,
};

#endif

/*!
 * @brief Initialize the LPUART driver for RF module
 * @param rf_config the LPUART configurations for the RF module
 */
void rf_init(rf_config_t *rf_config);

/*
 * void rf_deinit()
 * Do we need this function | if needed will add it in future
 * there is a JIRA ticket for this
 */

/*!
 * @brief Send data form the main MCU to the RF module
 * @param rf_config the LPUART configurations for the RF module
 * @param buffer the data to send
 * @param size length of the data to send
 */
void rf_send(const uint8_t *buffer, size_t size);

/*!
 * @brief Read data from the RF module
 * @param rf_config the LPUART configurations for the RF module
 * @param buffer pointer to read the data
 * @param length length of the data to read
 */
int32_t rf_read(uint8_t *buffer, size_t length);

/*
 * void rf_error()
 * Do we need this function | if needed will add it in future
 * there is a JIRA ticket for this
 */

#endif //UBIRCH_FIRMWARE_RF_H
