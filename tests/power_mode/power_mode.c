/**
 * Test the behaviour of the MODEM upon power enable.
 *
 * @author Matthias L. Jugel
 * @date 2017-01-10
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
#include <ubirch/modem.h>
#include <ubirch/dbgutil.h>
#include <ubirch/rtc.h>

//int main(void) {
//  board_init(BOARD_MODE_RUN);
//  board_console_init(BOARD_DEBUG_BAUD);
//
//  while (true) {
//    PRINTF("WAIT 10s\r\n");
//    for (int i = 0; i < 5; i++) {
//      BOARD_LED0(true);
//      delay(500);
//      BOARD_LED0(false);
//      delay(500);
//    }
//
//    PRINTF("SLEEP 10s\r\n");
//    sleep(10);
//  }
//}

#include "fsl_smc.h"
#include "fsl_llwu.h"
#include "fsl_rcm.h"
#include "fsl_pmc.h"

/*!
 * @brief LLWU interrupt handler.
 */
void LLWU_IRQHandler(void)
{
  /* If wakeup by LPTMR. */
  if (LLWU_GetInternalWakeupModuleFlag(LLWU, 5U))
  {
    RTC_DisableInterrupts(BOARD_RTC, kRTC_AlarmInterruptEnable);
    RTC_ClearStatusFlags(BOARD_RTC, kRTC_AlarmFlag);
  }
}


//LLWU_EnableInternalModuleInterruptWakup(LLWU, LLWU_LPTMR_IDX, true);


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

/*!
 * @brief main demo function.
 */
int main(void)
{
  uint32_t freq = 0;
  smc_power_state_t currentPowerState;

  /* Power related. */
  SMC_SetPowerModeProtection(SMC, kSMC_AllowPowerModeAll);
  rcm_reset_source_t wakeupFromVLLS0 = kRCM_SourceWakeup & RCM_GetPreviousResetSources(RCM);
  if (wakeupFromVLLS0 == kRCM_SourceWakeup) /* Wakeup from VLLS. */
  {
    PMC_ClearPeriphIOIsolationFlag(PMC);
    NVIC_ClearPendingIRQ(LLWU_IRQn);

    /* Wait for PLL lock. */
    while (!(kMCG_Pll0LockFlag & CLOCK_GetStatusFlags())) {}
    CLOCK_SetPeeMode();
  }

  board_init(BOARD_MODE_RUN);
  board_console_init(BOARD_DEBUG_BAUD);

  uint32_t counter = 0;
  while (1)
  {
    currentPowerState = SMC_GetPowerModeState(SMC);

    freq = CLOCK_GetFreq(kCLOCK_CoreSysClk);

    PRINTF("\r\nRUN COUNT=%d (wakeup from VLLS=%d)\r\n", ++counter, wakeupFromVLLS0);

    PRINTF("\r\n####################  Power Manager Demo ####################\n\r\n");
    PRINTF("    Core Clock = %dHz \r\n", freq);

    APP_ShowPowerMode(currentPowerState);

    PRINTF("SLEEP!\r\n");
    /* Wait for debug console output finished. */
    while (!(kLPUART_TransmissionCompleteFlag & LPUART_GetStatusFlags((LPUART_Type *)BOARD_DEBUG_UART))) {}
    DbgConsole_Deinit();
    PORT_SetPinMux(BOARD_DEBUG_PORT, BOARD_DEBUG_RX_PIN, kPORT_PinDisabledOrAnalog);

    // set LLWU wakeup config
    LLWU_EnableInternalModuleInterruptWakup(LLWU, 5U, true);
    NVIC_EnableIRQ(LLWU_IRQn);

    // set wakeup event
    rtc_init();
    rtc_set_alarm_in(5);

    smc_power_mode_vlls_config_t vlls_config; /* Local variable for vlls configuration */
    vlls_config.subMode = kSMC_StopSub3;
    vlls_config.enablePorDetectInVlls0 = true;
//    vlls_config.enableRam2InVlls2 = true;
    vlls_config.enableLpoClock = true;
    SMC_SetPowerModeVlls(SMC, &vlls_config);
  }
}
