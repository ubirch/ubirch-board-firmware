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

bool on = true;
volatile uint32_t milliseconds = 0;
static int file_handle = 0;

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
    const char file_name[] = "RAM:text.txt";
    char read_buffer[] = {0};

    int dl_len = 0;
    file_handle = 0;
    int read_len = 10;

    int get_ret_value = modem_http_get(url, 60 * 1000);
    if (get_ret_value > 0) break;

    dl_len = modem_http_dl_file(file_name, 5 * 5000);
    if (dl_len < 0 ) break;

    PRINTF("Now opening file\r\n");
    file_handle = http_file_open(file_name, 0, 5 * 1000);
    PRINTF("This is our file handle %d\r\n", file_handle);

    if (file_handle < 0) break;

    for (int i = 0; i < dl_len; i += read_len)
    {
      size_t data_len = http_file_read(read_buffer, file_handle, read_len);
      if (data_len < 0)
      {
      CIODEBUG("HTTP (%02d) -> '%s'\r\n", strlen(read_buffer), read_buffer);
        break;
      }

    }

    if (!http_file_close(file_handle)) PRINTF("sariyagi close aagilla\r\n");

    counter--;
    delay(10 * 1000);
  }

  modem_disable();

  return 0;
}

