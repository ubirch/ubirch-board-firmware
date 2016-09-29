/*!
 * @file ubirch1r03k01.h
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

#include "cell.h"

#ifndef _UBIRCH1R03_BOARD_H_
#define _UBIRCH1R03_BOARD_H_

//! @{

#define BOARD_UBIRCH_1R03
#define BOARD                   "ubirch1r03"

#define BOARD_SYSTICK_1MS       (SystemCoreClock / 1000 - 1)
#define BOARD_SYSTICK_100MS     (SystemCoreClock / 100 - 1)

// amount of LEDs available on the board
#define BOARD_LEDS              1

// default on-board LED (PTD4)
#define BOARD_LED0_PORT         PORTD
#define BOARD_LED0_GPIO         GPIOD
#define BOARD_LED0_PORT_CLOCK   kCLOCK_PortD
#define BOARD_LED0_PIN          4U

#define BOARD_LED0(on)          GPIO_WritePinOutput(BOARD_LED0_GPIO, BOARD_LED0_PIN, (on))

// on-board RGB led strip (WS2812b compatible)
#define BOARD_RGBS_PORT         PORTA
#define BOARD_RGBS_GPIO         GPIOA
#define BOARD_RGBS_PORT_CLOCK   kCLOCK_PortA
#define BOARD_RGBS_ALT          kPORT_MuxAlt5
#define BOARD_RGBS_PIN          14U
#define BOARD_RGBS_FLEXIO_PIN   20
#define BOARD_RGBS_LEN          1

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

//LPUART2 Configurations
// This is used by the MCU to communicate to the rf-sub-ghz module
#define BOARD_LPUART2_PORT        PORTC                      //!< port used
#define BOARD_LPUART2_PORT_CLOCK  kCLOCK_PortC               //!< port clock
#define BOARD_LPUART2_CLKSRC      kCLOCK_Osc0ErClk           //!< port clock csource
//#define BOARD_LPUART2_PORT_CLOCK_FREQ    CLOCK_GetPllFllSelClkFreq() //!< port clock frequency


#define BOARD_LPUART2_TX_PIN      4U                         //!< TX pin number
#define BOARD_LPUART2_TX_ALT      kPORT_MuxAlt3              //!< TX pin mux
#define BOARD_LPUART2_RX_PIN      3U                         //!< RX pin number
#define BOARD_LPUART2_RX_ALT      kPORT_MuxAlt3              //!< RX pin mux

#define BOARD_LPUART2             LPUART1
#define BOARD_LPUART2_IRQ         LPUART1_IRQn               //!< lpuart-2 irq
#define BOARD_LPUART2_IRQ_HANDLER LPUART1_IRQHandler         //!< lpuart-2 irq handler

// set uart-2 speed if not defined elsewhere
#ifndef BOARD_LPUART2_BAUD
# define BOARD_LPUART2_BAUD       115200  //!< uart-2 speed if not set elsewhere
#endif

// timer interrupt (PIT)
#define BOARD_TIMER             PIT
#define BOARD_TIMER_IRQ         PIT3_IRQn
#define BOARD_TIMER_HANDLER     PIT3_IRQHandler

// board rtc (PIT)
#define BOARD_RTC               RTC
#define BOARD_RTC_CLOCK         RTC_CR_OSCE_MASK
#define BOARD_RTC_IRQ           RTC_IRQn

// board sdhc slot (SDHC0)
#define BOARD_SDHC              SDHC
#define BOARD_SDHC_CLOCK        kCLOCK_CoreSysClk
#define BOARD_SDHC_IRQ          SDHC_IRQn
#define BOARD_SDHC_PORT         PORTE
#define BOARD_SDHC_PORT_CLOCK   kCLOCK_PortE
#define BOARD_SDHC_ALT          kPORT_MuxAlt4
#define BOARD_SDHC_PIN_D0       1U
#define BOARD_SDHC_PIN_D1       0U
#define BOARD_SDHC_PIN_D2       5U
#define BOARD_SDHC_PIN_D3       4U
#define BOARD_SDHC_PIN_DCLK     2U
#define BOARD_SDHC_PIN_CMD      3U
#define BOARD_SDHC_DET_PORT     PORTE
#define BOARD_SDHC_DET_GPIO     GPIOE
#define BOARD_SDHC_DET_CLOCK    kCLOCK_PortE
#define BOARD_SDHC_DET_PIN      7U
#define BOARD_SDHC_DET_HANDLER  PORTE_IRQHandler

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
