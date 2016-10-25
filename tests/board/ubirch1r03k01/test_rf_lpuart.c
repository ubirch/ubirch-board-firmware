#include <ubirch/rf-sub-ghz/rf_lpuart.h>

void test_uart(void) {
  rf_config_t *rf_config;
  rf_config = &rf_config_default;

//  uint8_t buf[] = {0};

  rf_init(rf_config);
  const char *hello = "HELLO WORLD\r\n";
//  rf_read(buf, 100);
  rf_send((const uint8_t *) hello, strlen(hello));
}
