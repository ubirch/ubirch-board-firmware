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

void test_pir(void) {
  if (!yesno("PIR test?")) return;

  // configure the digital input pin of the PIR
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

  // loop, the pin will send a signal on detected motion
  int n = 0;
  while (true) {
    PRINTF("%08b %d\r", PIR_GPIO->PDIR, n);
    while (!motion_detected);
    if (motion_detected) {
      PRINTF("%d Motion detected\r\n", n++);
      motion_detected = false;
    }
  }
}
