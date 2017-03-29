/*!
 * @file ubridge10.h
 * @brief __ubirch#1 r0.3k0.1__ board specific defines
 *
 * @author Matthias L. Jugel
 * @date 2016-05-01
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

#include "spirit1.h"

#ifndef _USENSE_BOARD_H_
#define _USENSE_BOARD_H_

//! @{

#define BOARD_USENSE
#define BOARD                   "uSense"

#define BOARD_MODE_RUN          0
#define BOARD_MODE_VLPR         1
#define BOARD_MODE_HSRUN        2

#define BOARD_SYSTICK_1MS       (SystemCoreClock / 1000 - 1)
#define BOARD_SYSTICK_100MS     (SystemCoreClock / 100 - 1)

// amount of LEDs available on the board
#define BOARD_LEDS              1

#define BOARD_LED0(on)

// on-board button (PTD0)
//#define BOARD_BUTTON0_PORT      PORTD
//#define BOARD_BUTTON0_GPIO      GPIOD
//#define BOARD_BUTTON0_PORT_CLOCK kCLOCK_PortD
//#define BOARD_BUTTON0_PIN       0
//#define BOARD_BUTTON0_IRQ       PORTD_IRQn
//#define BOARD_BUTTON0_HANDLER   PORTD_IRQHandler

// KL82z NMI pin (PTA4)
#define BOARD_NMI_PORT          PORTA
#define BOARD_NMI_GPIO          GPIOA
#define BOARD_NMI_PORT_CLOCK    kCLOCK_PortA
#define BOARD_NMI_PIN           4

// I2C configuration
#define BOARD_I2C               I2C1
#define BOARD_I2C_CLOCK         I2C1_CLK_SRC
#define BOARD_I2C_PORT          PORTC
#define BOARD_I2C_PORT_CLOCK    kCLOCK_PortC
#define BOARD_I2C_ALT           kPORT_MuxAlt2
#define BOARD_I2C_SCL_PIN       10U
#define BOARD_I2C_SDA_PIN       11U

// SPI configuration
// TODO add SPI config

// timer interrupt (PIT)
#define BOARD_TIMER             PIT
#define BOARD_TIMER_IRQ         PIT0_IRQn
#define BOARD_TIMER_HANDLER     PIT0_IRQHandler

//! LPUART1 (RX - PTA1, TX - PTA2)
#define BOARD_DEBUG_TYPE        DEBUG_CONSOLE_DEVICE_TYPE_LPUART
#define BOARD_DEBUG_UART        LPUART0
#define BOARD_DEBUG_CLK_FREQ    CLOCK_GetPllFllSelClkFreq()
#define BOARD_DEBUG_IRQ         LPUART0_IRQn
#define BOARD_DEBUG_IRQ_HANDLER LPUART0_IRQHandler
#define BOARD_DEBUG_BAUD        115200
#define BOARD_DEBUG_PORT        PORTA
#define BOARD_DEBUG_PORT_CLOCK  kCLOCK_PortA
#define BOARD_DEBUG_RX_PIN      1U
#define BOARD_DEBUG_RX_ALT      kPORT_MuxAlt2
#define BOARD_DEBUG_TX_PIN      2U
#define BOARD_DEBUG_TX_ALT      kPORT_MuxAlt2

//! @}

#endif // _USENSE_BOARD_H_
