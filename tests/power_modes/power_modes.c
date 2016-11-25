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

#include "fsl_common.h"
#include "fsl_smc.h"
#include "fsl_llwu.h"
#include "fsl_rcm.h"
#include "fsl_lptmr.h"
#include "fsl_port.h"
#include "fsl_debug_console.h"
#include "power_manager.h"
#include "fsl_notifier.h"
#include "board.h"

//#include "pin_mux.h"
#include "fsl_lpuart.h"
#include "fsl_pmc.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define BOARD_SW3_GPIO GPIOC
#define BOARD_SW3_PORT PORTC
#define BOARD_SW3_GPIO_PIN 6U
#define BOARD_SW3_IRQ PORTC_IRQn
#define BOARD_SW3_IRQ_HANDLER PORTC_IRQHandler
#define BOARD_SW3_NAME "SW3"


/* Debug console RX pin: PORTC14 MUX: 3 */
#define DEBUG_CONSOLE_RX_PORT PORTC
#define DEBUG_CONSOLE_RX_GPIO GPIOC
#define DEBUG_CONSOLE_RX_PIN 14
#define DEBUG_CONSOLE_RX_PINMUX kPORT_MuxAlt3
/* Debug console TX pin: PORTC15 MUX: 3 */
#define DEBUG_CONSOLE_TX_PORT PORTC
#define DEBUG_CONSOLE_TX_GPIO GPIOC
#define DEBUG_CONSOLE_TX_PIN 15
#define DEBUG_CONSOLE_TX_PINMUX kPORT_MuxAlt3

#define LPTMR0_IRQHandler LPTMR0_LPTMR1_IRQHandler
#define LPTMR0_IRQn LPTMR0_LPTMR1_IRQn

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*
 * Set the clock configuration for HSRUN mode.
 */
//extern void APP_SetClockHsrun(void);

/*
 * Power mode switch callback.
 */
//status_t callback0(notifier_notification_block_t *notify, void *dataPtr);

/*******************************************************************************
 * Variables
 ******************************************************************************/
//static uint8_t s_wakeupTimeout;            /* Wakeup timeout. (Unit: Second) */
//static app_wakeup_source_t s_wakeupSource; /* Wakeup source.                 */

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief LLWU interrupt handler.
 */

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
 * @brief button interrupt handler.
 */

/*!
 * @brief Get wakeup source by user input.
 */

/*! @brief Get wakeup timeout and wakeup source. */

/*! @brief Set wakeup timeout and wakeup source. */

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
 * @brief check whether could switch to target power mode from current mode.
 * Return true if could switch, return false if could not switch.
 */

/*!
 * @brief Power mode switch.
 * This function is used to register the notifier_handle_t struct's member userFunction.
 */


/*!
 * @brief main demo function.
 */
int main(void)
{
  uint32_t freq = 0;
//    uint8_t ch;
//    uint8_t targetConfigIndex;
//    notifier_handle_t powerModeHandle;
  smc_power_state_t currentPowerState;
//    app_power_mode_t targetPowerMode;
//    bool needSetWakeup; /* Flag of whether or not need to set wakeup. */
  lptmr_config_t lptmrConfig;

  /*Power mode configurations*/
  power_user_config_t vlprConfig = {
    kAPP_PowerModeVlpr,

    true,

    true,

    true,
  };

  power_user_config_t vlpwConfig = vlprConfig;
  power_user_config_t vlls1Config = vlprConfig;
  power_user_config_t vlls3Config = vlprConfig;
  power_user_config_t vlpsConfig = vlprConfig;
  power_user_config_t waitConfig = vlprConfig;
  power_user_config_t stopConfig = vlprConfig;
  power_user_config_t runConfig = vlprConfig;

  power_user_config_t llsConfig = vlprConfig;

  power_user_config_t vlls0Config = vlprConfig;

  power_user_config_t vlls2Config = vlprConfig;

  power_user_config_t hsrunConfig = vlprConfig;

  /* Initializes array of pointers to power mode configurations */
  notifier_user_config_t *powerConfigs[] = {
    &runConfig,   &waitConfig, &stopConfig, &vlprConfig, &vlpwConfig, &vlpsConfig, &llsConfig,

    &vlls0Config,

    &vlls1Config,

    &vlls2Config,

    &vlls3Config,

    &hsrunConfig,
  };

  /* User callback data0 */
//  user_callback_data_t callbackData0;
//
//  /* Initializes callback configuration structure */
//  notifier_callback_config_t callbackCfg0 = {callback0, kNOTIFIER_CallbackBeforeAfter, (void *)&callbackData0};
//
//  /* Initializes array of callback configurations */
//  notifier_callback_config_t callbacks[] = {callbackCfg0};
//
//  memset(&callbackData0, 0, sizeof(user_callback_data_t));

  /* Initializes configuration structures */
  vlpwConfig.mode = kAPP_PowerModeVlpw;
  vlls1Config.mode = kAPP_PowerModeVlls1;
  vlls3Config.mode = kAPP_PowerModeVlls3;
  vlpsConfig.mode = kAPP_PowerModeVlps;
  waitConfig.mode = kAPP_PowerModeWait;
  stopConfig.mode = kAPP_PowerModeStop;
  runConfig.mode = kAPP_PowerModeRun;

  llsConfig.mode = kAPP_PowerModeLls;

  vlls0Config.mode = kAPP_PowerModeVlls0;

  vlls2Config.mode = kAPP_PowerModeVlls2;

  hsrunConfig.mode = kAPP_PowerModeHsrun;

  /* Create Notifier Handle */



//    NOTIFIER_CreateHandle(&powerModeHandle, powerConfigs, ARRAY_SIZE(powerConfigs), callbacks, 1U, APP_PowerModeSwitch,
//                          NULL);

  /* Power related. */
  SMC_SetPowerModeProtection(SMC, kSMC_AllowPowerModeAll);
//  if (kRCM_SourceWakeup & RCM_GetPreviousResetSources(RCM)) /* Wakeup from VLLS. */
//  {
//    PMC_ClearPeriphIOIsolationFlag(PMC);
//    NVIC_ClearPendingIRQ(LLWU_IRQn);
//  }

//    BOARD_InitPins();
  board_init();
  BOARD_BootClockRUN();
//    APP_InitDebugConsole();
  board_console_init(BOARD_DEBUG_BAUD);
  /* Setup LPTMR. */
  /*
   * lptmrConfig.timerMode = kLPTMR_TimerModeTimeCounter;
   * lptmrConfig.pinSelect = kLPTMR_PinSelectInput_0;
   * lptmrConfig.pinPolarity = kLPTMR_PinPolarityActiveHigh;
   * lptmrConfig.enableFreeRunning = false;
   * lptmrConfig.bypassPrescaler = true;
   * lptmrConfig.prescalerClockSource = kLPTMR_PrescalerClock_1;
   * lptmrConfig.value = kLPTMR_Prescale_Glitch_0;
   */
  LPTMR_GetDefaultConfig(&lptmrConfig);
  /* Use LPO as clock source. */
  lptmrConfig.prescalerClockSource = kLPTMR_PrescalerClock_1;
  lptmrConfig.bypassPrescaler = true;

  LPTMR_Init(LPTMR0, &lptmrConfig);

//    NVIC_EnableIRQ(LLWU_IRQn);
//
//    NVIC_EnableIRQ(LPTMR0_IRQn);
//
//    NVIC_EnableIRQ(APP_WAKEUP_BUTTON_IRQ);

//    /* Wakeup from VLLS. */
//    if (kRCM_SourceWakeup & RCM_GetPreviousResetSources(RCM))
//    {
//        PRINTF("\r\nMCU wakeup from VLLS modes...\r\n");
//    }

  while (1)
  {
    currentPowerState = SMC_GetPowerModeState(SMC);

    freq = CLOCK_GetFreq(kCLOCK_CoreSysClk);

    PRINTF("\r\n####################  Power Manager Demo ####################\n\r\n");
    PRINTF("    Core Clock = %dHz \r\n", freq);

    APP_ShowPowerMode(currentPowerState);

//        PRINTF("\r\nSelect the desired operation \n\r\n");
//        PRINTF("Press  %c for enter: RUN      - Normal RUN mode\r\n", kAPP_PowerModeRun);
//        PRINTF("Press  %c for enter: WAIT     - Wait mode\r\n", kAPP_PowerModeWait);
//        PRINTF("Press  %c for enter: STOP     - Stop mode\r\n", kAPP_PowerModeStop);
//        PRINTF("Press  %c for enter: VLPR     - Very Low Power Run mode\r\n", kAPP_PowerModeVlpr);
//        PRINTF("Press  %c for enter: VLPW     - Very Low Power Wait mode\r\n", kAPP_PowerModeVlpw);
//        PRINTF("Press  %c for enter: VLPS     - Very Low Power Stop mode\r\n", kAPP_PowerModeVlps);
//        PRINTF("Press  %c for enter: LLS/LLS3 - Low Leakage Stop mode\r\n", kAPP_PowerModeLls);
//
//        PRINTF("Press  %c for enter: VLLS0    - Very Low Leakage Stop 0 mode\r\n", kAPP_PowerModeVlls0);
//
//        PRINTF("Press  %c for enter: VLLS1    - Very Low Leakage Stop 1 mode\r\n", kAPP_PowerModeVlls1);
//
//        PRINTF("Press  %c for enter: VLLS2    - Very Low Leakage Stop 2 mode\r\n", kAPP_PowerModeVlls2);
//
//        PRINTF("Press  %c for enter: VLLS3    - Very Low Leakage Stop 3 mode\r\n", kAPP_PowerModeVlls3);
//
//        PRINTF("Press  %c for enter: HSRUN    - High Speed RUN mode\r\n", kAPP_PowerModeHsrun);
//
//        PRINTF("\r\nWaiting for power mode select..\r\n\r\n");
//
//        /* Wait for user response */
//        ch = GETCHAR();
//
//        if ((ch >= 'a') && (ch <= 'z'))
//        {
//            ch -= 'a' - 'A';
//        }
//
//        targetPowerMode = (app_power_mode_t)ch;

//      BOARD_SetClockVLPR();

    if (kAPP_PowerModeVlpr == currentPowerState)
    {
      BOARD_SetClockRUNfromVLPR();
      board_console_init(BOARD_DEBUG_BAUD);
//        APP_SetClockRunFromVlpr();
    }
    else
    {
      BOARD_SetClockVLPR();
      board_console_vlpr_init(BOARD_DEBUG_BAUD);
    }
//        if ((targetPowerMode > kAPP_PowerModeMin) && (targetPowerMode < kAPP_PowerModeMax))
//        {
//            /* If could not set the target power mode, loop continue. */
//            if (!APP_CheckPowerMode(currentPowerState, targetPowerMode))
//            {
//                continue;
//            }
//
//            /* If target mode is RUN/VLPR/HSRUN, don't need to set wakeup source. */
//            if ((kAPP_PowerModeRun == targetPowerMode) || (kAPP_PowerModeHsrun == targetPowerMode) ||
//                (kAPP_PowerModeVlpr == targetPowerMode))
//            {
//                needSetWakeup = false;
//            }
//            else
//            {
//                needSetWakeup = true;
//            }
//
//            if (needSetWakeup)
//            {
//                APP_GetWakeupConfig(targetPowerMode);
//                APP_SetWakeupConfig(targetPowerMode);
//            }
//
//            callbackData0.originPowerState = currentPowerState;
//            targetConfigIndex = targetPowerMode - kAPP_PowerModeMin - 1;
//            NOTIFIER_SwitchConfig(&powerModeHandle, targetConfigIndex, kNOTIFIER_PolicyAgreement);
//            PRINTF("\r\nNext loop\r\n");
//        }
  }
}
