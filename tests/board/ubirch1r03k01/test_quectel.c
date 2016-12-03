#include <ubirch/modem.h>
#include <fsl_debug_console.h>
#include <ubirch/timer.h>
#include "support.h"
#include "config.h"

extern volatile int state;

void test_quectel() {
  yesno("QUECTEL M66 test?");

  modem_init();
  while (true) {
    ok("M66 enable", modem_enable());
    modem_send("ATI");
    ok("M66 identified", modem_expect("Quectel_Ltd", 5000));

    uint8_t status = 0;
    int level = 0, voltage = 0;
    double lat = 0.0, lon = 0.0;
    rtc_datetime_t date = {0};

    ok("M66 battery status", modem_battery(&status, &level, &voltage, 5000));
    PRINTF("M66 status: %d, battery level: %d%%, voltage: %dmV\r\n", status, level, voltage);

    state = 500;
    ok("M66 register network", modem_register(6 * 5000));
    state = 100;
    ok("M66 GPRS attach", modem_gprs_attach(CELL_APN, CELL_USER, CELL_PWD, 10 * 5000));

    state = 2000;

//  send("AT+QHTTPURL=32,30");
//  modem_expect("CONNECT", 1000);
//  send("http://thinkberg.com/keybase.txt");
//  modem_expect_OK(1000);
//  send("AT+QHTTPGET=60");
//  modem_expect_OK(5000);
//  send("AT+QHTTPREAD=30");
//  size_t size, total = 0;
//  do {
//    uint8_t buffer[384];
//    size = read_binary(buffer, 384, 1000);
//    total += size;
//  } while (size > 0);
//  modem_expect_OK(5000);
//  PRINTF("M66 download: %d bytes\r\n", total);

    ok("M66 Location", modem_location(&lat, &lon, &date, 2 * 60000));
    PRINTF("%04hd-%02hd-%02hd %02hd:%02hd:%02hd\r\n",
           date.year, date.month, date.day, date.hour, date.minute, date.second);

    modem_disable();
    state = 1000;

    delay(10000);
  }
}
