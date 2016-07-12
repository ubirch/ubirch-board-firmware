/*!
 * @file
 * @brief Test support
 *
 * Support for testing the board
 *
 * @author Matthias L. Jugel
 * @date   2016-07-12
 *
 * @copyright &copy; 2016 ubirch GmbH (https://ubirch.com)
 *
 * @section LICENSE
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

#include <stdarg.h>
#include <stdio.h>
#include <malloc.h>
#include <fsl_debug_console.h>

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

bool yesno(char *prompt) {
  PRINTF(prompt);
  PRINTF(" (y/n)");
  int c;
  while ((c = GETCHAR()) != 'y' && c != 'n');
  PRINTF("\r\n");
  return c == 'y';
}

void ok(char *prefix, bool r) {
  PRINTF("%s: ", prefix);
  if(r) PRINTF("OK\r\n"); else PRINTF("FAIL\r\n");
}
