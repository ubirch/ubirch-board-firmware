#include <ubirch/uart.h>

void test_uart(void) {
  uart2_init();
  const char *hello = "HELLO WORLD\r\n";
  uart2_write((const uint8_t *) hello, 13);
}
