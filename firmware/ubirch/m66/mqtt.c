//
// Created by dothraki on 10/4/16.
//

#include "mqtt.h"
#include "m66_core.h"
#include "m66_ops.h"
#include "m66_parser.h"
#include "ubirch/timer.h"
#include <stdio.h>

bool modem_mqtt_connect(const char *apn, const char *user, const char *password, uint32_t timeout) {
  modem_init();
  modem_enable();
//  modem_sim_pin();
  modem_register(timeout);
  modem_gprs_attach(apn, user, password, timeout);

  timer_set_timeout(timeout * 1000);

  modem_send("AT+QIMUX=0");
  if (!modem_expect_OK(uTimer_Remaining)) return false;

  modem_send("AT+QIMODE=0");
  if (!modem_expect_OK(uTimer_Remaining)) return false;
  printf("MUX and MODE selected \r\n");

  modem_send("AT+QIDNSIP=1");
  if (!modem_expect_OK(uTimer_Remaining)) return false;
  printf("DNS and not IP\r\n");

//    modem_send("AT+QILOCIP");
//    if (modem_expect("ERROR", uTimer_Remaining)) return false;

//     TODO: Add header to the received data
//     we need these AT commands to configure the way we receive the O/P
//     we also need this so that we can easily diff b/w received data and
//     the echo / URC replies or errors
//    AT+QINDI=1; //set mode when receiving the data
//    AT+QIHEAD=1;  // add info IPD<len>: before the received data
//    AT+QISHOWRA=1; // display IP address and port of the sender
//    AT+QISHOWPT=1; // show transmission layer protocol type, TCP or UDP
  modem_send("AT+QIOPEN=\"TCP\",\"api.ubirch.com\",\"80\"");
//  modem_send("AT+QIOPEN=\"TCP\",\"iot.eclipse.org\",1883");
//  modem_send("AT+OPEN=\"TCP\",\"192.168.67.28\", 80");
  if (!modem_expect_OK(uTimer_Remaining)) return false;
  if (!modem_expect("CONNECT OK", 2000)) return false;

  return true;
}


bool modem_mqtt_send(const char *data, uint8_t len)
{
  //TODO add timer here

  modem_send("AT+QISACK");
  if (modem_expect("+QISACK", 3000)) printf("see acack\r\n");


  modem_send("AT+QISEND=%d", len);
  delay(1000);
//  if (modem_expect('> ', 3000))
//  {
    modem_send(data);
//  }
//  else
//  {
//    printf("we did not receive < \r\n");
//  }
//  else return false;

  if (!modem_expect("SEND OK", uTimer_Remaining)) return false;

//  see if we need delay before requesting for ACK
//  delay(3000);
//  modem_send("AT+QISACK");
//  HERE SCAN FOR THE REPLY +QISACK: tx_len, ack_len, yet_to_ack_len

//  read the second line for OK
  if (!modem_expect_OK(uTimer_Remaining)) return false;

  modem_send("AT+QICLOSE");
  if (!modem_expect_OK(uTimer_Remaining)) return false;

  modem_send("AT+QIDEACT");
  if (!modem_expect_OK(uTimer_Remaining)) return false;

  modem_disable();

  return true;
}
