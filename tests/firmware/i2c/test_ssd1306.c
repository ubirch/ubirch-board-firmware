/**
 * Humidity Sensor test (BMPx80).
 *
 * Tests functionality of the BMPx80 sensor library.
 *
 * @author Matthias L. Jugel
 * @date 2016-06-16
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

#include <stdint.h>
#include <stdio.h>
#include <board.h>
#include <ubirch/i2c/ssd1306.h>
#include "../test.h"

int test_ssd1306() {
  PRINTF("= SSD1306 Test\r\n");
  ssd1306_reset(GPIOB, 2);

  ssd1306_clear(OLED_DEVICE_ADDRESS);

  ssd1306_cmd(OLED_DEVICE_ADDRESS, OLED_DISPLAY_ON);

  // set the display constraints
  ssd1306_cmd(OLED_DEVICE_ADDRESS, OLED_PAGE_ADDRESS);
  ssd1306_cmd(OLED_DEVICE_ADDRESS, 0x00);
  ssd1306_cmd(OLED_DEVICE_ADDRESS, 0x05);

  ssd1306_cmd(OLED_DEVICE_ADDRESS, OLED_COLUMN_ADDRESS);
  ssd1306_cmd(OLED_DEVICE_ADDRESS, 0x20);
  ssd1306_cmd(OLED_DEVICE_ADDRESS, 0x20 + 63);

  uint8_t buffer[65 * 6];
  memset(buffer, 0x00, 64 * 6);
  ssd1306_data(OLED_DEVICE_ADDRESS, buffer, 64 * 6);

  return 0;
}
