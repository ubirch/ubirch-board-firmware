/*!
 * @file
 * @brief Modem driver.
 *
 * Modem driver code. Necessary for cell phone connectivity.
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

#include <cell.h>

#if defined(BOARD_CELL_TYPE_M66)
  #include "m66/m66_core.h"
  #include "m66/m66_parser.h"
  #include "m66/m66_ops.h"
  #include "m66/m66_http.h"
#else
  #include "sim800h/sim800h_core.h"
  #include "sim800h/sim800h_parser.h"
  #include "sim800h/sim800h_ops.h"
  #include "sim800h/sim800h_http.h"
#endif

