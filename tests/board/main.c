/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
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
 * ANY THEORY OF LIABILITY, WHETHER IN -        CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
///////////////////////////////////////////////////////////////////////////////
//  Includes
///////////////////////////////////////////////////////////////////////////////

// Standard C Included Files
#include <stdio.h>
// SDK Included Files
#include "board.h"
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"
#include "fsl_device_registers.h"
#include "fsl_flexio_hal.h"
#include "fsl_flexio_driver.h"

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////
flexio_timer_config_t FlexioTimerConfigStruct;
flexio_shifter_config_t FlexioShfiterConfigStruct;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////


int main(void)
{
	static uint8_t x = 0;

    flexio_user_config_t userConfig =
    {
        .useInt = false,
        .onDozeEnable = false,
        .onDebugEnable = true,
        .fastAccessEnable = false
    };

    // Enable clock for PORTs, setup board clock source, config pin
    hardware_init();

    // Enable the four LEDs of the board
    LED1_EN;LED2_EN;LED3_EN;LED4_EN;

    FLEXIO_DRV_Init(0, &userConfig);

    /* SHIFTER 0 ***************************************************************/
    //SHIFTCTL0
    FlexioShfiterConfigStruct.timsel = 0;
    FlexioShfiterConfigStruct.timpol = kFlexioShifterTimerPolarityOnPositive;
    FlexioShfiterConfigStruct.pincfg = kFlexioPinConfigOutput;
    FlexioShfiterConfigStruct.pinsel = 0;
    FlexioShfiterConfigStruct.pinpol = kFlexioPinActiveHigh;
    FlexioShfiterConfigStruct.smode  = kFlexioShifterModeTransmit;
    //SHIFTCFG0
    FlexioShfiterConfigStruct.insrc = kFlexioShifterInputFromPin;
    FlexioShfiterConfigStruct.sstop = kFlexioShifterStopBitDisable;
    FlexioShfiterConfigStruct.sstart = kFlexioShifterStartBitDisabledLoadDataOnEnable;

    FLEXIO_HAL_ConfigureShifter(FLEXIO, 0, &FlexioShfiterConfigStruct);

    /* TIMER 0 ***************************************************************/
    //TIMCTL0
    FlexioTimerConfigStruct.trgsel = FLEXIO_HAL_TIMER_TRIGGER_SEL_SHIFTnSTAT(0);
    FlexioTimerConfigStruct.trgpol = kFlexioTimerTriggerPolarityActiveLow;
    FlexioTimerConfigStruct.trgsrc = kFlexioTimerTriggerSourceInternal;
    FlexioTimerConfigStruct.pincfg = kFlexioPinConfigOutput;
    FlexioTimerConfigStruct.pinsel = 1;
    FlexioTimerConfigStruct.pinpol = kFlexioPinActiveHigh;
    FlexioTimerConfigStruct.timod  = kFlexioTimerModeDual8BitBaudBit;
    //TIMCFG0
    FlexioTimerConfigStruct.timout = kFlexioTimerOutputOneNotAffectedByReset;
    FlexioTimerConfigStruct.timdec = kFlexioTimerDecSrcOnFlexIOClockShiftTimerOutput;
    FlexioTimerConfigStruct.timrst = kFlexioTimerResetNever;
    FlexioTimerConfigStruct.timdis = kFlexioTimerDisableOnTimerCompare;
    FlexioTimerConfigStruct.timena = kFlexioTimerEnableOnTriggerHigh;
    FlexioTimerConfigStruct.tstop  = kFlexioTimerStopBitDisabled;
    FlexioTimerConfigStruct.tstart = kFlexioTimerStartBitDisabled;
    //TIMCMP0
    FlexioTimerConfigStruct.timcmp = 0x00000F01;

    FLEXIO_HAL_ConfigureTimer(FLEXIO, 0, &FlexioTimerConfigStruct);

    /* TIMER 1 ***************************************************************/
    //TIMCTL1
    FlexioTimerConfigStruct.trgsel = FLEXIO_HAL_TIMER_TRIGGER_SEL_PININPUT(0);
    FlexioTimerConfigStruct.trgpol = kFlexioTimerTriggerPolarityActiveHigh;
    FlexioTimerConfigStruct.trgsrc = kFlexioTimerTriggerSourceExternal;
    FlexioTimerConfigStruct.pincfg = kFlexioPinConfigOutput;
    FlexioTimerConfigStruct.pinsel = 2;
    FlexioTimerConfigStruct.pinpol = kFlexioPinActiveLow;
    FlexioTimerConfigStruct.timod  = kFlexioTimerModeSingle16Bit;
    //TIMCFG1
    FlexioTimerConfigStruct.timout = kFlexioTimerOutputOneNotAffectedByReset;
    FlexioTimerConfigStruct.timdec = kFlexioTimerDecSrcOnFlexIOClockShiftTimerOutput;
    FlexioTimerConfigStruct.timrst = kFlexioTimerResetNever;
    FlexioTimerConfigStruct.timdis = kFlexioTimerDisableOnPreTimerDisable;
    FlexioTimerConfigStruct.timena = kFlexioTimerEnableOnPrevTimerEnable;
    FlexioTimerConfigStruct.tstop  = kFlexioTimerStopBitDisabled;
    FlexioTimerConfigStruct.tstart = kFlexioTimerStartBitDisabled;
    //TIMCMP0
    FlexioTimerConfigStruct.timcmp = 0x0000FFFF;

    FLEXIO_HAL_ConfigureTimer(FLEXIO, 1, &FlexioTimerConfigStruct);

    FLEXIO_DRV_Start(0);

    while(true)
    {
		x = GETCHAR();	// Get character from terminal
		PUTCHAR(x);		// Display the character in the terminal

		FLEXIO_HAL_SetShifterBuffer(FLEXIO, 0, x);		// Write the character to the shifter buffer

		while(!FLEXIO_HAL_GetTimerStatusFlags(FLEXIO));	// Wait for the transmission to complete
		FLEXIO_HAL_ClearTimerStatusFlags(FLEXIO, 0x1);	// Clear timer status flag

		GPIO_DRV_TogglePinOutput(ledPins[x % 4].pinName);	// Turn on an LED
    }
}

void hardware_init(void) {

  /* enable clock for PORTs */
  CLOCK_SYS_EnablePortClock(PORTA_IDX);
  CLOCK_SYS_EnablePortClock(PORTC_IDX);
  CLOCK_SYS_EnablePortClock(PORTD_IDX);
  CLOCK_SYS_EnablePortClock(PORTE_IDX);

  /* Init board clock */
  BOARD_ClockInit();
  dbg_uart_init();
  CLOCK_SYS_SetFlexioSrc(0,(clock_flexio_src_t)1);
  configure_flexio_pins(0,0);
  configure_flexio_pins(0,1);
  configure_flexio_pins(0,2);
  configure_flexio_pins(0,3);
}
