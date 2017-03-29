/*!
 * @file
 * @brief __uBridge r1.0__ board specific initialization functions
 *
 * Contains the declaration of initialization functions specific for the board.
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

#ifndef _UBIRCH_BOARD_H_
#define _UBIRCH_BOARD_H_

// processor specific includes
#include <MKL82Z7.h>
#include <MKL82Z7_features.h>
#include <core_cm0plus.h>

// peripherals
#include <drivers/fsl_common.h>
#include <drivers/fsl_port.h>
#include <drivers/fsl_gpio.h>
#include <drivers/fsl_i2c.h>

#include <utilities/fsl_debug_console.h>

// board specific includes
#include "usense.h"
#include "clock_config.h"

// TODO move to appropriate place
#define LPTMR_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_LpoClk)
/* Define LPTMR microseconds counts value */
#define LPTMR_USEC_COUNT 1000000U
#define LPTMR0_IRQn LPTMR0_LPTMR1_IRQn
#define BOARD_LPTMR_HANDLER LPTMR0_LPTMR1_IRQHandler

/*!
 * @brief Installs a bootloader hook that runs on pressing the board button.
 *
 * Pressing the ubirch#1 board button will initiate an NMI interrupt which
 * is handled in a way that it tries to call the bootloader. This install
 * is necessary to make it work.
 *
 * If the hook was not installed before, flashing works by removing the USB,
 * pressing the button (keep pressed) and re-insert USB. Then the board will
 * be in bootloader mode.
 */
void board_install_bootloader_hook(void);

/*!
 * @brief Initialize the basic board functions.
 *
 * Initialized the clocks and basic board functionality. It also installs
 * the bootloader hook and configures the NMI interrupt to allow pressing
 * the button to enter bootloader mode.
 */
static inline void board_init(const int mode) {
  switch (mode) {
//    case BOARD_MODE_VLPR:
//      BOARD_BootClockVLPR();
//          break;
//    case BOARD_MODE_HSRUN:
//      BOARD_BootClockHSRUN();
//          break;
    case BOARD_MODE_RUN:
    default:
      BOARD_BootClockRUN();
          break;
  }

//  // configure pins
//  const gpio_pin_config_t OUTFALSE = {kGPIO_DigitalOutput, false};
//  const gpio_pin_config_t IN = {kGPIO_DigitalInput, false};

//  PORT_SetPinMux(BOARD_BUTTON0_PORT, BOARD_BUTTON0_PIN, kPORT_MuxAsGpio);
//  GPIO_PinInit(BOARD_BUTTON0_GPIO, BOARD_BUTTON0_PIN, &IN);
//  PORT_SetPinInterruptConfig(BOARD_BUTTON0_PORT, BOARD_BUTTON0_PIN, kPORT_InterruptEitherEdge);
//  EnableIRQ(BOARD_BUTTON0_IRQ);

  // installs a bootloader hook
  board_install_bootloader_hook();

  // Disable the NMI handler
  const gpio_pin_config_t NMI_IN = {kGPIO_DigitalInput, false};

  CLOCK_EnableClock(BOARD_NMI_PORT_CLOCK);
  PORT_SetPinMux(BOARD_NMI_PORT, BOARD_NMI_PIN, kPORT_MuxAsGpio);
  GPIO_PinInit(BOARD_NMI_GPIO, BOARD_NMI_PIN, &NMI_IN);
}

/*!
 * @brief Disable NMI for this board (PTA4) and make it work as a normal input pin.
 *
 * If you disable the NMI for the board, entering the bootloader via button press
 * won't work anymore. However, you can then just call <code>runBootloader(NULL);</code>
 * in your own button handler to have the same functionality.
 *
 * This may be useful if the button is required. Just use a long or short press to
 * determine when to call your own code or the bootloader.
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
  CLOCK_EnableClock(BOARD_DEBUG_PORT_CLOCK);
  PORT_SetPinMux(BOARD_DEBUG_PORT, BOARD_DEBUG_TX_PIN, BOARD_DEBUG_TX_ALT);
  PORT_SetPinMux(BOARD_DEBUG_PORT, BOARD_DEBUG_RX_PIN, BOARD_DEBUG_RX_ALT);

  // since the LPuart initialization depends very much on the source clock and its
  // frequency, we do a check here and retrieve the frequency accordingly
  // The CLOCK_SetLpuartSrc() is already done during clock init.
  uint32_t lpuart_src_freq;
  switch (SIM->SOPT2 & SIM_SOPT2_LPUARTSRC_MASK) {
    case SIM_SOPT2_LPUARTSRC(3U): {
      lpuart_src_freq = CLOCK_GetInternalRefClkFreq();
      break;
    }
    case SIM_SOPT2_LPUARTSRC(2U): {
      lpuart_src_freq = CLOCK_GetOsc0ErClkFreq();
      break;
    }
    case SIM_SOPT2_LPUARTSRC(1U): {
      lpuart_src_freq = CLOCK_GetPllFllSelClkFreq();
      break;
    }
    default: {
      // lpuart source clock is disabled
      return kStatus_Fail;
    }
  }

  return DbgConsole_Init((uint32_t) BOARD_DEBUG_UART, baud, BOARD_DEBUG_TYPE, lpuart_src_freq);
}

/*!
 * @brief Used to enable interrupts in the INTMUX (NOT USED FOR K82F)
 * @param irq the irq to enable
 */
static inline void enable_interrupt(IRQn_Type irq) {
  EnableIRQ(irq);
}

#endif // _UBIRCH_BOARD_H_
