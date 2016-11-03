/*!
 * @file
 * @brief M66 AT command parser.
 *
 * AT command driver, handling the I/O via the UART and parsing
 * lines for expected responses. Also handles so call unsolicited
 * response codes which are output at unexpected times.
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
#ifndef _UBIRCH_M66_PARSER_H_
#define _UBIRCH_M66_PARSER_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * A list of unsolicited response codes we need to take care of.
 * The list of by no means complete and may change for other versions
 * of the SIM800 chip line or other GSM chips.
 *
 * The list originates here: this useful list found here: https://github.com/cloudyourcar/attentive
 */
const char *M66_URC[] = {
  "RDY",                      /*!< Device ready. */
  "+CFUN: 1",                 /*!< Device has entered full functional mode */
  "+CPIN: READY",             /*!< Device has SIM card inserted */
  "+CPIN: NOT READY",
  "+PDP DEACT",
  "Call Ready",
  "SMS Ready",
  "NORMAL POWER DOWN",
  "UNDER-VOLTAGE POWER DOWN",
  "UNDER-VOLTAGE WARNNING",
  "OVER-VOLTAGE POWER DOWN",
  "OVER-VOLTAGE WARNNING",
  "+CMTI: \"SM\",3",          /*!< SMS has arrived */
  NULL
};

enum urc {
    Ready = 0,
    Functional,
    SIMCardReady,
    SIMCardNotReady,
    PDPDeact,
    CallReady,
    SMSReady,
    NormalPowerDown,
    UnderVoltagePowerDown,
    UnderVoltageWarning,
    OverVoltagePowerDown,
    OverVoltageWarning,
    SMSArrived
};

//! registration status codes
enum creg_status {
    CREG_NOT_SEARCHING = 0, /*!< not searching */
    CREG_HOME = 1,          /*!< registered home network */
    CREG_SEARCHING = 2,     /*!< searching network */
    CREG_DENIED = 3,        /*!< network registration denied */
    CREG_UNKNOWN = 4,       /*!< status unknown */
    CREG_ROAMING = 5        /*!< registered roaming network */
};

//! AT "OK" response
const char *RESPONSE_OK = "OK";

/*!
 * Check if this line is an unsolicited result code.
 * @return the code index or -1 if it is no known code
 */
int modem_check_urc(const char *line);

/*! send a command */
void modem_send(const char *pattern, ...);

/*!
 * @brief Expect a specific URC, blocks until it is received or timeout.
 * @param n the URC number
 * @param timeout how long to wait for the urc in ms
 * @return whether the URC has been matched
 */
bool modem_expect_urc(int n, uint32_t timeout);

/*!
 * @brief Expect a certain response, blocks util the response received or timeout.
 * This function will ignore URCs and return when the first non-URC has been received.
 * @param expected the string to expect
 * @param timeout how long to wait for the response in ms
 * @return true if received or false if not
 */
bool modem_expect(const char *expected, uint32_t timeout);

/*!
 * Simply expect an "OK" response.
 * @param timeout how long to wait for OK
 * @returns true if OK else false
 */
static inline bool modem_expect_OK(uint32_t timeout) {
  return modem_expect(RESPONSE_OK, timeout);
}


/*!
 * @brief Expect a formatted response, blocks until the response is received or timeout.
 * This function will ignore URCs and return when the first non-URC has been received.
 * @param pattern the pattern to match
 * @param timeout how long to wait for the response in ms
 * @return the number of matched elements
 */
int modem_expect_scan(const char *pattern, uint32_t timeout, ...);

#ifdef __cplusplus
}
#endif

#endif // _UBIRCH_M66_PARSER_H_
