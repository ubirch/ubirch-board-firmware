/*!
 * @brief FRDM-K82F board specific initialization functions
 *
 * Contains the declaration of initialization functions specific for the board.
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

#ifndef _UBIRCH_BOARD_H_
#define _UBIRCH_BOARD_H_

// processor specific includes
#include <MKL82Z7/include/MKL82Z7.h>
#include <MKL82Z7/include/MKL82Z7_features.h>
#include <core_cm0plus.h>

// peripherals

#include <fsl_port_hal.h>
#include <fsl_gpio_driver.h>
#include <fsl_debug_console.h>

// board specific includes
#include "frdm_kl82z.h"
#include "clock_config.h"

enum status_t {
    kStatus_Success = 0
};

const gpio_output_pin_user_config_t LED_PINS[] = {
  {
    .pinName = GPIO_MAKE_PIN(BOARD_LED0_GPIO, BOARD_LED0_PIN),
    .config.outputLogic = 1,
    .config.slewRate = kPortSlowSlewRate,
    .config.isOpenDrainEnabled = false,
  },
  {
    .pinName = GPIO_MAKE_PIN(BOARD_LED1_GPIO, BOARD_LED1_PIN),
    .config.outputLogic = 1,
    .config.slewRate = kPortSlowSlewRate,
    .config.isOpenDrainEnabled = false,
  },
  {
    .pinName = GPIO_MAKE_PIN(BOARD_LED2_GPIO, BOARD_LED2_PIN),
    .config.outputLogic = 1,
    .config.slewRate = kPortSlowSlewRate,
    .config.isOpenDrainEnabled = false,
  },
  {
    .pinName = GPIO_PINS_OUT_OF_RANGE,
  }
};

const gpio_input_pin_user_config_t BUTTON_PINS[] = {
  {
    .pinName = GPIO_MAKE_PIN(BOARD_BUTTON0_GPIO, BOARD_BUTTON0_PIN),
    .config.isPullEnable = true,
    .config.pullSelect = kPortPullUp,
    .config.isPassiveFilterEnabled = false,
    .config.interrupt = kPortIntDisabled
  },
  {
    .pinName = GPIO_MAKE_PIN(BOARD_BUTTON1_GPIO, BOARD_BUTTON2_PIN),
    .config.isPullEnable = true,
    .config.pullSelect = kPortPullUp,
    .config.isPassiveFilterEnabled = false,
    .config.interrupt = kPortIntDisabled
  },
  {
    .pinName = GPIO_MAKE_PIN(BOARD_BUTTON2_GPIO, BOARD_BUTTON2_PIN),
    .config.isPullEnable = true,
    .config.pullSelect = kPortPullUp,
    .config.isPassiveFilterEnabled = false,
    .config.interrupt = kPortIntDisabled
  },
  {
    .pinName = GPIO_PINS_OUT_OF_RANGE,
  }
};

const gpio_input_pin_user_config_t NMI_PIN = {
    .pinName = GPIO_MAKE_PIN(BOARD_NMI_GPIO, BOARD_NMI_PIN),
    .config.isPullEnable = true,
    .config.pullSelect = kPortPullUp,
    .config.isPassiveFilterEnabled = false,
    .config.interrupt = kPortIntDisabled
};

/*!
 * @brief Initialize the basic board functions.
 */
static inline void board_init() {
  BOARD_ClockInit();

  // enable led/button clock
  CLOCK_SYS_EnablePortClock(BOARD_LED0_PORT_CLOCK);
  CLOCK_SYS_EnablePortClock(BOARD_LED1_PORT_CLOCK);
  CLOCK_SYS_EnablePortClock(BOARD_LED2_PORT_CLOCK);

  CLOCK_SYS_EnablePortClock(BOARD_BUTTON0_PORT_CLOCK);
  CLOCK_SYS_EnablePortClock(BOARD_BUTTON1_PORT_CLOCK);
  CLOCK_SYS_EnablePortClock(BOARD_BUTTON2_PORT_CLOCK);

  PORT_HAL_SetMuxMode(BOARD_LED0_PORT, BOARD_LED0_PIN, kPortMuxAsGpio);
  GPIO_DRV_OutputPinInit(&LED_PINS[0]);
  GPIO_DRV_WritePinOutput(LED_PINS[0].pinName, true);

  PORT_HAL_SetMuxMode(BOARD_LED1_PORT, BOARD_LED1_PIN, kPortMuxAsGpio);
  GPIO_DRV_OutputPinInit(&LED_PINS[1]);
  GPIO_DRV_WritePinOutput(LED_PINS[1].pinName, true);

  PORT_HAL_SetMuxMode(BOARD_LED2_PORT, BOARD_LED2_PIN, kPortMuxAsGpio);
  GPIO_DRV_OutputPinInit(&LED_PINS[2]);
  GPIO_DRV_WritePinOutput(LED_PINS[2].pinName, true);

  PORT_HAL_SetMuxMode(BOARD_BUTTON0_PORT, BOARD_BUTTON0_PIN, kPortMuxAsGpio);
  GPIO_DRV_InputPinInit(&BUTTON_PINS[0]);

  PORT_HAL_SetMuxMode(BOARD_BUTTON1_PORT, BOARD_BUTTON1_PIN, kPortMuxAsGpio);
  GPIO_DRV_InputPinInit(&BUTTON_PINS[1]);
}

/*!
 * @brief Disable NMI for this board (PTA4) and make it work as a normal input pin.
 */
static inline void board_nmi_disable() {
  CLOCK_SYS_EnablePortClock(BOARD_NMI_PORT_CLOCK);
  PORT_HAL_SetMuxMode(BOARD_NMI_PORT, BOARD_NMI_PIN, kPortMuxAsGpio);
  GPIO_DRV_InputPinInit(&NMI_PIN);
}

/*!
 * @brief Initialize the Debug Console with specified baud rate.
 * @param baud the baud rate of the debug console
 */
static inline status_t board_console_init(uint32_t baud) {

#if (CLOCK_INIT_CONFIG == CLOCK_VLPR)
  CLOCK_SYS_SetLpuartSrc(BOARD_DEBUG_INSTANCE, kClockLpuartSrcMcgIrClk);
#else
  CLOCK_SYS_SetLpuartSrc(BOARD_DEBUG_UART_INSTANCE, BOARD_LPUART_CLOCK_SOURCE);
#endif

  CLOCK_SYS_EnablePortClock(BOARD_DEBUG_PORT_CLOCK);
  PORT_HAL_SetMuxMode(BOARD_DEBUG_PORT,BOARD_DEBUG_TX_PIN,kPortMuxAlt3);
  PORT_HAL_SetMuxMode(BOARD_DEBUG_PORT,BOARD_DEBUG_RX_PIN,kPortMuxAlt3);

  return DbgConsole_Init(BOARD_DEBUG_INSTANCE, baud, BOARD_DEBUG_TYPE, kDebugConsoleLPUART);
}

#endif // _UBIRCH_BOARD_H_
