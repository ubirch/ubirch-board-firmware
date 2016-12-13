/*
 * ubirch#1 r0.3k0.1 board specific initialization functions
 *
 * This file just contains some references to the initialization and support
 * functions that are defined as inlinebale in the corresponding header file.
 *
 * The declaration of extern here will ensure that the compiler may choose
 * when to inline and when to copy the code.
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

#include <fsl_lptmr.h>
#include "board.h"

static void (*runBootloader)(void *arg);

extern void board_init();
extern void board_nmi_disable();
extern status_t board_console_init(uint32_t baud);
extern void enable_interrupt(IRQn_Type irq);

void board_install_bootloader_hook(void) {
  // install bootloader hook
  uint32_t runBootloaderAddress = **(uint32_t **) (0x1c00001c);
  runBootloader = (void (*)(void *arg)) runBootloaderAddress;
}

void NMI_Handler(void) {
  runBootloader(NULL);
}

volatile uint32_t lptmrCounter = 0U;
volatile bool button_press_status = false;

void BOARD_LPTMR_HANDLER(void)
{
  LPTMR_ClearStatusFlags(LPTMR0, kLPTMR_TimerCompareFlag);
  lptmrCounter++;
  /*
   * Workaround for TWR-KV58: because write buffer is enabled, adding
   * memory barrier instructions to make sure clearing interrupt flag completed
   * before go out ISR
   */
  __DSB();
  __ISB();
}

int Board_Reset_Bootloader(void) {
  lptmr_config_t lptmrConfig;

  GPIO_ClearPinsInterruptFlags(BOARD_BUTTON0_GPIO, 0x00000001);

  if (button_press_status) {
    button_press_status = false;

    PRINTF("\r\nThe LPTimer count is %dsec\r\n", lptmrCounter);
    if (lptmrCounter > 1) {
      lptmrCounter = 0;
      PRINTF("\r\nBoard resetting\r\n");
      //Initiates a system reset request to reset the MCU.
      NVIC_SystemReset();
    }
    else {
      PRINTF("\r\nBoard going into bootloader mode\r\n");
      lptmrCounter = 0;
      runBootloader(NULL);
    }
    LPTMR_Deinit(LPTMR0);
    return 0;
  }

  if (!button_press_status) {
    button_press_status = true;

    GPIO_ClearPinsInterruptFlags(BOARD_BUTTON0_GPIO, 0x00000001);

    LPTMR_GetDefaultConfig(&lptmrConfig);
    LPTMR_Init(LPTMR0, &lptmrConfig);
    //The time period is set to one second
    // Every second a interrupt is triggered and increments the counter
    LPTMR_SetTimerPeriod(LPTMR0, USEC_TO_COUNT(LPTMR_USEC_COUNT, LPTMR_SOURCE_CLOCK));
    LPTMR_EnableInterrupts(LPTMR0, kLPTMR_TimerInterruptEnable);
    /* Enable at the NVIC */
    EnableIRQ(LPTMR0_IRQn);
    // Start the LPTIMER
    LPTMR_StartTimer(LPTMR0);
  }
  return 0;
}
