/*!
 * @brief Driver for the RGB sensor ISL29125.
 *
 * @author Matthias L. Jugel
 * @date 2016-04-01
 *
 * Copyright 2016 ubirch GmbH (https://ubirch.com)
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

#include <utilities/fsl_debug_console.h>
#include <ubirch/i2c.h>
#include "isl29125.h"

bool isl_set(uint8_t reg, uint8_t data) {
  return i2c_write(ISL_DEVICE_ADDRESS, reg, &data, 1) == kStatus_Success;
}

uint8_t isl_get(uint8_t reg) {
  return i2c_read_reg(ISL_DEVICE_ADDRESS, reg);
}

bool isl_reset(void) {
  // check device is there
  uint8_t device_id = i2c_read_reg(ISL_DEVICE_ADDRESS, ISL_R_DEVICE_ID);
  if (device_id != ISL_DEVICE_ID) {
#ifndef NDEBUG
    PRINTF("device id: 0x%02x (should be 0x7d)\r\n", device_id);
#endif
    return false;
  }

  // reset and make sure we are actually done resetting
  uint8_t reset = ISL_RESET;
  return i2c_write(ISL_DEVICE_ADDRESS, 0x00, &reset, 1) == kStatus_Success;
}

// TODO: burst read 48 bit
void isl_read_rgb48(rgb48_t *rgb48) {
  rgb48->red = i2c_read_reg16(ISL_DEVICE_ADDRESS, ISL_R_RED_L);
  rgb48->green = i2c_read_reg16(ISL_DEVICE_ADDRESS, ISL_R_GREEN_L);
  rgb48->blue = i2c_read_reg16(ISL_DEVICE_ADDRESS, ISL_R_BLUE_L);
}

void isl_read_rgb24(rgb24_t *rgb24) {
  rgb48_t rgb48;
  const int shift = (i2c_read_reg(ISL_DEVICE_ADDRESS, ISL_R_COLOR_MODE) & ISL_MODE_12BIT) ? 4 : 8;

  isl_read_rgb48(&rgb48);

  rgb24->red = (uint8_t) (rgb48.red >> shift);
  rgb24->green = (uint8_t) (rgb48.green >> shift);
  rgb24->blue = (uint8_t) (rgb48.blue >> shift);
}
