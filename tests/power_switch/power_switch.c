//
// Created by nirao on 20.03.17.
//

#include "power_switch.h"

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
#include "power_switch.h"
#include "board.h"
#include "fsl_debug_console.h"

//#include "pin_mux.h"
#include "fsl_lpuart.h"
#include "fsl_pmc.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
//#define APP_DEBUG_UART_BAUDRATE 9600 /* Debug console baud rate. */

/* Default debug console clock source. */
//#define APP_DEBUG_UART_DEFAULT_CLKSRC_NAME kCLOCK_Osc0ErClk /* OSC0 external reference clock (OSC0ERCLK) */
//#define APP_DEBUG_UART_DEFAULT_CLKSRC 0x02

/* Debug console clock source in VLPR mode. */
#define APP_DEBUG_UART_VLPR_CLKSRC_NAME kCLOCK_McgInternalRefClk /* MCGIRCLK */
#define APP_DEBUG_UART_VLPR_CLKSRC 0x03

#define LLWU_LPTMR_IDX 8U       /* LLWU_M0IF */
#define LLWU_WAKEUP_PIN_IDX 7U /* LLWU_P7 */
#define LLWU_WAKEUP_PIN_TYPE kLLWU_ExternalPinFallingEdge

#define APP_WAKEUP_BUTTON_GPIO GPIOC
#define APP_WAKEUP_BUTTON_PORT PORTC
#define APP_WAKEUP_BUTTON_GPIO_PIN 3U
#define APP_WAKEUP_BUTTON_IRQ PORTC_IRQn
#define APP_WAKEUP_BUTTON_IRQ_HANDLER PORTC_IRQHandler
#define APP_WAKEUP_BUTTON_NAME "Board Button"
#define APP_WAKEUP_BUTTON_IRQ_TYPE kPORT_InterruptFallingEdge

//PORT_SetPinMux(APP_WAKEUP_BUTTON_PORT, LLWU_WAKEUP_PIN_IDX, kPORT_MuxAsGpio);
//GPIO_PinInit(APP_WAKEUP_BUTTON_GPIO, LLWU_WAKEUP_PIN_IDX, &IN);

/* Debug console RX pin: PORTC14 MUX: 3 */
//#define DEBUG_CONSOLE_RX_PORT PORTC
//#define DEBUG_CONSOLE_RX_GPIO GPIOC
//#define DEBUG_CONSOLE_RX_PIN 14
//#define DEBUG_CONSOLE_RX_PINMUX kPORT_MuxAlt3
/* Debug console TX pin: PORTC15 MUX: 3 */
//#define DEBUG_CONSOLE_TX_PORT PORTC
//#define DEBUG_CONSOLE_TX_GPIO GPIOC
//#define DEBUG_CONSOLE_TX_PIN 15
//#define DEBUG_CONSOLE_TX_PINMUX kPORT_MuxAlt3

#define LPTMR0_IRQHandler LPTMR0_LPTMR1_IRQHandler
#define LPTMR0_IRQn LPTMR0_LPTMR1_IRQn

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void APP_PowerPreSwitchHook(smc_power_state_t originPowerState, app_power_mode_t targetMode);
void APP_PowerPostSwitchHook(smc_power_state_t originPowerState, app_power_mode_t targetMode);
/*
 * Set the clock configuration for HSRUN mode.
 */
extern void APP_SetClockHsrun(void);

/*
 * Set the clock configuration for RUN mode from HSRUN mode.
 */
extern void APP_SetClockRunFromHsrun(void);

/*
 * Set the clock configuration for RUN mode from VLPR mode.
 */
extern void APP_SetClockRunFromVlpr(void);

/*
 * Set the clock configuration for VLPR mode.
 */
extern void APP_SetClockVlpr(void);

/*
 * Hook function called before power mode switch.
 */
extern void APP_PowerPreSwitchHook(smc_power_state_t originPowerState, app_power_mode_t targetMode);

/*
 * Hook function called after power mode switch.
 */
extern void APP_PowerPostSwitchHook(smc_power_state_t originPowerState, app_power_mode_t targetMode);

/*******************************************************************************
 * Variables
 ******************************************************************************/
static uint8_t s_wakeupTimeout;            /* Wakeup timeout. (Unit: Second) */
static app_wakeup_source_t s_wakeupSource; /* Wakeup source.                 */

/*******************************************************************************
 * Code
 ******************************************************************************/



volatile uint32_t milliseconds = 0;
bool on = true;

void SysTick_Handler() {
    milliseconds++;
    if (milliseconds % 1000 == 0) on = !on;
    BOARD_LED0(on);
}



void APP_SetClockVlpr(void)
{
    const sim_clock_config_t simConfig = {
            .pllFllSel = 3U,        /* PLLFLLSEL select IRC48MCLK. */
            .pllFllDiv = 0U,        /* PLLFLLSEL clock divider divisor. */
            .pllFllFrac = 0U,       /* PLLFLLSEL clock divider fraction. */
            .er32kSrc = 5U,         /* ERCLK32K selection, use RTC. */
            .clkdiv1 = 0x00040000U, /* SIM_CLKDIV1. */
    };

    CLOCK_SetSimSafeDivs();
    CLOCK_SetInternalRefClkConfig(kMCG_IrclkEnable, kMCG_IrcFast, 0U);

    /* MCG works in PEE mode now, will switch to BLPI mode. */

    CLOCK_ExternalModeToFbeModeQuick();  /* Enter FBE. */
    CLOCK_SetFbiMode(kMCG_Dmx32Default, kMCG_DrsLow, NULL); /* Enter FBI. */
    CLOCK_SetLowPowerEnable(true);       /* Enter BLPI. */

    CLOCK_SetSimConfig(&simConfig);
}

void APP_SetClockRunFromVlpr(void)
{
    const sim_clock_config_t simConfig = {
            .pllFllSel = 1U,        /* PLLFLLSEL select PLL. */
            .pllFllDiv = 0U,        /* PLLFLLSEL clock divider divisor. */
            .pllFllFrac = 0U,       /* PLLFLLSEL clock divider fraction. */
            .er32kSrc = 5U,         /* ERCLK32K selection, use RTC. */
            .clkdiv1 = 0x01140000U, /* SIM_CLKDIV1. */
    };

    const mcg_pll_config_t pll0Config = {
            .enableMode = 0U, .prdiv = 0x00U, .vdiv = 0x04U,
    };

    CLOCK_SetSimSafeDivs();

    /* Currently in BLPI mode, will switch to PEE mode. */
    /* Enter FBI. */
    CLOCK_SetLowPowerEnable(false);
    /* Enter FBE. */
    CLOCK_SetFbeMode(4U, kMCG_Dmx32Default, kMCG_DrsLow, NULL);
    /* Enter PBE. */
    CLOCK_SetPbeMode(kMCG_PllClkSelPll0, &pll0Config);
    /* Enter PEE. */
    CLOCK_SetPeeMode();

    CLOCK_SetSimConfig(&simConfig);
}

void APP_SetClockHsrun(void)
{
    const sim_clock_config_t simConfig = {
            .pllFllSel = 1U,        /* PLLFLLSEL select PLL. */
            .pllFllDiv = 0U,        /* PLLFLLSEL clock divider divisor. */
            .pllFllFrac = 0U,       /* PLLFLLSEL clock divider fraction. */
            .er32kSrc = 5U,         /* ERCLK32K selection, use RTC. */
            .clkdiv1 = 0x01150000U, /* SIM_CLKDIV1. */
    };

    const mcg_pll_config_t pll0Config = {
            .enableMode = 0U, .prdiv = 0x00U, .vdiv = 0x09U,
    };

    CLOCK_SetPbeMode(kMCG_PllClkSelPll0, &pll0Config);
    CLOCK_SetPeeMode();

    CLOCK_SetSimConfig(&simConfig);
}

void APP_SetClockRunFromHsrun(void)
{
    const sim_clock_config_t simConfig = {
            .pllFllSel = 1U,        /* PLLFLLSEL select PLL. */
            .pllFllDiv = 0U,        /* PLLFLLSEL clock divider divisor. */
            .pllFllFrac = 0U,       /* PLLFLLSEL clock divider fraction. */
            .er32kSrc = 5U,         /* ERCLK32K selection, use RTC. */
            .clkdiv1 = 0x01140000U, /* SIM_CLKDIV1. */
    };

    const mcg_pll_config_t pll0Config = {
            .enableMode = 0U, .prdiv = 0x00U, .vdiv = 0x04U,
    };

    CLOCK_SetPbeMode(kMCG_PllClkSelPll0, &pll0Config);
    CLOCK_SetPeeMode();

    CLOCK_SetSimConfig(&simConfig);
}

//static void APP_InitDefaultDebugConsole(void)
//{
//    uint32_t uartClkSrcFreq;
//    CLOCK_SetLpuartClock(APP_DEBUG_UART_DEFAULT_CLKSRC);
//    uartClkSrcFreq = CLOCK_GetFreq(APP_DEBUG_UART_DEFAULT_CLKSRC_NAME);
//    DbgConsole_Init(BOARD_DEBUG_UART_BASEADDR, APP_DEBUG_UART_BAUDRATE, BOARD_DEBUG_UART_TYPE, uartClkSrcFreq);
//}
//
//static void APP_InitVlprDebugConsole(void)
//{
//    uint32_t uartClkSrcFreq;
//    CLOCK_SetLpuartClock(APP_DEBUG_UART_VLPR_CLKSRC);
//    uartClkSrcFreq = CLOCK_GetFreq(APP_DEBUG_UART_VLPR_CLKSRC_NAME);
//    DbgConsole_Init(BOARD_DEBUG_UART_BASEADDR, APP_DEBUG_UART_BAUDRATE, BOARD_DEBUG_UART_TYPE, uartClkSrcFreq);
//}


void APP_PowerPreSwitchHook(smc_power_state_t originPowerState, app_power_mode_t targetMode)
{
    /* Wait for debug console output finished. */
    while (!(kLPUART_TransmissionCompleteFlag & LPUART_GetStatusFlags((LPUART_Type *)BOARD_DEBUG_UART)))
    {
    }
    DbgConsole_Deinit();

    if ((kAPP_PowerModeRun != targetMode) && (kAPP_PowerModeHsrun != targetMode) && (kAPP_PowerModeVlpr != targetMode))
    {
        /*
         * Set pin for current leakage.
         * Debug console RX pin: Set to pinmux to disable.
         * Debug console TX pin: Don't need to change.
         */
        PORT_SetPinMux(BOARD_DEBUG_PORT, BOARD_DEBUG_RX_PIN, kPORT_PinDisabledOrAnalog);
    }
}

void APP_PowerPostSwitchHook(smc_power_state_t originPowerState, app_power_mode_t targetMode)
{
    smc_power_state_t powerState = SMC_GetPowerModeState(SMC);

    /*
     * For some other platforms, if enter LLS mode from VLPR mode, when wakeup, the
     * power mode is VLPR. But for some platforms, if enter LLS mode from VLPR mode,
     * when wakeup, the power mode is RUN. In this case, the clock setting is still
     * VLPR mode setting, so change to RUN mode setting here.
     */
    if ((kSMC_PowerStateVlpr == originPowerState) && (kSMC_PowerStateRun == powerState))
    {
        APP_SetClockRunFromVlpr();
    }

    if ((kAPP_PowerModeRun != targetMode) && (kAPP_PowerModeHsrun != targetMode) && (kAPP_PowerModeVlpr != targetMode))
    {
        /*
         * Debug console RX pin is set to disable for current leakage, nee to re-configure pinmux.
         * Debug console TX pin: Don't need to change.
         */
        PORT_SetPinMux(BOARD_DEBUG_PORT, BOARD_DEBUG_RX_PIN, BOARD_DEBUG_RX_ALT);
    }

    /*
     * If enter stop modes when MCG in PEE mode, then after wakeup, the MCG is in PBE mode,
     * need to enter PEE mode manually.
     */
    if ((kAPP_PowerModeRun != targetMode) && (kAPP_PowerModeWait != targetMode) && (kAPP_PowerModeVlpw != targetMode) &&
        (kAPP_PowerModeHsrun != targetMode) && (kAPP_PowerModeVlpr != targetMode))
    {
        if (kSMC_PowerStateRun == originPowerState)
        {
            /* Wait for PLL lock. */
            while (!(kMCG_Pll0LockFlag & CLOCK_GetStatusFlags()))
            {
            }
            CLOCK_SetPeeMode();
        }
    }

    /* Set debug console clock source. */
//    if (kSMC_PowerStateVlpr == powerState)
//    {
//        APP_InitVlprDebugConsole();
    CLOCK_EnableClock(BOARD_DEBUG_PORT_CLOCK);
    uint32_t lpuart_src_freq;
    switch (SIM->SOPT2 & SIM_SOPT2_LPUARTSRC_MASK) {
        case SIM_SOPT2_LPUARTSRC(3U): {
            lpuart_src_freq = CLOCK_GetInternalRefClkFreq();
            break;
        }
        case SIM_SOPT2_LPUARTSRC(2U): {
            lpuart_src_freq = CLOCK_GetOsc0ErClkFreq();
            break;
        }
        case SIM_SOPT2_LPUARTSRC(1U): {
            lpuart_src_freq = CLOCK_GetPllFllSelClkFreq();
            break;
        }
        default: {
            // lpuart source clock is disabled
//            return kStatus_Fail;
            break;
        }
    }
    DbgConsole_Init((uint32_t) BOARD_DEBUG_UART, 115200, BOARD_DEBUG_TYPE, lpuart_src_freq);
//    }
//    else
//    {
//        APP_InitDefaultDebugConsole();
//        board_console_init(BOARD_DEBUG_BAUD);
//
//    }
}

/*!
 * @brief LLWU interrupt handler.
 */
void LLWU_IRQHandler(void)
{
    /* If wakeup by LPTMR. */
    if (LLWU_GetInternalWakeupModuleFlag(LLWU, LLWU_LPTMR_IDX))
    {
        LPTMR_DisableInterrupts(LPTMR0, kLPTMR_TimerInterruptEnable);
        LPTMR_ClearStatusFlags(LPTMR0, kLPTMR_TimerCompareFlag);
        LPTMR_StopTimer(LPTMR0);
    }
    /* If wakeup by external pin. */
    if (LLWU_GetExternalWakeupPinFlag(LLWU, LLWU_WAKEUP_PIN_IDX))
    {
        PORT_SetPinInterruptConfig(APP_WAKEUP_BUTTON_PORT, APP_WAKEUP_BUTTON_GPIO_PIN, kPORT_InterruptOrDMADisabled);
        PORT_ClearPinsInterruptFlags(APP_WAKEUP_BUTTON_PORT, (1U << APP_WAKEUP_BUTTON_GPIO_PIN));
        LLWU_ClearExternalWakeupPinFlag(LLWU, LLWU_WAKEUP_PIN_IDX);
    }
}

void LPTMR0_IRQHandler(void)
{
    if (kLPTMR_TimerInterruptEnable & LPTMR_GetEnabledInterrupts(LPTMR0))
    {
        LPTMR_DisableInterrupts(LPTMR0, kLPTMR_TimerInterruptEnable);
        LPTMR_ClearStatusFlags(LPTMR0, kLPTMR_TimerCompareFlag);
        LPTMR_StopTimer(LPTMR0);
    }
}

void APP_WAKEUP_BUTTON_IRQ_HANDLER(void)
{
    if ((1U << APP_WAKEUP_BUTTON_GPIO_PIN) & PORT_GetPinsInterruptFlags(APP_WAKEUP_BUTTON_PORT))
    {
        /* Disable interrupt. */
        PORT_SetPinInterruptConfig(APP_WAKEUP_BUTTON_PORT, APP_WAKEUP_BUTTON_GPIO_PIN, kPORT_InterruptOrDMADisabled);
        PORT_ClearPinsInterruptFlags(APP_WAKEUP_BUTTON_PORT, (1U << APP_WAKEUP_BUTTON_GPIO_PIN));
    }
}

/*!
 * @brief Get input from user about wakeup timeout
 */
static uint8_t APP_GetWakeupTimeout(void)
{
    uint8_t timeout;

    while (1)
    {
        PRINTF("Select the wake up timeout in seconds.\r\n");
        PRINTF("The allowed range is 1s ~ 9s.\r\n");
        PRINTF("Eg. enter 5 to wake up in 5 seconds.\r\n");
        PRINTF("\r\nWaiting for input timeout value...\r\n\r\n");

        timeout = GETCHAR();
        PRINTF("%c\r\n", timeout);
        if ((timeout > '0') && (timeout <= '9'))
        {
            return timeout - '0';
        }
        PRINTF("Wrong value!\r\n");
    }
}

/* Get wakeup source by user input. */
static app_wakeup_source_t APP_GetWakeupSource(void)
{
    uint8_t ch;

    while (1)
    {
        PRINTF("Select the wake up source:\r\n");
        PRINTF("Press T for LPTMR - Low Power Timer\r\n");
        PRINTF("Press S for switch/button %s. \r\n", APP_WAKEUP_BUTTON_NAME);

        PRINTF("\r\nWaiting for key press..\r\n\r\n");

        ch = GETCHAR();

        if ((ch >= 'a') && (ch <= 'z'))
        {
            ch -= 'a' - 'A';
        }

        if (ch == 'T')
        {
            return kAPP_WakeupSourceLptmr;
        }
        else if (ch == 'S')
        {
            return kAPP_WakeupSourcePin;
        }
        else
        {
            PRINTF("Wrong value!\r\n");
        }
    }
}

/* Get wakeup timeout and wakeup source. */
void APP_GetWakeupConfig(app_power_mode_t targetMode)
{
    /* Get wakeup source by user input. */
    if (targetMode == kAPP_PowerModeVlls0)
    {
        /* In VLLS0 mode, the LPO is disabled, LPTMR could not work. */
        PRINTF("Not support LPTMR wakeup because LPO is disabled in VLLS0 mode.\r\n");
        s_wakeupSource = kAPP_WakeupSourcePin;
    }
    else
    {
        /* Get wakeup source by user input. */
        s_wakeupSource = APP_GetWakeupSource();
    }

    if (kAPP_WakeupSourceLptmr == s_wakeupSource)
    {
        /* Wakeup source is LPTMR, user should input wakeup timeout value. */
        s_wakeupTimeout = APP_GetWakeupTimeout();
        PRINTF("Will wakeup in %d seconds.\r\n", s_wakeupTimeout);
    }
    else
    {
        PRINTF("Press %s to wake up.\r\n", APP_WAKEUP_BUTTON_NAME);
    }
}

void APP_SetWakeupConfig(app_power_mode_t targetMode)
{
    /* Set LPTMR timeout value. */
    if (kAPP_WakeupSourceLptmr == s_wakeupSource)
    {
        LPTMR_SetTimerPeriod(LPTMR0, (LPO_CLK_FREQ * s_wakeupTimeout) - 1U);
        LPTMR_StartTimer(LPTMR0);
    }

    /* Set the wakeup module. */
    if (kAPP_WakeupSourceLptmr == s_wakeupSource)
    {
        LPTMR_EnableInterrupts(LPTMR0, kLPTMR_TimerInterruptEnable);
    }
    else
    {
        PORT_SetPinInterruptConfig(APP_WAKEUP_BUTTON_PORT, APP_WAKEUP_BUTTON_GPIO_PIN, kPORT_InterruptEitherEdge);
    }

    /* If targetMode is VLLS/LLS, setup LLWU. */
    if ((kAPP_PowerModeWait != targetMode) && (kAPP_PowerModeVlpw != targetMode) &&
        (kAPP_PowerModeVlps != targetMode) && (kAPP_PowerModeStop != targetMode))
    {
        if (kAPP_WakeupSourceLptmr == s_wakeupSource)
        {
            LLWU_EnableInternalModuleInterruptWakup(LLWU, LLWU_LPTMR_IDX, true);
        }
        else
        {
            LLWU_SetExternalWakeupPinMode(LLWU, LLWU_WAKEUP_PIN_IDX, LLWU_WAKEUP_PIN_TYPE);
        }
        NVIC_EnableIRQ(LLWU_IRQn);
    }
}

void APP_ShowPowerMode(smc_power_state_t powerMode)
{
    switch (powerMode)
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

/*
 * Check whether could switch to target power mode from current mode.
 * Return true if could switch, return false if could not switch.
 */
bool APP_CheckPowerMode(smc_power_state_t curPowerState, app_power_mode_t targetPowerMode)
{
    bool modeValid = true;

    /*
     * Check wether the mode change is allowed.
     *
     * 1. If current mode is HSRUN mode, the target mode must be RUN mode.
     * 2. If current mode is RUN mode, the target mode must not be VLPW mode.
     * 3. If current mode is VLPR mode, the target mode must not be HSRUN/WAIT/STOP mode.
     * 4. If already in the target mode.
     */
    switch (curPowerState)
    {
        case kSMC_PowerStateHsrun:
            if (kAPP_PowerModeRun != targetPowerMode)
            {
                PRINTF("Current mode is HSRUN, please choose RUN mode as the target mode.\r\n");
                modeValid = false;
            }
            break;

        case kSMC_PowerStateRun:
            if (kAPP_PowerModeVlpw == targetPowerMode)
            {
                PRINTF("Could not enter VLPW mode from RUN mode.\r\n");
                modeValid = false;
            }
            break;

        case kSMC_PowerStateVlpr:
            if ((kAPP_PowerModeWait == targetPowerMode) || (kAPP_PowerModeHsrun == targetPowerMode) ||
                (kAPP_PowerModeStop == targetPowerMode))
            {
                PRINTF("Could not enter HSRUN/STOP/WAIT modes from VLPR mode.\r\n");
                modeValid = false;
            }
            break;
        default:
            PRINTF("Wrong power state.\r\n");
            modeValid = false;
            break;
    }

    if (!modeValid)
    {
        return false;
    }

    /* Don't need to change power mode if current mode is already the target mode. */
    if (((kAPP_PowerModeRun == targetPowerMode) && (kSMC_PowerStateRun == curPowerState)) ||
        ((kAPP_PowerModeHsrun == targetPowerMode) && (kSMC_PowerStateHsrun == curPowerState)) ||
        ((kAPP_PowerModeVlpr == targetPowerMode) && (kSMC_PowerStateVlpr == curPowerState)))
    {
        PRINTF("Already in the target power mode.\r\n");
        return false;
    }

    return true;
}

/*
 * Power mode switch.
 */
void APP_PowerModeSwitch(smc_power_state_t curPowerState, app_power_mode_t targetPowerMode)
{
    smc_power_mode_vlls_config_t vlls_config;
    vlls_config.enablePorDetectInVlls0 = true;
    vlls_config.enableRam2InVlls2 = true; /*!< Enable RAM2 power in VLLS2 */
    vlls_config.enableLpoClock = true;    /*!< Enable LPO clock in VLLS mode */

    smc_power_mode_lls_config_t lls_config;
    lls_config.enableLpoClock = true;
    lls_config.subMode = kSMC_StopSub3;

    switch (targetPowerMode)
    {
        case kAPP_PowerModeVlpr:
            APP_SetClockVlpr();
            SMC_SetPowerModeVlpr(SMC);
            while (kSMC_PowerStateVlpr != SMC_GetPowerModeState(SMC))
            {
            }
            break;

        case kAPP_PowerModeRun:
            /* If enter RUN from HSRUN, fisrt change clock. */
            if (kSMC_PowerStateHsrun == curPowerState)
            {
                APP_SetClockRunFromHsrun();
            }

            /* Power mode change. */
            SMC_SetPowerModeRun(SMC);
            while (kSMC_PowerStateRun != SMC_GetPowerModeState(SMC))
            {
            }

            /* If enter RUN from VLPR, change clock after the power mode change. */
            if (kSMC_PowerStateVlpr == curPowerState)
            {
                APP_SetClockRunFromVlpr();
            }
            break;

        case kAPP_PowerModeHsrun:
            SMC_SetPowerModeHsrun(SMC);
            while (kSMC_PowerStateHsrun != SMC_GetPowerModeState(SMC))
            {
            }

            APP_SetClockHsrun(); /* Change clock setting after power mode change. */
            break;

        case kAPP_PowerModeWait:
            SMC_PreEnterWaitModes();
            SMC_SetPowerModeWait(SMC);
            SMC_PostExitWaitModes();
            break;

        case kAPP_PowerModeStop:
            SMC_PreEnterStopModes();
            SMC_SetPowerModeStop(SMC, kSMC_PartialStop);
            SMC_PostExitStopModes();
            break;

        case kAPP_PowerModeVlpw:
            SMC_PreEnterWaitModes();
            SMC_SetPowerModeVlpw(SMC);
            SMC_PostExitWaitModes();
            break;

        case kAPP_PowerModeVlps:
            SMC_PreEnterStopModes();
            SMC_SetPowerModeVlps(SMC);
            SMC_PostExitStopModes();
            break;

        case kAPP_PowerModeLls:
            SMC_PreEnterStopModes();
            SMC_SetPowerModeLls(SMC, &lls_config);
            SMC_PostExitStopModes();
            break;

        case kAPP_PowerModeVlls0:
            vlls_config.subMode = kSMC_StopSub0;
            SMC_PreEnterStopModes();
            SMC_SetPowerModeVlls(SMC, &vlls_config);
            SMC_PostExitStopModes();
            break;

        case kAPP_PowerModeVlls1:
            vlls_config.subMode = kSMC_StopSub1;
            SMC_PreEnterStopModes();
            SMC_SetPowerModeVlls(SMC, &vlls_config);
            SMC_PostExitStopModes();
            break;

        case kAPP_PowerModeVlls2:
            vlls_config.subMode = kSMC_StopSub2;
            SMC_PreEnterStopModes();
            SMC_SetPowerModeVlls(SMC, &vlls_config);
            SMC_PostExitStopModes();
            break;

        case kAPP_PowerModeVlls3:
            vlls_config.subMode = kSMC_StopSub3;
            SMC_PreEnterStopModes();
            SMC_SetPowerModeVlls(SMC, &vlls_config);
            SMC_PostExitStopModes();
            break;

        default:
            PRINTF("Wrong value");
            break;
    }
}

typedef enum _pin_mux_direction
{
    kPIN_MUX_DirectionInput = 0U,         /* Input direction */
    kPIN_MUX_DirectionOutput = 1U,        /* Output direction */
    kPIN_MUX_DirectionInputOrOutput = 2U  /* Input or output direction */
} pin_mux_direction_t;

#define PCR_PE_ENABLED                0x01u   /*!< Pull Enable: Internal pullup or pulldown resistor is enabled on the corresponding pin, if the pin is configured as a digital input. */
#define PCR_PS_UP                     0x01u   /*!< Pull Select: Internal pullup resistor is enabled on the corresponding pin, if the corresponding PE field is set. */
#define PIN3_IDX                         3u   /*!< Pin number for pin 6 in a port */
//#define PIN14_IDX                       14u   /*!< Pin number for pin 14 in a port */
//#define PIN15_IDX                       15u   /*!< Pin number for pin 15 in a port */
const gpio_pin_config_t IN = {kGPIO_DigitalInput, false};

/*!
 * @brief main demo function.
 */
int main(void)
{
    uint32_t freq = 0;
    uint8_t ch;
    smc_power_state_t curPowerState;
    app_power_mode_t targetPowerMode;
    bool needSetWakeup; /* Need to set wakeup. */
    lptmr_config_t lptmrConfig;

    /* Power related. */
    SMC_SetPowerModeProtection(SMC, kSMC_AllowPowerModeAll);
    if (kRCM_SourceWakeup & RCM_GetPreviousResetSources(RCM)) /* Wakeup from VLLS. */
    {
        PMC_ClearPeriphIOIsolationFlag(PMC);
        NVIC_ClearPendingIRQ(LLWU_IRQn);
    }

    board_init(BOARD_MODE_RUN);
    board_console_init(BOARD_DEBUG_BAUD);
//    BOARD_InitPins();
//    BOARD_BootClockRUN();
//    APP_InitDefaultDebugConsole();
    SysTick_Config(BOARD_SYSTICK_100MS / 10);

    PORT_SetPinMux(PORTC, PIN3_IDX, kPORT_MuxAsGpio);          /* PORTC6 (pin 78) is configured as PTC6 */
    PORTC->PCR[3] = ((PORTC->PCR[3] &
                      (~(PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_ISF_MASK))) /* Mask bits to zero which are setting */
                     | PORT_PCR_PS(PCR_PS_UP)                               /* Pull Select: Internal pullup resistor is enabled on the corresponding pin, if the corresponding PE field is set. */
                     | PORT_PCR_PE(PCR_PE_ENABLED)                          /* Pull Enable: Internal pullup or pulldown resistor is enabled on the corresponding pin, if the pin is configured as a digital input. */
    );
    GPIO_PinInit(APP_WAKEUP_BUTTON_GPIO, APP_WAKEUP_BUTTON_GPIO_PIN, &IN);

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
    lptmrConfig.prescalerClockSource = kLPTMR_PrescalerClock_1; /* Use LPO as clock source. */
    lptmrConfig.bypassPrescaler = true;

    LPTMR_Init(LPTMR0, &lptmrConfig);

    NVIC_EnableIRQ(LLWU_IRQn);
    NVIC_EnableIRQ(LPTMR0_IRQn);
    NVIC_EnableIRQ(APP_WAKEUP_BUTTON_IRQ);

    if (kRCM_SourceWakeup & RCM_GetPreviousResetSources(RCM)) /* Wakeup from VLLS. */
    {
        PRINTF("\r\nMCU wakeup from VLLS modes...\r\n");
    }
    while (1)
    {
        curPowerState = SMC_GetPowerModeState(SMC);

        freq = CLOCK_GetFreq(kCLOCK_CoreSysClk);

        PRINTF("\r\n####################  Power Mode Switch Demo ####################\n\r\n");
        PRINTF("    Core Clock = %dHz \r\n", freq);

        APP_ShowPowerMode(curPowerState);

        PRINTF("\r\nSelect the desired operation \n\r\n");
        PRINTF("Press  %c for enter: RUN      - Normal RUN mode\r\n", kAPP_PowerModeRun);
        PRINTF("Press  %c for enter: WAIT     - Wait mode\r\n", kAPP_PowerModeWait);
        PRINTF("Press  %c for enter: STOP     - Stop mode\r\n", kAPP_PowerModeStop);
        PRINTF("Press  %c for enter: VLPR     - Very Low Power Run mode\r\n", kAPP_PowerModeVlpr);
        PRINTF("Press  %c for enter: VLPW     - Very Low Power Wait mode\r\n", kAPP_PowerModeVlpw);
        PRINTF("Press  %c for enter: VLPS     - Very Low Power Stop mode\r\n", kAPP_PowerModeVlps);
        PRINTF("Press  %c for enter: LLS/LLS3 - Low Leakage Stop mode\r\n", kAPP_PowerModeLls);
        PRINTF("Press  %c for enter: VLLS0    - Very Low Leakage Stop 0 mode\r\n", kAPP_PowerModeVlls0);
        PRINTF("Press  %c for enter: VLLS1    - Very Low Leakage Stop 1 mode\r\n", kAPP_PowerModeVlls1);
        PRINTF("Press  %c for enter: VLLS2    - Very Low Leakage Stop 2 mode\r\n", kAPP_PowerModeVlls2);
        PRINTF("Press  %c for enter: VLLS3    - Very Low Leakage Stop 3 mode\r\n", kAPP_PowerModeVlls3);
        PRINTF("Press  %c for enter: HSRUN    - High Speed RUN mode\r\n", kAPP_PowerModeHsrun);

        PRINTF("\r\nWaiting for power mode select..\r\n\r\n");

        /* Wait for user response */
        ch = GETCHAR();

        if ((ch >= 'a') && (ch <= 'z'))
        {
            ch -= 'a' - 'A';
        }

        targetPowerMode = (app_power_mode_t)ch;

        if ((targetPowerMode > kAPP_PowerModeMin) && (targetPowerMode < kAPP_PowerModeMax))
        {
            /* If could not set the target power mode, loop continue. */
            if (!APP_CheckPowerMode(curPowerState, targetPowerMode))
            {
                continue;
            }

            /* If target mode is RUN/VLPR/HSRUN, don't need to set wakeup source. */
            if ((kAPP_PowerModeRun == targetPowerMode) || (kAPP_PowerModeHsrun == targetPowerMode) ||
                (kAPP_PowerModeVlpr == targetPowerMode))
            {
                needSetWakeup = false;
            }
            else
            {
                needSetWakeup = true;
            }

            if (needSetWakeup)
            {
                APP_GetWakeupConfig(targetPowerMode);
            }

            APP_PowerPreSwitchHook(curPowerState, targetPowerMode);

            if (needSetWakeup)
            {
                APP_SetWakeupConfig(targetPowerMode);
            }

            APP_PowerModeSwitch(curPowerState, targetPowerMode);
            APP_PowerPostSwitchHook(curPowerState, targetPowerMode);

            PRINTF("\r\nNext loop\r\n");
        }
    }
}
