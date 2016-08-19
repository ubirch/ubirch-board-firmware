/*!
 * @file
 * @brief RGB LED strip driver code (for WS2812b compatible LEDs).
 *
 * This driver drives strips of WS2812b or compatible LEDs.
 *
 * @author Matthias L. Jugel
 * @date 2016-04-06
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
#ifndef _UBIRCH_WS2812B_H_
#define _UBIRCH_WS2812B_H_

#include <stdint.h>
#include <board.h>
#include "ws2812b.h"

#define WS2812B_FLEXIO_SRC_CLOCK    kCLOCK_CoreSysClk //!< default flexio src clock

#define WS2812B_FLEXIO_SHIFTER      0
#define WS2812B_FLEXIO_CLOCK_TIMER  0
#define WS2812B_FLEXIO_0_TIMER      1
#define WS2812B_FLEXIO_1_TIMER      2

#define WS2812B_FLEXIO_SHIFTER_PIN  2
#define WS2812B_FLEXIO_CLOCK_PIN    3

typedef struct {
    uint8_t flexio_shifter;
    uint8_t flexio_clk_timer;
    uint8_t flexio_t0_timer;
    uint8_t flexio_t1_timer;
    uint32_t flexio_shifter_pin;
    uint32_t flexio_clk_timer_pin;
    uint32_t flexio_data_pin;
} ws2812b_config_t;

ws2812b_config_t ws2812b_config_default = {
  .flexio_shifter = WS2812B_FLEXIO_SHIFTER,
  .flexio_clk_timer = WS2812B_FLEXIO_CLOCK_TIMER,
  .flexio_t0_timer = WS2812B_FLEXIO_0_TIMER,
  .flexio_t1_timer = WS2812B_FLEXIO_1_TIMER,
  .flexio_shifter_pin = WS2812B_FLEXIO_SHIFTER_PIN,
  .flexio_clk_timer_pin = WS2812B_FLEXIO_CLOCK_PIN,
#if defined(BOARD_RGBS_FLEXIO_PIN)
  .flexio_data_pin = BOARD_RGBS_FLEXIO_PIN,
#endif
};

/*!
 * @brief Convert r, g, b color value to uint32_t (00GGRRBB) for WS2812b)
 * @param r red color part (0-255)
 * @param g green color part (0-255
 * @param b blue color part (0-255)
 * @return the converted packed structure as uint32_t
 */
static inline uint32_t rgb(uint8_t r, uint8_t g, uint8_t b) {
  return g << 16 | r << 8 | b;
}

/*!
 * @brief Initialize the driver for the WS2812b RGB LEDs.
 *
 * Requires that flexio is initialized and clock are ungated for FlexIO as well as the data out pin.
 *
 * @code
 * // initialize FlexIO
 * flexio_config_t flexio_config;
 * CLOCK_SetFlexio0Clock(kCLOCK_CoreSysClk);
 * FLEXIO_GetDefaultConfig(&flexio_config);
 * FLEXIO_Init(FLEXIO0, &flexio_config);
 * // enable external pin to output LED data signal
 * CLOCK_EnableClock(kCLOCK_PortA);
 * PORT_SetPinMux(PORTA, 14U, kPORT_MuxAlt5);
 * // initialize LED driver
 * ws2812b_init(&ws2812b_config_default);
 * @endcode
 *
 * @param config the flexio configuration for the LED driver
 * @param src_clk_freq source clock frequency
 */
void ws2812b_init(ws2812b_config_t *config);

/*!
 * @brief Send color data to the LED strip.
 *
 * @code
 * uint32_t colors[2] = {0xFF0000, 0x0000FF};
 * ws2812b_send(colors, 2);
 * @endcode
 *
 * @param data the color data as an array of 24 bit (00GGRRBB)
 * @param num_leds the number of LEDs that need to be driven
 */
void ws2812b_send(uint32_t *data, unsigned int num_leds);

#endif // _UBIRCH_WS2812B_H_
