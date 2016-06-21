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
#include <ubirch/i2c/bmp180.h>
#include <ubirch/i2c.h>
#include <ubirch/timer.h>
#include "../test.h"

#define PRESSURE_SEA_LEVEL 101325

static void test_read_temperature() {
  const int temp = bmp180_temperature();
  PRINTF("- %.2f℃\r\n", temp / 100.0f);
  ASSERT_TRUE(temp != 0);
}

static void test_read_pressure() {
  const int pressure = bmp180_pressure();
  PRINTF("- %.2fhPa\r\n", pressure / 100.0f);
  ASSERT_TRUE(pressure != 0);
}

static void test_read_altitude() {
  const float altitude = bmp180_altitude(PRESSURE_SEA_LEVEL, (uint32_t) bmp180_pressure());
  PRINTF("- %.3fm\r\n", altitude);
  ASSERT_TRUE(altitude != 0);
}

int test_bmp180() {
  PRINTF("= BMP180 Test\r\n");

  ASSERT_TRUE(bmp180_init());

  ASSERT_TRUE(BMP180_CHIP_ID == i2c_read_reg(BMP180_DEVICE_ADDRESS, BMP180_CHIP_ID_REG));

  test_read_temperature();
  test_read_pressure();
  test_read_altitude();

  return 0;
}
