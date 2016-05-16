/*!
 * @brief FRDM-K82F board specific defines
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

#ifndef _FRDM_K82F_BOARD_H_
#define _FRDM_K82F_BOARD_H_

#define BOARD_SYSTICK_1MS       (SystemCoreClock / 1000 - 1)
#define BOARD_SYSTICK_100MS     (SystemCoreClock / 100 - 1)

// number of boards on the board
#define BOARD_LEDS              3

// on-board LED (red, PTC8)
#define BOARD_LED0_PORT         PORTC
#define BOARD_LED0_GPIO         GPIOC_IDX
#define BOARD_LED0_PORT_CLOCK   PORTC_IDX
#define BOARD_LED0_PIN          1U

// on-board LED (green, PTC9)
#define BOARD_LED1_PORT         PORTC
#define BOARD_LED1_GPIO         GPIOC_IDX
#define BOARD_LED1_PORT_CLOCK   PORTC_IDX
#define BOARD_LED1_PIN          2U

// on-board LED (blue, PTC10)
#define BOARD_LED2_PORT         PORTC
#define BOARD_LED2_GPIO         GPIOC_IDX
#define BOARD_LED2_PORT_CLOCK   PORTC_IDX
#define BOARD_LED2_PIN          0U

#define BOARD_LED0(on)          GPIO_WritePinOutput(BOARD_LED0_GPIO, BOARD_LED0_PIN, !(on))
#define BOARD_LED1(on)          GPIO_WritePinOutput(BOARD_LED1_GPIO, BOARD_LED1_PIN, !(on))
#define BOARD_LED2(on)          GPIO_WritePinOutput(BOARD_LED2_GPIO, BOARD_LED2_PIN, !(on))

// on-board button (PTD0)
#define BOARD_BUTTON0_PORT      PORTD
#define BOARD_BUTTON0_GPIO      GPIOD_IDX
#define BOARD_BUTTON0_PIN       0U
#define BOARD_BUTTON0_PORT_CLOCK PORTD_IDX
#define BOARD_BUTTON0_IRQ       PORTD_IRQHandler

// on-board button 2 (PTA4)
#define BOARD_BUTTON1_PORT      PORTA
#define BOARD_BUTTON1_GPIO      GPIOA_IDX
#define BOARD_BUTTON1_PIN       4U
#define BOARD_BUTTON1_PORT_CLOCK PORTA_IDX
#define BOARD_BUTTON1_IRQ       PORTA_IRQHandler

// on-board button 3 (PTC6)
#define BOARD_BUTTON2_PORT      PORTC
#define BOARD_BUTTON2_GPIO      GPIOC_IDX
#define BOARD_BUTTON2_PIN       6U
#define BOARD_BUTTON2_PORT_CLOCK PORTC_IDX
#define BOARD_BUTTON2_IRQ       PORTC_IRQHandler

// K82F NMI pin (PTA4)
#define BOARD_NMI_PORT          PORTA
#define BOARD_NMI_GPIO          GPIOA_IDX
#define BOARD_NMI_PORT_CLOCK    PORTA_IDX
#define BOARD_NMI_PIN           4
#define BOARD_NMI_ALT           kPORT_MuxAsGpio

// (debug) console
#define BOARD_DEBUG_TYPE        kDebugConsoleLPUART
#define BOARD_DEBUG_INSTANCE    0
#define BOARD_DEBUG_UART        LPUART0
#define BOARD_DEBUG_CLKSRC      kClockLpuartSrcOsc0erClk
//#define BOARD_DEBUG_CLK_FREQ    CLOCK_GetOsc0ErClkFreq()
#define BOARD_DEBUG_IRQ         LPUART0_IRQn
#define BOARD_DEBUG_IRQ_HANDLER LPUART0_IRQHandler
#define BOARD_DEBUG_PORT        PORTB
#define BOARD_DEBUG_PORT_CLOCK  PORTB_IDX
#define BOARD_DEBUG_RX_PIN      16
#define BOARD_DEBUG_RX_ALT      kPortMuxAlt3
#define BOARD_DEBUG_TX_PIN      17
#define BOARD_DEBUG_TX_ALT      kPortMuxAlt3

#ifndef BOARD_DEBUG_BAUD
#  define BOARD_DEBUG_BAUD      115200
#endif

#endif // _FRDM_K82F_BOARD_H_
