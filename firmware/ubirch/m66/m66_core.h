/*!
 * @file
 * @brief M66 core functionality.
 *
 * Contains the core for enabling, disabling and
 * reading and writing to the serial port.
 *
 * @author Matthias L. Jugel
 * @date 2016-04-09
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
#ifndef _UBIRCH_M66_CORE_H_
#define _UBIRCH_M66_CORE_H_

#include <stddef.h>
#include <stdint.h>
#include <fsl_lpuart.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Enable the power domain for the M66.
 * If voltage sensing is available, returns as soon as the voltage is
 * high enough to make the chip work.
 * @return true if the chip was enabled, false if there was a problem
 */
bool modem_enable();

/*! @brief Disable the power domain for the M66 */
void modem_disable();

/*!
 * @brief Enable communication with the M66.
 * This actually configures the UART and powers/resets the chip to
 * start a fresh cycle.
 */
void modem_init();

/*!
 * @brief Read a single byte from the ringbuffer.
 * @return character or -1 if no data available
 */
int modem_read();

/*!
 * @brief Read binary data into a buffer
 * @param buffer the buffer to read into
 * @param max the number of bytes to read
 * @param timeout how long to wait
 * @return the amount of bytes read
 */
size_t modem_read_binary(uint8_t *buffer, size_t max, uint32_t timeout);

/*!
 * @brief Read a single line from the M66
 * @param buffer the character line buffer to read into
 * @param max the number of characters to read
 * @param timeout how long to wait
 * @return the number of characters read
 */
size_t modem_readline(char *buffer, size_t max, uint32_t timeout);

/*!
 * @brief write a number of bytes to the modem.
 * @param buffer byte buffer to write to
 * @param size amount of bytes to write
 */
void modem_write(const uint8_t *buffer, size_t size);

/*! @brief Write a line to the M66 and terminate it using CR LF */
void modem_writeline(const char *buffer);

#ifdef __cplusplus
}
#endif

#endif // _UBIRCH_M66_CORE_H_
