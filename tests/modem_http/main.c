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


//  const char url[] = "http://developer.ubirch.com/test.txt";
    const char url[] = "http://developer.ubirch.com/test2048.txt";
    modem_send("AT+QHTTPURL=%d,%d", strlen(url), 60);

    if (!modem_expect("CONNECT", 5 * 1000)) {
      CIODEBUG("HTTP (25) -> 'Failed to receive CONNECT'\r\n");
    } else {
      modem_send(url);
      if (!modem_expect_OK(5 * 5000)) {
        PRINTF("Failed to connect to the server");
        return false;
      } else {
        modem_send("AT+QHTTPGET=40");
        if (!modem_expect_OK(40 * 1000)) {
          char get_response[] = {0};
          modem_readline(get_response, 20, 3 * 5000);
          CIODEBUG("HTTP (%02d) -> '%s'\r\n", strlen(get_response), get_response);
          return false;
        }
      }
    }

//    modem_send("AT+QHTTPREAD=60");
//    modem_expect_OK(5000);
//    uint8_t read_buff[] = {0};
//    modem_http_read(read_buff, 10 * 6000);
    int dl_len, content_len, error_code = 0;
    const char file_name[] = "RAM:text1.txt";

    modem_send("AT+QHTTPDL=\"%s\"", file_name);
    if (modem_expect_OK(5 * 1000)) PRINTF("OK HTTPDL siktu guru\r\n");
    if (modem_expect_scan("+QHTTPDL: %d,%d,%d", 10 * 5000, &dl_len, &content_len, &error_code))
//    if (modem_expect("+QHTTPDL: 2048,2048,0", 5 * 2000))
    {
//      PRINTF("no file handle number lfasdfasdf\r\n");
      PRINTF("dl_len = %d, content_len = %d, error_code = %d\r\n", dl_len, content_len, error_code);
    }

//    if (!modem_expect_OK(10 * 5000)) PRINTF("HTTPDL OK not received\r\n");

    PRINTF("Now opening file\r\n");
    int file_handle = 0;
    uint8_t rw_mode = 0;
    uint16_t len = 10;
    char read_buffer[] = {0};

    modem_send("AT+QFOPEN=\"%s\",%d", file_name, rw_mode);
    if (modem_expect_scan("+QFOPEN: %u", 5 * 1000, &file_handle))
    {
      PRINTF("File handle number %u\r\n", file_handle);
    }

    if (!modem_expect_OK(10 * 3000)) PRINTF("No file handle received\r\n");

    modem_send("AT+QFREAD=%d,%d", file_handle, len);
    size_t  data_len = modem_readline(read_buffer, 10, 5 * 5000);
//    size_t data_len = modem_read_binary((uint8_t *) read_buffer, len, 5 * 5000);
    CIODEBUG("HTTP (%02d) -> '%s'\r\n", strlen(read_buffer), data_len);
    if (!modem_expect_OK(50000)) PRINTF("Nothing read\r\n");

//    http_file_close(file_handle);
    modem_send("AT+QFCLOSE=%d", file_handle);
    if (!modem_expect_OK(2000)) PRINTF("sariyagi close aagilla\r\n")
        ;
    counter--;
    delay(10 * 1000);
  }

  modem_disable();

  return 0;
}

