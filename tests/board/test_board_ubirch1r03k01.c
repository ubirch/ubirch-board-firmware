/**
 * Test individual components of the board.
 *
 * Blinks at 100ms interval while working and 1s interval when finished successfully.
 * Also prints messages do debug output.
 *
 * @author Matthias L. Jugel
 * @date 2016-05-09
 *
 * @copyright &copy; 2016 ubirch GmbH (https://ubirch.com)
 *
 * ```
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ```
 */

#include <stdint.h>
#include <board.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(BOARD_FRDM_KL82Z) || defined(BOARD_FRDM_K82F)
#  define LED BOARD_LED1
#elif defined(BOARD_UBIRCH_1R02) || defined(BOARD_UBIRCH_1R03K01)
#  define LED BOARD_LED0
#else
#  define LED(...)
#endif

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


volatile static int state = 100;
volatile static bool on = false;
volatile static int cnt = 0;

void SysTick_Handler() {
  if (++cnt % state == 0) on = !on;
  LED(on);
}

void enter(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int size = vsnprintf(NULL, 0, fmt, ap);
  char *msg = (char *) malloc((size_t) size);
  vsnprintf(msg, (size_t) size, fmt, ap);
  PRINTF(msg);
  free(msg);
  va_end(ap);

  int c;
  while ((c = GETCHAR()) != '\r' && c != '\n') PRINTF("%02x", c);
  PRINTF("\r\n");
}

void test_gpio_pin_output(char port_char, uint32_t pin, char *comment) {
  assert(port_char >= 'A' && port_char <= 'E');

  const gpio_pin_config_t OUTFALSE = {kGPIO_DigitalOutput, 0};
  port_config_t *config = &testPortConfigs['A' - port_char];

  CLOCK_EnableClock(config->clock);
  PORT_SetPinMux(config->port, pin, kPORT_MuxAsGpio);
  GPIO_PinInit(config->gpio, pin, &OUTFALSE);
  GPIO_WritePinOutput(config->gpio, pin, true);
  enter("GPIO [%d%d] PT%c%d -- [%s]...", OUTFALSE.pinDirection, OUTFALSE.outputLogic, port_char, pin, comment);
  CLOCK_DisableClock(config->clock);
}

void test_gpio_pins() {
  PRINTF("GPIO Test\r\n");

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
}

int main(void) {
  // INITIALIZATION
  board_init();
  // INITIAILIZE CONSOLE (Tests debug uart pins!)
  board_console_init(BOARD_DEBUG_BAUD);

  // 100ms led blink, only works if setup for LED was correct
  SysTick_Config(BOARD_SYSTICK_100MS);

  PRINTF(BOARD "\r\n");
  enter("Press Enter to test debug console input: ");
  PRINTF("OK\r\n");

  // Test GPIO pins where it makes sense
  test_gpio_pins();

  PRINTF("DONE\r\n");
  while (true);
}
