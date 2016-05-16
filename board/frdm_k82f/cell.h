/*!
 * @brief ubirch#1 r0.2 cell phone chip definitions
 *
 * The chip is on-board so we can define all the settings here as it is hardwired
 * to the MCU.
 *
 * @author Matthias L. Jugel
 * @date 2016-05-03
 *
 * Copyright 2016 ubirch GmbH (https://ubirch.com)
 *
 * == LICENSE ==
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
 */

#ifndef _UBIRCH_CELL_BOARD_H_
#define _UBIRCH_CELL_BOARD_H_

// cell phone chip config
#define BOARD_CELL_PORT             PORTD
#define BOARD_CELL_GPIO             GPIOD
#define BOARD_CELL_PORT_CLOCK       kCLOCK_PortD
#define BOARD_CELL_PORT_CLOCK_FREQ  (CLOCK_GetOsc0ErClkFreq())
#define BOARD_CELL_UART_TX_PIN      3U
#define BOARD_CELL_UART_TX_ALT      kPORT_MuxAlt3
#define BOARD_CELL_UART_RX_PIN      2U
#define BOARD_CELL_UART_RX_ALT      kPORT_MuxAlt3
#define BOARD_CELL_UART             LPUART2
#define BOARD_CELL_UART_IRQ         LPUART2_IRQn
#define BOARD_CELL_UART_IRQ_HANDLER LPUART2_IRQHandler
#define BOARD_CELL_STATUS_PIN       4U
#define BOARD_CELL_RESET_PIN        1U
#define BOARD_CELL_PWRKEY_PIN       0U
//#define BOARD_CELL_RI_PIN           9U

#ifndef BOARD_CELL_UART_BAUD
# define BOARD_CELL_UART_BAUD       57600
#endif

#endif // _UBIRCH_CELL_BOARD_H_
