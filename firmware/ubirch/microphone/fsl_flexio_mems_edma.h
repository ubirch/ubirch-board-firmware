//#ifndef _FSL_FLEXIO_MEMS_EDMA_H_
//#define _FSL_FLEXIO_MEMS_EDMA_H_
//
//#include "fsl_flexio_mems.h"
//#include "fsl_dmamux.h"
//#include "fsl_edma.h"
//
///*!
// * @addtogroup flexio_edma_mems
// * @{
// */
//
///*! @file*/
//
///*******************************************************************************
// * Definitions
// ******************************************************************************/
//
///*! @brief Forward declaration of the handle typedef. */
//typedef struct _flexio_mems_edma_handle flexio_mems_edma_handle_t;
//
///*! @brief MEMS transfer callback function. */
//typedef void (*flexio_mems_edma_transfer_callback_t)(FLEXIO_MEMS_Type *base,
//                                                       flexio_mems_edma_handle_t *handle,
//                                                       status_t status,
//                                                       void *userData);
//
///*!
//* @brief MEMS eDMA handle
//*/
//struct _flexio_mems_edma_handle
//{
//    flexio_mems_edma_transfer_callback_t callback; /*!< Callback function. */
//    void *userData;                                  /*!< MEMS callback function parameter.*/
//    size_t rxSize;                                   /*!< Total bytes to be received. */
//    edma_handle_t *rxEdmaHandle;                     /*!< The eDMA RX channel used. */
//    volatile uint8_t rxState;                        /*!< RX transfer state */
//};
//
///*******************************************************************************
// * API
// ******************************************************************************/
//
//#if defined(__cplusplus)
//extern "C" {
//#endif
//
///*!
// * @name eDMA transactional
// * @{
// */
//
///*!
// * @brief Initializes the mems handle, which is used in transactional functions.
// *
// * @param base pointer to FLEXIO_MEMS_Type.
// * @param handle Pointer to flexio_mems_edma_handle_t structure.
// * @param callback The callback function.
// * @param userData The parameter of the callback function.
// * @param rxEdmaHandle User requested DMA handle for RX DMA transfer.
// * @retval kStatus_Success Successfully create the handle.
// * @retval kStatus_OutOfRange The FlexIO mems eDMA type/handle table out of range.
// */
//status_t FLEXIO_MEMS_TransferCreateHandleEDMA(FLEXIO_MEMS_Type *base,
//                                                flexio_mems_edma_handle_t *handle,
//                                                flexio_mems_edma_transfer_callback_t callback,
//                                                void *userData,
//                                                edma_handle_t *rxEdmaHandle);
//
///*!
// * @brief Receives data using eDMA.
// *
// * This function receives data using eDMA. This is a non-blocking function, which returns
// * right away. When all data is received, the receive callback function is called.
// *
// * @param base Pointer to the FLEXIO_MEMS_Type.
// * @param handle Pointer to the flexio_mems_edma_handle_t structure.
// * @param xfer MEMS eDMA transfer structure, see #flexio_mems_transfer_t.
// * @retval kStatus_Success if succeeded, others failed.
// * @retval kStatus_MEMS_RxBusy Previous transfer on going.
// */
//status_t FLEXIO_MEMS_TransferReceiveEDMA(FLEXIO_MEMS_Type *base,
//                                           flexio_mems_edma_handle_t *handle,
//                                           flexio_mems_transfer_t *xfer);
//
///*!
// * @brief Aborts the receive data which used the eDMA.
// *
// * This function aborts the receive data which used the eDMA.
// *
// * @param base Pointer to the FLEXIO_MEMS_Type.
// * @param handle Pointer to the flexio_mems_edma_handle_t structure.
// */
//void FLEXIO_MEMS_TransferAbortReceiveEDMA(FLEXIO_MEMS_Type *base, flexio_mems_edma_handle_t *handle);
//
///*!
// * @brief Gets the remaining bytes to be received.
// *
// * This function gets the number of bytes still not received.
// *
// * @param base Pointer to the FLEXIO_MEMS_Type.
// * @param handle Pointer to the flexio_mems_edma_handle_t structure.
// * @param count Number of bytes sent so far by the non-blocking transaction.
// * @retval kStatus_Success Succeed get the transfer count.
// * @retval kStatus_InvalidArgument The count parameter is invalid.
// */
//status_t FLEXIO_MEMS_TransferGetReceiveCountEDMA(FLEXIO_MEMS_Type *base,
//                                                   flexio_mems_edma_handle_t *handle,
//                                                   size_t *count);
//
///*@}*/
//
//#if defined(__cplusplus)
//}
//#endif
//
///*! @}*/
//
//#endif /* _FSL_MEMS_EDMA_H_ */
