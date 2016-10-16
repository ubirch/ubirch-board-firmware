/*!
 * @file
 * @brief M66 HTTP operations.
 *
 * @author Matthias L. Jugel
 * @date 2016-10-14
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

#ifndef _UBIRCH_M66_HTTP_H_
#define _UBIRCH_M66_HTTP_H_

#ifdef __cplusplus
extern "C" {
#endif

// TODO: add enum for return codes and normalize codes

//! supported HTTP methods
typedef enum http_methods {
    HTTP_GET = 0,   /*!< HTTP GET */
    HTTP_POST = 1   /*!< HTTP POST */
} http_method_t;

/*!
 * @brief Prepare a HTTP request. Used by http().
 * @param url the URL to connect to
 * @param timeout how to wait for finishing the preparation
 * @return status of preparation
 */
int modem_http_prepare(const char *url, uint32_t timeout);

/*!
 * @brief Open HTTP connection using the specified method.
 *
 * Currently, the method is ignored with the M66, to execute
 * a specific GET or POST request either use modem_http_get() or
 * modem_http_post(), or execute the commands directly:
 *
 * GET request:
 * \code{.c}
 * modem_send("AT+QHTTPGET=%d", timeout_in_s);
 * if (!modem_expect_OK(timeout_in_ms)) PRINTF("ERROR");
 * modem_http(HTTP_GET, timeout_in_ms);
 * \endcode
 *
 * POST request:
 * \code{.c}
 * modem_send("AT+QHTTPPOST=%d,%d,%d", size, send_timeout_in_s, response_timeout_in_s);
 * if (!modem_expect("CONNECT", timeout_in_ms)) PRINTF("ERROR");
 * modem_http_write(data, data_size, timeout_in_mss);
 * if (!modem_expect_OK(timeout_in_ms)) PRINTF("ERROR");
 * modem_http(HTTP_POST, timeout_in_ms);
 * \endcode
 *
 * The M66 makes it difficult to load data if the server does not send
 * 'Content-Length' headers. Ensure a long enough timeout to be able to
 * receive the data.
 *
 * @param op the HTTP method (see ::http_methods)
 * @param res_size the size of the response
 * @param timeout how long to wait for the connection
 * @return connection status
 */
int modem_http(http_method_t op, size_t *res_size, uint32_t timeout);

/*!
 * @brief Write data to an opened HTTP connection.
 * @param buffer the data to write
 * @param size the size of the data
 * @param timeout how long to wait for the connection
 * @return amount of data written
 */
size_t modem_http_write(const uint8_t *buffer, size_t size, uint32_t timeout);

/*!
 * @brief Read data from a HTTP connection (the response).
 *
 * Reading may be done in multiple steps until the whole response is read.
 *
 * @param buffer the buffer to store the read data into
 * @param start the start index in the http buffer to start reading
 * @param size the amount of data to read
 * @param timeout how long to wait for the connection
 * @return amount of data read
 */
size_t modem_http_read(uint8_t *buffer, uint32_t start, size_t size, uint32_t timeout);

/*!
 * @brief Prepare and open a GET request.
 * @param url the url to open
 * @param res_size pointer to where the size of the response will be stored
 * @param timeout how long to wait for the connection
 * @return size of the downloaded file
 */
int modem_http_get(const char *url, size_t *res_size, uint32_t timeout);

/*!
 * @brief Prepare and open a POST request.
 * @param url the url to open
 * @param res_size pointer to where the size of the response will be stored
 * @param request the request data to be posted
 * @param req_size the amount of request data to be posted
 * @param timeout how long to wait for the connection
 * @return the HTTP status
 */
int modem_http_post(const char *url, size_t *res_size, uint8_t *request, size_t req_size, uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif // _UBIRCH_M66_HTTP_H_
