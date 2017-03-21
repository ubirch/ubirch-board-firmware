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
#include "power_mode.h"
#include "board.h"
#include "fsl_debug_console.h"

#include "fsl_lpuart.h"
#include "fsl_pmc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define LLWU_LPTMR_IDX 8U       /* LLWU_M0IF */
#define LLWU_WAKEUP_PIN_IDX 7U /* LLWU_P7 */
#define LLWU_WAKEUP_PIN_TYPE kLLWU_ExternalPinFallingEdge

#define BOARD_WAKEUP_GPIO GPIOC
#define BOARD_WAKEUP_PORT PORTC
#define BOARD_WAKEUP_GPIO_PIN 3U
#define BOARD_WAKEUP_IRQ PORTC_IRQn

volatile uint32_t milliseconds = 0;
bool on = true;

void SysTick_Handler() {
  milliseconds++;
  if (milliseconds % 1000 == 0) on = !on;
  BOARD_LED0(on);
}

/*!
 * @brief LLWU interrupt handler.
 */
void LLWU_IRQHandler(void)
{
  /* If wakeup by external pin. */
  if (LLWU_GetExternalWakeupPinFlag(LLWU, LLWU_WAKEUP_PIN_IDX))
  {
    PORT_SetPinInterruptConfig(BOARD_WAKEUP_PORT, BOARD_WAKEUP_GPIO_PIN, kPORT_InterruptOrDMADisabled);
    PORT_ClearPinsInterruptFlags(BOARD_WAKEUP_PORT, (1U << BOARD_WAKEUP_GPIO_PIN));
    LLWU_ClearExternalWakeupPinFlag(LLWU, LLWU_WAKEUP_PIN_IDX);
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

/*!
 * @brief main demo function.
 */
int main(void) {

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


  port_pin_config_t pinConfig = {0};

  pinConfig.mux = kPORT_MuxAsGpio;
  pinConfig.pullSelect = kPORT_PullUp;
  pinConfig.openDrainEnable = kPORT_OpenDrainEnable;

  PORT_SetPinConfig(PORTC, BOARD_WAKEUP_GPIO_PIN, &pinConfig);

  PORT_SetPinMux(PORTC, BOARD_WAKEUP_GPIO_PIN, kPORT_MuxAsGpio);          /* PORTC6 (pin 78) is configured as PTC6 */

  const gpio_pin_config_t IN = {kGPIO_DigitalInput, false};

  GPIO_PinInit(BOARD_WAKEUP_GPIO, BOARD_WAKEUP_GPIO_PIN, &IN);

  NVIC_EnableIRQ(LLWU_IRQn);
  NVIC_EnableIRQ(BOARD_WAKEUP_IRQ);

  PRINTF("\r\n####################  Power Mode Switch Demo ####################\n\r\n");

  if (kRCM_SourceWakeup & RCM_GetPreviousResetSources(RCM)) /* Wakeup from VLLS. */
  {
    PRINTF("\r\nMCU wakeup from VLLS modes...\r\n");
  }
  while (1) {

    delay(5 * 1000);

    curPowerState = SMC_GetPowerModeState(SMC);

    freq = CLOCK_GetFreq(kCLOCK_CoreSysClk);

    PRINTF("    Core Clock = %dHz \r\n", freq);

    APP_ShowPowerMode(curPowerState);
    PRINTF("\r\n+++++++++++++++  Pull down LLWU_P7, to exit VLLS0  ++++++++++++++\r\n");

    /* Wait for debug console output finished. */
    while (!(kLPUART_TransmissionCompleteFlag & LPUART_GetStatusFlags((LPUART_Type *)BOARD_DEBUG_UART)))
    {
    }
    DbgConsole_Deinit();
    PORT_SetPinMux(BOARD_DEBUG_PORT, BOARD_DEBUG_RX_PIN, kPORT_PinDisabledOrAnalog);

//        PORT_SetPinInterruptConfig(BOARD_WAKEUP_PORT, BOARD_WAKEUP_GPIO_PIN, kPORT_InterruptEitherEdge);
    LLWU_SetExternalWakeupPinMode(LLWU, LLWU_WAKEUP_PIN_IDX, LLWU_WAKEUP_PIN_TYPE);
    NVIC_EnableIRQ(LLWU_IRQn);

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
}