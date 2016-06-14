/*!
 * @file
 * @brief SSD1306 chip driver for OLED displays.
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

#ifndef _UBIRCH_SSD1306_H_
#define _UBIRCH_SSD1306_H_

// the default controller device address
#define OLED_DEVICE_ADDRESS     0x3d //!< SSD1306 device address

// =================================
#define OLED_ADDRESSING_MODE    0x20 //!< current addressing mode
#define OLED_COLUMN_ADDRESS     0x21 //!< column address
#define OLED_PAGE_ADDRESS       0x22 //!< page address

#define OLED_ADDR_MODE_PAGE     0b10 //!< address mode page
#define OLED_ADDR_MODE_HORIZ    0b00 //!< address mode horizontal
#define OLED_ADDR_MODE_VERT     0b01 //!< address mode vertical

#define OLED_SCROLL_RIGHT       0x26 //!< scroll right
#define OLED_SCROLL_LEFT        0x27 //!< scroll left
#define OLED_SCROLL_DIAG_RIGHT  0x29 //!< scroll diagonal right
#define OLED_SCROLL_DIAG_LEFT   0x2A //!< scroll diagonal left

#define OLED_SCROLL_STOP        0x2E //!< stop scrolling
#define OLED_SCROLL_START       0x2F //!< start scrolling

#define OLED_START_LINE         0x40 //!< set start line

#define OLED_CONTRAST           0x81 //!< set contrast
#define OLED_CHARGE_PUMP        0x8D //!< set charge pump

#define OLED_SEGMENT_REMAP0     0xA0 //!< set segment remap 0
#define OLED_SEGMENT_REMAP1     0xA1 //!< set segment remap 1

#define OLED_DISPLAY_RESUME     0xA4 //!< resume display
#define OLED_DISPLAY_ALL_ON     0xA5 //!< set display all on

// display mode
#define OLED_DISPLAY_NORMAL     0xA6 //!< normal display mode
#define OLED_DISPLAY_INVERSE    0xA7 //!< inverted display mode
#define OLED_MULTIPLEX_RATIO    0xA8 //!< aspect ratio setting

// display on off in general
#define OLED_DISPLAY_OFF        0xAE //!< switch off display
#define OLED_DISPLAY_ON         0xAF //!< switch on display

// page start address for page addressing mode
#define OLED_PAGE_ADDR_START    0xB0 //!< page start address: 0xB0-0xB7

// output scan direction
#define OLED_SCAN_NORMAL        0xC0 //!< output scan direction normal (top-down)
#define OLED_SCAN_REVERSE       0xC8 //!< output scan direction inverted (down-top)

// display offset (double byte command)
#define OLED_DISPLAY_OFFSET     0xD3 //!< display offset of the actual OLED

#define OLED_CLOCK_DIV_FREQ     0xD5 //!< arg: B0-3 clock divide ratio, B4-7 frequency
#define OLED_PRECHARGE_PERIOD   0xD9 //!< pre-charge period
#define OLED_COM_PIN_CONFIG     0xDA //!< COM pin configuration

#define OLED_VCOM_DESELECT      0xDB //!< VCOM deselect
// =================================

#include <fsl_gpio.h>

/*!
 * Reset the controller.
 */
void ssd1306_reset(GPIO_Type *gpio, uint32_t reset_pin);

/*!
 * Send a command to the OLED display controller.
 * @param address the controller i2c address
 * @param command the command byte
 */
void ssd1306_cmd(uint8_t address, uint8_t command);

/*!
 * Send data to the OLED display controller.
 * @param address the controller i2c address
 * @param data pointer to the data array
 * @param size size of the data array
 */
void ssd1306_data(uint8_t address, uint8_t *data, size_t size);

/*!
 * Clear the display.
 * @param address the controller i2c address
 */
void ssd1306_clear(uint8_t address);

#endif // _UBIRCH_SSD1306_H_
