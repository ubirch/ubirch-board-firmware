/**
 * I2C bus scan.
 *
 * Scans the bus by addressing each of the possible device ids.
 * Reports a device as found if it responds with an ACK. This
 * is useful if we have a new device and want to check if it
 * is there and active.
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

#include <board.h>
#include <stdio.h>
#include <ubirch/i2c.h>
#include <ubirch/isl29125.h>
#include "test.h"

int test_isl29125();

static const i2c_config_t i2c_config = {
  .i2c = BOARD_I2C,
  .i2c_clock = BOARD_I2C_CLOCK,
  .port = BOARD_I2C_PORT,
  .mux = BOARD_I2C_ALT,
  .port_clock = BOARD_I2C_PORT_CLOCK,
  .SCL = BOARD_I2C_SCL_PIN,
  .SDA = BOARD_I2C_SDA_PIN,
  .baud = I2C_FULL_SPEED
};

int test_i2c(void) {
  i2c_init(i2c_config);
  assert(i2c_ping(0x00) == kStatus_Success);

  for (uint8_t address = 0x01; address <= 0x7f; address++) {
    status_t status = i2c_ping(address);
    assert(status == kStatus_Success || status == kStatus_I2C_Nak);
    if (status == kStatus_Success) {
      switch (address) {
        case ISL_DEVICE_ADDRESS: {
          TEST("ISL29125", test_isl29125());
          break;
        }
        default: {
          PRINTF("- I2C: 0x%02x: unknown device detected\r\n", address);
        }
      }
    }
  }

  i2c_deinit();

  return 0;
}
