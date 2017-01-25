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
 * @copyright &copy; 2016 ubirch GmbH (https://ubirch.com)
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

#include <board.h>
#include <ubirch/i2c.h>
#include <ubirch/i2c/isl29125.h>
#include <ubirch/i2c/bmp180.h>
#include <ubirch/i2c/bme280.h>
#include <ubirch/i2c/ssd1306.h>
#include <ubirch/timer.h>
#include "test.h"

int test_bmp180();
int test_bme280();
int test_isl29125();
int test_ssd1306();

void reset_ssd1306() {
  CLOCK_EnableClock(kCLOCK_PortB);
  PORT_SetPinMux(PORTB, 2, kPORT_MuxAsGpio);

  const gpio_pin_config_t OUTFALSE = {kGPIO_DigitalOutput, false};
  GPIO_PinInit(GPIOB, 2, &OUTFALSE);

  // ssd1306 reset sequence
  GPIO_WritePinOutput(GPIOB, 2, true);
  delay_us(100);
  GPIO_WritePinOutput(GPIOB, 2, false);
  delay_us(100);
  GPIO_WritePinOutput(GPIOB, 2, true);
}

int test_i2c(void) {
  i2c_init(&i2c_config_default);
  ASSERT_EQUALS(i2c_ping(0x00), kStatus_Success);

  // do some basic initialization in case an OLED is attached
  reset_ssd1306();

  for (uint8_t address = 0x01; address <= 0x7f; address++) {
    status_t status = i2c_ping(address);
    ASSERT_TRUE(status == kStatus_Success || status == kStatus_I2C_Addr_Nak);
    if (status == kStatus_Success) {
      switch (address) {
        case ISL_DEVICE_ADDRESS: {
          TEST("ISL29125", test_isl29125());
          break;
        }
        case OLED_DEVICE_ADDRESS: {
          TEST("SSD1306", test_ssd1306());
          break;
        }
        // BMP180, BME280
        case BMP180_DEVICE_ADDRESS:
        case BMP180_DEVICE_ADDRESS - 1: {
          const uint8_t chip_id = i2c_read_reg(address, (0xD0));
          switch(chip_id) {
            case BMP180_CHIP_ID: {
              TEST("BMP180", test_bmp180());
              break;
            }
            case BME280_CHIP_ID: {
              TEST("BME280", test_bme280());
              break;
            }
            default:
              PRINTF("- I2C: Bosch Sensor 0x%02x: unknown chip ID: 0x%02x\r\n", address, chip_id);
          }
          break;
        }
        default:
          PRINTF("- I2C: 0x%02x: unknown device detected\r\n", address);
      }
    }
  }

  i2c_deinit();

  return 0;
}

