/*!
 * @file
 * @brief M66 debug helpers.
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

#ifndef _UBIRCH_M66_DEBUG_
#define _UBIRCH_M66_DEBUG_

#include <fsl_debug_console.h>
#include <ubirch/dbgutil.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NCIODEBUG
#  define CIODEBUG(...)
#  define CIODUMP(...)
#else
#  define CIODEBUG(...)  PRINTF(__VA_ARGS__)                  /*!< Debug I/O message (AT commands) */
#  define CIODUMP(buffer, size) dbg_dump("GSM", buffer, size) /*!< Debug and dump a buffer */
#endif

#ifdef NCSTDEBUG
#  define CSTDEBUG(...)
#else
#  define CSTDEBUG(...)  PRINTF(__VA_ARGS__)                  /*!< Standard debug message (info) */
#endif

//! textual representation of the registration status
const char *reg_status[6] = {
  "NOT SEARCHING",
  "HOME",
  "SEARCHING",
  "DENIED",
  "UNKNOWN",
  "ROAMING"
};

#ifdef __cplusplus
}
#endif

#endif // _UBIRCH_M66_DEBUG_
