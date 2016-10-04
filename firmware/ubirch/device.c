/*!
 * @file
 * @brief Device identification functions.
 *
 * @author Matthias L. Jugel
 * @date 2016-10-04
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

#include "device.h"

status_t device_uuid(uint32_t *uuid) {
#ifdef SIM_UIDH_UID_MASK
  uuid[0] = SIM->UIDH;
  uuid[1] = SIM->UIDMH;
  uuid[2] = SIM->UIDML;
  uuid[3] = SIM->UIDL;
  return kStatus_Success;
#else
  return kStatus_Fail;
#endif
}
