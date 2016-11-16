#ifndef _FSL_FLEXIO_MEMS_H_
#define _FSL_FLEXIO_MEMS_H_

#include "fsl_common.h"
#include "fsl_flexio.h"

/*!
 * @addtogroup flexio_mems
 * @{
 */

/*! @file */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief FlexIO MEMS driver version 0.0.1. */
#define FSL_FLEXIO_MEMS_DRIVER_VERSION (MAKE_VERSION(0, 0, 1))
/*@}*/

/*! @brief FlexIO MEMS transfer status */
enum _flexio_mems_status
{
    kStatus_FLEXIO_MEMS_Idle = MAKE_STATUS(kStatusGroup_MEMS_MIC, 0),      /*!< FlexIO MEMS is in idle state */
    kStatus_FLEXIO_MEMS_RxBusy = MAKE_STATUS(kStatusGroup_MEMS_MIC, 2),    /*!< FlexIO MEMS Tx is busy */
    kStatus_FLEXIO_MEMS_Error = MAKE_STATUS(kStatusGroup_MEMS_MIC, 3),     /*!< FlexIO MEMS error occurred */
    kStatus_FLEXIO_MEMS_QueueFull = MAKE_STATUS(kStatusGroup_MEMS_MIC, 4), /*!< FlexIO MEMS transfer queue is full. */
};

/*! @brief Define FlexIO MEMS access structure typedef */
typedef struct _flexio_mems_type
{
    FLEXIO_Type *flexioBase; /*!< FlexIO base pointer */
    uint8_t rxPinIndex;      /*!< Rx data pin index */
    uint8_t bclkPinIndex;    /*!< Bit clock pin index */
    uint8_t rxShifterIndex;  /*!< Rx data shifter index */
    uint8_t bclkTimerIndex;  /*!< Bit clock timer index */
    uint8_t bitWidth;       /*!< Bit width of audio data, always 8/16/24/32 bits */
    uint32_t sampleRate_Hz;	 /*!< bit PDM sample rate */
} FLEXIO_MEMS_Type;


/*! @brief Define FlexIO FlexIO MEMS interrupt mask. */
enum _flexio_mems_interrupt_enable
{
    kFLEXIO_MEMS_RxDataRegFullInterruptEnable = 0x1U,  /*!< Receive buffer full interrupt enable. */
};

/*! @brief Define FlexIO FlexIO MEMS status mask. */
enum _flexio_mems_status_flags
{
    kFLEXIO_MEMS_RxDataRegFullFlag = 0x1U,  /*!< Receive buffer full flag. */
};

/*! @brief FlexIO MEMS configure structure */
typedef struct _flexio_mems_config
{
    bool enableMems;
    bool enableInDoze;
    bool enableInDebug;
    bool enableFastAccess;
} flexio_mems_config_t;

/*! @brief FlexIO MEMS audio format, FlexIO MEMS only support the same format in Tx and Rx */
typedef struct _flexio_mems_format
{
    uint8_t bitWidth;       /*!< Bit width of audio data, always 8/16/24/32 bits */
    uint32_t sampleRate_Hz; /*!< Sample rate of the audio data */
} flexio_mems_format_t;

/*!@brief FlexIO MEMS transfer queue size, user can refine it according to use case. */
#define FLEXIO_MEMS_XFER_QUEUE_SIZE (2)			// we defined as 2 so we can match with our upper layer ping pong buffer

/*! @brief Audio sample rate */
typedef enum _flexio_mems_sample_rate
{
    kFLEXIO_MEMS_SampleRate8KHz = 8000U,     /*!< Sample rate 8000Hz */
    kFLEXIO_MEMS_SampleRate11025Hz = 11025U, /*!< Sample rate 11025Hz */
    kFLEXIO_MEMS_SampleRate12KHz = 12000U,   /*!< Sample rate 12000Hz */
    kFLEXIO_MEMS_SampleRate16KHz = 16000U,   /*!< Sample rate 16000Hz */
    kFLEXIO_MEMS_SampleRate22050Hz = 22050U, /*!< Sample rate 22050Hz */
    kFLEXIO_MEMS_SampleRate24KHz = 24000U,   /*!< Sample rate 24000Hz */
    kFLEXIO_MEMS_SampleRate32KHz = 32000U,   /*!< Sample rate 32000Hz */
    kFLEXIO_MEMS_SampleRate44100Hz = 44100U, /*!< Sample rate 44100Hz */
    kFLEXIO_MEMS_SampleRate48KHz = 48000U,   /*!< Sample rate 48000Hz */
    kFLEXIO_MEMS_SampleRate96KHz = 96000U    /*!< Sample rate 96000Hz */
} flexio_mems_sample_rate_t;

/*! @brief Audio word width */
typedef enum _flexio_mems_word_width
{
    kFLEXIO_MEMS_WordWidth8bits = 8U,   /*!< Audio data width 8 bits */
    kFLEXIO_MEMS_WordWidth16bits = 16U, /*!< Audio data width 16 bits */
    kFLEXIO_MEMS_WordWidth24bits = 24U, /*!< Audio data width 24 bits */
    kFLEXIO_MEMS_WordWidth32bits = 32U  /*!< Audio data width 32 bits */
} flexio_mems_word_width_t;

/*! @brief Define FlexIO MEMS transfer structure. */
typedef struct _flexio_mems_transfer
{
    uint8_t *data;   /*!< Data buffer start pointer */
    size_t dataSize; /*!< Bytes to be transferred. */
} flexio_mems_transfer_t;

typedef struct _flexio_mems_pingpong
{
    uint8_t *ping;
    uint8_t *pong;
    size_t dataSize; /*!< Bytes to be transferred. */
} flexio_mems_pingpong_t;


typedef struct _flexio_mems_handle flexio_mems_handle_t;

/*! @brief FlexIO MEMS xfer callback prototype */
typedef void (*flexio_mems_callback_t)(FLEXIO_MEMS_Type *base,
                                      flexio_mems_handle_t *handle,
                                      status_t status,
                                      void *userData);

/*! @brief Define FlexIO MEMS handle structure. */
struct _flexio_mems_handle
{
    uint32_t state;                                          /*!< Internal state */
    flexio_mems_callback_t callback;                          /*!< Callback function called at transfer event*/
    void *userData;                                          /*!< Callback parameter passed to callback function*/
    uint8_t bitWidth;                                        /*!< Bit width for transfer, 8/16/24/32bits */
    flexio_mems_transfer_t queue[FLEXIO_MEMS_XFER_QUEUE_SIZE]; /*!< Transfer queue storing queued transfer */
    uint16_t recvSize;										 /* the size of ping pong buffers */
    volatile uint8_t queueUser;                              /*!< Index for user to queue transfer */
};

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /*_cplusplus*/

/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief Initializes the FlexIO MEMS.
 *
 * This API configures FlexIO pins and shifter to MEMS and configure FlexIO MEMS with configuration structure.
 * The configuration structure can be filled by the user, or be set with default values by
 * FLEXIO_MEMS_GetDefaultConfig().
 *
 * @note  This API should be called at the beginning of the application to use
 * the FlexIO MEMS driver, or any access to the FlexIO MEMS module could cause hard fault
 * because clock is not enabled.
 *
 * @param base FlexIO MEMS base pointer
 * @param config FlexIO MEMS configure structure.
*/
void FLEXIO_MEMS_Init(FLEXIO_MEMS_Type *base, const flexio_mems_config_t *config);

/*!
 * @brief  Sets the FlexIO MEMS configuration structure to default values.
 *
 * The purpose of this API is to get the configuration structure initialized for use in FLEXIO_MEMS_Init().
 * User may use the initialized structure unchanged in FLEXIO_MEMS_Init(), or modify
 * some fields of the structure before calling FLEXIO_MEMS_Init().
 *
 * @param config pointer to master configuration structure
 */
void FLEXIO_MEMS_GetDefaultConfig(flexio_mems_config_t *config);

/*!
 * @brief De-initializes the FlexIO MEMS.
 *
 * Calling this API gates the FlexIO mems clock. After calling this API, call the FLEXO_MEMS_Init to use the
 * FlexIO MEMS module.
 *
 * @param base FlexIO MEMS base pointer
*/
void FLEXIO_MEMS_Deinit(FLEXIO_MEMS_Type *base);

/*!
 * @brief Enables/disables the FlexIO MEMS module operation.
 *
 * @param base pointer to FLEXIO_MEMS_Type
 * @param enable True to enable, false to disable.
*/
static inline void FLEXIO_MEMS_Enable(FLEXIO_MEMS_Type *base, bool enable)
{
    if (enable)
    {
        base->flexioBase->CTRL |= FLEXIO_CTRL_FLEXEN_MASK;
    }
    else
    {
        base->flexioBase->CTRL &= ~FLEXIO_CTRL_FLEXEN_MASK;
    }
}

/*! @} */

/*!
 * @name Status
 * @{
 */

/*!
 * @brief Gets the FlexIO MEMS status flags of Shifter.
 *
 * @param base pointer to FLEXIO_MEMS_Type structure
 * @return Status flag, which are ORed by the enumerators in the _flexio_mems_status_flags.
*/
uint32_t FLEXIO_MEMS_GetStatusFlags(FLEXIO_MEMS_Type *base);


/*!
 * @brief Gets the FlexIO MEMS status flags of Timer.
 *
 * @param base pointer to FLEXIO_MEMS_Type structure
 * @return Status flag, which are ORed by the enumerators in the _flexio_mems_status_flags.
*/
uint32_t FLEXIO_MEMS_GetTimerStatusFlags(FLEXIO_MEMS_Type *base);


/*! @} */

/*!
 * @name Interrupts
 * @{
 */

/*!
 * @brief Enables the FlexIO MEMS interrupt.
 *
 * This function enables the FlexIO UART interrupt.
 *
 * @param base pointer to FLEXIO_MEMS_Type structure
 * @param mask interrupt source
 */
void FLEXIO_MEMS_EnableInterrupts(FLEXIO_MEMS_Type *base, uint32_t mask);

/*!
 * @brief Disables the FlexIO MEMS interrupt.
 *
 * This function enables the FlexIO UART interrupt.
 *
 * @param base pointer to FLEXIO_MEMS_Type structure
 * @param mask interrupt source
 */
void FLEXIO_MEMS_DisableInterrupts(FLEXIO_MEMS_Type *base, uint32_t mask);

/*! @} */

/*!
 * @name DMA Control
 * @{
 */

/*!
 * @brief Enables/disables the FlexIO MEMS Rx DMA requests.
 *
 * @param base FlexIO MEMS base pointer
 * @param enable True means enable DMA, false means disable DMA.
 */
static inline void FLEXIO_MEMS_RxEnableDMA(FLEXIO_MEMS_Type *base, bool enable)
{
    FLEXIO_EnableShifterStatusDMA(base->flexioBase, 1 << base->rxShifterIndex, enable);
}

/*!
 * @brief Gets the FlexIO MEMS receive data register address.
 *
 * This function returns the MEMS data register address, mainly used by DMA/eDMA.
 *
 * @param base pointer to FLEXIO_MEMS_Type structure
 * @return FlexIO mems receive data register address.
 */
static inline uint32_t FLEXIO_MEMS_RxGetDataRegisterAddress(FLEXIO_MEMS_Type *base)
{
    return FLEXIO_GetShifterBufferAddress(base->flexioBase, kFLEXIO_ShifterBufferBitSwapped, base->rxShifterIndex);
}

/*! @} */

/*!
 * @name Bus Operations
 * @{
 */

/*!
 * @brief Configures the FlexIO MEMS audio format in master mode.
 *
 * Audio format can be changed in run-time of FlexIO MEMS. This function configures the sample rate and audio data
 * format to be transferred.
 *
 * @param base pointer to FLEXIO_MEMS_Type structure
 * @param format Pointer to FlexIO MEMS audio data format structure.
 * @param srcClock_Hz MEMS master clock source frequency in Hz.
*/
void FLEXIO_MEMS_MasterSetFormat(FLEXIO_MEMS_Type *base, flexio_mems_format_t *format, uint32_t srcClock_Hz);



/*!
 * @brief Receives a piece of data using a blocking method.
 *
 * @note This function blocks via polling until data is ready to be sent.
 *
 * @param base FlexIO MEMS base pointer
 * @param bitWidth How many bits in a audio word, usually 8/16/24/32 bits.
 * @param rxData Pointer to the data to be read.
 * @param size Bytes to be read.
 */
void FLEXIO_MEMS_ReadBlocking(FLEXIO_MEMS_Type *base, uint8_t bitWidth, uint8_t *rxData, size_t size);

/*!
 * @brief Reads a data from the data register.
 *
 * @param base FlexIO MEMS base pointer
 * @return Data read from data register.
 */
static inline uint32_t FLEXIO_MEMS_ReadData(FLEXIO_MEMS_Type *base)
{
    return base->flexioBase->SHIFTBUFBIS[base->rxShifterIndex];
}

/*! @} */

/*!
 * @name Transactional
 * @{
 */

/*!
 * @brief Configures the FlexIO MEMS audio format.
 *
 * Audio format can be changed in run-time of FlexIO mems. This function configures the sample rate and audio data
 * format to be transferred.
 *
 * @param base pointer to FLEXIO_MEMS_Type structure.
 * @param handle FlexIO MEMS handle pointer.
 * @param format Pointer to audio data format structure.
 * @param srcClock_Hz FlexIO MEMS bit clock source frequency in Hz. This parameter should be 0 while in slave mode.
*/
void FLEXIO_MEMS_TransferSetFormat(FLEXIO_MEMS_Type *base,
                                  flexio_mems_handle_t *handle,
                                  flexio_mems_format_t *format,
                                  uint32_t srcClock_Hz);

/*!
 * @brief Initializes the FlexIO MEMS receive handle.
 *
 * This function initializes the FlexIO MEMS handle which can be used for other
 * FlexIO MEMS transactional APIs. Usually, user only need to call this API once to get the
 * initialized handle.
 *
 * @param base pointer to FLEXIO_MEMS_Type structure.
 * @param handle pointer to flexio_mems_handle_t structure to store the transfer state.
 * @param callback FlexIO MEMS callback function, which is called while finished a block.
 * @param userData User parameter for the FlexIO MEMS callback.
 */
void FLEXIO_MEMS_TransferRxCreateHandle(FLEXIO_MEMS_Type *base,
                                       flexio_mems_handle_t *handle,
                                       flexio_mems_callback_t callback,
                                       void *userData);



status_t FLEXIO_MEMS_TransferPingPongNonBlocking(FLEXIO_MEMS_Type *base,
													flexio_mems_handle_t *handle,
													flexio_mems_pingpong_t *xfer);

/*!
 * @brief Performs an interrupt non-blocking receive transfer on FlexIO MEMS.
 *
 * @note The API returns immediately after transfer initiates.
 * Call FLEXIO_MEMS_GetRemainingBytes to poll the transfer status to check whether
 * the transfer is finished. If the return status is 0, the transfer is finished.
 *
 * @param base pointer to FLEXIO_MEMS_Type structure.
 * @param handle pointer to flexio_mems_handle_t structure which stores the transfer state
 * @param xfer pointer to flexio_mems_transfer_t structure
 * @retval kStatus_Success Successfully start the data receive.
 * @retval kStatus_FLEXIO_MEMS_RxBusy Previous receive still not finished.
 * @retval kStatus_InvalidArgument The input parameter is invalid.
 */
status_t FLEXIO_MEMS_TransferReceiveNonBlocking(FLEXIO_MEMS_Type *base,
                                               flexio_mems_handle_t *handle,
                                               flexio_mems_transfer_t *xfer);


/*!
 * @brief Aborts the current receive.
 *
 * @note This API can be called at any time when interrupt non-blocking transfer initiates
 * to abort the transfer in a early time.
 *
 * @param base pointer to FLEXIO_MEMS_Type structure.
 * @param handle pointer to flexio_mems_handle_t structure which stores the transfer state
 */
void FLEXIO_MEMS_TransferAbortReceive(FLEXIO_MEMS_Type *base, flexio_mems_handle_t *handle);


/*!
 * @brief Rx interrupt handler.
 *
 * @param memsBase pointer to FLEXIO_MEMS_Type structure.
 * @param memsHandle pointer to flexio_mems_handle_t structure
 */
void FLEXIO_MEMS_TransferRxHandleIRQ(void *memsBase, void *memsHandle);

/*! @} */

#if defined(__cplusplus)
}
#endif /*_cplusplus*/

/*! @} */

#endif /* _FSL_FLEXIO_MEMS_H_ */
