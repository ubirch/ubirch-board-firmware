/*!
 * @file spirit1.h
 * @brief __ubridge 1.0__ Sub 1GHz RF chip definitions
 *
 * The chip is on-board so we can define all the settings here as it is hardwired
 * to the MCU.
 *
 * @author Matthias L. Jugel
 * @date 2016-05-03
 *
 * @copyright &copy; 2015 ubirch GmbH (https://ubirch.com)
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

#ifndef _UBIRCH_SPIRIT1_BOARD_H_
#define _UBIRCH_SPIRIT1_BOARD_H_

#define BOARD_RF_TYPE_SPIRIT1     1                             //!< SPIRIT1 type RF

// SPI config
#define BOARD_RF_PORT             PORTB                         //!< port used
#define BOARD_RF_PORT_CLOCK       kCLOCK_PortB                  //!< port clock
#define BOARD_RF_PORT_CLOCK_FREQ  (CLOCK_GetPllFllSelClkFreq()) //!< port clock frequency
#define BOARD_RF_CS_PIN           20U                           //!< CS pin number
#define BOARD_RF_MOSI_PIN         21U                           //!< MOSI pin number
#define BOARD_RF_MISO_PIN         22U                           //!< MISO pin number
#define BOARD_RF_SCLK_PIN         23U                           //!< SCLK pin number
#define BOARD_RF_PIN_ALT          kPORT_MuxAlt2                 //!< pin mux
#define BOARD_RF_UART             LPUART4                       //!< uart the cell modem is connected to
#define BOARD_RF_SPI              SPI2

// I/O pin config
#define BOARD_RF_IO_PORT          PORTC
#define BOARD_RF_IO_PORT_CLOCK    kCLOCK_PortC
#define BOARD_RF_IO_GPIO          GPIOC
#define BOARD_RF_IO0              6U
#define BOARD_RF_IO1              7U
#define BOARD_RF_IO2              10U
#define BOARD_RF_IO3              11U

// shutdown pin config
#define BOARD_RF_SHDN_PORT        PORTA                         //!< shutdown port
#define BOARD_RF_SHDN_PORT_CLOCK  kCLOCK_PortA                  //!< shutdown port clock
#define BOARD_RF_SHDN_GPIO        GPIOA                         //!< shutdown GPIO
#define BOARD_RF_SHDN             18U                           //!< shutdown GPIO pin

#endif // _UBIRCH_SPIRIT1_BOARD_H_
