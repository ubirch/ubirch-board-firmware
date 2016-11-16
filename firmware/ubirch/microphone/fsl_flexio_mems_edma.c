///**
// * KINETICS NXP DESIGN CONTEST PROJECT - FLEXUIDO PLATFORM
// * FLEX IO MEMS MIC EDMA LIBRARY
// * Author: Manuel Iglesias
// **/
//
//#include "fsl_flexio_mems_edma.h"
//#include "fsl_dmamux.h"
//
//
///*******************************************************************************
// * Definitions
// ******************************************************************************/
//
///*<! Structure definition for mems_edma_private_handle_t. The structure is private. */
//typedef struct _flexio_mems_edma_private_handle
//{
//    FLEXIO_MEMS_Type *base;
//    flexio_mems_edma_handle_t *handle;
//} flexio_mems_edma_private_handle_t;
//
///* MEMS EDMA transfer handle. */
//enum _flexio_mems_edma_tansfer_states
//{
//    kFLEXIO_MEMS_RxIdle, /* RX idle. */
//    kFLEXIO_MEMS_RxBusy  /* RX busy. */
//};
//
///*******************************************************************************
// * Definitions
// ******************************************************************************/
///*< @brief user configurable flexio mems handle count. */
//#define FLEXIO_MEMS_HANDLE_COUNT 2					// right and left channel
//
///*<! Private handle only used for internally. */
//static flexio_mems_edma_private_handle_t s_edmaPrivateHandle[FLEXIO_MEMS_HANDLE_COUNT];
//
///*******************************************************************************
// * Prototypes
// ******************************************************************************/
//
///*!
// * @brief FLEXIO MEMS EDMA receive finished callback function.
// *
// * This function is called when FLEXIO MEMS EDMA receive finished. It disables the MEMS
// * RX EDMA request and sends @ref kStatus_FLEXIO_MEMS_RxIdle to MEMS callback.
// *
// * @param handle The EDMA handle.
// * @param param Callback function parameter.
// */
//static void FLEXIO_MEMS_TransferReceiveEDMACallback(edma_handle_t *handle,
//                                                      void *param,
//                                                      bool transferDone,
//                                                      uint32_t tcds);
//
///*******************************************************************************
// * Code
// ******************************************************************************/
//
//static void FLEXIO_MEMS_TransferReceiveEDMACallback(edma_handle_t *handle,
//                                                      void *param,
//                                                      bool transferDone,
//                                                      uint32_t tcds)
//{
//    flexio_mems_edma_private_handle_t *memsPrivateHandle = (flexio_mems_edma_private_handle_t *)param;
//
//    /* Avoid the warning for unused variables. */
//    handle = handle;
//    tcds = tcds;
//
//    if (transferDone)
//    {
//        FLEXIO_MEMS_TransferAbortReceiveEDMA(memsPrivateHandle->base, memsPrivateHandle->handle);
//
//        if (memsPrivateHandle->handle->callback)
//        {
//            memsPrivateHandle->handle->callback(memsPrivateHandle->base, memsPrivateHandle->handle,
//                                                  kStatus_FLEXIO_MEMS_Idle, memsPrivateHandle->handle->userData);
//        }
//    }
//}
//status_t FLEXIO_MEMS_TransferCreateHandleEDMA(FLEXIO_MEMS_Type *base,
//                                                flexio_mems_edma_handle_t *handle,
//                                                flexio_mems_edma_transfer_callback_t callback,
//                                                void *userData,
//                                                edma_handle_t *rxEdmaHandle)
//{
//    assert(handle);
//
//    uint8_t index = 0;
//
//    /* Find the an empty handle pointer to store the handle. */
//    for (index = 0; index < FLEXIO_MEMS_HANDLE_COUNT; index++)
//    {
//        if (s_edmaPrivateHandle[index].base == NULL)
//        {
//            s_edmaPrivateHandle[index].base = base;
//            s_edmaPrivateHandle[index].handle = handle;
//            break;
//        }
//    }
//
//    if (index == FLEXIO_MEMS_HANDLE_COUNT)
//    {
//        return kStatus_OutOfRange;
//    }
//
//    s_edmaPrivateHandle[index].base = base;
//    s_edmaPrivateHandle[index].handle = handle;
//
//    memset(handle, 0, sizeof(*handle));
//
//    handle->rxState = kFLEXIO_MEMS_RxIdle;
//    handle->rxEdmaHandle = rxEdmaHandle;
//
//    handle->callback = callback;
//    handle->userData = userData;
//
//    /* Configure RX. */
//    if (rxEdmaHandle)
//    {
//        EDMA_SetCallback(handle->rxEdmaHandle, FLEXIO_MEMS_TransferReceiveEDMACallback, &s_edmaPrivateHandle);
//    }
//
//    return kStatus_Success;
//}
//
//status_t FLEXIO_MEMS_TransferReceiveEDMA(FLEXIO_MEMS_Type *base,
//                                           flexio_mems_edma_handle_t *handle,
//                                           flexio_mems_transfer_t *xfer)
//{
//    assert(handle->rxEdmaHandle);
//
//    edma_transfer_config_t xferConfig;
//    status_t status;
//
//    /* If previous RX not finished. */
//    if (kFLEXIO_MEMS_RxBusy == handle->rxState)
//    {
//        status = kStatus_FLEXIO_MEMS_RxBusy;
//    }
//    else
//    {
//        handle->rxState = kFLEXIO_MEMS_RxBusy;
//
//        /* Prepare transfer. */
//        EDMA_PrepareTransfer(&xferConfig, (void *)FLEXIO_MEMS_GetRxBufferAddress(base), 4, (void *)xfer->dataAddress,
//                             32, 32, xfer->dataNum, kEDMA_PeripheralToMemory);
//
//        /* Submit transfer. */
//        EDMA_SubmitTransfer(handle->rxEdmaHandle, &xferConfig);
//        EDMA_StartTransfer(handle->rxEdmaHandle);
//        /* Enable MEMS RX EDMA. */
//        FLEXIO_MEMS_EnableRxDMA(base, true);
//        status = kStatus_Success;
//    }
//
//    return status;
//}
//
//void FLEXIO_MEMS_TransferAbortReceiveEDMA(FLEXIO_MEMS_Type *base, flexio_mems_edma_handle_t *handle)
//{
//    assert(handle->rxEdmaHandle);
//
//    /* Disable MEMS RX EDMA. */
//    FLEXIO_MEMS_EnableRxDMA(base, false);
//
//    /* Stop transfer. */
//    EDMA_StopTransfer(handle->rxEdmaHandle);
//
//    handle->rxState = kFLEXIO_MEMS_RxIdle;
//}
//
//status_t FLEXIO_MEMS_TransferGetReceiveCountEDMA(FLEXIO_MEMS_Type *base,
//                                                   flexio_mems_edma_handle_t *handle,
//                                                   size_t *count)
//{
//    assert(handle->rxEdmaHandle);
//
//    if (!count)
//    {
//        return kStatus_InvalidArgument;
//    }
//
//    if (kFLEXIO_MEMS_RxBusy == handle->rxState)
//    {
//        *count = (handle->rxSize - EDMA_GetRemainingBytes(handle->rxEdmaHandle->base, handle->rxEdmaHandle->channel));
//    }
//    else
//    {
//        *count = handle->rxSize;
//    }
//
//    return kStatus_Success;
//}
