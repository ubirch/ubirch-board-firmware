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

#ifndef _UBIRCH_CELL_BOARD_H_
#define _UBIRCH_CELL_BOARD_H_

#define BOARD_CELL_TYPE_M66         1

// cell phone chip config
#define BOARD_CELL_UART_PORT        PORTC                         //!< cell modem port used
#define BOARD_CELL_UART_PORT_CLOCK  kCLOCK_PortC                  //!< port clock
#define BOARD_CELL_PORT_CLOCK_FREQ  (CLOCK_GetPllFllSelClkFreq()) //!< port clock frequency
#define BOARD_CELL_UART_TX_PIN      15U                           //!< TX pin number
#define BOARD_CELL_UART_TX_ALT      kPORT_MuxAlt3                 //!< TX pin mux
#define BOARD_CELL_UART_RX_PIN      14U                           //!< RX pin number
#define BOARD_CELL_UART_RX_ALT      kPORT_MuxAlt3                 //!< RX pin mux
#define BOARD_CELL_UART             LPUART4                       //!< uart the cell modem is connected to
#define BOARD_CELL_UART_IRQ         LPUART4_IRQn                  //!< uart irq
#define BOARD_CELL_UART_IRQ_HANDLER LPUART4_IRQHandler            //!< uart irq handler

#define BOARD_CELL_PIN_PORT         PORTE
#define BOARD_CELL_PIN_PORT_CLOCK   kCLOCK_PortE
#define BOARD_CELL_PIN_GPIO      GPIOE
#define BOARD_CELL_STATUS_PIN       6U                            //!< status pin number
#define BOARD_CELL_PWRKEY_PIN       8U                            //!< pwrkey pin number
#define BOARD_CELL_RI_PIN           9U                            //!< ri (ring indicator) pin number
#define BOARD_CELL_DCD_PIN          10U

// set uart speed if not defined elsewhere
#ifndef BOARD_CELL_UART_BAUD
# define BOARD_CELL_UART_BAUD       57600                         //!< cell modem uart speed if not set elsewhere
#endif

#ifndef BOARD_CELL_BUFSIZE
# define BOARD_CELL_BUFSIZE         255                           //!< size of the parser buffer
#endif

// power domain enabler
#define BOARD_CELL_PWR_DOMAIN       1             //!< this board has a power domain for the modem chip
#define BOARD_CELL_PWR_EN_PORT      PORTD         //!< the port for the power domain switch
#define BOARD_CELL_PWR_EN_GPIO      GPIOD         //!< gpio to switch the power domain
#define BOARD_CELL_PWR_EN_PIN       7U            //!< gpio pin number for the power domain switch
#define BOARD_CELL_PWR_EN_CLOCK     kCLOCK_PortD  //!< port clock (for enabling port)


// special AT commands, chip specific

#define AT_CON_CLOSE               "QICLOSE"
#define AT_MUX_MODE                "QIMUX"
#define AT_RECEIVE_MODE            "QINDI"

#endif // _UBIRCH_CELL_BOARD_H_
