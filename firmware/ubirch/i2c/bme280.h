/*!
 * @file
 * @brief  Bosch BME280 temperature, relative humidity and pressure sensor driver.
 *
 * See [Datasheet](https://www.bosch-sensortec.com/bst/products/all_products/bme280)
 * for a detailed description of the sensor and an example how to calculate the compensated
 * temperature as is done in this driver. The names of the parameters are used as they
 * are named in the data sheet.
 *
 * This code uses the driver code from
 * [Bosch Sensortec BME280 driver](https://github.com/BoschSensortec/BME280_driver).
 *
 * @author Matthias L. Jugel
 * @date   2016-06-20
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

#ifndef UBIRCH_FIRMWARE_BME280_H
#define UBIRCH_FIRMWARE_BME280_H

#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <ubirch/i2c/bosch_sensortec/bme280.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BME280_DEVICE_ADDRESS 0x77  //!< BME280 device address (same address for multiple devices)
#define BME280_CHIP_ID 0x60         //!< BME280 chip id is fixed

//! Data structure holding all sensor values for the BME280
typedef struct {
    int32_t temperature;  //!< temperature data in 0.01℃
    uint32_t pressure;    //!< pressure data in Pascal
    uint32_t humidity;    //!< humidity .001 rH%
} bme280_data_t;

//! BME280 power mode setting
typedef enum {
    BME280_Normal = BME280_NORMAL_MODE, //!< normal measuring mode
    BME280_Forced = BME280_FORCED_MODE, //!< force measurement
    BME280_Sleep = BME280_SLEEP_MODE    //!< enter low power sleep mode, also necessary to set config
} power_mode_t;

/*!
 * @brief Initialize BME280 driver.
 * @return if initialization was successful
 */
bool bme280_init(void);

/*!
 * @brief Change sensor mode (normal, forced, sleep).
 *
 * @see #power_mode_t
 *
 * @return whether the sleep mode setting was successful
 */
static inline bool bme280_power_mode(power_mode_t mode) {
  return !bme280_set_power_mode(mode);
}

/*!
 * @brief Read current temperature.
 *
 * The temperature is calculalated from the raw temperature using
 * calibration data read from the chips memory.
 *
 * @return temperature in 0.01℃ (2345 is 23.45℃), -INT32_MAX for error condition
 */
int32_t bme280_temperature(void);

/*!
 * @brief Read current pressure.
 *
 * The pressure is calculalated from the raw temperature and pressure
 * data using calibration data read from the chips memory.
 *
 * @return atmospheric pressure in Pa, 0 if there was an error
 */
uint32_t bme280_pressure(void);

/*!
 * @brief Read current humidity.
 *
 * The humidity is calculalated from the raw temperature and pressure
 * data using calibration data read from the chips memory.
 *
 * An output value of 42313 represents 42313 / 1024 = 41.321 %rH
 *
 * @return humidity in %rH, 0 for error condition
 */
uint32_t bme280_humidity();

/*!
 * @brief Sample the complete set of data from this sensor.
 *
 * Reads temperature, pressure and humidity.
 *
 * @param data the sample data structure
 * @return whether the sampling was successful
 */
bool bme280_sample(bme280_data_t *data);

/*!
 * @brief Read current altitude.
 *
 * Requires the current pressure at sea level to calculate.
 *
 * @param pressure_sea_level the current pressure at sea level
 * @param pressure the current pressure measured by this sensor
 * @return the altitude in meters
 */
static inline float bme280_altitude(uint32_t pressure_sea_level, uint32_t pressure) {
  return 44330.0f * (1.0f - (float) pow((float) pressure / (float) pressure_sea_level, 1 / 5.255));
}

/*!
 * @brief Calculate current sea level pressure.
 * @param pressure the current pressure as measure by this sensor
 * @param altitude the altitude of the sensor
 * @return the sea level pressure in Pa
 */
static inline float bme280_pressure_sea_level(uint32_t pressure, float altitude) {
  return (float) pressure / (float) pow(1.0f - (altitude / 44330.0), 5.255);
}

#ifdef __cplusplus
}
#endif

#endif // UBIRCH_BME280_H
