/**
 * Establish HTTP connection, download a file and process it
 */

#include <stdint.h>
#include <stdbool.h>
#include <board.h>
#include <stdio.h>
#include <ubirch/timer.h>
#include <ubirch/modem.h>
#include "config.h"
#include <ubirch/m66/m66_debug.h>
#include <stdarg.h>


volatile uint32_t milliseconds = 0;
bool on = true;

void SysTick_Handler() {
  milliseconds++;
  if (milliseconds % 1000 == 0) on = !on;
  BOARD_LED0(on);
}

#define ERROR(...)  {PRINTF(__VA_ARGS__); PRINTF("\r\n"); while(true) {}}

#define URL_WITH_CONTENT_LENGTH "http://api.ubirch.com/rp15/any-content.php?len=255&set=true"

#define URL_WITHOUT_CONTENT_LENGTH "http://api.ubirch.com/rp15/any-content.php?len=255"

size_t read_response(uint8_t *buffer, size_t buffer_size, size_t res_size) {
  size_t position = 0;
  do {
    size_t received = modem_http_read(buffer, position, buffer_size, 60000);
    if(!received) break;
    position += received;
    CIODUMP(buffer, received);
  } while (position < res_size);
  return position;
}


int main(void) {
  board_init(BOARD_MODE_RUN);
  board_console_init(BOARD_DEBUG_BAUD);
  SysTick_Config(BOARD_SYSTICK_100MS / 10);

  modem_init();
  modem_enable();

  modem_register(6 * 5000);
  modem_gprs_attach(CELL_APN, CELL_USER, CELL_PWD, 60000);

  size_t res_size;
  int status;
  uint8_t buffer[256];
  for(uint8_t i = 0; i < sizeof(buffer) -1; i++) buffer[i] = (uint8_t) i;

  char version[40];
  size_t chars;
  modem_send("ATI");
  do {
    chars = modem_readline(version, sizeof(version)-1, 500);
    PRINTF("ATI: %s\r\n", version);
  } while(chars && strncmp("OK", version, 2));

  // We need to test the GET and POST requests for version with a response that
  // contains Content-Length and one without. This is important as the response
  // is handled quite differently in the underlying M66 modem.
  PRINTF("------- GET\r\n");
   res_size = 0;
  status = modem_http_get(URL_WITH_CONTENT_LENGTH, &res_size, 60000);
  if (status) ERROR("GET error: %d", status);
  read_response(buffer, sizeof(buffer), res_size);

  PRINTF("------- GET (no Content-Length expected)\r\n");
  res_size = 0;
  status = modem_http_get(URL_WITHOUT_CONTENT_LENGTH, &res_size, 30000);
  if (status) ERROR("GET error: %d", status);
  read_response(buffer, sizeof(buffer), res_size);

  PRINTF("------- POST\r\n");
  res_size = 0;
  status = modem_http_post(URL_WITH_CONTENT_LENGTH, &res_size, buffer, sizeof(buffer) / 4, 80000);
  if (status) ERROR("POST error: %d\r\n", status);
  read_response(buffer, sizeof(buffer), res_size);

  PRINTF("------- POST\r\n");
  res_size = 0;
  status = modem_http_post(URL_WITHOUT_CONTENT_LENGTH, &res_size, buffer, sizeof(buffer) / 4, 80000);
  if (status) ERROR("POST error: %d\r\n", status);
  read_response(buffer, sizeof(buffer), res_size);

  modem_disable();

  return 0;
}

