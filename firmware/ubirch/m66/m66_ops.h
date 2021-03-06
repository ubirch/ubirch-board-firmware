/*!
 * @file
 * @brief M66 high level operations.
 *
 * @author Matthias L. Jugel
 * @date 2016-05-06
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

#ifndef _UBIRCH_M66_OPS_H_
#define _UBIRCH_M66_OPS_H_

#include <stdbool.h>
#include <stdint.h>
#include <fsl_rtc.h>

#ifdef __cplusplus
extern "C" {
#endif

//! M66 battery status codes
enum battery_status {
    battery_NotCharging = 0,  //!< battery is not charging
    battery_Charging = 1,     //!< battery is currently charging
    battery_Full = 2          //!< battery is not charging and full
};

//! M66 GPS location status codes
enum location_status {
    loc_Success = 0,          //!< GPS localization successful
    loc_NotFound = 404,       //!< GPS location could not be found
    loc_RequestTimeout = 408, //!< Request timeout locating
    loc_NetworkError = 601,   //!< Network error during location query
    loc_NoMemory = 602,       //!< Memory has been exhausted in SIM800 module
    loc_DNSError = 603,       //!< DNS error during GPS location query
    loc_StackBusy = 604,      //!< Internal code stack is busy
    loc_Error = 65535         //!< Unknown error occurred
};

/*!
 * @brief Register to the cell network.
 * @param timeout how long to wait until registration
 */
bool modem_register(uint32_t timeout);

/*!
 * @brief Attach to the GPRS network.
 * @param apn the network APN
 * @param user the apn user name
 * @param password the apn password
 * @param timeout how long to wait in ms
 */
bool modem_gprs_attach(const char *apn, const char *user, const char *password, uint32_t timeout);

/*!
 * @brief Detach from GPRS.
 * @param timeout how long to wait in ms
 */
bool modem_gprs_detach(uint32_t timeout);

/*!
 * @brief If connected, get status, current battery level, and voltage.
 * @param status status (0 not charging, 1 charging, 2 charged)
 * @param level where the level is stored (1-100)
 * @param voltage where voltage is stored (in mV)
 * @param timeout how long to wait in ms
 */
bool modem_battery(uint8_t *status, int *level, int *voltage, uint32_t timeout);

/*!
 * @brief Get GSM location information, as well as date and time.
 * @param lat latitude
 * @param lon longitude
 * @param datetime date and time from the GSM network
 * @param timeout how long to wait in ms
 */
bool modem_location(double *lat, double *lon, rtc_datetime_t *datetime, uint32_t timeout);

/*!
 * @brief Get the IMEI number.
 * @param imei the IMEI
 * @param timeout how long to wait in ms
 */
bool modem_imei(char *imei, const uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif // _UBIRCH_M66_OPS_H_
