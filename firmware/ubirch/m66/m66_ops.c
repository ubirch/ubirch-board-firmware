/*
 * ubirch#1 M66 high level functions.
 *
 * @author Matthias L. Jugel
 * @date 2016-04-09
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

// this is necessary when using the arm-none-eabi-gcc from launchpad
#define _GNU_SOURCE

#include <string.h>
#include <stdlib.h>
#include <fsl_rtc.h>
#include <ubirch/timer.h>
#include "m66_parser.h"
#include "m66_core.h"
#include "m66_debug.h"

bool modem_register(uint32_t timeout) {
  timer_set_timeout(timeout * 1000);

  bool registered = false;
  while (!registered && timer_timeout_remaining()) {
    int bearer = 0, status = 0;
    modem_send("AT+CREG?");
    const int matched = modem_expect_scan("+CREG: %d,%d", uTimer_Remaining, &bearer, &status);
    if (matched == 2) {
      CSTDEBUG("GSM INFO !! [%02d] %s\r\n", status, status < 6 ? reg_status[status] : "???");
      registered = ((status == CREG_HOME) || (status == CREG_ROAMING));
      modem_expect_OK(uTimer_Remaining);
    }
    if (!registered) delay(2000);
  }

  return registered;
}

bool modem_gprs_attach(const char *apn, const char *user, const char *password, uint32_t timeout) {
  timer_set_timeout(timeout * 1000);

  modem_send("AT+QIDEACT");
  modem_expect_OK(uTimer_Remaining);

  // attach to the network
  bool attached = false;
  do {
    modem_send("AT+CGATT=1");
    attached = modem_expect_OK(uTimer_Remaining);
    if (!attached) delay(1000);
  } while (!attached && timer_timeout_remaining());
  if (!attached) return false;

  modem_send("AT+QIFGCNT=0");
  if (!modem_expect_OK(uTimer_Remaining)) return false;

  if (apn) {
    modem_send("AT+QICSGP=1,\"%s\",\"%s\",\"%s\"", apn, user, password);
    if (!modem_expect_OK(uTimer_Remaining)) return false;
  }

  modem_send("AT+QIREGAPP", uTimer_Remaining);
  if (!modem_expect_OK(uTimer_Remaining)) return false;

  modem_send("AT+QIACT", uTimer_Remaining);
  if (!modem_expect_OK(uTimer_Remaining)) return false;

  return attached;
}

bool modem_gprs_detach(uint32_t timeout) {
  timer_set_timeout(timeout * 1000);

  modem_send("AT+CIPSHUT");
  if (!modem_expect("SHUT OK", uTimer_Remaining)) return false;

  modem_send("AT+SAPBR=0,1");
  if (!modem_expect_OK(uTimer_Remaining)) return false;

  modem_send("AT+CGATT=0");
  return modem_expect_OK(uTimer_Remaining);
}


bool modem_battery(status_t *status, int *level, int *voltage, uint32_t timeout) {
  modem_send("AT+CBC");
  modem_expect_scan("+CBC: %d,%d,%d", timeout, status, level, voltage);
  return modem_expect_OK(500);
}

bool modem_location(status_t *status, double *lat, double *lon, rtc_datetime_t *datetime, uint32_t timeout) {
  char response[60];

  modem_send("AT+QLOCC=60,60");
  modem_expect_OK(uTimer_Remaining);
  modem_send("AT+QGSMLOC=4");
  modem_expect_scan("+QGSMLOC: %d, %s", uTimer_Remaining, &status, response);

  if (status == 0) {
    *lon = atof(strtok(response, ","));
    *lat = atof(strtok(NULL, ","));

    datetime->year = (uint16_t) atoi(strtok(NULL, "/"));
    datetime->month = (uint8_t) atoi(strtok(NULL, "/"));
    datetime->day = (uint8_t) atoi(strtok(NULL, ","));
    datetime->hour = (uint8_t) atoi(strtok(NULL, ":"));
    datetime->minute = (uint8_t) atoi(strtok(NULL, ":"));
    datetime->second = (uint8_t) atoi(strtok(NULL, ":"));
  }

  return modem_expect_OK(uTimer_Remaining) && status == 0;
}

bool modem_imei(char *imei, const uint32_t timeout) {
  modem_send("AT+GSN");
  modem_readline(imei, 15, timeout);
  CIODEBUG("GSM (%02d) -> '%s'\r\n", strnlen(imei, 15), imei);
  return modem_expect_OK(500);
}
