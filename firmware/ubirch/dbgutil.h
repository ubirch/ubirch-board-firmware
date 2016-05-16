/**
 * @brief ubirch#1 SIM800H debug helpers.
 *
 * @author Matthias L. Jugel
 * @date 2016-04-09
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

#ifndef _UBIRCH_DEBUG_H_
#define _UBIRCH_DEBUG_H_

/*! @brief Dump the buffer in hex and ascii format. */
void dbg_dump(const char *prefix, const uint8_t *b, size_t size);

/*! @brief Dump the buffer in hex format readable by xxd. */
void dbg_xxd(const char *prefix, const uint8_t *b, size_t size);


#endif // _UBIRCH_DEBUG_H_
