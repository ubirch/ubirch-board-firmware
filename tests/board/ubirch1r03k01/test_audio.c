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
#include <math.h>
#include <stdlib.h>
#include "support.h"

#define BOARD_FTM_BASEADDR FTM3
#define BOARD_FIRST_FTM_CHANNEL 4U
#define BOARD_SECOND_FTM_CHANNEL 5U

#define FTM_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_BusClk)

uint8_t max = 0x4b;

void send_cmd(uint8_t *cmd, size_t size) {
  i2c_master_transfer_t transfer;
  memset(&transfer, 0, sizeof(transfer));

  transfer.slaveAddress = max;
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
  if(!yesno("Audio?")) return;

  const gpio_pin_config_t OUTFALSE = {kGPIO_DigitalOutput, 0};

  CLOCK_EnableClock(kCLOCK_PortC);
  PORT_SetPinMux(PORTC, 12U, kPORT_MuxAsGpio);
  PORT_SetPinMux(PORTC, 13U, kPORT_MuxAsGpio);

  GPIO_PinInit(GPIOC, 12U, &OUTFALSE);
  GPIO_PinInit(GPIOC, 13U, &OUTFALSE);

  GPIO_WritePinOutput(GPIOC, 12, true);
  GPIO_WritePinOutput(GPIOC, 13, true);

  i2c_init(&i2c_config_default);

  uint8_t cmd[1] = {0b00001111};
  send_cmd(cmd, sizeof(cmd));

  GPIO_WritePinOutput(GPIOC, 12, false);

  PORT_SetPinMux(PORTC, 9U, kPORT_MuxAlt3);

  ftm_config_t ftmInfo;
  uint8_t updatedDutycycle = 100U;
  ftm_chnl_pwm_signal_param_t ftmParam[2];

  /* Configure ftm params with frequency 24kHZ */
  ftmParam[0].chnlNumber = (ftm_chnl_t) BOARD_SECOND_FTM_CHANNEL;
  ftmParam[0].level = kFTM_LowTrue;
  ftmParam[0].dutyCyclePercent = 0U;
  ftmParam[0].firstEdgeDelayPercent = 0U;

  FTM_GetDefaultConfig(&ftmInfo);
  FTM_Init(BOARD_FTM_BASEADDR, &ftmInfo);
  FTM_SetupPwm(BOARD_FTM_BASEADDR, ftmParam, 1U, kFTM_EdgeAlignedPwm, 1000000, FTM_SOURCE_CLOCK);
  FTM_StartTimer(BOARD_FTM_BASEADDR, kFTM_SystemClock);

  FTM_UpdatePwmDutycycle(BOARD_FTM_BASEADDR, (ftm_chnl_t) BOARD_SECOND_FTM_CHANNEL, kFTM_EdgeAlignedPwm,
                         updatedDutycycle);

  int idx = 0;
  int cnt = 0;
  while (1) {
    if(cnt++ % 100 == 0) {
      uint8_t duty_cycle = (uint8_t) abs((sin(idx++ * 3.14159265358979f / 180) * 100.0));
      if (idx > 360) idx = 0;
      FTM_UpdatePwmDutycycle(BOARD_FTM_BASEADDR, (ftm_chnl_t) BOARD_SECOND_FTM_CHANNEL, kFTM_EdgeAlignedPwm,
                             (uint8_t) (duty_cycle++ % 100));

      /* Software trigger to update registers */
      FTM_SetSoftwareTrigger(BOARD_FTM_BASEADDR, true);
    }
  }
}
