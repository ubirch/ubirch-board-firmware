#include <stdint.h>
#include <assert.h>
#include <fsl_gpio.h>
#include <fsl_port.h>
#include "support.h"

extern systick_callback_t callback;

#define PORT_A 'A'
#define PORT_B 'B'
#define PORT_C 'C'
#define PORT_D 'D'
#define PORT_E 'E'

typedef struct {
    clock_ip_name_t clock;
    PORT_Type *port;
    GPIO_Type *gpio;
} port_config_t;

port_config_t testPortConfigs[5] = {
  {kCLOCK_PortA, PORTA, GPIOA},
  {kCLOCK_PortB, PORTB, GPIOB},
  {kCLOCK_PortC, PORTC, GPIOC},
  {kCLOCK_PortD, PORTD, GPIOD},
  {kCLOCK_PortE, PORTE, GPIOE},
};

static uint32_t tested_pin;
static port_config_t *tested_port;

void gpio_onoff_callback(bool on) {
  GPIO_WritePinOutput(tested_port->gpio, tested_pin, on);
}

void test_gpio_pin_output(char port_char, uint32_t test_pin, char *comment) {
  assert(port_char >= 'A' && port_char <= 'E');

  const gpio_pin_config_t OUTFALSE = {kGPIO_DigitalOutput, 0};
  port_config_t *port_config = &testPortConfigs[port_char - 'A'];

  CLOCK_EnableClock(port_config->clock);
  PORT_SetPinMux(port_config->port, test_pin, kPORT_MuxAsGpio);
  GPIO_PinInit(port_config->gpio, test_pin, &OUTFALSE);
  tested_pin = test_pin;
  tested_port = port_config;

  callback = gpio_onoff_callback;
  enter("GPIO [%d%d] PT%c%d -- [%s]...", OUTFALSE.pinDirection, OUTFALSE.outputLogic, port_char, test_pin, comment);
  callback = NULL;

  CLOCK_DisableClock(port_config->clock);
}

void test_gpio() {
#if TEST_GPIO
  if(!yesno("GPIO test?")) return;

  // test GPIO PTA5,12-18
  test_gpio_pin_output(PORT_A, 5, "CHG_EN");
  test_gpio_pin_output(PORT_A, 12, "CHG_AUX");
  test_gpio_pin_output(PORT_A, 13, "CHG_STATUS");
  test_gpio_pin_output(PORT_A, 14, "RGB_DO_A");
  test_gpio_pin_output(PORT_A, 15, "DM_DO");
  test_gpio_pin_output(PORT_A, 16, "DM_WS");
  test_gpio_pin_output(PORT_A, 17, "DM_CLK");
  test_gpio_pin_output(PORT_A, 18, "DM_SEL");


  // Test GPIO PTB0-23 (with gaps)
  test_gpio_pin_output(PORT_B, 0, "SENSE_3");
  test_gpio_pin_output(PORT_B, 1, "SENSE_4");
  test_gpio_pin_output(PORT_B, 2, "SENSE_5 (P1)");
  test_gpio_pin_output(PORT_B, 3, "SENSE_6 (P2)");
  test_gpio_pin_output(PORT_B, 9, "EXT_D1 (P3)");
  test_gpio_pin_output(PORT_B, 10, "EXT_I2C_SCL (P4)");
  test_gpio_pin_output(PORT_B, 11, "EXT_I2C_SDA (P5)");
  test_gpio_pin_output(PORT_B, 16, "SENSE_1");
  test_gpio_pin_output(PORT_B, 17, "SENSE_2");
  test_gpio_pin_output(PORT_B, 18, "EXT_T1 (P8)");
  test_gpio_pin_output(PORT_B, 19, "EXT_T2 (P11)");
  test_gpio_pin_output(PORT_B, 20, "EXT_SPI2_CS0 (P12)");
  test_gpio_pin_output(PORT_B, 21, "EXT_SPI2_SCK (P13)");
  test_gpio_pin_output(PORT_B, 22, "EXT_SPI2_MOSI (P14)");
  test_gpio_pin_output(PORT_B, 23, "EXT_SPI2_MISO (P15)");

  // Test GPIO PTC0-17
  test_gpio_pin_output(PORT_C, 0, "EXT_SPI0_CS4 (P17)");
  test_gpio_pin_output(PORT_C, 1, "EXT_UART2_RTS (P16)");
  test_gpio_pin_output(PORT_C, 2, "EXT_UART2_CTS (P18)");
  test_gpio_pin_output(PORT_C, 3, "EXT_UART2_RX (P19)");
  test_gpio_pin_output(PORT_C, 4, "EXT_UART2_TX (P20)");
  test_gpio_pin_output(PORT_C, 5, "EXT_SPI0_SCK (P21)");
  test_gpio_pin_output(PORT_C, 6, "EXT_SPI0_MOSI (P22)");
  test_gpio_pin_output(PORT_C, 7, "EXT_SPI0_MISO (P23)");
  test_gpio_pin_output(PORT_C, 8, "EXT_PWR_EN ");

  // Test GPIO PTD5,6
  test_gpio_pin_output(PORT_D, 5, "RGB_DO_B");
  test_gpio_pin_output(PORT_D, 5, "GSM_PWR_EN");

  // Test PTE0-5,7
  test_gpio_pin_output(PORT_E, 0, "SD_D1");
  test_gpio_pin_output(PORT_E, 1, "SD_D0");
  test_gpio_pin_output(PORT_E, 2, "SD_CLK");
  test_gpio_pin_output(PORT_E, 3, "SD_CMD");
  test_gpio_pin_output(PORT_E, 4, "SD_D3");
  test_gpio_pin_output(PORT_E, 5, "SD_D2");
  test_gpio_pin_output(PORT_E, 7, "SD_CD");
#endif
}
