/*!
 * @file
 * @brief  Bosch BMP180 temperature and pressure sensor driver.
 *
 * See [Datasheet](https://www.bosch-sensortec.com/bst/products/all_products/bmp180)
 * for a detailed description of the sensor and an example how to calculate the compensated
 * temperature as is done in this driver. The names of the parameters are used as they
 * are named in the data sheet.
 *
 * @author Matthias L. Jugel
 * @date   2016-06-16
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

#ifndef UBIRCH_FIRMWARE_BMP180_H
#define UBIRCH_FIRMWARE_BMP180_H

#ifdef __cplusplus
extern "C" {
#endif

#define BMP180_DEVICE_ADDRESS 0x77  //!< BMP180 device address (same address for multiple devices)
#define BMP180_CHIP_ID 0x55         //!< BMP180 chip id is fixed

//! BMP180 register map
typedef enum _bmp180_register {
    ChipId = 0xD0,          //!< device id register
    ChipVersion = 0xD1,     //!< chip version register
    SoftReset = 0xE0,       //!< reset device (write 0xB6)
    MeasurementCtrl = 0xF4, //!< measurement control register
    ResultMSB = 0xF6,       //!< measurement result value (MSB)
    ResultLSB = 0xF7,       //!< measurement result value (LSB)
    ResultXLSB = 0xF8,      //!< measurement result value (XLSB)

    CalibrationAC1 = 0xAA,  //!< calibration data EEPROM AC1
    CalibationAC2 = 0xAC,   //!< calibration data EEPROM AC2
    CalibationAC3 = 0xAE,   //!< calibration data EEPROM AC3
    CalibationAC4 = 0xB0,   //!< calibration data EEPROM AC4
    CalibationAC5 = 0xB2,   //!< calibration data EEPROM AC5
    CalibationAC6 = 0xB4,   //!< calibration data EEPROM AC6
    CalibationB1 = 0xB6,    //!< calibration data EEPROM B1
    CalibationB2 = 0xB8,    //!< calibration data EEPROM B2
    CalibationMB = 0xBA,    //!< calibration data EEPROM MB
    CalibationMC = 0xBC,    //!< calibration data EEPROM MC
    CalibationMD = 0xBE,    //!< calibration data EEPROM MD
} bmp180_register_t;

//! BMP180 commands
typedef enum _bmp180_commands {
    cmdReset = 0xB6,          //!< reset value to be written to BMx_R_RESET
    cmdMeasureTemp = 0x2E,    //!< control command to measure temperature
    cmdMeasurePressure = 0x34 //!< control command to measure pressure (add oversampling 0-3)
} bmp180_command_t;

//! BMP180 oversample options
typedef enum _bmp180_oversample {
    NoOversample = 0,         //!< no oversampling
    Oversample1 = 1,          //!< single oversampling
    Oversample2 = 2,          //!< double oversampling
    Oversample3 = 3,          //!< triple oversampling
} bmp180_oversample_t;

//! calibration register layout
typedef struct bmp180_calibration {
    int16_t AC1;  //!< parameter AC1
    int16_t AC2;  //!< parameter AC2
    int16_t AC3;  //!< parameter AC3
    uint16_t AC4; //!< parameter AC4
    uint16_t AC5; //!< parameter AC5
    uint16_t AC6; //!< parameter AC6
    int16_t B1;   //!< parameter B1
    int16_t B2;   //!< parameter B2
    int16_t MB;   //!< parameter MB
    int16_t MC;   //!< parameter MC
    int16_t MD;   //!< parameter MD
} bmp180_calibration_t;

// Parameters used in the pressure compensation, see spec sheet
// @{
#define BMP180_PARAM_MG		3038
#define BMP180_PARAM_MH		-7357
#define BMP180_PARAM_MI		3791
// @}

/*!
 * @brief Set a single register byte.
 *
 * @param reg the register to set
 * @param data the data byte to set
 * @return wether the setting was successful
 */
bool bmp180_set(bmp180_register_t reg, uint8_t data);

/*!
 * @brief Get a single register byte.
 * @param reg the register to query
 * @return the result value
 */
uint8_t bmp180_get(bmp180_register_t reg);

/*!
 * @brief Initialize BMxx80 driver.
 * @return if initialization was successful
 */
bool bmp180_init(void);

/*!
 * @brief Read calibration data from sensore into the parameter struct.
 * @param calibration the structure holding the parameters
 */
bool bmp180_read_calibration(bmp180_calibration_t *calibration);

/*!
 * @brief Reset the sensore.
 * @return whether the reset was successful
 */
bool bmp180_reset();

/*!
 * @brief Read raw temperature data from sensor.
 *
 * The value returned here is of no use if you do not use the
 * calibaration data from the chip and use the algorithm from the
 * spec sheet to calculate the real temperatur.
 *
 * @return the raw (uncompensated) temperatur value
 */
uint16_t bmp180_temperature_raw();

/*!
 * @brief Read raw pressure data from sensor.
 *
 * The value returned here is of no use if you do not use the
 * calibaration data from the chip and use the algorithm from the
 * spec sheet to calculate the real temperatur.
 *
 * @param oversample how many times to over-sample (0-3)
 * @return the raw (uncompensated) pressure value
 */
uint32_t bmp180_pressure_raw(bmp180_oversample_t oversample);

/*!
 * @brief Read current temperature.
 *
 * The temperature is calculalated from the raw temperature using
 * calibration data read from the chips memory.
 *
 * @return the compensated temperature value
 */
int16_t bmp180_temperature();

/*!
 * @brief Read current pressure.
 *
 * The pressure is calculalated from the raw temperature and pressure
 * data using calibration data read from the chips memory.
 *
 * @param oversample how many times to over-sample (0-3)
 * @return the compensated pressure value
 */
long bmp180_pressure(bmp180_oversample_t oversample);

#ifdef __cplusplus
}
#endif

#endif // UBIRCH_BMP180_H
