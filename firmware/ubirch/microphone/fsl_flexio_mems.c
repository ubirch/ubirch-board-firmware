/**
 * KINETICS NXP DESIGN CONTEST PROJECT - FLEXUIDO PLATFORM
 * FLEX IO MEMS MIC LIBRARY
 * Author: Manuel Iglesias
 **/


#include "fsl_flexio_mems.h"

/*******************************************************************************
* Definitions
******************************************************************************/
enum _sai_transfer_state
{
    kFLEXIO_MEMS_Busy = 0x0U, /*!< FLEXIO_I2S is busy */
    kFLEXIO_MEMS_Idle,        /*!< Transfer is done. */
};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Receive a piece of data in non-blocking way.
 *
 * @param base FLEXIO MEMS base pointer
 * @param bitWidth How many bits in a audio word, usually 8/16/24/32 bits.
 * @param buffer Pointer to the data to be read.
 * @param size Bytes to be read.
 */
static void FLEXIO_MEMS_ReadNonBlocking(FLEXIO_MEMS_Type *base, uint8_t bitWidth, uint8_t *rxData, size_t size);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

static void FLEXIO_MEMS_ReadNonBlocking(FLEXIO_MEMS_Type *base, uint8_t bitWidth, uint8_t *rxData, size_t size)
{
    uint32_t i = 0;
    uint8_t j = 0;
    uint8_t bytesPerWord = bitWidth / 8U;
    uint32_t data = 0;

    for (i = 0; i < size / bytesPerWord; i++)
    {
        data = base->flexioBase->SHIFTBUFBIS[base->rxShifterIndex];
        for (j = 0; j < bytesPerWord; j++)
        {
            *rxData = (data >> (8U * j)) & 0xFF;
            rxData++;
        }
    }
}

void FLEXIO_MEMS_Init(FLEXIO_MEMS_Type *base, const flexio_mems_config_t *config)
{
    assert(base && config);

    flexio_shifter_config_t shifterConfig = {0};
    flexio_timer_config_t timerConfig = {0};

    /* Ungate flexio clock. */
    CLOCK_EnableClock(kCLOCK_Flexio0);

    FLEXIO_Reset(base->flexioBase);

    /* Set shifter for MEMS Rx Data */
    shifterConfig.timerSelect    = base->bclkTimerIndex;
    shifterConfig.pinSelect      = base->rxPinIndex;
    shifterConfig.timerPolarity  = kFLEXIO_ShifterTimerPolarityOnNegitive;			// Left Channel ... and is Negative...
    shifterConfig.pinConfig      = kFLEXIO_PinConfigOutputDisabled;						// it's an input
    shifterConfig.pinPolarity    = kFLEXIO_PinActiveHigh;
    shifterConfig.shifterMode    = kFLEXIO_ShifterModeReceive;
    shifterConfig.inputSource    = kFLEXIO_ShifterInputFromPin;
    shifterConfig.shifterStop    = kFLEXIO_ShifterStopBitDisable;
    shifterConfig.shifterStart   = kFLEXIO_ShifterStartBitDisabledLoadDataOnEnable;

    FLEXIO_SetShifterConfig(base->flexioBase, base->rxShifterIndex, &shifterConfig);

    /* Set Timer to MEMS bit clock */
  timerConfig.triggerSelect   = 0;	//FLEXIO_TIMER_TRIGGER_SEL_TIMn(base->bclkTimerIndex);
	timerConfig.triggerPolarity = kFLEXIO_TimerTriggerPolarityActiveHigh;
	timerConfig.triggerSource   = kFLEXIO_TimerTriggerSourceExternal; //kFLEXIO_TimerTriggerSourceInternal; // 		// Trigger will be selected by TRGSEL. See FlexIO Trigger Options
	timerConfig.pinSelect       = base->bclkPinIndex;
	timerConfig.pinConfig       = kFLEXIO_PinConfigOutput;					// drive the pin clock
	timerConfig.pinPolarity     = kFLEXIO_PinActiveHigh;
	timerConfig.timerMode       = kFLEXIO_TimerModeDual8BitBaudBit;			// upper 8 bit for bit count and lower 8 bit for time division
	timerConfig.timerOutput     = kFLEXIO_TimerOutputZeroNotAffectedByReset;
	timerConfig.timerDecrement  = kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput;	// configure the baud rate divider
	timerConfig.timerReset      = kFLEXIO_TimerResetNever;
	timerConfig.timerDisable    = kFLEXIO_TimerDisableNever;
	timerConfig.timerEnable     = kFLEXIO_TimerEnabledAlways;
	timerConfig.timerStart      = kFLEXIO_TimerStartBitDisabled;
	timerConfig.timerStop       = kFLEXIO_TimerStopBitDisabled;
	timerConfig.timerCompare    = 0; 		// need to call FLEXIO_MEMS_MasterSetFormat after this function call return and before enabling module

  FLEXIO_SetTimerConfig(base->flexioBase, base->bclkTimerIndex, &timerConfig);

  /* Clear flags. */
  FLEXIO_ClearShifterErrorFlags(base->flexioBase, (1 << base->rxShifterIndex));
  FLEXIO_ClearTimerStatusFlags(base->flexioBase, 1U << (base->bclkTimerIndex));
}

void FLEXIO_MEMS_GetDefaultConfig(flexio_mems_config_t *config)
{
    assert(config);

    config->enableMems = false;
    config->enableInDoze = false;
    config->enableInDebug = false;
    config->enableFastAccess = false;
}

void FLEXIO_MEMS_Deinit(FLEXIO_MEMS_Type *base)
{
    /* Disable FLEXIO MEMS module. */
    FLEXIO_MEMS_Enable(base, false);

    /* Gate flexio clock. */
    CLOCK_DisableClock(kCLOCK_Flexio0);
}

void FLEXIO_MEMS_EnableInterrupts(FLEXIO_MEMS_Type *base, uint32_t mask)
{
    if (mask & kFLEXIO_MEMS_RxDataRegFullInterruptEnable)
    {
        FLEXIO_EnableShifterStatusInterrupts(base->flexioBase, 1U << base->rxShifterIndex);
    }
}

uint32_t FLEXIO_MEMS_GetStatusFlags(FLEXIO_MEMS_Type *base)
{
	uint32_t status =
			(((FLEXIO_GetShifterStatusFlags(base->flexioBase) & (1U << base->rxShifterIndex)) >> (base->rxShifterIndex)));
	return status;
}

uint32_t FLEXIO_MEMS_GetTimerStatusFlags(FLEXIO_MEMS_Type *base)
{
	uint32_t status =
			(((FLEXIO_GetTimerStatusFlags(base->flexioBase) & (1U << base->bclkTimerIndex)) >> (base->bclkTimerIndex)));
	return status;
}

void FLEXIO_MEMS_DisableInterrupts(FLEXIO_MEMS_Type *base, uint32_t mask)
{
    if (mask & kFLEXIO_MEMS_RxDataRegFullInterruptEnable)
    {
        FLEXIO_DisableShifterStatusInterrupts(base->flexioBase, 1U << base->rxShifterIndex);
    }
}

void FLEXIO_MEMS_MasterSetFormat(FLEXIO_MEMS_Type *base, flexio_mems_format_t *format, uint32_t srcClock_Hz)
{
	uint32_t timerDiv = srcClock_Hz / format->sampleRate_Hz;
    timerDiv = timerDiv / 2 - 1;

    uint32_t timerCmp = ((uint32_t)(format->bitWidth * 2 - 1U)) << 8U;			// Number of bits in each word equal to (CMP[15:8] + 1) / 2
    timerCmp |= timerDiv;

    /* Set bit clock timer cmp */
    base->flexioBase->TIMCMP[base->bclkTimerIndex] = FLEXIO_TIMCMP_CMP(timerCmp);
}



void FLEXIO_MEMS_ReadBlocking(FLEXIO_MEMS_Type *base, uint8_t bitWidth, uint8_t *rxData, size_t size)
{
    uint32_t i = 0;
    uint8_t bytesPerWord = bitWidth / 8U;

    for (i = 0; i < size / bytesPerWord; i++)
    {
        /* Wait until data is received */
        while (!(FLEXIO_GetShifterStatusFlags(base->flexioBase) & (1U << base->rxShifterIndex)))
        {
        }

        FLEXIO_MEMS_ReadNonBlocking(base, bitWidth, rxData, bytesPerWord);
        rxData += bytesPerWord;
    }
}

void FLEXIO_MEMS_TransferRxCreateHandle(FLEXIO_MEMS_Type *base,
                                       flexio_mems_handle_t *handle,
                                       flexio_mems_callback_t callback,
                                       void *userData)
{
    assert(handle);

    IRQn_Type flexio_irqs[] = FLEXIO_IRQS;

    /* Zero the handle. */
    memset(handle, 0, sizeof(*handle));

    /* Store callback and user data. */
    handle->callback = callback;
    handle->userData = userData;

    handle->bitWidth = base->bitWidth;		// set the bit width to gather data on interrupt

    /* Save the context in global variables to support the double weak mechanism. */
    FLEXIO_RegisterHandleIRQ(base, handle, FLEXIO_MEMS_TransferRxHandleIRQ);

    /* Set the TX/RX state. */
    handle->state = kFLEXIO_MEMS_Idle;

    /* Enable interrupt in NVIC. */
    EnableIRQ(flexio_irqs[0]);
}

void FLEXIO_MEMS_TransferSetFormat(FLEXIO_MEMS_Type *base,
                                  flexio_mems_handle_t *handle,
                                  flexio_mems_format_t *format,
                                  uint32_t srcClock_Hz)
{
    assert(handle && format);

    /* Set the bitWidth to handle */
    handle->bitWidth = format->bitWidth;

	/* It is master */
	FLEXIO_MEMS_MasterSetFormat(base, format, srcClock_Hz);

}


status_t FLEXIO_MEMS_TransferPingPongNonBlocking(FLEXIO_MEMS_Type *base,
                                               flexio_mems_handle_t *handle,
											   flexio_mems_pingpong_t *xfer)
{
    assert(handle);


    /* Add into queue */
    handle->queueUser = 0;
    handle->recvSize = xfer->dataSize;			// need this to restart the dataSize values of each capture
    // initialize ping pong buffer
    handle->queue[handle->queueUser].data = xfer->ping;
    handle->queue[handle->queueUser].dataSize = xfer->dataSize;
    handle->queue[handle->queueUser+1].data = xfer->pong;
    handle->queue[handle->queueUser+1].dataSize = xfer->dataSize;


    /* Set state to busy */
    handle->state = kFLEXIO_MEMS_Busy;

    /* Enable interrupt */
    FLEXIO_MEMS_EnableInterrupts(base, kFLEXIO_MEMS_RxDataRegFullInterruptEnable);

    /* Enable Rx transfer */
    FLEXIO_MEMS_Enable(base, true);

    return kStatus_Success;
}


status_t FLEXIO_MEMS_TransferReceiveNonBlocking(FLEXIO_MEMS_Type *base,
                                               flexio_mems_handle_t *handle,
                                               flexio_mems_transfer_t *xfer)
{
    assert(handle);

    /* Check if the queue is full */
    if (handle->queue[handle->queueUser].data)
    {
        return kStatus_FLEXIO_MEMS_QueueFull;
    }

    if ((xfer->dataSize == 0) || (xfer->data == NULL))
    {
        return kStatus_InvalidArgument;
    }

    /* Add into queue */
    handle->queue[handle->queueUser].data = xfer->data;
    handle->queue[handle->queueUser].dataSize = xfer->dataSize;
    handle->queueUser = (handle->queueUser + 1) % FLEXIO_MEMS_XFER_QUEUE_SIZE;
    /* Set state to busy */
    handle->state = kFLEXIO_MEMS_Busy;

    /* Enable interrupt */
    FLEXIO_MEMS_EnableInterrupts(base, kFLEXIO_MEMS_RxDataRegFullInterruptEnable);

    /* Enable Rx transfer */
    FLEXIO_MEMS_Enable(base, true);

    return kStatus_Success;
}


void FLEXIO_MEMS_TransferAbortReceive(FLEXIO_MEMS_Type *base, flexio_mems_handle_t *handle)
{
    assert(handle);

    /* Stop Tx transfer and disable interrupt */
    FLEXIO_MEMS_Enable(base, false);

    FLEXIO_MEMS_DisableInterrupts(base, kFLEXIO_MEMS_RxDataRegFullInterruptEnable);
    handle->state = kFLEXIO_MEMS_Idle;

    /* Clear the queue */
    memset(handle->queue, 0, sizeof(flexio_mems_transfer_t) * FLEXIO_MEMS_XFER_QUEUE_SIZE);
    handle->queueUser = 0;
}


void FLEXIO_MEMS_TransferRxHandleIRQ(void *memsBase, void *memsHandle)
{
    assert(memsHandle);

    flexio_mems_handle_t *handle = (flexio_mems_handle_t *)memsHandle;
    FLEXIO_MEMS_Type *base = (FLEXIO_MEMS_Type *)memsBase;
    uint8_t *buffer = handle->queue[handle->queueUser].data;
    uint8_t dataSize = handle->bitWidth / 8U;

    /* Handle transfer */
    if ((FLEXIO_MEMS_GetStatusFlags(base) & kFLEXIO_MEMS_RxDataRegFullFlag) != 0)
    {
        FLEXIO_MEMS_ReadNonBlocking(base, handle->bitWidth, buffer, dataSize);

        /* Update internal state */
        handle->queue[handle->queueUser].dataSize -= dataSize;
        handle->queue[handle->queueUser].data += dataSize;
    }

    /* If finished a block, call the callback function */
    if (handle->queue[handle->queueUser].dataSize == 0U)
    {
        if (handle->callback)
        {
            (handle->callback)(base, handle, kStatus_Success, handle->userData);
        }
        handle->queue[handle->queueUser].dataSize = handle->recvSize;		// restore size of PDM data to gather from MEMS
        handle->queue[handle->queueUser].data -= handle->recvSize;			// return the pointer to original position
        handle->queueUser = (handle->queueUser + 1) % FLEXIO_MEMS_XFER_QUEUE_SIZE;	 // change current buffer - ping pong fashion after call back
    }

    /* If all data finished, just stop the transfer */
    if (handle->queue[handle->queueUser].data == NULL)
    {
        FLEXIO_MEMS_TransferAbortReceive(base, handle);
    }
}
