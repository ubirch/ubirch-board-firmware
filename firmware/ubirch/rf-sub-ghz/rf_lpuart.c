/*
 * ubirch#1 Low Power UART Driver Code (for CC430 sub-GHz Transceiver MCU).
 *
 * @author Niranjan H. Rao
 * @date 2012.09.23
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

#include "rf_lpuart.h"

static rf_config_t *config;

void rf_init(rf_config_t *rf_config) {
  config = rf_config;

  // initialize Board UART pins
  CLOCK_EnableClock(config->lpuart_port_clock);
  PORT_SetPinMux(config->port, config->rx_pin, config->rx_mux);
  PORT_SetPinMux(config->port, config->tx_pin, config->tx_mux);

  uint32_t port_clock_freq = CLOCK_GetPllFllSelClkFreq();
  //TODO: We need to find a better way to define the clock frequency
  LPUART_Init(config->lpuart, (lpuart_config_t *)&config->lpuart_config,
              port_clock_freq);
  LPUART_EnableRx(config->lpuart, true);
  LPUART_EnableTx(config->lpuart, true);
}

void rf_send(const uint8_t *buffer, size_t size)
{
  LPUART_WriteBlocking(config->lpuart, buffer, size);
}

int32_t rf_read(uint8_t *buffer, size_t length)
{
  status_t lpuart_read_status;
  lpuart_read_status = LPUART_ReadBlocking(config->lpuart, buffer, length);

  return lpuart_read_status;
}

