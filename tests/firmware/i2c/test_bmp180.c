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

void test_reset() {
  assert(bmp180_reset());
  assert(bmp180_get(ResultMSB) == 0x80);
}

void test_read_raw_temperature() {
  long temp_raw = bmp180_temperature_raw();
  assert(temp_raw != 0x0000 && temp_raw != 0x8000);
}

void test_read_temperature() {
  long temp = bmp180_temperature();
  assert(temp != 0);
}

void test_read_raw_pressure() {
  long press_raw = bmp180_pressure_raw(NoOversample);
  assert(press_raw != 0);
}

void test_read_pressure() {
  long pressure = bmp180_pressure(Oversample3);
  assert(pressure != 0);
}

int test_bmp180() {
  assert(bmp180_init());

  assert(BMP180_CHIP_ID == bmp180_get(ChipId));

  test_reset();
  test_read_raw_temperature();
  test_read_temperature();
  test_read_raw_pressure();
  test_read_pressure();

  return 0;
}
