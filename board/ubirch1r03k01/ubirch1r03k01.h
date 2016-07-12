/*!
 * @file ubirch1r02.h
 * @brief __ubirch#1 r0.2__ board specific defines
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

#include "cell.h"

#ifndef _UBIRCH1R03_BOARD_H_
#define _UBIRCH1R03_BOARD_H_

//! @{

#define BOARD_UBIRCH_1R03K01
#define BOARD                   "ubirch1r03k01"

#define BOARD_SYSTICK_1MS       (SystemCoreClock / 1000 - 1)
#define BOARD_SYSTICK_100MS     (SystemCoreClock / 100 - 1)

// amount of LEDs available on the board
#define BOARD_LEDS              2

// default on-board LED (PTD4)
#define BOARD_LED0_PORT         PORTD
#define BOARD_LED0_GPIO         GPIOD
#define BOARD_LED0_PORT_CLOCK   kCLOCK_PortD
#define BOARD_LED0_PIN          4U

#define BOARD_LED1_PORT         PORTA
#define BOARD_LED1_GPIO         GPIOA
#define BOARD_LED1_PORT_CLOCK   kCLOCK_PortA
#define BOARD_LED1_ALT          kPORT_MuxAlt3
#define BOARD_LED1_PIN          14U

#define BOARD_LED0(on)          GPIO_WritePinOutput(BOARD_LED0_GPIO, BOARD_LED0_PIN, (on))
#define BOARD_LED1(on)          board_rgb_led(1, (on?255:0), (on?255:0), (on?255:0))
//#define BOARD_LED1(r, g, b)     board_rgb_led(BOARD_LED1, r, g, b)

// on-board button (PTD0)
#define BOARD_BUTTON0_PORT      PORTD
#define BOARD_BUTTON0_GPIO      GPIOD
#define BOARD_BUTTON0_PORT_CLOCK kCLOCK_PortD
#define BOARD_BUTTON0_PIN       0
#define BOARD_BUTTON0_HANDLER   PORTD_IRQHandler

// K82F NMI pin (PTA4)
#define BOARD_NMI_PORT          PORTA
#define BOARD_NMI_GPIO          GPIOA
#define BOARD_NMI_PORT_CLOCK    kCLOCK_PortA
#define BOARD_NMI_PIN           4

// header row switchable power pins
#define BOARD_PWR_EN_PORT       PORTC
#define BOARD_PWR_EN_GPIO       GPIOC
#define BOARD_PWR_EN_PORT_CLOCK kCLOCK_PortC
#define BOARD_PWR_EN_PIN        5

// I2C configuration
#define BOARD_I2C               I2C1
#define BOARD_I2C_CLOCK         I2C1_CLK_SRC
#define BOARD_I2C_PORT          PORTC
#define BOARD_I2C_PORT_CLOCK    kCLOCK_PortC
#define BOARD_I2C_ALT           kPORT_MuxAlt2
#define BOARD_I2C_SCL_PIN       10U
#define BOARD_I2C_SDA_PIN       11U

// timer interrupt (PIT)
#define BOARD_TIMER             PIT
#define BOARD_TIMER_IRQ         PIT3_IRQn
#define BOARD_TIMER_HANDLER     PIT3_IRQHandler

// board rtc (PIT)
#define BOARD_RTC               RTC
#define BOARD_RTC_CLOCK         RTC_CR_OSCE_MASK
#define BOARD_RTC_IRQ           RTC_IRQn

// on-board Qi charger socket
#define BOARD_QI_CHG_PORT       PORTA
#define BOARD_QI_CHG_GPIO       GPIOA
#define BOARD_QI_CHG_PORT_CLOCK kCLOCK_PortA
#define BOARD_QI_CHG_EN_PIN     5U
#define BOARD_QI_CGH_AUX_PIN    12U
#define BOARD_QI_CGH_STATUS_PIN 13U

// (debug) console                             `
//! LPUART0 (RX - PTB16, TX - PTB17)
#define BOARD_DEBUG_TYPE        DEBUG_CONSOLE_DEVICE_TYPE_LPUART
#define BOARD_DEBUG_UART        LPUART0
// TODO check if this clock source as and the clock frequency settings match
#define BOARD_DEBUG_CLKSRC      kCLOCK_Osc0ErClk
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

#endif // _UBIRCH1R03_BOARD_H_
