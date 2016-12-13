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
  PRINTF("BT from NMI\r\n");
  runBootloader(NULL);
}

#define LPTMR_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_LpoClk)
/* Define LPTMR microseconds counts value */
#define LPTMR_USEC_COUNT 1000000U
#define LPTMR0_IRQn LPTMR0_LPTMR1_IRQn
#define LPTMR_LED_HANDLER LPTMR0_LPTMR1_IRQHandler

volatile uint32_t lptmrCounter = 0U;
volatile bool our_p_flag = 0;

uint64_t previous_timer_count;
uint64_t current_timer_count;
int riq_count = 0;

void LPTMR_LED_HANDLER(void)
{
  LPTMR_ClearStatusFlags(LPTMR0, kLPTMR_TimerCompareFlag);
  lptmrCounter++;
//  LED_TOGGLE();
  /*
   * Workaround for TWR-KV58: because write buffer is enabled, adding
   * memory barrier instructions to make sure clearing interrupt flag completed
   * before go out ISR
   */
  __DSB();
  __ISB();
}

int run_the_bootloader(void) {

  riq_count++;
  PRINTF("BT from Button %d\r\n", riq_count);
  lptmr_config_t lptmrConfig;

   if (our_p_flag == 1) {
     our_p_flag = 0;
     current_timer_count = COUNT_TO_USEC(LPTMR_GetCurrentTimerCount(LPTMR0), LPTMR_SOURCE_CLOCK);

     PRINTF("\r\nthe timer count is %ld :: %d\r\n", previous_timer_count, current_timer_count);
//     if ((current_timer_count - previous_timer_count) > 1000001) {
     if (lptmrCounter >= 1) {
       PRINTF("reset\r\n");
      runBootloader(NULL);
    } else {
      PRINTF("Jus the boot loader\r\n");
      runBootloader(NULL);
    }
     LPTMR_Deinit(LPTMR0);
     return 0;
  }
  if (our_p_flag == 0) {
    our_p_flag = 1;
    GPIO_ClearPinsInterruptFlags(BOARD_BUTTON0_GPIO, 0x00000001);
    LPTMR_GetDefaultConfig(&lptmrConfig);
    LPTMR_Init(LPTMR0, &lptmrConfig);

    LPTMR_SetTimerPeriod(LPTMR0, USEC_TO_COUNT(LPTMR_USEC_COUNT, LPTMR_SOURCE_CLOCK));
    LPTMR_EnableInterrupts(LPTMR0, kLPTMR_TimerInterruptEnable);

    /* Enable at the NVIC */
    EnableIRQ(LPTMR0_IRQn);

    LPTMR_StartTimer(LPTMR0);

    PRINTF("lptimer set\r\n");
    previous_timer_count = COUNT_TO_USEC(LPTMR_GetCurrentTimerCount(LPTMR0), LPTMR_SOURCE_CLOCK);

    return 0;
  }
}
