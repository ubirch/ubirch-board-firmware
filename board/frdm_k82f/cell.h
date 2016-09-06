/*!
 * @file
 * @brief __FRDM-K82F__ cell phone chip definitions
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



// cell phone chip config
#define BOARD_CELL_UART_PORT        PORTD                      //!< cell modem port used
#define BOARD_CELL_UART_PORT_CLOCK  kCLOCK_PortD               //!< port clock
#define BOARD_CELL_PORT_CLOCK_FREQ  (CLOCK_GetOsc0ErClkFreq()) //!< port clock frequency
#define BOARD_CELL_UART_TX_PIN      3U                         //!< TX pin number
#define BOARD_CELL_UART_TX_ALT      kPORT_MuxAlt3              //!< TX pin mux
#define BOARD_CELL_UART_RX_PIN      2U                         //!< RX pin number
#define BOARD_CELL_UART_RX_ALT      kPORT_MuxAlt3              //!< RX pin mux
#define BOARD_CELL_UART             LPUART2                    //!< uart the cell modem is connected to
#define BOARD_CELL_UART_IRQ         LPUART2_IRQn               //!< uart irq
#define BOARD_CELL_UART_IRQ_HANDLER LPUART2_IRQHandler         //!< uart irq handler

#define BOARD_CELL_PIN_PORT         PORTD                      //!< cell modem pins port
#define BOARD_CELL_PIN_PORT_CLOCK   kCLOCK_PortD               //!< cell modem pins clock
#define BOARD_CELL_PIN_GPIO         GPIOD                      //!< cell modem gpio
#define BOARD_CELL_STATUS_PIN       4U                         //!< status pin number
#define BOARD_CELL_RESET_PIN        1U                         //!< reset pin number
#define BOARD_CELL_PWRKEY_PIN       0U                         //!< pwrkey pin number


// set uart speed if not defined elsewhere
#ifndef BOARD_CELL_UART_BAUD
# define BOARD_CELL_UART_BAUD       57600  //!< cell modem uart speed if not set elsewhere
#endif

#ifndef BOARD_CELL_BUFSIZE
# define BOARD_CELL_BUFSIZE         255                       //!< size of the parser buffer
#endif

#endif // _UBIRCH_CELL_BOARD_H_
