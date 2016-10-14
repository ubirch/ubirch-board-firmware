/**
 * Simple SIM800 GSM Modem Console.
 */

#include <stdint.h>
#include <stdbool.h>
#include <board.h>
#include <stdio.h>
#include <ubirch/timer.h>
#include <ubirch/modem.h>
#include "config.h"
#include <ubirch/m66/m66_debug.h>


volatile uint32_t milliseconds = 0;
bool on = true;

void SysTick_Handler()
{
  milliseconds++;
  if (milliseconds % 1000 == 0) on = !on;
  BOARD_LED0(on);
}

int main(void) {
  board_init();
  board_console_init(BOARD_DEBUG_BAUD);
  SysTick_Config(BOARD_SYSTICK_100MS / 10);

  modem_init();
  modem_enable();
  modem_register(6 * 5000);
  modem_gprs_attach(CELL_APN, CELL_USER, CELL_PWD, 60000);

  PRINTF("GSM console ready.\r\n");

  int counter = 10;

  while (counter != 0)
  {
    const char url[] = "http://developer.ubirch.com/test2048.txt";

    uint8_t read_buffer[256] = {0}; // <<- allocate an array, which is equal to char *read_buffer
    size_t data_len = 0;
    size_t *res_size = {0};
    size_t read_len = 100;
    int dl_size = 0;

    dl_size  = modem_http_get(url, res_size, 60 * 1000);
    if (!dl_size)
    {
      PRINTF("DL_SIZE = 0\r\n");
      break;
    }

    data_len = modem_http_read(read_buffer, 0, read_len, 60 * 1000);

    if (!data_len)
    {
      PRINTF("No data read\r\n");
      break;
    }
    PRINTF("DATA LEN %d\r\n", data_len);
    PRINTF("READ BUF: '%s'\r\n", read_buffer);

    counter--;
    delay(10 * 1000);
  }

  modem_disable();

  return 0;
}
