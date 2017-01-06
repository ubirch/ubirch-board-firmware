/*!
 * @file
 * @brief __FRDM-K82F__ board specific initialization functions
 *
 * Contains the declaration of initialization functions specific for the board.
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

#ifndef _UBIRCH_BOARD_H_
#define _UBIRCH_BOARD_H_

// processor specific includes
#include <MK82F25615.h>
#include <MK82F25615_features.h>
#include <core_cm4.h>

// peripherals
#include <drivers/fsl_common.h>
#include <drivers/fsl_port.h>
#include <drivers/fsl_gpio.h>
#include <drivers/fsl_i2c.h>

#include <utilities/fsl_debug_console.h>

// board specific includes
#include "frdm_k82f.h"
#include "clock_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Initialize the basic board functions.
 */
static inline void board_init(int mode /* ignored */) {
  BOARD_BootClockRUN();

  // enable led/button clock
  CLOCK_EnableClock(BOARD_LED0_PORT_CLOCK);
  CLOCK_EnableClock(BOARD_LED1_PORT_CLOCK);
  CLOCK_EnableClock(BOARD_LED2_PORT_CLOCK);

  CLOCK_EnableClock(BOARD_BUTTON0_PORT_CLOCK);
  CLOCK_EnableClock(BOARD_BUTTON1_PORT_CLOCK);

  // configure pins
  const gpio_pin_config_t OUTFALSE = {kGPIO_DigitalOutput, 0};
  const gpio_pin_config_t IN = {kGPIO_DigitalInput, false};

  PORT_SetPinMux(BOARD_LED0_PORT, BOARD_LED0_PIN, kPORT_MuxAsGpio);
  GPIO_PinInit(BOARD_LED0_GPIO, BOARD_LED0_PIN, &OUTFALSE);
  GPIO_WritePinOutput(BOARD_LED0_GPIO, BOARD_LED0_PIN, true);

  PORT_SetPinMux(BOARD_LED1_PORT, BOARD_LED1_PIN, kPORT_MuxAsGpio);
  GPIO_PinInit(BOARD_LED1_GPIO, BOARD_LED1_PIN, &OUTFALSE);
  GPIO_WritePinOutput(BOARD_LED1_GPIO, BOARD_LED1_PIN, true);

  PORT_SetPinMux(BOARD_LED2_PORT, BOARD_LED2_PIN, kPORT_MuxAsGpio);
  GPIO_PinInit(BOARD_LED2_GPIO, BOARD_LED2_PIN, &OUTFALSE);
  GPIO_WritePinOutput(BOARD_LED2_GPIO, BOARD_LED2_PIN, true);

  PORT_SetPinMux(BOARD_BUTTON0_PORT, BOARD_BUTTON0_PIN, kPORT_MuxAsGpio);
  GPIO_PinInit(BOARD_BUTTON0_GPIO, BOARD_BUTTON0_PIN, &IN);

  PORT_SetPinMux(BOARD_BUTTON1_PORT, BOARD_BUTTON1_PIN, kPORT_MuxAsGpio);
  GPIO_PinInit(BOARD_BUTTON1_GPIO, BOARD_BUTTON1_PIN, &IN);
}

/*!
 * @brief Disable NMI for this board (PTA4) and make it work as a normal input pin.
 */
static inline void board_nmi_disable() {
  const gpio_pin_config_t IN = {kGPIO_DigitalInput, false};

  CLOCK_EnableClock(BOARD_NMI_PORT_CLOCK);
  PORT_SetPinMux(BOARD_NMI_PORT, BOARD_NMI_PIN, kPORT_MuxAsGpio);
  GPIO_PinInit(BOARD_NMI_GPIO, BOARD_NMI_PIN, &IN);
}

/*!
 * @brief Initialize the Debug Console with specified baud rate.
 * @param baud the baud rate of the debug console
 */
static inline status_t board_console_init(uint32_t baud) {
  CLOCK_SetLpuartClock(2);
  CLOCK_EnableClock(BOARD_DEBUG_PORT_CLOCK);
  PORT_SetPinMux(BOARD_DEBUG_PORT, BOARD_DEBUG_TX_PIN, BOARD_DEBUG_TX_ALT);
  PORT_SetPinMux(BOARD_DEBUG_PORT, BOARD_DEBUG_RX_PIN, BOARD_DEBUG_RX_ALT);
  return DbgConsole_Init((uint32_t) BOARD_DEBUG_UART, baud, BOARD_DEBUG_TYPE, BOARD_DEBUG_CLK_FREQ);
}

/*!
 * @brief Used to enable interrupts in the INTMUX (NOT USED FOR K82F)
 * @param irq the irq to enable
 */
static inline void enable_interrupt(IRQn_Type irq) {
  EnableIRQ(irq);
}

#ifdef __cplusplus
}
#endif

#endif // _UBIRCH_BOARD_H_
