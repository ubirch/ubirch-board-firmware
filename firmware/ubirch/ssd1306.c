/*
 * SSD1306 OLD display driver.
 *
 * @author Matthias L. Jugel
 *
 * Copyright 2015 ubirch GmbH (http://www.ubirch.com)
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

#include <ubirch/i2c.h>
#include <drivers/fsl_gpio.h>
#include <drivers/fsl_port.h>
#include "timer.h"
#include "ssd1306.h"

void ssd1306_reset(GPIO_Type *gpio, uint32_t reset_pin) {
  const gpio_pin_config_t OUTFALSE = {kGPIO_DigitalOutput, false};
  GPIO_PinInit(gpio, reset_pin, &OUTFALSE);

  // reset sequence
  GPIO_WritePinOutput(gpio, reset_pin, true);
  delay_us(100);
  GPIO_WritePinOutput(gpio, reset_pin, false);
  delay_us(100);
  GPIO_WritePinOutput(gpio, reset_pin, true);

  // software configuration according to specs
  ssd1306_cmd(OLED_DEVICE_ADDRESS, OLED_DISPLAY_OFF);
  ssd1306_cmd(OLED_DEVICE_ADDRESS, OLED_CLOCK_DIV_FREQ);
  ssd1306_cmd(OLED_DEVICE_ADDRESS, 0b100000); // 0x80: 1000 (freq) 00 (divider)
  ssd1306_cmd(OLED_DEVICE_ADDRESS, OLED_MULTIPLEX_RATIO);
  ssd1306_cmd(OLED_DEVICE_ADDRESS, 0x2F);
  ssd1306_cmd(OLED_DEVICE_ADDRESS, OLED_DISPLAY_OFFSET);
  ssd1306_cmd(OLED_DEVICE_ADDRESS, 0x00);
  ssd1306_cmd(OLED_DEVICE_ADDRESS, OLED_START_LINE | 0x00);
  ssd1306_cmd(OLED_DEVICE_ADDRESS, OLED_CHARGE_PUMP);
  ssd1306_cmd(OLED_DEVICE_ADDRESS, 0x14);
  ssd1306_cmd(OLED_DEVICE_ADDRESS, OLED_SCAN_REVERSE);
  ssd1306_cmd(OLED_DEVICE_ADDRESS, OLED_SEGMENT_REMAP1);
  ssd1306_cmd(OLED_DEVICE_ADDRESS, OLED_COM_PIN_CONFIG);
  ssd1306_cmd(OLED_DEVICE_ADDRESS, 0x12);
  ssd1306_cmd(OLED_DEVICE_ADDRESS, OLED_CONTRAST);
  ssd1306_cmd(OLED_DEVICE_ADDRESS, 0x10);
  ssd1306_cmd(OLED_DEVICE_ADDRESS, OLED_PRECHARGE_PERIOD);
  ssd1306_cmd(OLED_DEVICE_ADDRESS, 0x22);
  ssd1306_cmd(OLED_DEVICE_ADDRESS, OLED_VCOM_DESELECT);
  ssd1306_cmd(OLED_DEVICE_ADDRESS, 0x00);
  ssd1306_cmd(OLED_DEVICE_ADDRESS, OLED_DISPLAY_RESUME);
  ssd1306_cmd(OLED_DEVICE_ADDRESS, OLED_DISPLAY_NORMAL);

  // set addressing mode to horizontal wrapping around
  ssd1306_cmd(OLED_DEVICE_ADDRESS, OLED_ADDRESSING_MODE);
  ssd1306_cmd(OLED_DEVICE_ADDRESS, OLED_ADDR_MODE_HORIZ);

}

void ssd1306_cmd(uint8_t address, uint8_t command) {
  i2c_write(address, 0x00, &command, 1);
}


void ssd1306_data(uint8_t address, uint8_t *data, size_t size) {
  i2c_write(address, 0x40, data, size);
}

//== higher level functions ================================

void ssd1306_clear(uint8_t address) {
  ssd1306_cmd(address, 0x21);
  ssd1306_cmd(address, 0);
  ssd1306_cmd(address, 127);

  ssd1306_cmd(address, 0x22);
  ssd1306_cmd(address, 0);
  ssd1306_cmd(address, 7);

  uint8_t cleared_buffer[128];
  memset(cleared_buffer, 0xff, 128);
  for (uint8_t page = 0; page < 8; page++) {
    ssd1306_cmd(address, (uint8_t) 0xb0 | page);
    ssd1306_cmd(address, 0x00);
    i2c_write(address, 0x40, cleared_buffer, 128);
  }
}

