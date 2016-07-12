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
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <fsl_ftm.h>
#include <ubirch/i2c.h>
#include <ubirch/timer.h>
#include "support.h"

#define BOARD_AUDIO_AMP_I2C_ADDR  0x4b
#define BOARD_FTM_BASEADDR FTM3
#define BOARD_SECOND_FTM_CHANNEL 5U

#define FTM_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_BusClk)

void send_cmd(uint8_t *cmd, size_t size) {
  i2c_master_transfer_t transfer;
  memset(&transfer, 0, sizeof(transfer));

  transfer.slaveAddress = BOARD_AUDIO_AMP_I2C_ADDR;
  transfer.direction = kI2C_Write;
  transfer.subaddress = 0;
  transfer.subaddressSize = 0;
  transfer.data = cmd;
  transfer.dataSize = size;
  transfer.flags = kI2C_TransferDefaultFlag;

  status_t status = I2C_MasterTransferBlocking(i2c_config_default.i2c, &transfer);
#ifndef NDEBUG
  if (status != kStatus_Success) {
    PRINTF("I2C write(%02d) <= %02x\r\n", 0, &cmd);
    i2c_error("write reg", status);
  }
#endif
}

/*!
 * @brief Main function
 */
void test_audio(void) {
  const gpio_pin_config_t OUTFALSE = {kGPIO_DigitalOutput, 0};

  yesno("Audio amplifier test?");

  // enable MUTE and SHTDWN ports
  CLOCK_EnableClock(kCLOCK_PortC);
  PORT_SetPinMux(PORTC, 12U, kPORT_MuxAsGpio);
  PORT_SetPinMux(PORTC, 13U, kPORT_MuxAsGpio);

  // enable AMP PWM port
  PORT_SetPinMux(PORTC, 9U, kPORT_MuxAlt3);

  GPIO_PinInit(GPIOC, 12U, &OUTFALSE);
  GPIO_PinInit(GPIOC, 13U, &OUTFALSE);

  GPIO_WritePinOutput(GPIOC, 12, false);
  GPIO_WritePinOutput(GPIOC, 13, true);

  delay(300);

  i2c_init(i2c_config_default);

  // send max amp volume and normal pwm mode setting
  uint8_t cmd[2] = { 0b00111111, 0b11010101 };
  send_cmd(cmd, sizeof(cmd));

  ftm_config_t ftmInfo;
  uint8_t updatedDutycycle = 100U;
  ftm_chnl_pwm_signal_param_t ftmParam[2];

  /* Configure ftm params with frequency 24kHZ */
  ftmParam[0].chnlNumber = (ftm_chnl_t) BOARD_SECOND_FTM_CHANNEL;
  ftmParam[0].level = kFTM_LowTrue;
  ftmParam[0].dutyCyclePercent = 0U;
  ftmParam[0].firstEdgeDelayPercent = 0U;

  FTM_GetDefaultConfig(&ftmInfo);
  /* Initialize FTM module */
  FTM_Init(BOARD_FTM_BASEADDR, &ftmInfo);

  FTM_SetupPwm(BOARD_FTM_BASEADDR, ftmParam, 1U, kFTM_EdgeAlignedPwm, 1000000, FTM_SOURCE_CLOCK);
  FTM_StartTimer(BOARD_FTM_BASEADDR, kFTM_SystemClock);

  FTM_UpdatePwmDutycycle(BOARD_FTM_BASEADDR, (ftm_chnl_t) BOARD_SECOND_FTM_CHANNEL, kFTM_EdgeAlignedPwm,
                         0 /* percent */);

  int cycle = 0;
  updatedDutycycle = 0;
  int direction = 1;
  while (1) {
    if(++cycle % 500 == 0) {
      if (updatedDutycycle >= 100) direction = -15;
      else if (updatedDutycycle <= 0) direction = 15;
      FTM_UpdatePwmDutycycle(BOARD_FTM_BASEADDR, (ftm_chnl_t) BOARD_SECOND_FTM_CHANNEL, kFTM_EdgeAlignedPwm,
                             updatedDutycycle += direction);
      /* Software trigger to update registers */
      FTM_SetSoftwareTrigger(BOARD_FTM_BASEADDR, true);
    }

  }
}
