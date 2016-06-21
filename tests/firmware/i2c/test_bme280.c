/**
 * Temperature, Pressure, Humidity Sensor test (BME280).
 *
 * Tests functionality of the BME280 sensor driver.
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
#include <board.h>
#include <ubirch/i2c/bme280.h>
#include "../test.h"

#define PRESSURE_SEA_LEVEL 101325

static void test_read_temperature() {
  const int temp = bme280_temperature();
  PRINTF("- %.2f℃\r\n", temp / 100.0f);
  ASSERT_TRUE(temp != 0);
}

static void test_read_pressure() {
  const unsigned int pressure = bme280_pressure();
  PRINTF("- %.2fhPa\r\n", pressure / 100.0f);
  ASSERT_TRUE(pressure != 0);
}

static void test_read_humidity() {
  const unsigned int humidity = bme280_humidity();
  PRINTF("- %.3f%%rH\r\n", humidity / 1024.0f);
  ASSERT_TRUE(humidity != 0);
}

static void test_read_altitude() {
  const float altitude = bme280_altitude(PRESSURE_SEA_LEVEL, bme280_pressure());
  PRINTF("- %.3fm\r\n", altitude);
  ASSERT_TRUE(altitude != 0);
}

int test_bme280() {
  PRINTF("= BME280 Test\r\n");

  ASSERT_TRUE(bme280_init());

  test_read_temperature();
  test_read_pressure();
  test_read_humidity();
  test_read_altitude();

  ASSERT_TRUE(bme280_power_mode(BME280_Sleep));

  return 0;
}
