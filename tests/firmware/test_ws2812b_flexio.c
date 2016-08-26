#include <ubirch/ws2812b.h>
#include <ubirch/timer.h>
#include <stdlib.h>
#include <fsl_flexio.h>

#ifndef BOARD_RGBS_LEN
#  define BOARD_RGBS_LEN  1
#endif

#define RANGE 50

int test_ws2812b(void) {
  // initialize FlexIO
  flexio_config_t flexio_config;
  CLOCK_SetFlexio0Clock(kCLOCK_CoreSysClk);
  FLEXIO_GetDefaultConfig(&flexio_config);
  FLEXIO_Init(FLEXIO0, &flexio_config);
  FLEXIO_Reset(FLEXIO0);

  // enable external pin to output LED data signal
  CLOCK_EnableClock(kCLOCK_PortA);
  PORT_SetPinMux(PORTA, 14U, kPORT_MuxAlt5);

  ws2812b_init(&ws2812b_config_default);

  uint32_t colors[2];
  // swap blue and green (left/right)
  colors[0] = rgb(0, 0, 20);
  colors[1] = rgb(10, 0, 0);
  ws2812b_send(colors, BOARD_RGBS_LEN);
  for (int i = 0; i < RANGE; i++) {
    delay((uint32_t) (abs((i - RANGE/2) % RANGE))>>2);
    uint32_t tmp = colors[0];
    colors[0] = colors[1];
    colors[1] = tmp;
    ws2812b_send(colors, BOARD_RGBS_LEN);
  }
  delay(10);

  colors[0] = 0;
  colors[1] = 0;
  ws2812b_send(colors, 2);

  return 0;
}



