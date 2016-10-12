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

void SysTick_Handler() {
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
  while (counter != 0) {

    const char url[] = "http://developer.ubirch.com/test2048.txt";

    int get_ret_value = modem_http_get(url, 60 * 1000);

//    Use this to read the packets directly
//    modem_send("AT+QHTTPREAD=60");
//    modem_expect_OK(5000);
//    uint8_t read_buff[] = {0};
//    modem_http_read(read_buff, 10 * 6000);

    if (get_ret_value > 0)
    {
      const char file_name[] = "RAM:text.txt";
      int dl_len = 0;
//      int file_handle_num = 0;

      dl_len = modem_http_dl_file(file_name, 5 * 5000);
      if (dl_len < 0 ) break;

      PRINTF("Now opening file\r\n");
      int file_handle = 0;
//      uint8_t rw_mode = 0;
//      uint16_t len = 10;
//      int file_handle = 0;
      char read_buffer[] = {0};

      file_handle = http_file_open(file_name, 0, 5 * 1000);
      PRINTF("This is our file handle %d\r\n", file_handle);

      if (file_handle < 0) break;

      int read_len = 10;
      for (int i = 0; i < dl_len; i += read_len)
      {
        size_t data_len = http_file_read(read_buffer, file_handle, read_len);
//        modem_send("AT+QFREAD=%d,%d", file_handle, read_len);

//        modem_expect("CONNECT", 5000);
//        size_t data_len = modem_readline(read_buffer, 10, 5 * 5000);
        if (data_len < 0)
        {
          PRINTF("data_len < 0 \r\n");
          break;
        }
//        CIODEBUG("HTTP 123 (%02d) -> '%s'\r\n", strlen(read_buffer), read_buffer);

      }
      if (!modem_expect_OK(50000)) PRINTF("Nothing read\r\n");

      modem_send("AT+QFCLOSE=%d", file_handle);
      if (!modem_expect_OK(2000)) PRINTF("sariyagi close aagilla\r\n");
    }

    counter--;
    delay(10 * 1000);
  }

  modem_disable();

  return 0;
}

