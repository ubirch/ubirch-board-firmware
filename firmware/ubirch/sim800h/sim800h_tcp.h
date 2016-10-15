/*!
 * @file
 * @brief SIM800H TCP/IP operations.
 *
 * @author Niranjan H. Rao, Matthias L. Jugel
 * @date 2016-10-07
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

#ifndef UBIRCH_FIRMWARE_TCP_H
#define UBIRCH_FIRMWARE_TCP_H


#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Open a TCP connection to specified host and port.
 * @param host IP address or host name and domain
 * @param port the port number to connect to
 * @param timeout timout in ms to wait for establishing the connection
 * @return true if conenction was established, false on error
 */
bool modem_tcp_connect(const char *host, uint16_t port, uint32_t timeout);

/*!
 * @brief Send data via TCP socket.
 * @param buffer the data to send (max 1500 bytes)
 * @param len the length of the data to be sent
 * @param timeout the timeout for sending
 * @return true if sending was okay, false otherwise
 */
bool modem_tcp_send(const uint8_t *buffer, uint8_t len, uint32_t timeout);

/*!
 * @brief Receive data from TCP socket.
 * @param buffer the buffer to read data into (max 1500 bytes are read)
 * @param size the size of the buffer
 * @param timeout the timeout for receiving
 * @return how many bytes are actually read, 0 on error (or if request size is too big)
 */
size_t modem_tcp_receive(uint8_t *buffer, size_t size, uint32_t timeout);

/*!
 * @brief Close the TCP connection
 * @param timeout timeout for waiting for the close
 * @return true if the close was successful, false otherwise
 */
bool modem_tcp_close(uint32_t timeout);

#endif //UBIRCH_FIRMWARE_MQTT_H
