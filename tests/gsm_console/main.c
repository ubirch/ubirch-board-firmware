/**
 * Simple SIM800 GSM Modem Console.
 */

#include <stdint.h>
#include <stdbool.h>
#include <board.h>
#include <stdio.h>
#include <ubirch/modem.h>
#include "config.h"


static char buffer[128];
static volatile uint16_t idx = 0;

void SysTick_Handler() {
  static uint8_t counter = 0;
  // we misuse the systick handler to read from the modem ringbuffer
  int c = modem_read();
  if(c != -1 && idx < 127 && c != '\r') {
    if(c == '\n') {
      buffer[idx] = '\0';
      PRINTF("%s\r\n", buffer);
      idx = 0;
    } else buffer[idx++] = (char) c;
  }

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

  SysTick_Config(BOARD_SYSTICK_100MS);

  PRINTF("GSM console ready.\r\n");
//  uint8_t buffer[128], idx = 0;
  while (true) {
//    int ch = GETCHAR();
//    if (ch == '\r' || ch == '\n') {
//      PUTCHAR('\r');
//      PUTCHAR('\n');
//      buffer[idx] = '\0';
//      if(!strncasecmp((const char *) buffer, "quit", 4)) break;
//      modem_writeline((const char *) buffer);
//      idx = 0;
//    } else {
//      PUTCHAR(ch);
//      buffer[idx++] = (uint8_t) ch;
//    }

    const char url_str[] = "http://developer.ubirch.com/text2048.txt";
    uint8_t url_delay = 70; //seconds

    bool get_status = modem_http_get(url_str, 100 * 1000);

    if (get_status)
    {
      // read dara or read file
      uint32_t file_size = modem_http_dl_file("text1", 60 * 1000);
    }

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
  }

  modem_disable();

  return 0;
}
