/*!
 * @file
 * @brief __FRDM-KL82Z__ board specific defines
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

#include "cell.h"

#ifndef _FRDM_K82F_BOARD_H_
#define _FRDM_K82F_BOARD_H_

#define BOARD_FRDM_KL82Z
#define BOARD                   "FRDM-KL82Z"

#define BOARD_SYSTICK_1MS       (SystemCoreClock / 1000 - 1)
#define BOARD_SYSTICK_100MS     (SystemCoreClock / 100 - 1)

// number of boards on the board
#define BOARD_LEDS              3

// on-board LED (red, PTC1)
#define BOARD_LED0_PORT         PORTC
#define BOARD_LED0_GPIO         GPIOC
#define BOARD_LED0_PORT_CLOCK   kCLOCK_PortC
#define BOARD_LED0_PIN          1U

// on-board LED (green, PTC2)
#define BOARD_LED1_PORT         PORTC
#define BOARD_LED1_GPIO         GPIOC
#define BOARD_LED1_PORT_CLOCK   kCLOCK_PortC
#define BOARD_LED1_PIN          2U

// on-board LED (blue, PTC0)
#define BOARD_LED2_PORT         PORTC
#define BOARD_LED2_GPIO         GPIOC
#define BOARD_LED2_PORT_CLOCK   kCLOCK_PortC
#define BOARD_LED2_PIN          0U

#define BOARD_LED0(on)          GPIO_WritePinOutput(BOARD_LED0_GPIO, BOARD_LED0_PIN, !(on))
#define BOARD_LED1(on)          GPIO_WritePinOutput(BOARD_LED1_GPIO, BOARD_LED1_PIN, !(on))
#define BOARD_LED2(on)          GPIO_WritePinOutput(BOARD_LED2_GPIO, BOARD_LED2_PIN, !(on))

// on-board button (PTD0)
#define BOARD_BUTTON0_PORT      PORTD
#define BOARD_BUTTON0_GPIO      GPIOD
#define BOARD_BUTTON0_PORT_CLOCK kCLOCK_PortD
#define BOARD_BUTTON0_PIN       0U
#define BOARD_BUTTON0_IRQ       PORTD_IRQHandler

// on-board button 2 (PTA4)
#define BOARD_BUTTON1_PORT      PORTA
#define BOARD_BUTTON1_GPIO      GPIOA
#define BOARD_BUTTON1_PORT_CLOCK kCLOCK_PortA
#define BOARD_BUTTON1_PIN       4U
#define BOARD_BUTTON1_IRQ       PORTA_IRQHandler

// KL82Z NMI pin (PTA4)
#define BOARD_NMI_PORT          PORTA
#define BOARD_NMI_GPIO          GPIOA
#define BOARD_NMI_PORT_CLOCK    kCLOCK_PortA
#define BOARD_NMI_PIN           4U

// I2C configuration
#define BOARD_I2C               I2C0
#define BOARD_I2C_CLOCK         I2C0_CLK_SRC
#define BOARD_I2C_PORT          PORTB
#define BOARD_I2C_PORT_CLOCK    kCLOCK_PortB
#define BOARD_I2C_ALT           kPORT_MuxAlt2
#define BOARD_I2C_SCL_PIN       0U
#define BOARD_I2C_SDA_PIN       1U

#define BOARD_RTC               RTC
#define BOARD_RTC_CLOCK         RTC_CR_OSCE_MASK
#define BOARD_RTC_IRQ           RTC_IRQn

#define BOARD_TIMER_PIT         PIT0

// (debug) console                             `
//! LPUART0 (RX - PTB16, TX - PTB17)
#define BOARD_DEBUG_TYPE        DEBUG_CONSOLE_DEVICE_TYPE_LPUART
#define BOARD_DEBUG_UART        LPUART0
// TODO check if this clock source as and the clock frequency settings match
#define BOARD_DEBUG_CLKSRC      kCLOCK_Osc0ErClk
#define BOARD_DEBUG_CLK_FREQ    CLOCK_GetOsc0ErClkFreq()
#define BOARD_DEBUG_IRQ         LPUART0_IRQn
#define BOARD_DEBUG_IRQ_HANDLER LPUART0_IRQHandler
#define BOARD_DEBUG_PORT        PORTB
#define BOARD_DEBUG_PORT_CLOCK  kCLOCK_PortB
#define BOARD_DEBUG_RX_PIN      16
#define BOARD_DEBUG_RX_ALT      kPORT_MuxAlt3
#define BOARD_DEBUG_TX_PIN      17
#define BOARD_DEBUG_TX_ALT      kPORT_MuxAlt3

#ifndef BOARD_DEBUG_BAUD
#  define BOARD_DEBUG_BAUD      115200
#endif

#endif // _FRDM_K82F_BOARD_H_
