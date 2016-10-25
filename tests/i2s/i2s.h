//
// Created by dothraki on 10/24/16.
//

#ifndef UBIRCH_FIRMWARE_I2S_H
#define UBIRCH_FIRMWARE_I2S_H



void BOARD_InitPins(void)
{
  port_pin_config_t i2c_pin_config = {0};

  i2c_pin_config.pullSelect = kPORT_PullUp;
  i2c_pin_config.openDrainEnable = kPORT_OpenDrainEnable;
  i2c_pin_config.mux = kPORT_MuxAlt2;

  /* Initialize UART0 pins below */
  /* Ungate the port clock */
  CLOCK_EnableClock(kCLOCK_PortC);
  CLOCK_EnableClock(kCLOCK_PortD);
  CLOCK_EnableClock(kCLOCK_PortA);

  /* Affects PORTC_PCR3 register */
  PORT_SetPinMux(PORTC, 3u, kPORT_MuxAlt3);
  /* Affects PORTC_PCR4 register */
  PORT_SetPinMux(PORTC, 4u, kPORT_MuxAlt3);

  /* Configure sai pins */
  /* Tx BCLK */
  PORT_SetPinMux(PORTA, 17u, kPORT_MuxAlt6);
  /* Tx Frame sync */
  PORT_SetPinMux(PORTA, 5u, kPORT_MuxAlt6);
  /* Master clock */
  PORT_SetPinMux(PORTA, 13u, kPORT_MuxAlt6);
  /* Tx data */
  PORT_SetPinMux(PORTA, 15u, kPORT_MuxAlt6);

  PORT_SetPinMux(PORTA, 12u, kPORT_MuxAlt6);

  /* I2C SCL */
  PORT_SetPinConfig(PORTD, 8u, &i2c_pin_config);
  /* I2C SDA */
  PORT_SetPinConfig(PORTD, 9u, &i2c_pin_config);
}

#endif //UBIRCH_FIRMWARE_I2S_H
