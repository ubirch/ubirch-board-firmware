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

#include "board.h"

static void (*runBootloader)(void *arg);

extern void board_init(const int mode);
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
