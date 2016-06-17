/**
 * Bosch BMP180 temperature and pressure sensor driver.
 *
 * @author Matthias L. Jugel
 * @date   2016-06-16
 *
 * Copyright 2015 ubirch GmbH (https://ubirch.com)
 *
 * == LICENSE ==
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
 */

#include <stdint.h>
#include <fsl_trng.h>
#include <ubirch/i2c.h>
#include <ubirch/timer.h>
#include "bmp180.h"

static bmp180_calibration_t calibration;
static int32_t b5 = 0;

bool bmp180_set(bmp180_register_t reg, uint8_t data) {
  return i2c_write(BMP180_DEVICE_ADDRESS, reg, &data, 1) == kStatus_Success;
}

uint8_t bmp180_get(bmp180_register_t reg) {
  return i2c_read_reg(BMP180_DEVICE_ADDRESS, reg);
}

bool bmp180_reset(void) {
  return bmp180_set(SoftReset, cmdReset);
}

bool bmp180_read_calibration(bmp180_calibration_t *calibration) {
  // read calibration data from chip
  if (i2c_read(BMP180_DEVICE_ADDRESS, CalibrationAC1,
               (uint8_t *) calibration, sizeof(bmp180_calibration_t)) != kStatus_Success)
    return false;

  // revert the byte order (MSB comes first)
  for (int i = 0; i < 11; i++) {
    *((uint16_t *) calibration + i) = __builtin_bswap16(*((uint16_t *) calibration + i));
  }

  return true;
}

bool bmp180_init(void) {
  // check that this is the correct chip, they are not all fully compatible
  if(i2c_read_reg(BMP180_DEVICE_ADDRESS, ChipId) != BMP180_CHIP_ID) return false;

  // reset sensor
  if (!bmp180_reset()) return false;
  // we need to wait about 5ms to let the chip boot, then we can start reading
  delay(5);

  return bmp180_read_calibration(&calibration);
}

uint16_t bmp180_temperature_raw() {
  if (!bmp180_set(MeasurementCtrl, cmdMeasureTemp)) return 0;
  delay(5);
  uint8_t data[2];

  i2c_read(BMP180_DEVICE_ADDRESS, ResultMSB, data, 2);
  uint16_t value = (data[0] << 8 | data[1]);
  return value;
}

uint32_t bmp180_pressure_raw(bmp180_oversample_t oversample) {
  uint8_t data[3];

  if (!bmp180_set(MeasurementCtrl, cmdMeasurePressure + oversample)) return 0;
  delay((uint32_t) (2 + (3 << oversample)));

  i2c_read(BMP180_DEVICE_ADDRESS, ResultMSB, data, 3);
  uint32_t result = data[0] << 16 | data[1] << 8 | data[2];
  return result >> (8 - oversample);
}

int16_t bmp180_temperature() {
  uint16_t ut = bmp180_temperature_raw();
  int32_t x1 = (ut - calibration.AC6) * calibration.AC5 >> 15;
  if (x1 == 0 && calibration.MD == 0) return 0;

  int32_t x2 = (calibration.MC << 11) / (x1 + calibration.MD);
  b5 = x1 + x2;

  return (int16_t) ((b5 + 8) >> 4);
}

long bmp180_pressure(bmp180_oversample_t oversample) {
  bmp180_temperature(); // implicitly sets b5
  int32_t up = bmp180_pressure_raw(oversample);

  // calculate B3
  int32_t b6 = b5 - 4000;
  int32_t x1 = (calibration.B2 * ((b6 * b6) >> 12)) >> 11;
  int32_t x2 = (calibration.AC2 * b6) >> 11;
  int32_t x3 = x1 + x2;
  int32_t b3 = (((((int32_t) calibration.AC1) * 4 + x3) << oversample) + 2) >> 2;

  // calculate B4
  x1 = (calibration.AC3 * b6) >> 13;
  x2 = (calibration.B1 * ((b6 * b6) >> 12)) >> 16;
  x3 = ((x1 + x2) + 2) >> 2;
  uint32_t b4 = (calibration.AC4 * (uint32_t) (x3 + 32768)) >> 15;

  // calculate comprensated pressure
  uint32_t b7 = ((uint32_t) up - b3) * (50000 >> oversample);
  int32_t p = (b7 < 0x80000000) ? ((b7 * 2) / b4) : ((b7 / b4) * 2);
  x1 = (p >> 8) * (p >> 8);
  x1 = (x1 * BMP180_PARAM_MG) >> 16;
  x2 = (BMP180_PARAM_MH * p) >> 16;
  p += (x1 + x2 + BMP180_PARAM_MI) >> 4;

  return p;
}

