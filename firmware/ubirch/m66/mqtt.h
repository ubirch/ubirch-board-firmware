//
// Created by dothraki on 10/4/16.
//

#ifndef UBIRCH_FIRMWARE_MQTT_H
#define UBIRCH_FIRMWARE_MQTT_H


#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//void modem_sim_pin(void);
bool modem_mqtt_connect(const char *apn, const char *user, const char *password, uint32_t timeout);
bool modem_mqtt_send(const char *data, uint8_t len);


#endif //UBIRCH_FIRMWARE_MQTT_H
