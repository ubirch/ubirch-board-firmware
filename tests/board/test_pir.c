#include <stdint.h>
#include <fsl_gpio.h>
#include <fsl_debug_console.h>
#include <fsl_port.h>
#include <board.h>
#include "support.h"

#include "fsl_adc16.h"

volatile bool motion_detected = false;

// ====================================================================================================
// == DIGITAL PIR MODE
// ====================================================================================================

#define PIR_CLOCK   kCLOCK_PortD
#define PIR_PORT    PORTD
#define PIR_PIN     1
#define PIR_GPIO    GPIOD
#define PIR_IRQ     PORTD_IRQn
#define PIR_IRQH    PORTD_IRQHandler

void PIR_IRQH(void) {
  PRINTF(".");
  GPIO_ClearPinsInterruptFlags(PIR_GPIO, 1U << PIR_PIN);
  motion_detected = true;
}

void test_pir_digital(void) {
  const gpio_pin_config_t IN = {kGPIO_DigitalInput, 0};
  port_pin_config_t config;
  memset(&config, 0, sizeof(config));
  config.mux = kPORT_MuxAsGpio;
  config.pullSelect = kPORT_PullDown;

  CLOCK_EnableClock(PIR_CLOCK);
  PORT_SetPinConfig(PIR_PORT, PIR_PIN, &config);
  PORT_SetPinInterruptConfig(PIR_PORT, PIR_PIN, kPORT_InterruptFallingEdge);
  EnableIRQ(PIR_IRQ);
  GPIO_PinInit(PIR_GPIO, PIR_PIN, &IN);

  int n = 0;
  while (true) {
    PRINTF("%08b\r", PIR_GPIO->PDIR);
    while (!motion_detected);
    if (motion_detected) {
      PRINTF("%d Motion detected\r\n", n++);
      motion_detected = false;
    }
  }
}

// ====================================================================================================
// == ANALOG PIR MODE
// ====================================================================================================

#define DEMO_ADC16_BASE ADC0
#define DEMO_ADC16_CHANNEL_GROUP 0U
#define DEMO_ADC16_USER_CHANNEL 5U

volatile uint32_t adcValue;
volatile uint32_t counter;
volatile bool done = false;

void ADC0_IRQHandler(void) {
  /* Read conversion result to clear the conversion completed flag. */
  adcValue = ADC16_GetChannelConversionValue(DEMO_ADC16_BASE, DEMO_ADC16_CHANNEL_GROUP);
  motion_detected = adcValue > 1400;
  counter++;
  done = true;
}

void test_pir_analog(void) {
  adc16_config_t adc16ConfigStruct;
  adc16_channel_config_t adc16ChannelConfigStruct;

  EnableIRQ(ADC0_IRQn);

  ADC16_GetDefaultConfig(&adc16ConfigStruct);
  adc16ConfigStruct.enableContinuousConversion = true;
  adc16ConfigStruct.enableHighSpeed = true;
  ADC16_Init(DEMO_ADC16_BASE, &adc16ConfigStruct);
  adc16_hardware_compare_config_t compare = {
    .value1 = 1436,
    .hardwareCompareMode = kADC16_HardwareCompareMode1
  };
  ADC16_SetHardwareCompareConfig(DEMO_ADC16_BASE, &compare);

  ADC16_EnableHardwareTrigger(DEMO_ADC16_BASE, true); /* Make sure the software trigger is used. */
  ADC16_SetChannelMuxMode(DEMO_ADC16_BASE, kADC16_ChannelMuxB);

  if (kStatus_Success == ADC16_DoAutoCalibration(DEMO_ADC16_BASE)) {
    PRINTF("Calibration done.\r\n");
  } else {
    PRINTF("Calibration failed.\r\n");
  }

  adc16ChannelConfigStruct.channelNumber = DEMO_ADC16_USER_CHANNEL;
  adc16ChannelConfigStruct.enableInterruptOnConversionCompleted = true; /* Enable the interrupt. */
  adc16ChannelConfigStruct.enableDifferentialConversion = false;
  ADC16_SetChannelConfig(DEMO_ADC16_BASE, DEMO_ADC16_CHANNEL_GROUP, &adc16ChannelConfigStruct);

  while (true) {
    motion_detected = false;
    while (!done);
    PRINTF("\e[K%d\r", adcValue);
    if (motion_detected) PRINTF("%d %d Motion detected \r\n", counter, adcValue);
  }
}

#define VBAT_SENSE_PORT PORTD
#define VBAT_SENSE_GPIO GPIOD
#define VBAT_SENSE_PIN 1
#define VBAT_SENSE_ALT kPORT_PinDisabledOrAnalog
#define VBAT_SENSE_ADC ADC0
#define VBAT_SENSE_ADC_GROUP 0
#define VBAT_SENSE_CHANNEL 5
#define VBAT_SENSE_CHANNEL_MUX kADC16_ChannelMuxB

void test_pir_analog1() {
  //PORT_SetPinMux(VBAT_SENSE_PORT, VBAT_SENSE_PIN, VBAT_SENSE_ALT);

  adc16_config_t adc16ConfigStruct;
  ADC16_GetDefaultConfig(&adc16ConfigStruct);
  ADC16_Init(VBAT_SENSE_ADC, &adc16ConfigStruct);
  ADC16_EnableHardwareTrigger(VBAT_SENSE_ADC, false); // no hardware trigger

  ADC16_SetChannelMuxMode(VBAT_SENSE_ADC, VBAT_SENSE_CHANNEL_MUX);
  adc16_channel_config_t adc16ChannelConfigStruct;
  adc16ChannelConfigStruct.channelNumber = VBAT_SENSE_CHANNEL;
  adc16ChannelConfigStruct.enableInterruptOnConversionCompleted = false;
  adc16ChannelConfigStruct.enableDifferentialConversion = false;
  while (true) {
    ADC16_SetChannelConfig(VBAT_SENSE_ADC, VBAT_SENSE_ADC_GROUP, &adc16ChannelConfigStruct);
    uint32_t status;
    do {
      status = ADC16_GetChannelStatusFlags(VBAT_SENSE_ADC, VBAT_SENSE_ADC_GROUP);
    } while (!(status & kADC16_ChannelConversionDoneFlag));
    uint16_t val = (uint16_t) ADC16_GetChannelConversionValue(VBAT_SENSE_ADC, VBAT_SENSE_ADC_GROUP);
    PRINTF("%06d\r", val);
  }
}


void test_pir(void) {
  if (!yesno("PIR test?")) return;
//  test_pir_digital();
//  test_pir_analog();
  test_pir_analog1();
}
