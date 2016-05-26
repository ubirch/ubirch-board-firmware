/*!
 * @file
 * @brief __ubirch#1 r0.2__ cell phone chip definitions
 *
 * The chip is on-board so we can define all the settings here as it is hardwired
 * to the MCU.
 *
 * @author Matthias L. Jugel
 * @date 2016-05-03
 *
 * Copyright 2016 ubirch GmbH (https://ubirch.com)
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

#ifndef _UBIRCH_CELL_BOARD_H_
#define _UBIRCH_CELL_BOARD_H_

// cell phone chip config
#define BOARD_CELL_PORT             PORTE                         //!< cell modem port used
#define BOARD_CELL_GPIO             GPIOE                         //!< gpio port for (ri, status etc)
#define BOARD_CELL_PORT_CLOCK       kCLOCK_PortE                  //!< port clock
#define BOARD_CELL_PORT_CLOCK_FREQ  (CLOCK_GetPllFllSelClkFreq()) //!< port clock frequency
#define BOARD_CELL_UART_TX_PIN      4U                            //!< TX pin number
#define BOARD_CELL_UART_TX_ALT      kPORT_MuxAlt3                 //!< TX pin mux
#define BOARD_CELL_UART_RX_PIN      5U                            //!< RX pin number
#define BOARD_CELL_UART_RX_ALT      kPORT_MuxAlt3                 //!< RX pin mux
#define BOARD_CELL_UART             LPUART3                       //!< uart the cell modem is connected to
#define BOARD_CELL_UART_IRQ         LPUART3_IRQn                  //!< uart irq
#define BOARD_CELL_UART_IRQ_HANDLER LPUART3_IRQHandler            //!< uart irq handler
#define BOARD_CELL_STATUS_PIN       6U                            //!< status pin number
#define BOARD_CELL_RESET_PIN        7U                            //!< reset pin number
#define BOARD_CELL_PWRKEY_PIN       8U                            //!< pwrkey pin number
#define BOARD_CELL_RI_PIN           9U                            //!< ri (ring indicator) pin number

// set uart speed if not defined elsewhere
#ifndef BOARD_CELL_UART_BAUD
# define BOARD_CELL_UART_BAUD       57600         //!< cell modem uart speed if not set elsewhere
#endif

// power domain enabler
#define BOARD_CELL_PWR_DOMAIN       1             //!< this board has a power domain for the modem chip
#define BOARD_CELL_PWR_EN_PORT      PORTD         //!< the port for the power domain switch
#define BOARD_CELL_PWR_EN_GPIO      GPIOD         //!< gpio to switch the power domain
#define BOARD_CELL_PWR_EN_PIN       7U            //!< gpio pin number for the power domain switch
#define BOARD_CELL_PWR_EN_CLOCK     kCLOCK_PortD  //!< port clock (for enabling port)

#endif // _UBIRCH_CELL_BOARD_H_
