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

#ifndef UBIRCH_DEVICE_H
#define UBIRCH_DEVICE_H

#include <fsl_common.h>

/*!
 * @brief Get a 128 bit UUID that is chip specific.
 * @param uuid 128 bit to store the uuid
 * @return whether this operation was successful
 */
status_t device_uuid(uint32_t *uuid);

#endif // UBIRCH_DEVICE_H
