/*!
 * @brief ubirch#1 i2c driver code (blocking).
 *
 * Driver for I2C chipset on the ubirch#1.
 * This code uses the synchronous functions for executing i2c transfers.
 *
 * @author Matthias L. Jugel
 * @date 2016-04-01
 *
 * Copyright 2016 ubirch GmbH (https://ubirch.com)
 *
 * == LICENSE ==
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _UBIRCH_I2C_CORE_H_
#define _UBIRCH_I2C_CORE_H_

#include <drivers/fsl_common.h>
#include <drivers/fsl_port.h>

/**
 * Available I2C speeds.
 */
typedef enum i2c_speed {
    I2C_STANDARD   =  100000U, // 100kHz
    I2C_FULL_SPEED =  400000U, // 400kHz
    I2C_FAST_MODE  = 1000000U, //   1MHz
    I2C_HIGH_SPEED = 3400000U  // 3.4MHz
} i2c_speed_t;

typedef struct i2c_config {
    I2C_Type *i2c;
    PORT_Type *port;
    clock_name_t i2c_clock;
    clock_ip_name_t port_clock;
    uint32_t SCL, SDA;
    port_mux_t mux;
    uint32_t baud;
} i2c_config_t;

/**
 * Initialize the I2C bus with a specific speed.
 * @param speed bus speed
 */
void i2c_init(i2c_config_t speed);

/**
 * Deinitialize I2C bus
 */
void i2c_deinit();

/**
 * Ping an I2C device on the bus, will return kStatus_Success if available.
 * @param address the 7 bit device address
 */
status_t i2c_ping(uint8_t address);

/**
 * Write data to a specific register on the bus.
 * @param address the 7 bit device address
 * @param reg the register to write to
 * @param data a data array to write
 * @param size the amount of bytes to write
 */
status_t i2c_write(uint8_t address, uint8_t reg, uint8_t *data, size_t size);

/**
 * Read data from the device.
 * @param address the 7 bit device address
 * @param reg start address to read from
 * @param data a data array to read into
 * @param size the amount of bytes to read
 */
status_t i2c_read(uint8_t address, uint8_t reg, uint8_t *data, size_t size);

/**
 * Read a single byte from a register.
 * @param address the 7 bit device address
 * @param reg the register to read from
 */
uint8_t i2c_read_reg(uint8_t address, uint8_t reg);

/**
 * Read a two byte integer from a register
 * @param address the 7 bit device address
 * @param reg the register to read from
 */
uint16_t i2c_read_reg16(uint8_t address, uint8_t reg);

#ifndef NDEBUG
/**
 * Write a human readable I2C response code translation
 * to the debug console including explanation.
 * @param s a string with details on the current operation
 * @param status the status_t reference to check
 */
void i2c_error(char *s, status_t status);
#endif

#endif // _UBIRCH_I2C_CORE_H
