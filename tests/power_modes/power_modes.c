/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <ubirch/timer.h>
#include "fsl_common.h"
#include "fsl_smc.h"
#include "fsl_lptmr.h"
#include "board.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define LPTMR1_IRQHandler LPTMR0_LPTMR1_IRQHandler
#define LPTMR1_IRQn LPTMR0_LPTMR1_IRQn

/* Power mode definition used in application. */
typedef enum _app_power_mode
{
    kAPP_PowerModeMin = 'A' - 1,
    kAPP_PowerModeRun,   /*!< Run mode. All Kinetis chips. */
    kAPP_PowerModeWait,  /*!< Wait mode. All Kinetis chips. */
    kAPP_PowerModeStop,  /*!< Stop mode. All Kinetis chips. */
    kAPP_PowerModeVlpr,  /*!< Very low power run mode. All Kinetis chips. */
    kAPP_PowerModeVlpw,  /*!< Very low power wait mode. All Kinetis chips. */
    kAPP_PowerModeVlps,  /*!< Very low power stop mode. All Kinetis chips. */
    kAPP_PowerModeLls,   /*!< Low leakage stop mode. All Kinetis chips. */
    kAPP_PowerModeVlls0, /*!< Very low leakage stop 0 mode. Chip-specific. */
    kAPP_PowerModeVlls1, /*!< Very low leakage stop 1 mode. All Kinetis chips. */
    kAPP_PowerModeVlls2, /*!< Very low leakage stop 2 mode. All Kinetis chips. */
    kAPP_PowerModeVlls3, /*!< Very low leakage stop 3 mode. All Kinetis chips. */
    kAPP_PowerModeHsrun, /*!< High-speed run mode. Chip-specific. */
    kAPP_PowerModeMax
} app_power_mode_t;
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief LPTMR0 interrupt handler.
 */
void LPTMR1_IRQHandler(void)
{
  if (kLPTMR_TimerInterruptEnable & LPTMR_GetEnabledInterrupts(LPTMR1))
  {
    LPTMR_DisableInterrupts(LPTMR1, kLPTMR_TimerInterruptEnable);
    LPTMR_ClearStatusFlags(LPTMR1, kLPTMR_TimerCompareFlag);
    LPTMR_StopTimer(LPTMR1);
  }
}

/*!
 * @brief main demo function.
 */
int main(void) {
  uint32_t freq = 0;
  smc_power_state_t currentPowerState;
  lptmr_config_t lptmrConfig;

  SMC_SetPowerModeProtection(SMC, kSMC_AllowPowerModeAll);
  board_init();
  board_console_init(BOARD_DEBUG_BAUD);

  LPTMR_GetDefaultConfig(&lptmrConfig);
  /* Use LPO as clock source. */
  lptmrConfig.prescalerClockSource = kLPTMR_PrescalerClock_2;
  lptmrConfig.bypassPrescaler = true;

  LPTMR_Init(LPTMR1, &lptmrConfig);
  while (1) {
    currentPowerState = SMC_GetPowerModeState(SMC);

    BOARD_ShowPowerMode(SMC_GetPowerModeState(SMC));

    delay(100);

    if (kAPP_PowerModeVlpr == currentPowerState) {
      BOARD_SetClockRUNfromVLPR();
      board_console_init(BOARD_DEBUG_BAUD);
    }
    else {
      BOARD_SetClockVLPR();
      board_console_vlpr_init(BOARD_DEBUG_BAUD);
    }
  }
}
