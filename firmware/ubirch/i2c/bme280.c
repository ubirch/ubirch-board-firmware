/**
 * @brief  Bosch BME280 temperature, relative humidity and pressure sensor driver.
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
#include "bosch_sensortec/bme280.h"
#include "bme280.h"
#include "bmp180.h"

static struct bme280_t bme280;

s8 bme280_bus_read(uint8_t address, uint8_t reg, uint8_t *data, uint8_t size) {
  return (s8) (i2c_read(address, reg, data, size) == kStatus_Success ? 0 : -1);
}

s8 bme280_bus_write(uint8_t address, uint8_t reg, uint8_t *data, uint8_t size) {
  return (s8) (i2c_write(address, reg, data, size) == kStatus_Success ? 0 : -1);
}

bool bme280_init(void) {
  bme280.dev_addr = BME280_DEVICE_ADDRESS;

  // check that this is the correct chip, they are not all fully compatible
  if (i2c_read_reg(bme280.dev_addr, BME280_CHIP_ID_REG) != BME280_CHIP_ID) {
    bme280.dev_addr = BME280_DEVICE_ADDRESS - 1;
    if (i2c_read_reg(bme280.dev_addr, BME280_CHIP_ID_REG) != BME280_CHIP_ID) return false;
  }

  bme280.bus_read = bme280_bus_read;
  bme280.bus_write = bme280_bus_write;
  bme280.delay_msec = (void (*)(u16)) delay;

  int result = _bme280_init(&bme280);
  result += bme280_power_mode(BME280_Normal);

  return !result;
}

bool bme280_power_mode(power_mode_t mode) {
  int result = 0;

  if(bme280_set_power_mode(mode)) return false;

  if(mode == BME280_Normal || mode == BME280_Forced) {
    result += bme280_set_power_mode(BME280_NORMAL_MODE);
    result += bme280_set_oversamp_humidity(BME280_OVERSAMP_16X);
    result += bme280_set_oversamp_pressure(BME280_OVERSAMP_16X);
    result += bme280_set_oversamp_temperature(BME280_OVERSAMP_16X);

    // do some initial waiting to make sure measurements are correct
    if (!result) {
      uint8_t wait_time;
      bme280_compute_wait_time(&wait_time);
      delay(wait_time);
    }
  }

  return !result;
}

int32_t bme280_temperature(void) {
  s32 temp_raw;
  if (bme280_read_uncomp_temperature(&temp_raw)) return -INT32_MAX;
  return bme280_compensate_temperature_int32(temp_raw);
}

uint32_t bme280_pressure(void) {
  s32 press_raw;
  bme280_temperature();
  if (bme280_read_uncomp_pressure(&press_raw)) return 0;
  return bme280_compensate_pressure_int32(press_raw);
}

uint32_t bme280_humidity(void) {
  s32 humidity_raw;
  bme280_temperature();
  if (bme280_read_uncomp_humidity(&humidity_raw)) return 0;
  return bme280_compensate_humidity_int32(humidity_raw);
}

bool bme280_sample(bme280_data_t *data) {
  s32 temp_raw, press_raw, humid_raw;

  if (bme280_read_uncomp_temperature(&temp_raw)) return false;
  if (bme280_read_uncomp_pressure(&press_raw)) return false;
  if (bme280_read_uncomp_humidity(&humid_raw)) return false;

  data->temperature = bme280_compensate_temperature_int32(temp_raw);
  data->pressure = bme280_compensate_pressure_int32(press_raw);
  data->humidity = bme280_compensate_humidity_int32(humid_raw);

  return true;
}


extern float bme280_altitude(uint32_t pressure_sea_level, uint32_t pressure);

extern float bme280_pressure_sea_level(uint32_t pressure, float altitude);


