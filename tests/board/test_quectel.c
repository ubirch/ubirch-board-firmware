#include <ubirch/sim800h.h>
#include <fsl_debug_console.h>
#include <ubirch/timer.h>
#include "support.h"
#include "config.h"

extern volatile int state;

void test_quectel() {
  yesno("QUECTEL M66 test?");

  sim800h_init();
  while (true) {
    ok("M66 enable", sim800h_enable());
    sim800h_send("ATI");
    ok("M66 identified", sim800h_expect("Quectel_Ltd", 5000));

    status_t status = 0;
    int level = 0, voltage = 0;
    double lat = 0.0, lon = 0.0;
    rtc_datetime_t date = {0};

    ok("M66 battery status", sim800h_battery(&status, &level, &voltage, 5000));
    PRINTF("M66 status: %d, battery level: %d%%, voltage: %dmV\r\n", status, level, voltage);

    state = 500;
    ok("M66 register network", sim800h_register(6 * 5000));
    state = 100;
    ok("M66 GPRS attach", sim800h_gprs_attach(CELL_APN, CELL_USER, CELL_PWD, 10 * 5000));

// QUECTEL M66 doesn't seem to have a geolocation command
//  state = 2000;
//  ok("M66 Location", sim800h_location(&status, &lat, &lon, &date, 6 * 5000));
//    PRINTF("%04hd-%02hd-%02hd %02hd:%02hd:%02hd\r\n",
//           date.year, date.month, date.day, date.hour, date.minute, date.second);

    sim800h_disable();
    state = 1000;

    delay(10000);
  }
}
