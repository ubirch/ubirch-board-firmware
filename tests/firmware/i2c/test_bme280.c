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
#include <ubirch/timer.h>
#include <ubirch/i2c.h>

static void test_read_temperature() {
  const int temp = bme280_temperature();
  PRINTF("- temperature: %.2f\r\n", temp / 100.0f);
  assert(temp != 0);
}

static void test_read_pressure() {
  const unsigned int pressure = bme280_pressure();
  PRINTF("- pressure:    %.2fhPa\r\n", pressure / 100.0f);
  assert(pressure != 0);
}

static void test_read_humidity() {
  const unsigned int humidity = bme280_humidity();
  PRINTF("- humidity:    %.3f\r\n", humidity / 1024.0f);
  assert(humidity != 0);
}

static void test_read_altitude() {
  const float altitude = bme280_altitude(1013.25);
  PRINTF("- altitude:    %.3f\r\n", altitude);
  assert(altitude != 0);
}

int test_bme280() {
  PRINTF("= BME280 Test\r\n");

  assert(bme280_init());

  test_read_temperature();
  test_read_pressure();
  test_read_humidity();
  test_read_altitude();

  assert(bme280_power_mode(BME280_Sleep));

  return 0;
}
