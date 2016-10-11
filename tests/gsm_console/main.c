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


static char buffer[128];
static volatile uint16_t idx = 0;

void SysTick_Handler() {
  static uint8_t counter = 0;
  // we misuse the systick handler to read from the modem ringbuffer
//  int c = modem_read();
//  if(c != -1 && idx < 127 && c != '\r') {
//    if(c == '\n') {
//      buffer[idx] = '\0';
//      PRINTF("%s\r\n", buffer);
//      idx = 0;
//    } else buffer[idx++] = (char) c;
//  }

  static bool on = true;
  if((counter++ % 100) == 0) on = !on;
  BOARD_LED0(on);
}

int main(void) {
  board_init();
  board_console_init(BOARD_DEBUG_BAUD);

  modem_init();
  modem_enable();
  modem_register(60000);
  modem_gprs_attach(CELL_APN, CELL_USER, CELL_PWD, 60000);

//  SysTick_Config(BOARD_SYSTICK_100MS);

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
        modem_send("AT+QHTTPGET=%d", 40);
        if (!modem_expect_OK(40 * 1000)) {
          char get_response[] = {0};
          modem_readline(get_response, 20, 3 * 5000);
          CIODEBUG("HTTP (%02d) -> '%s'\r\n", strlen(get_response), get_response);
          return false;
        }
      }
    }

    static uint32_t dl_len, content_len = 0;
    static uint16_t error_code = 0;
    const char file_name[] = "RAM:text1.txt";
    modem_send("AT+QHTTPDL=\"%s\"", file_name);
    if (!modem_expect_scan("+QHTTPDL: %d,%d,%d", 10 * 5000, &dl_len, &content_len, &error_code))
    {

    }
    PRINTF("dl_len = %d, content_len = %d, error_code = %d\r\n", dl_len, content_len, error_code);
    if (!modem_expect_OK(10 * 5000)) PRINTF("HTTPDL OK not received\r\n");

    PRINTF("Now opening file\r\n");
    uint32_t file_handle = 0;
    uint8_t rw_mode = 2;
    uint16_t len = 0;
    char read_buffer[] = {0};
    modem_send("AT+QFOPEN=\"%s\",%u", file_name, rw_mode);
//  delay(1000 * 1000);
    modem_expect_scan("+QFOPEN: %u", &file_handle, 5 * 1000);
    PRINTF("File handle number %u\r\n", file_handle);
//  if (!modem_expect_OK(2 * 3000)) PRINTF("No file handle received\r\n");

    modem_send("AT+QFREAD=%u,%d", file_handle, len);
    size_t data_len = modem_read_binary((uint8_t *) read_buffer, len, 5 * 5000);
    CIODEBUG("HTTP (%02d) -> '%s'\r\n", strlen(data_len), data_len);
    if (!modem_expect_OK(5000)) PRINTF("Nothing read\r\n");

    counter--;
  }

  modem_disable();

  return 0;
}
//  static uint32_t file_handle = 0;
//  static char data[] = {0};
////    uint8_t url_delay = 70; //seconds
//
//    int get_status = modem_http_get(url_str, 90 * 1000);
//
//    if (get_status) {
//      // read dara or read file
//      uint32_t file_size = modem_http_dl_file("text1", 60 * 1000);
//
//      if (file_size > 0) PRINTF("file size zero");
//
//      file_handle = http_file_open("RAM:text1.txt", 2, 6 * 1000);
//
//      if (!file_handle) PRINTF("no file handle received");
//
//
//      int file_read_status = http_file_read(data, file_handle, 10);
//
//      if (file_read_status)
//      {
//        if (!http_file_close(file_handle)) PRINTF("close not sucessful");
//      }
//    }
//  }
//    if (file_size)
//    {
//      http_file_open
//    }


//    modem_send("AT+QHTTPURL=%d,%d", (uint8_t)strlen(url_str), url_delay);
//    if (modem_expect("CONNECT", 300))
//    {
//      modem_send(url_str);
//      if (!modem_expect_OK(5 * 1000))
//      {
//        PRINTF("Failed to input the URL\r\n");
//      }
//    }
//
//    modem_send("AT+QHTTPGET=40");
//    if (modem_expect_OK(40 * 1000))
//    {
//      modem_send("AT+QHTTPDL=\"RAM:1.TXT\"");
//      modem_expect("+QHTTPDL:", 100);
//
//      if (modem_expect_OK(15 * 1000))
//      {
//        modem_send("AT+QFLST=RAM:*");
//        modem_expect_OK(1000);
//        PRINTF("List of files \r\n");
//      }
//
//    }


//  modem_disable();
//
//  return 0;
//}
