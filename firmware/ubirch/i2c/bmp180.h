/*!
 * @file
 * @brief  Bosch BMP180 temperature and pressure sensor driver.
 *
 * See [Datasheet](https://www.bosch-sensortec.com/bst/products/all_products/bmp180)
 * for a detailed description of the sensor and an example how to calculate the compensated
 * temperature as is done in this driver. The names of the parameters are used as they
 * are named in the data sheet.
 *
 * This code uses the driver code from
 * [Bosch Sensortec BMP180 driver](https://github.com/BoschSensortec/BMP180_driver).
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

#ifndef UBIRCH_FIRMWARE_BMP180_H
#define UBIRCH_FIRMWARE_BMP180_H

#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <ubirch/i2c/bosch_sensortec/bmp180.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BMP180_DEVICE_ADDRESS 0x77  //!< BMP180 device address (same address for multiple devices)
#define BMP180_CHIP_ID 0x55         //!< BMP180 chip id is fixed

//! Data structure holding all sensor values for the BMP180
typedef struct {
    int32_t temperature;  //!< temperature data in 0.01℃
    int32_t pressure;     //!< pressure data in Pascal
} bme180_data_t;

/*!
 * @brief Initialize BMP180 driver.
 * @return if initialization was successful
 */
bool bmp180_init(void);

/*!
 * @brief Read current temperature.
 *
 * The temperature is calculalated from the raw temperature using
 * calibration data read from the chips memory.
 *
 * @return temperature in 0.01℃ (2345 is 23.45℃)
 */
int32_t bmp180_temperature(void);

/*!
 * @brief Read current pressure.
 *
 * The pressure is calculalated from the raw temperature and pressure
 * data using calibration data read from the chips memory.
 *
 * @return atmospheric pressure in Pa, 0 if there was an error
 */
int32_t bmp180_pressure(void);

/*!
 * @brief Sample temperature and pressure.
 *
 * @param data the full data structure set
 * @return whether the sample was successful
 */
bool bmp180_sample(bme180_data_t *data);

/*!
 * @brief Read current altitude.
 *
 * Requires the current pressure at sea level to calculate.
 *
 * @param sea_level_pressure the current pressure at sea level
 * @param pressure the pressure as measured by this sensor
 * @return the altitude in meters
 */
static inline float bmp180_altitude(uint32_t sea_level_pressure, uint32_t pressure) {
  return 44330.0f * (1.0f - (float) pow((float) pressure / (float) sea_level_pressure, 1 / 5.255));
}

/*!
 * @brief Calculate current sea level pressure.
 * @param pressure the pressure as measured by this sensor
 * @param altitude the altitude of the sensor
 * @return the sea level pressure in Pa
 */
static inline float bmp180_pressure_sea_level(uint32_t pressure, float altitude) {
  return pressure / (float) pow(1.0f - (altitude / 44330.0), 5.255);
}

#ifdef __cplusplus
}
#endif

#endif // UBIRCH_BMP180_H
