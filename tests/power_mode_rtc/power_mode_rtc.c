//
// Created by nirao on 20.03.17.
//

#include <ubirch/modem.h>
#include <ubirch/timer.h>

#include "fsl_common.h"
#include "fsl_smc.h"
#include "fsl_llwu.h"
#include "fsl_rcm.h"
#include "fsl_lptmr.h"
#include "fsl_port.h"
#include "power_mode_rtc.h"
#include "board.h"
#include "fsl_debug_console.h"

#include "fsl_lpuart.h"
#include "fsl_pmc.h"

#include "fsl_rtc.h"

#define RTC_WAKEUP_INTERVAL 20 //Seconds
#define LLWU_LPTMR_IDX 5U       /* LLWU_M0IF */

volatile uint32_t milliseconds = 0;
bool on = true;

void SysTick_Handler() {
    milliseconds++;
    if (milliseconds % 1000 == 0) on = !on;
    BOARD_LED0(on);
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

/*!
* @brief ISR for Alarm interrupt
*
*/
void RTC_IRQHandler(void)
{
    if (RTC_GetStatusFlags(RTC) & kRTC_AlarmFlag)
    {
        /* Clear alarm flag */
        RTC_ClearStatusFlags(RTC, kRTC_AlarmInterruptEnable);
    }
}

/*!
 * @brief LLWU interrupt handler.
 */
void LLWU_IRQHandler(void)
{
    if (RTC_GetStatusFlags(RTC) & kRTC_AlarmFlag)
    {
        RTC_ClearStatusFlags(RTC, kRTC_AlarmInterruptEnable);
    }
}

void initrtc(void) {
    /* Init RTC */
    /*
     * rtcConfig.wakeupSelect = false;
     * rtcConfig.updateMode = false;
     * rtcConfig.supervisorAccess = false;
     * rtcConfig.compensationInterval = 0;
     * rtcConfig.compensationTime = 0;
     */
    rtc_datetime_t date;
    rtc_config_t rtcConfig;

    RTC_GetDefaultConfig(&rtcConfig);
    RTC_Init(RTC, &rtcConfig);
    /* Select RTC clock source */
    /* Enable the RTC 32KHz oscillator */
    RTC->CR |= RTC_CR_OSCE_MASK;

    PRINTF("RTC example: set up time to wake up an alarm\r\n");

    /* Set a start date time and start RT */
    date.year   = 2017U;
    date.month  = 03U;
    date.day    = 22U;
    date.hour   = 14U;
    date.minute = 36;
    date.second = 40;

    /* RTC time counter has to be stopped before setting the date & time in the TSR register */
    RTC_StopTimer(RTC);
    /* Set RTC time to default */
    RTC_SetDatetime(RTC, &date);
    /* Get RTC time */
    RTC_GetDatetime(RTC, &date);

    /* print default time */
    PRINTF("Current datetime: %04hd-%02hd-%02hd %02hd:%02hd:%02hd\r\n",
           date.year,
           date.month,
           date.day,
           date.hour,
           date.minute,
           date.second);

    /* Enable RTC alarm interrupt */
    RTC_EnableInterrupts(RTC, kRTC_AlarmInterruptEnable);

    /* Enable at the NVIC */
    EnableIRQ(RTC_IRQn);
}

/*!
 * @brief main demo function.
 */
int main(void) {

    uint32_t sec;
    uint32_t currSeconds;
    uint32_t freq = 0;

    smc_power_state_t curPowerState;

    /* Power related. */
    SMC_SetPowerModeProtection(SMC, kSMC_AllowPowerModeAll);
    if (kRCM_SourceWakeup & RCM_GetPreviousResetSources(RCM)) /* Wakeup from VLLS. */
    {
        PMC_ClearPeriphIOIsolationFlag(PMC);
        NVIC_ClearPendingIRQ(LLWU_IRQn);
    }

    board_init(BOARD_MODE_RUN);
    board_console_init(BOARD_DEBUG_BAUD);

    SysTick_Config(BOARD_SYSTICK_100MS / 10);

    initrtc();

    NVIC_EnableIRQ(LLWU_IRQn);

    /* Start the RTC time counter */
    RTC_StartTimer(RTC);

    PRINTF("\r\n####################  Power Mode Switch Demo ####################\n\r\n");

    if (kRCM_SourceWakeup & RCM_GetPreviousResetSources(RCM)) /* Wakeup from VLLS. */
    {
        PRINTF("    MCU wakeup from VLLS modes...\r\n");
    }

    while (1) {

        delay(5 * 1000);

        /*++++++++++++++++++++++++++++++++++++++++*/
        /* Set Alaram in seconds */
        sec = RTC_WAKEUP_INTERVAL;
        /* Read the RTC seconds register to get current time in seconds */
        currSeconds = RTC->TSR;
        /* Add alarm seconds to current time */
        currSeconds += sec;
        /* Set alarm time in seconds */
        RTC->TAR = currSeconds;
        /*++++++++++++++++++++++++++++++++++++++++*/


        curPowerState = SMC_GetPowerModeState(SMC);
        freq = CLOCK_GetFreq(kCLOCK_CoreSysClk);
        APP_ShowPowerMode(curPowerState);
        PRINTF("    Core Clock = %dHz \r\n", freq);
        PRINTF("\r\n++++++  Wait for RTC alarm to wake-up +++++\r\n");

        /* Wait for debug console output finished. */
        while (!(kLPUART_TransmissionCompleteFlag & LPUART_GetStatusFlags((LPUART_Type *) BOARD_DEBUG_UART))) {
        }
        DbgConsole_Deinit();
        PORT_SetPinMux(BOARD_DEBUG_PORT, BOARD_DEBUG_RX_PIN, kPORT_PinDisabledOrAnalog);

        LLWU_EnableInternalModuleInterruptWakup(LLWU, LLWU_LPTMR_IDX, true);

        smc_power_mode_vlls_config_t vlls_config;
        vlls_config.enablePorDetectInVlls0 = true;
        vlls_config.enableRam2InVlls2 = true; /*!< Enable RAM2 power in VLLS2 */
        vlls_config.enableLpoClock = true;    /*!< Enable LPO clock in VLLS mode */

        smc_power_mode_lls_config_t lls_config;
        lls_config.enableLpoClock = true;
        lls_config.subMode = kSMC_StopSub3;

        vlls_config.subMode = kSMC_StopSub0;
        SMC_PreEnterStopModes();
        SMC_SetPowerModeVlls(SMC, &vlls_config);
        SMC_PostExitStopModes();
    }
    return 0;
}