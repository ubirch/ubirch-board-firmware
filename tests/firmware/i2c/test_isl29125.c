/**
 * RGB Sensor test (ISL29125).
 *
 * Tests functionality of the ISL29125 RGB sensor library.
 *
 * @author Matthias L. Jugel
 * @date 2016-05-25
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
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <board.h>
#include <ubirch/isl29125.h>
#include <ubirch/i2c.h>
#include <ubirch/timer.h>

#define INIT_RED    0xf0f0
#define INIT_GREEN  0x0f0f
#define INIT_BLUE   0x1f1f

static inline void init_color(rgb48_t *color) {
  color->red = INIT_RED;
  color->green = INIT_GREEN;
  color->blue = INIT_BLUE;

  assert(INIT_RED == color->red);
  assert(INIT_GREEN == color->green);
  assert(INIT_BLUE == color->blue);
}

void test_isl29125_color(int bits, int lux) {
  rgb48_t color;
  init_color(&color);

  assert(isl_reset());

  assert(isl_set(ISL_R_FILTERING, ISL_FILTER_IR_MAX));
  assert(isl_set(ISL_R_COLOR_MODE, (uint8_t) (ISL_MODE_RGB | lux | bits)));

  isl_read_rgb48(&color);
  assert(!(INIT_RED == color.red && INIT_GREEN == color.green && INIT_BLUE == color.blue));
}

int test_isl29125() {
  assert(isl_reset());

  // set sampling mode, ir filter and interrupt mode
  assert(isl_set(ISL_R_FILTERING, ISL_FILTER_IR_NONE));
  assert(isl_set(ISL_R_COLOR_MODE, ISL_MODE_RGB | ISL_MODE_375LUX | ISL_MODE_16BIT));

  assert(0x05 == isl_get(ISL_R_COLOR_MODE));
  assert(0x00 == isl_get(ISL_R_FILTERING));
  assert(0x00 == isl_get(ISL_R_INTERRUPT));

  test_isl29125_color(ISL_MODE_375LUX, ISL_MODE_16BIT);
  test_isl29125_color(ISL_MODE_375LUX, ISL_MODE_12BIT);
  test_isl29125_color(ISL_MODE_10KLUX, ISL_MODE_16BIT);
  test_isl29125_color(ISL_MODE_10KLUX, ISL_MODE_12BIT);

  assert(isl_set(ISL_R_COLOR_MODE, ISL_MODE_POWERDOWN));

  return 0;
}
