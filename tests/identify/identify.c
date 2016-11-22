/**
 * Identify the board.
 *
 * Outputs the UUID and the IMEI (if available)
 *
 * @author Matthias L. Jugel
 * @date 2016-10-04
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

#include <stdint.h>
#include <board.h>
#include <stdio.h>
#include <ubirch/timer.h>
#include <ubirch/device.h>
#include <ubirch/modem.h>
#include <fsl_smc.h>

smc_power_state_t thePowerState;
uint32_t freq = 0;

/*! @brief Show current power mode. */
void APP_ShowPowerMode(smc_power_state_t currentPowerState)
{
  switch (currentPowerState)
  {
    case kSMC_PowerStateRun:
      PRINTF("    Power mode: RUN\r\n");
      break;
    case kSMC_PowerStateVlpr:
      PRINTF("    Power mode: VLPR\r\n");
      break;
    case kSMC_PowerStateHsrun:
      PRINTF("    Power mode: HSRUN\r\n");
      break;
    default:
      PRINTF("    Power mode wrong\r\n");
      break;
  }
}


bool on = true;
volatile uint32_t milliseconds = 0;

void SysTick_Handler() {
  milliseconds++;
  if (milliseconds % 1000 == 0) on = !on;
  BOARD_LED0(on);
}


int main(void) {
  // INITIALIZATION
  board_init();

  // INITIALIZE CONSOLE (Tests debug uart pins!)
  board_console_init(BOARD_DEBUG_BAUD);

  // 100ms led blink, only works if setup for LED was correct
  SysTick_Config(BOARD_SYSTICK_100MS / 10);

  uint32_t uuid[4];
  device_uuid(uuid);
  printf("UUID: %08lX-%04lX-%04lX-%04lX-%04lX%08lX\r\n",
         uuid[0],                     // 8
         uuid[1] >> 16,               // 4
         uuid[1] & 0xFFFF,            // 4
         uuid[2] >> 16,               // 4
         uuid[2] & 0xFFFF, uuid[3]);  // 4+8

  modem_init();
  modem_enable();
  char imei[17];
  modem_imei(imei, 1000);
  printf("IMEI : %s\r\n", imei);
  modem_disable();

  thePowerState = SMC_GetPowerModeState(SMC);
  APP_ShowPowerMode(thePowerState);
  freq = CLOCK_GetFreq(kCLOCK_CoreSysClk);
  PRINTF("    Core Clock = %dHz \r\n", freq);

  delay(1000);

  // Get into HSRUN mode
  BOARD_BootClockHSRUN();

  thePowerState = SMC_GetPowerModeState(SMC);
  APP_ShowPowerMode(thePowerState);
  freq = CLOCK_GetFreq(kCLOCK_CoreSysClk);
  PRINTF("    Core Clock = %dHz \r\n", freq);

  delay(1000);

  // Get back to RUN mode
  APP_SetClockRunFromHsrun();

  thePowerState = SMC_GetPowerModeState(SMC);
  APP_ShowPowerMode(thePowerState);
  freq = CLOCK_GetFreq(kCLOCK_CoreSysClk);
  PRINTF("    Core Clock = %dHz \r\n", freq);

  delay(1000);

  //  Get into VLPR mode
  BOARD_BootClockVLPR();
  board_console_vlpr_init(BOARD_DEBUG_BAUD);

  thePowerState = SMC_GetPowerModeState(SMC);
  APP_ShowPowerMode(thePowerState);
  freq = CLOCK_GetFreq(kCLOCK_CoreSysClk);
  PRINTF("    Core Clock = %dHz \r\n", freq);


  while (true) {
    delay(1000);
  };
}
