/*!
 * @file
 * @brief ISL29125 RGB sensor driver.
 *
 * data sheet: http://www.intersil.com/content/dam/Intersil/documents/isl2/isl29125.pdf
 *
 * The library assumes that the i2c bus has been initialized already.
 * It will only communicate by issuing start/stop conditions and transmitting
 * command and data requests.
 *
 * The 36 bit color mode is supported. However, downsampling it to 24 bit will
 * incur an extra cost of reading the color mode register (i2c transmission) for
 * each color.
 *
 * @author Matthias L. Jugel
 * @date 2016-04-01
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
#ifndef _UBIRCH_ISL29125_H_
#define _UBIRCH_ISL29125_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ISL_DEVICE_ADDRESS  0x44      //!< ISL29125 device address
#define ISL_R_DEVICE_ID     0x00      //!< Device ID register
#define ISL_DEVICE_ID       0x7D      //!< Device ID (when reading ID register)

//! config registers
#define ISL_R_COLOR_MODE    0x01      //!< color mode settings register
#define ISL_R_FILTERING     0x02      //!< infrared filter settings register
#define ISL_R_INTERRUPT     0x03      //!< interrupt settings register

//! threshold registers
#define ISL_R_THRESHOLD_LL  0x04      //!< interrupt threshold register
#define ISL_R_THRESHOLD_LH  0x05      //!< interrupt threshold register
#define ISL_R_THRESHOLD_HL  0x06      //!< interrupt threshold register
#define ISL_R_THRESHOLD_HH  0x07      //!< interrupt threshold register

//! general status register
#define ISL_R_STATUS        0x08      //!< status register

//! the color registers
#define ISL_R_GREEN_L       0x09      //!< lower green register
#define ISL_R_GREEN_H       0x0A      //!< upper green register
#define ISL_R_RED_L         0x0B      //!< lower red register
#define ISL_R_RED_H         0x0C      //!< upper red register
#define ISL_R_BLUE_L        0x0D      //!< lower blue register
#define ISL_R_BLUE_H        0x0E      //!< upper blue register

//! reset command
#define ISL_RESET           0x46      //!< reset command

//! ISL_R_MODE values (B0-2 color mode selection)
#define ISL_MODE_POWERDOWN  0b000000  //!< power down device
#define ISL_MODE_GREEN      0b000001  //!< sample green values
#define ISL_MODE_RED        0b000010  //!< sample red values
#define ISL_MODE_BLUE       0b000011  //!< sample blue values
#define ISL_MODE_STANDBY    0b000100  //!< put device in standby
#define ISL_MODE_RGB        0b000101  //!< sample RGB
#define ISL_MODE_RG         0b000110  //!< sample RG
#define ISL_MODE_GB         0b000111  //!< sample GB

//! the lux selection also indicates sensible bit width (more bits offer more lux)
#define ISL_MODE_375LUX     0b000000  //!< B3 0 default is 375 lux
#define ISL_MODE_10KLUX     0b001000  //!< B3 1
#define ISL_MODE_16BIT      0b000000  //!< B4 0 16 bit operation
#define ISL_MODE_12BIT      0b010000  //!< B4 1 12 bit operation
#define ISL_MODE_INT_TRIG   0b100000  //!< set INT pin to trigger sampling

// ISL_R_FILTERING values (the actual filter values can be beweel 0x00 (NONE) and 0x3F (MAX)
#define ISL_FILTER_IR_RANGE 0x80      //!< default is not capping the values (top & bottom)
#define ISL_FILTER_IR_NONE  0x00      //!< no IR filtering (or low)
#define ISL_FILTER_IR_MAX   0x3F      //!< maximum IR filter

// ISL_R_INTERRPT values (how to trigger interrupts, if selected)
#define ISL_INTERRUPT_NONE  0b000000  //!< do not trigger interrupts
#define ISL_INTERRUPT_GREEN 0b000001  //!< trigger interrupt on green
#define ISL_INTERRUPT_RED   0b000010  //!< trigger interrupt on red
#define ISL_INTERRUPT_BLUE  0b000011  //!< trigger interrupt on blue

// IS_R_INTERRUPT values (interrupt persist control)
#define ISL_INT_PERSIST1    0b000000  //!< 1 integration cycle
#define ISL_INT_PERSIST2    0b000100  //!< 2 integration cycles
#define ISL_INT_PERSIST4    0b001000  //!< 4 integration cycles
#define ISL_INT_PERSIST8    0b001100  //!< 8 integration cycles

// the default mode triggers interrupts based on thresholds
#define ISL_INT_ON_THRSLD   0b000000  //!< threshold triggering
#define ISL_INT_ON_SAMPLE   0b010000  //!< trigger interrupt on a finished sample cycle

// status flags
#define ISL_STATUS_INT      0b000001  //!< interrupt was triggered
#define ISL_STATUS_ADC_DONE 0b000010  //!< conversion done
#define ISL_STATUS_BROWNOUT 0b000100  //!< power down or brownout occured
#define ISL_STATUS_GREEN    0b010000  //!< green being converted
#define ISL_STATUS_RED      0b100000  //!< red being converted
#define ISL_STATUS_BLUE     0b110000  //!< blue being converted

//! 48 bit color value (the maximum available), also used for 12 bit color reads
typedef struct {
    uint16_t red;   //!< 16 bit red component
    uint16_t green; //!< 16 bit green component
    uint16_t blue;  //!< 16 bit blue component
} rgb48_t;

//! 24 bit color value (downsampled)
typedef struct {
    uint8_t red;   //!< 8 bit red component
    uint8_t green; //!< 8 bit green component
    uint8_t blue;  //!< 8 bit blue component
} rgb24_t;

/*!
 * Set up a value in a register on the sensor.
 * @param reg the register to write
 * @param data the value to write
 */
bool isl_set(uint8_t reg, uint8_t data);

/*!
 * Get current setup from a register on the sensor.
 * @param reg the register to write
 */
uint8_t isl_get(uint8_t reg);

/*!
 * Reset the sensor.
 * A non-zero return indicates an error condition.
 */
bool isl_reset(void);

/*! Read 16 bit red component */
uint16_t isl_read_red(void);

/*! Read 16 bit green component */
uint16_t isl_read_green(void);

/*! Read 16 bit blue component */
uint16_t isl_read_blue(void);

/*! Read 8 bit red component */
uint8_t isl_read_red8(void);

/*! Read 16 bit green component */
uint8_t isl_read_green8(void);

/*! Read 16 bit blue component */
uint8_t isl_read_blue8(void);

/*!
 * Read full 48 bit color from sensor
 * @param rgb48 the color struct to store the color in
 */
void isl_read_rgb48(rgb48_t *rgb48);

/*!
 * Read sensor data as 24 bit RGB
 * @param rgb24 the color struct to store the color in
 */
void isl_read_rgb24(rgb24_t *rgb24);

#ifdef __cplusplus
}
#endif

#endif // _UBIRCH_ISL29125_H_
