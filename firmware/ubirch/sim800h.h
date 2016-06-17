/*!
 * @file
 * @brief SIM800H cell phone chip driver.
 *
 * This is the SIM800H driver for the cell phone chip on the ubirch#1.
 * As the SIM800H is hard-wired to the LPUART1 all the configuration
 * is wrapped away from the user. This driver provides the functionality
 * to power on/off the chip and has low-level functionality for
 * communicating with the SIM800H using the AT command set.
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

#include "sim800h/sim800h_core.h"
#include "sim800h/sim800h_parser.h"
#include "sim800h/sim800h_ops.h"
#include "sim800h/sim800h_http.h"
