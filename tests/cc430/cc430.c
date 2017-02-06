/**
 * Establish HTTP connection, download a file and process it
 */

#include <stdint.h>
#include <stdbool.h>
#include <board.h>
#include <stdio.h>
#include <ubirch/timer.h>
#include <stdarg.h>
#include <ubirch/rf-sub-ghz/rf_lpuart.h>
#include <ubirch/dbgutil.h>
#include <ubirch/ws2812b.h>
#include <fsl_flexio.h>


volatile uint32_t milliseconds = 0;
bool on = false;

void SysTick_Handler() {
  milliseconds++;
  if (milliseconds % 200 == 0) {
    on = !on;
    BOARD_LED0(false);
  }
}

#define ERROR(...)  {PRINTF(__VA_ARGS__); PRINTF("\r\n"); while(true) {}}

int main(void) {
  board_init(BOARD_MODE_RUN);
  board_console_init(BOARD_DEBUG_BAUD);
  SysTick_Config(BOARD_SYSTICK_100MS / 10);

  // enable external pin to output LED data signal
  CLOCK_EnableClock(BOARD_RGBS_PORT_CLOCK);
  PORT_SetPinMux(PORTA, BOARD_RGBS_PIN, BOARD_RGBS_ALT);

  // initialize FlexIO
  flexio_config_t flexio_config;
  CLOCK_SetFlexio0Clock(kCLOCK_CoreSysClk);
  FLEXIO_GetDefaultConfig(&flexio_config);
  FLEXIO_Init(FLEXIO0, &flexio_config);
  FLEXIO_Reset(FLEXIO0);


  uint32_t led[2] = {0x0000ff00,0x00000000};
  ws2812b_init(&ws2812b_config_default);
  ws2812b_send(led, BOARD_RGBS_LEN);
  delay(10);
  led[0] = 0;
  led[1] = 0;
  ws2812b_send(led, BOARD_RGBS_LEN);
  delay(10);

  PRINTF("CC430\r\n");

  CLOCK_EnableClock(kCLOCK_PortC);
  PORT_SetPinMux(PORTC, 3U, kPORT_MuxAlt3);
  PORT_SetPinMux(PORTC, 4U, kPORT_MuxAlt3);

  lpuart_config_t config;
  LPUART_GetDefaultConfig(&config);
  config.baudRate_Bps = 115200;

  uint32_t lpuart_src_freq;
  switch (SIM->SOPT2 & SIM_SOPT2_LPUARTSRC_MASK) {
    case SIM_SOPT2_LPUARTSRC(3U): {
      lpuart_src_freq = CLOCK_GetInternalRefClkFreq();
      break;
    }
    case SIM_SOPT2_LPUARTSRC(2U): {
      lpuart_src_freq = CLOCK_GetOsc0ErClkFreq();
      break;
    }
    case SIM_SOPT2_LPUARTSRC(1U): {
      lpuart_src_freq = CLOCK_GetPllFllSelClkFreq();
      break;
    }
    default: {
      // lpuart source clock is disabled
      return kStatus_Fail;
    }
  }

  LPUART_Init(LPUART1, &config, lpuart_src_freq);
  LPUART_EnableTx(LPUART1, true);
  LPUART_EnableRx(LPUART1, true);

  while (true) {
    uint8_t c;
    LPUART_ReadBlocking(LPUART1, &c, 1);
    if (!on && milliseconds % 100 == 0) BOARD_LED0(true);
    PRINTF("%02x", c);
  }

  return 0;
}

