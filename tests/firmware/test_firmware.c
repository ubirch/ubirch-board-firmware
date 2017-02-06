/**
 * Test the Firmware code.
 *
 * Blinks at 100ms interval while working and 1s interval when finished successfully.
 * Also prints messages do debug output.
 *
 * @author Matthias L. Jugel
 * @date 2016-05-09
 *
 * @copyright &copy; 2016 ubirch GmbH (https://ubirch.com)
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

#include <stdint.h>
#include <board.h>
#include <ubirch/modem.h>
#include <stdio.h>
#include <ubirch/device.h>
#include <ubirch/dbgutil.h>
#include "test.h"
#include "bca.h"

int test_i2c();

int test_timer();

int test_rtc();

int test_ws2812b();

int test_sdhc_fat();

#if defined(BOARD_FRDM_KL82Z) || defined(BOARD_FRDM_K82F)
#  define LED BOARD_LED1
#elif defined(BOARD_UBIRCH_1R02) || defined(BOARD_UBIRCH_1R03)
#  define LED BOARD_LED0
#else
#  define LED(...)
#endif

uint32_t test_1ms_ticker = 0;
volatile bool on = true;

void SysTick_Handler() {
  test_1ms_ticker++;
  if (test_1ms_ticker % 500 == 0) on = !on;
  LED(on);
}

int main(void) {
  board_init(BOARD_MODE_RUN);
  board_console_init(BOARD_DEBUG_BAUD);

  // set the ticker to 1ms for comparison rates
  SysTick_Config(BOARD_SYSTICK_1MS);

  PRINTF("Testing Board and Firmware: " BOARD "\r\n\r\n");
  uint32_t uuid[4];
  device_uuid(uuid);
  printf("UUID: %08lX-%04lX-%04lX-%04lX-%04lX%08lX\r\n",
         uuid[0],                     // 8
         uuid[1] >> 16,               // 4
         uuid[1] & 0xFFFF,            // 4
         uuid[2] >> 16,               // 4
         uuid[2] & 0xFFFF, uuid[3]);  // 4+8

  modem_init();
  if(modem_enable()) {
    char imei[17];
    modem_imei(imei, 1000);
    printf("IMEI: %s\r\n", imei);
    modem_disable();
    PRINTF("= %-15s: %s\r\n", "MODEM", "OK");
  } else {
    PRINTF("= %-15s: %s\r\n", "MODEM", "FAIL");
  }

  TEST("SDHC", test_sdhc_fat());
  TEST("I2C", test_i2c());
  TEST("TIMER", test_timer());
  TEST("RTC", test_rtc());
  TEST("WS2812B", test_ws2812b());

  PRINTF("Test finished.\r\n");

  SysTick_Config(BOARD_SYSTICK_100MS);

  dbg_dump("BCA", (const uint8_t *) 0x000003c0, 0x33);
  PRINTF("Entering bootloader.\r\n");
  uint32_t runBootloaderAddress = **(uint32_t **) (0x1c00001c);
  ((void (*)(void *arg)) runBootloaderAddress)(NULL);

  return 0;
}

