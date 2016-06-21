/**
 * @brief  Bosch BMP180 temperature and pressure sensor driver.
 *
 * @author Matthias L. Jugel
 * @date   2016-06-20
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
#include <fsl_trng.h>
#include <ubirch/i2c.h>
#include <ubirch/timer.h>
#include "bosch_sensortec/bmp180.h"
#include "bmp180.h"

static struct bmp180_t bmp180;

s8 bmp180_bus_read(uint8_t address, uint8_t reg, uint8_t *data, uint8_t size) {
  return (s8) (i2c_read(address, reg, data, size) == kStatus_Success ? 0 : -1);
}

s8 bmp180_bus_write(uint8_t address, uint8_t reg, uint8_t *data, uint8_t size) {
  return (s8) (i2c_write(address, reg, data, size) == kStatus_Success ? 0 : -1);
}

bool bmp180_init(void) {
  // check that this is the correct chip, they are not all fully compatible
  if(i2c_read_reg(BMP180_DEVICE_ADDRESS, BMP180_CHIP_ID_REG) != BMP180_CHIP_ID) return false;

  bmp180.dev_addr = BMP180_DEVICE_ADDRESS;
  bmp180.bus_read = bmp180_bus_read;
  bmp180.bus_write = bmp180_bus_write;
  bmp180.delay_msec = (void (*)(u32)) delay;

  int result = _bmp180_init(&bmp180);
  result += bmp180_get_calib_param();

  return !result;
}

int32_t bmp180_temperature(void) {
  return bmp180_get_temperature(bmp180_get_uncomp_temperature());
}

int32_t bmp180_pressure(void) {
  bmp180_get_uncomp_temperature();
  return bmp180_get_pressure(bmp180_get_uncomp_pressure());
}

extern float bmp180_altitude(uint32_t sea_level_pressure, uint32_t pressure);

extern float bmp180_pressure_sea_level(uint32_t pressure, float altitude);

