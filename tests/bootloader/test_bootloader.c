/**
 * Bootloader Test.
 *
 * Test the ROM bootloader built into the K82F device.
 *
 * @author Matthias L. Jugel
 * @date 2016-05-26
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

#include <board.h>
#include <stdio.h>
#include <ubirch/i2c.h>
#include <ubirch/isl29125.h>

int main(void) {
  board_init();
  board_console_init(BOARD_DEBUG_BAUD);

  runBootloader(NULL);
}
