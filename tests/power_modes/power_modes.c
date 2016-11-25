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
#include "power_manager.h"
#include "board.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define LPTMR0_IRQHandler LPTMR0_LPTMR1_IRQHandler
#define LPTMR0_IRQn LPTMR0_LPTMR1_IRQn

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
void LPTMR0_IRQHandler(void)
{
  if (kLPTMR_TimerInterruptEnable & LPTMR_GetEnabledInterrupts(LPTMR0))
  {
    LPTMR_DisableInterrupts(LPTMR0, kLPTMR_TimerInterruptEnable);
    LPTMR_ClearStatusFlags(LPTMR0, kLPTMR_TimerCompareFlag);
    LPTMR_StopTimer(LPTMR0);
  }
}

/*!
 * @brief main demo function.
 */
int main(void) {
  uint32_t freq = 0;
  smc_power_state_t currentPowerState;
  lptmr_config_t lptmrConfig;

//    BOARD_InitPins();
  SMC_SetPowerModeProtection(SMC, kSMC_AllowPowerModeAll);
  board_init();
  BOARD_BootClockRUN();
  board_console_init(BOARD_DEBUG_BAUD);

  LPTMR_GetDefaultConfig(&lptmrConfig);
  /* Use LPO as clock source. */
  lptmrConfig.prescalerClockSource = kLPTMR_PrescalerClock_1;
  lptmrConfig.bypassPrescaler = true;

  LPTMR_Init(LPTMR0, &lptmrConfig);

  while (1) {
    currentPowerState = SMC_GetPowerModeState(SMC);

    BOARD_ShowPowerMode(SMC_GetPowerModeState(SMC));

//    delay(1000);
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
