/*!
* @file
* @brief SDHC config necessary for fsl sd disk middleware.
*
* @author Matthias L. Jugel
* @date 2016-04-06
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

#ifndef _UBIRCH_SDHC_CONFIG_H_
#define _UBIRCH_SDHC_CONFIG_H_

#include <board.h>

// ===============================================================================================
// CODE necessary for SDHC file transfer, taken from SDK examples
// ===============================================================================================

#define BOARD_SDHC_BASEADDR         BOARD_SDHC
#define BOARD_SDHC_CLKSRC           BOARD_SDHC_CLOCK

#define SDHC_ENDIAN_MODE            kSDHC_EndianModeLittle
#define SDHC_DMA_MODE               kSDHC_DmaModeAdma2
#define SDHC_READ_WATERMARK_LEVEL   (0x80U)
#define SDHC_WRITE_WATERMARK_LEVEL  (0x80U)
#define SDHC_ADMA_TABLE_WORDS       (8U)

#define EVENT_TIMEOUT_TRANSFER_COMPLETE (1000U)

#ifdef __cplusplus
extern "C" {
#endif

/*! @brief SDHC event type */
typedef enum {
    kEVENT_TransferComplete = 0U, /*!< Transfer complete event */
} event_t;

/*! @brief initialize event timer (done implicitly for ubirch firmware). */
static inline void EVENT_InitTimer(void) {};

/*!
 * @brief Create new SDHC event.
 * @param eventType The event type
 * @retval true Create event successfully.
 * @retval false Create event failed.
 */
bool EVENT_Create(event_t eventType);

/*!
 * @brief Wait for an SDHC event.
 * @param eventType The event type
 * @param timeoutMilliseconds Timeout time in milliseconds.
 * @retval true Wait event successfully.
 * @retval false Wait event failed.
 */
bool EVENT_Wait(event_t eventType, uint32_t timeoutMilliseconds);

/*!
 * @brief Notify of SDHC event.
 * @param eventType The event type
 * @retval true Notify event successfully.
 * @retval false Notify event failed.
 */
bool EVENT_Notify(event_t eventType);

/*!
 * @brief Delete SDHC event.
 * @param eventType The event type
 */
void EVENT_Delete(event_t eventType);

#ifdef __cplusplus
}
#endif

#endif /* _SDHC_CONFIG_H_ */
