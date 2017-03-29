/*
 * ubirch#1 i2c driver code (blocking).
 *
 * @author Matthias L. Jugel
 * @date 2016-04-01
 *
 * @copyright &copy; 2016 ubirch GmbH (https://ubirch.com)
 *
 * ```
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
 * ```
 */

#include <drivers/fsl_i2c.h>
#include <utilities/fsl_debug_console.h>
#include "i2c.h"

static i2c_config_t *_config;

void i2c_init(i2c_config_t *config) {
  _config = config;

  port_pin_config_t pinConfig;

  // do a sensible default configuration for the I2C pins
  pinConfig.pullSelect = kPORT_PullUp;
  pinConfig.slewRate = kPORT_FastSlewRate;
  pinConfig.passiveFilterEnable = kPORT_PassiveFilterDisable;
  pinConfig.openDrainEnable = kPORT_OpenDrainEnable;
  pinConfig.driveStrength = kPORT_LowDriveStrength;
  pinConfig.mux = _config->mux;

  CLOCK_EnableClock(_config->port_clock);
  PORT_SetPinConfig(_config->port, _config->SCL, &pinConfig);
  PORT_SetPinConfig(_config->port, _config->SDA, &pinConfig);

  // configure I2C
  i2c_master_config_t i2c_config;
  I2C_MasterGetDefaultConfig(&i2c_config);
  i2c_config.baudRate_Bps = config->baud;
  I2C_MasterInit(_config->i2c, &i2c_config, CLOCK_GetFreq(_config->i2c_clock));
}

void i2c_deinit() {
  I2C_MasterDeinit(_config->i2c);
  CLOCK_DisableClock(_config->port_clock);
}

status_t i2c_ping(uint8_t address) {
  i2c_master_transfer_t transfer;
  memset(&transfer, 0, sizeof(transfer));

  transfer.slaveAddress = address;
  transfer.direction = kI2C_Write;
  transfer.flags = kI2C_TransferDefaultFlag;
  status_t status = I2C_MasterTransferBlocking(_config->i2c, &transfer);
  I2C_MasterStop(_config->i2c);
  return status;
}

status_t i2c_write(uint8_t address, uint8_t reg, uint8_t *data, size_t size) {
  i2c_master_transfer_t transfer;
  memset(&transfer, 0, sizeof(transfer));

  transfer.slaveAddress = address;
  transfer.direction = kI2C_Write;
  transfer.subaddress = reg;
  transfer.subaddressSize = 1;
  transfer.data = data;
  transfer.dataSize = size;
  transfer.flags = kI2C_TransferDefaultFlag;

  status_t status = I2C_MasterTransferBlocking(_config->i2c, &transfer);
#ifndef NDEBUG
  if (status != kStatus_Success) {
    PRINTF("I2C write(%02d) <= %02x\r\n", reg, *data);
    i2c_error("write reg", status);
  }
#endif

  if (!size && status == kStatus_Success)
    status = I2C_MasterStop(_config->i2c);

  return status;
}

status_t i2c_read(uint8_t address, uint8_t reg, uint8_t *data, size_t size) {

  i2c_master_transfer_t transfer;
  memset(&transfer, 0, sizeof(transfer));

  transfer.slaveAddress = address;
  transfer.direction = kI2C_Read;
  transfer.subaddress = reg;
  transfer.subaddressSize = 1;
  transfer.data = data;
  transfer.dataSize = size;
  transfer.flags = kI2C_TransferDefaultFlag;
  status_t status = I2C_MasterTransferBlocking(_config->i2c, &transfer);

#ifndef NDEBUG
  if (status != kStatus_Success) {
    PRINTF("I2C read(%02d) => %d byte\r\n", reg, transfer.dataSize);
    i2c_error("read (address)", status);
    I2C_MasterStop(_config->i2c);
  }
#endif

  return status;
}

uint8_t i2c_read_reg(uint8_t address, uint8_t reg) {
  uint8_t value = 0;
  i2c_read(address, reg, &value, 1);
  return value;
}

void i2c_error(char *s, status_t status) {
  if (status == kStatus_Success) return;
  PRINTF("%s (%x): ", s, status);
  switch (status) {
    case kStatus_I2C_Nak:
      PRINTF("NAK\r\n", s);
          break;
    case kStatus_I2C_ArbitrationLost:
      PRINTF("ARBITRATION LOST\r\n");
          break;
    case kStatus_I2C_Busy:
      PRINTF("BUSY\r\n");
          break;
    case kStatus_I2C_Idle:
      PRINTF("IDLE\r\n");
          break;
    case kStatus_I2C_Timeout:
      PRINTF("TIMEOUT\r\n");
          break;
#if !(defined(BOARD_FRDM_KL82Z) || defined(BOARD_USENSE))
    case kStatus_I2C_Addr_Nak:
      PRINTF("ADDR NAK\r\n");
          break;
#endif
    default:
      PRINTF("UNKNOWN ERROR\r\n", status);
          break;
  }
}
