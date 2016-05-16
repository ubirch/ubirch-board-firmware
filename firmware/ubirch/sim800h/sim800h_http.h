/**
 * @brief ubirch#1 SIM800H HTTP operations.
 *
 * @author Matthias L. Jugel
 * @date 2016-05-08
 *
 * Copyright 2016 ubirch GmbH (https://ubirch.com)
 *
 * == LICENSE ==
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
 */

#ifndef _UBIRCH_SIM800H_HTTP_H_
#define _UBIRCH_SIM800H_HTTP_H_

typedef enum sim800h_http_methods {
    HTTP_GET = 0,
    HTTP_POST = 1
} sim800h_http_method_t;


int sim800h_http_prepare(const char *url, uint32_t timeout);

int sim800h_http(sim800h_http_method_t op, size_t *res_size, uint32_t timeout);

size_t sim800h_http_write(const uint8_t *buffer, size_t size, uint32_t timeout);

size_t sim800h_http_read(uint8_t *buffer, uint32_t start, size_t size, uint32_t timeout);

int sim800h_http_get(const char *url, size_t *res_size, uint32_t timeout);

int sim800h_http_post(const char *url, size_t *res_size, uint8_t *request, size_t req_size, uint32_t timeout);

#endif // _UBIRCH_SIM800H_HTTP_H_
