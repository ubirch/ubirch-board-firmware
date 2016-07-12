#include <ubirch/sim800h.h>
#include <fsl_debug_console.h>
#include "support.h"
#include "config.h"

void test_quectel() {
  yesno("QUECTEL M66 test?");

  sim800h_init();
  ok("M66 enable", sim800h_enable());
  sim800h_send("ATI");
  ok("M66 identified", sim800h_expect("Quectel_Ltd", 5000));

  status_t status = 0;
  int level = 0, voltage = 0;

  ok("M66 battery status", sim800h_battery(&status, &level, &voltage, 5000));
  PRINTF("M66 status: %d, battery level: %d%%, voltage: %dmV\r\n", status, level, voltage);

  ok("M66 register network", sim800h_register(6*5000));
  ok("M66 GPRS attach",sim800h_gprs_attach(CELL_APN, CELL_USER, CELL_PWD, 6 * 5000) );
}
