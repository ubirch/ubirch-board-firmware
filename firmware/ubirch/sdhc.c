/*
 * ubirch#1 SDHC initialization code.
 *
 * @author Matthias L. Jugel
 * @date 2016-04-06
 *
 * @copyright &copy; 2015 ubirch GmbH (https://ubirch.com)
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

#include <fsl_port.h>
#include <fsl_gpio.h>
#include <board.h>
#include <ubirch/timer.h>
#include "sdhc.h"

status_t sdhc_init(void) {
#if  defined(FSL_FEATURE_SOC_SDHC_COUNT) && FSL_FEATURE_SOC_SDHC_COUNT > 0
  const gpio_pin_config_t IN = {kGPIO_DigitalInput, false};

  // enable port clock for SDHC ports
  CLOCK_EnableClock(BOARD_SDHC_PORT_CLOCK);

  port_pin_config_t config = {0};
  config.pullSelect = kPORT_PullUp;
  config.driveStrength = kPORT_HighDriveStrength;
  config.mux = BOARD_SDHC_ALT;

  // SD pins
  PORT_SetPinConfig(BOARD_SDHC_PORT, BOARD_SDHC_PIN_D0, &config);
  PORT_SetPinConfig(BOARD_SDHC_PORT, BOARD_SDHC_PIN_D1, &config);
  PORT_SetPinConfig(BOARD_SDHC_PORT, BOARD_SDHC_PIN_D2, &config);
  PORT_SetPinConfig(BOARD_SDHC_PORT, BOARD_SDHC_PIN_D3, &config);
  PORT_SetPinConfig(BOARD_SDHC_PORT, BOARD_SDHC_PIN_CMD, &config);
  PORT_SetPinConfig(BOARD_SDHC_PORT, BOARD_SDHC_PIN_DCLK, &config);

  // SD card detect pin
  CLOCK_EnableClock(BOARD_SDHC_DET_CLOCK);

  config.driveStrength = kPORT_LowDriveStrength;
  config.mux = kPORT_MuxAsGpio;
  PORT_SetPinConfig(BOARD_SDHC_DET_PORT, BOARD_SDHC_DET_PIN, &config);
  GPIO_PinInit(BOARD_SDHC_DET_GPIO, BOARD_SDHC_DET_PIN, &IN);

  return kStatus_Success;
#else
  return kStatus_Fail;
#endif
}

// ===============================================================================================
// CODE necessary for SDHC file transfer
// ===============================================================================================
static volatile uint32_t sdhc_transfer_complete;
static uint32_t sdhc_adma_table[SDHC_ADMA_TABLE_WORDS];

status_t sdhc_transfer_function(SDHC_Type *base, sdhc_transfer_t *content) {
  return SDHC_TransferBlocking(base, sdhc_adma_table, SDHC_ADMA_TABLE_WORDS, content);
}

extern void EVENT_InitTimer(void);

static volatile uint32_t *EVENT_GetInstance(event_t eventType) {
  volatile uint32_t *event;

  switch (eventType)
  {
    case kEVENT_TransferComplete:
      event = &sdhc_transfer_complete;
          break;
    default:
      event = NULL;
          break;
  }

  return event;
}

bool EVENT_Create(event_t eventType) {
  volatile uint32_t *event = EVENT_GetInstance(eventType);

  if (event) {
    *event = 0;
    return true;
  } else {
    return false;
  }
}

bool EVENT_Wait(event_t eventType, uint32_t timeoutMilliseconds) {
  volatile uint32_t *event = EVENT_GetInstance(eventType);

  if (timeoutMilliseconds && event) {
    timer_set_timeout(1000 * timeoutMilliseconds);
    while ((*event == 0U) && timer_timeout_remaining());
    *event = 0U;

    return timer_timeout_remaining() > 0;
  } else {
    return false;
  }
}

bool EVENT_Notify(event_t eventType) {
  volatile uint32_t *event = EVENT_GetInstance(eventType);

  if (event) {
    *event = 1U;
    return true;
  } else {
    return false;
  }
}

void EVENT_Delete(event_t eventType) {
  volatile uint32_t *event = EVENT_GetInstance(eventType);
  if (event) *event = 0U;
}
