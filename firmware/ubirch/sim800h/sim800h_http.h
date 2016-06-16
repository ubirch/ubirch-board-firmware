/*!
 * @file
 * @brief SIM800H HTTP operations.
 *
 * @author Matthias L. Jugel
 * @date 2016-05-08
 *
 * Copyright 2016 ubirch GmbH (https://ubirch.com)
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

#ifndef _UBIRCH_SIM800H_HTTP_H_
#define _UBIRCH_SIM800H_HTTP_H_

#ifdef __cplusplus
extern "C" {
#endif

//! supported HTTP methods
typedef enum sim800h_http_methods {
    HTTP_GET = 0,   /*!< HTTP GET */
    HTTP_POST = 1   /*!< HTTP POST */
} sim800h_http_method_t;


/*!
 * @brief Prepare a HTTP request. Used by sim800h_http().
 * @param url the URL to connect to
 * @param timeout how to wait for finishing the preparation
 * @return status of preparation
 */
int sim800h_http_prepare(const char *url, uint32_t timeout);

/*!
 * @brief Open HTTP connection using the specified method.
 * @param op the HTTP method (see ::sim800h_http_methods)
 * @param res_size the size of the response
 * @param timeout how long to wait for the connection
 * @return connection status
 */
int sim800h_http(sim800h_http_method_t op, size_t *res_size, uint32_t timeout);

/*!
 * @brief Write data to an opened HTTP connection.
 * @param buffer the data to write
 * @param size the size of the data
 * @param timeout how long to wait for the connection
 * @return amount of data written
 */
size_t sim800h_http_write(const uint8_t *buffer, size_t size, uint32_t timeout);

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
size_t sim800h_http_read(uint8_t *buffer, uint32_t start, size_t size, uint32_t timeout);

/*!
 * @brief Prepare and open a GET request.
 * @param url the url to open
 * @param res_size pointer to where the size of the response will be stored
 * @param timeout how long to wait for the connection
 * @return the HTTP status
 */
int sim800h_http_get(const char *url, size_t *res_size, uint32_t timeout);

/*!
 * @brief Prepare and open a POST request.
 * @param url the url to open
 * @param res_size pointer to where the size of the response will be stored
 * @param request the request data to be posted
 * @param req_size the amount of request data to be posted
 * @param timeout how long to wait for the connection
 * @return the HTTP status
 */
int sim800h_http_post(const char *url, size_t *res_size, uint8_t *request, size_t req_size, uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif // _UBIRCH_SIM800H_HTTP_H_
