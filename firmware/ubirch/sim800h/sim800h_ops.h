/**
 * @brief ubirch#1 SIM800H high level operations.
 *
 * @author Matthias L. Jugel
 * @date 2016-05-06
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

#ifndef _UBIRCH_SIM800H_OPS_H_
#define _UBIRCH_SIM800H_OPS_H_

#include <stdbool.h>
#include <stdint.h>
#include <fsl_rtc.h>

enum sim800_battery_status {
    battery_NotCharging = 0,
    battery_Charging = 1,
    battery_Full = 2
};

enum sim800h_location_status {
    loc_Success = 0,
    loc_NotFound = 404,
    loc_RequestTimeout = 408,
    loc_NetworkError = 601,
    loc_NoMemory = 602,
    loc_DNSError = 603,
    loc_StackBusy = 604,
    loc_Error = 65535
};

/*!
 * @brief Register to the cell network.
 * @param timeout how long to wait until registration
 */
bool sim800h_register(uint32_t timeout);

/*!
 * @brief Attach to the GPRS network.
 * @param apn the network APN
 * @param user the apn user name
 * @param password the apn password
 * @param timeout how long to wait in ms
 */
bool sim800h_gprs_attach(const char *apn, const char *user, const char *password, uint32_t timeout);

/*!
 * @brief Detach from GPRS.
 * @param timeout how long to wait in ms
 */
bool sim800h_gprs_detach(uint32_t timeout);

/*!
 * @brief If connected, get status, current battery level, and voltage.
 * @param status status (0 not charging, 1 charging, 2 charged)
 * @param level where the level is stored (1-100)
 * @param voltage where voltage is stored (in mV)
 * @param timeout how long to wait in ms
 */
bool sim800h_battery(status_t *status, short int *level, int *voltage, uint32_t timeout);

/*!
 * @brief Get GSM location information, as well as date and time.
 * @param status request status (0 ok)
 * @param lat latitude
 * @param lon longitude
 * @param datetime date and time from the GSM network
 * @param timeout how long to wait in ms
 */
bool sim800h_location(status_t *status, double *lat, double *lon, rtc_datetime_t *datetime, uint32_t timeout);

/*!
 * @brief Get the IMEI number.
 * @param imei the IMEI
 * @param timeout how long to wait in ms
 */
bool sim800h_imei(char *imei, const uint32_t timeout);

#endif // _UBIRCH_SIM800H_OPS_H_
