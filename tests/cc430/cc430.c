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


volatile uint32_t milliseconds = 0;
bool on = true;

void SysTick_Handler() {
  milliseconds++;
  if (milliseconds % 1000 == 0) on = !on;
  BOARD_LED0(on);
}

#define ERROR(...)  {PRINTF(__VA_ARGS__); PRINTF("\r\n"); while(true) {}}

int main(void) {
  board_init();
  board_console_init(BOARD_DEBUG_BAUD);
  SysTick_Config(BOARD_SYSTICK_100MS / 10);
  PRINTF("CC430\r\n");

  CLOCK_SetLpuartClock(1);
  CLOCK_EnableClock(kCLOCK_PortC);
  PORT_SetPinMux(PORTC, 3U, kPORT_MuxAlt3);
  PORT_SetPinMux(PORTC, 4U, kPORT_MuxAlt3);

  lpuart_config_t config;
  LPUART_GetDefaultConfig(&config);
  config.baudRate_Bps = 115200;
  config.enableRx = true;
  LPUART_Init(LPUART1, &config, CLOCK_GetPllFllSelClkFreq());

  while (true) {
    uint8_t c;
    LPUART_ReadBlocking(LPUART1, &c, 1);
    PRINTF("%02x", c);
  }

  return 0;
}

