#include <support.h>
#include <ubirch/rf-sub-ghz/rf_lpuart.h>
#include <ubirch/timer.h>

void test_uart(void) {
#if TEST_RFUART
  const char *hello = "HELLO WORLD\r\n";

  rf_config_t *rf_config;
  rf_config = &rf_config_default;

  rf_init(rf_config);
  rf_send((const uint8_t *) hello, strlen(hello));
#endif
}
