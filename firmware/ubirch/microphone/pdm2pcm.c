/**
 * INTERCOM PROJECT - KINETICS NXP PDM to PCM MEMS MIC
 * Author: Manuel Iglesias
 **/

#include "pdm2pcm.h"

#include "fsl_debug_console.h"
#include "fsl_flexio_mems.h"
#include "board.h"

#include "clock_config.h"
#include "fsl_edma.h"
#include "fsl_dmamux.h"
#include "fsl_clock.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MIC_USE_DMA	1

#define TRUE 	1
#define FALSE	0
#define BOARD_FLEXIO_BASE FLEXIO0
//#define FLEXIO_SPI_SOUT_PIN 4U
#define FLEXIO_MEMS_DAT_PIN 15U								// J1 - 1 / FXIO_D15 PTA5
#define FLEXIO_MEMS_CLK_PIN 18U    //PTA12    //23U J1 - 7 / FXIO_D23 PTA17
//#define FLEXIO_SPI_PCS_PIN 2U
#define FLEXIO_CLOCK_FREQUENCY CLOCK_GetFreq(kCLOCK_CoreSysClk)			// System Clock
#define FLEXIO_DMA_REQUEST_BASE kDmaRequestMux0Group1FlexIO0Channel0
#define MEMS_MIC_IRQ FLEXIO0_IRQn
#define PDM_BAUDRATE 500000U

#define FLEXIO_MEMS_DMA_CHANNEL		(18U)
#define FLEXIO_MEMS_DMA_SOURCE		(4U)				// This will allow to have channels 0-3 for camera

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
#if MIC_USE_DMA
// MEMS EDMA data reception handler
void MEMS_Handler(edma_handle_t *handle, void *param, bool transferDone, uint32_t tcds);
static void MEMS_FLEXIO_EDMA_Init(void);
static void MEMS_EDMA_Start(void);
static void MEMS_EDMA_Stop(void);

#else
void FLEXIO_MEMS_UserCallback(FLEXIO_MEMS_Type *base,
									flexio_mems_handle_t *handle,
									status_t status,
									void *userData);

static void MEMS_FLEXIO_Init(void);
static void MEMS_Start(void);
static void MEMS_Stop(void);
#endif


/*******************************************************************************
 * Variables
 ******************************************************************************/

static  __attribute__((aligned(32))) edma_tcd_t s_TcdMemoryPtrMEMSToFrame[1];

static FLEXIO_MEMS_Type s_FlexioMemsDevice = {
	.flexioBase = BOARD_FLEXIO_BASE,
    .rxPinIndex = FLEXIO_MEMS_DAT_PIN,
    .bclkPinIndex = FLEXIO_MEMS_CLK_PIN,
    .rxShifterIndex = FLEXIO_MEMS_DMA_SOURCE,
    .bclkTimerIndex = FLEXIO_MEMS_DMA_SOURCE,
	.bitWidth = 32,
    .sampleRate_Hz = PDM_BAUDRATE,
};

static flexio_mems_config_t s_FlexioMemsConfig;

static flexio_mems_handle_t	g_fioHandle;

static flexio_mems_pingpong_t	memDataHlr;

uint8_t __attribute__(( aligned(32), section(".myRAM"))) PDMRxData[2][PDM_TRANSFER_SIZE];

FLEXIO_MEMS_Type flexMic;

// mems handler
edma_handle_t 	g_EDMA_MEMS_Handle;

volatile tpfMemsFrameReadyCb		gpfMemsFrameCb;

volatile uint8_t Recording_Buff_A;
/*******************************************************************************
 * Code
 ******************************************************************************/

#if !MIC_USE_DMA
void FLEXIO_MEMS_UserCallback(FLEXIO_MEMS_Type *base,
									flexio_mems_handle_t *handle,
									status_t status,
									void *userData)
{
    if (status == kStatus_Success)
    {
        __NOP();
    }
}
#endif

void FLEXIO_MemMicInit(void){
#if MIC_USE_DMA
	// Follow DMA configuration
	MEMS_FLEXIO_EDMA_Init();
#else
	// Follow FlexIO Interrupt Mode
	memDataHlr.ping = &PDMRxData[0][0];
	memDataHlr.pong = &PDMRxData[1][0];
	memDataHlr.dataSize = PDM_TRANSFER_SIZE;
	MEMS_FLEXIO_Init();
#endif
}

void Mic_Start(void){
#if MIC_USE_DMA
	MEMS_EDMA_Start();
#else
	MEMS_Start();
#endif
}

void Mic_Stop(void){
#if MIC_USE_DMA
	MEMS_EDMA_Stop();
#else
	MEMS_Stop();
#endif
}

#if !MIC_USE_DMA
static void MEMS_FLEXIO_Init(void)
{
	flexio_mems_format_t memsFormat;

	FLEXIO_MEMS_GetDefaultConfig(&s_FlexioMemsConfig);
	FLEXIO_MEMS_Init(&s_FlexioMemsDevice, &s_FlexioMemsConfig);

	memsFormat.bitWidth = s_FlexioMemsDevice.bitWidth;
	memsFormat.sampleRate_Hz = s_FlexioMemsDevice.sampleRate_Hz;

	FLEXIO_MEMS_MasterSetFormat(&s_FlexioMemsDevice, &memsFormat, CLOCK_GetFreq(kCLOCK_CoreSysClk));
  FLEXIO_MEMS_TransferRxCreateHandle(&s_FlexioMemsDevice, &g_fioHandle, (flexio_mems_callback_t)FLEXIO_MEMS_UserCallback, NULL);
  FLEXIO_MEMS_TransferPingPongNonBlocking(&s_FlexioMemsDevice, &g_fioHandle, &memDataHlr);

}

static void MEMS_Start(void)
{
	FLEXIO_MEMS_Enable(&s_FlexioMemsDevice, true);
}

static void MEMS_Stop(void){
	FLEXIO_MEMS_Enable(&s_FlexioMemsDevice, false);
}
#endif

/*******************************************************************************
 *  EDMA code
 ******************************************************************************/
#if MIC_USE_DMA

static void MEMS_EDMA_Start(void)
{
	Recording_Buff_A = FALSE;
	FLEXIO_MEMS_Enable(&s_FlexioMemsDevice, true);
	EDMA_EnableChannelRequest(DMA0, FLEXIO_MEMS_DMA_CHANNEL);
}

static void MEMS_EDMA_Stop(void){
//	Recording_Buff_A = FALSE;
	FLEXIO_MEMS_Enable(&s_FlexioMemsDevice, false);
	EDMA_DisableChannelRequest(DMA0, FLEXIO_MEMS_DMA_CHANNEL);
}

static void EDMA_MEMSTransferInitTCD(DMA_Type *base, uint32_t channel, edma_tcd_t *tcd)
{
    assert(channel < FSL_FEATURE_EDMA_MODULE_CHANNEL);
    assert(tcd != NULL);
    assert(((uint32_t)tcd & 0x1FU) == 0);

    /* Push tcd into hardware TCD register */
    base->TCD[channel].SADDR = tcd->SADDR;
    base->TCD[channel].SOFF = tcd->SOFF;
    base->TCD[channel].ATTR = tcd->ATTR;
    base->TCD[channel].NBYTES_MLNO = tcd->NBYTES;
    base->TCD[channel].SLAST = tcd->SLAST;
    base->TCD[channel].DADDR = tcd->DADDR;
    base->TCD[channel].DOFF = tcd->DOFF;
    base->TCD[channel].CITER_ELINKNO = tcd->CITER;
    base->TCD[channel].DLAST_SGA = tcd->DLAST_SGA;
    base->TCD[channel].CSR = 0;
    base->TCD[channel].CSR = tcd->CSR;
    base->TCD[channel].BITER_ELINKNO = tcd->BITER;
}


static void MEMS_EDMA_Init(){
    /* Configure DMAMUX */
    DMAMUX_Init(DMAMUX0);
	edma_config_t edmaConfig;
	EDMA_GetDefaultConfig(&edmaConfig);
    /* Configure DMA */
    edmaConfig.enableDebugMode = true;
	EDMA_Init(DMA0, &edmaConfig);
}

static void EDMA_ChannelTransferInit(DMA_Type *EDMAUsed, uint8_t channelNum, uint8_t source, edma_tcd_t *tcd)
{
	DMAMUX_DisableChannel(DMAMUX0, channelNum);
    DMAMUX_SetSource(DMAMUX0, channelNum, source);
    EDMA_MEMSTransferInitTCD(EDMAUsed, channelNum, tcd);
    EDMA_SetModulo(EDMAUsed, channelNum, kEDMA_ModuloDisable, kEDMA_ModuloDisable);			// no circular addressing S&D
    EDMA_ClearChannelStatusFlags(EDMAUsed, channelNum, kEDMA_DoneFlag);
    EDMA_EnableChannelRequest(EDMAUsed, channelNum);
    DMAMUX_EnableChannel(DMAMUX0, channelNum);
}

static void MEMS_FLEXIO_EDMA_Init(void)
{
	flexio_mems_format_t memsFormat;

	FLEXIO_MEMS_GetDefaultConfig(&s_FlexioMemsConfig);
	FLEXIO_MEMS_Init(&s_FlexioMemsDevice, &s_FlexioMemsConfig);
	memsFormat.bitWidth = s_FlexioMemsDevice.bitWidth;
	memsFormat.sampleRate_Hz = s_FlexioMemsDevice.sampleRate_Hz;
	FLEXIO_MEMS_MasterSetFormat(&s_FlexioMemsDevice, &memsFormat, CLOCK_GetFreq(kCLOCK_CoreSysClk));
	/* Enable the flexio edma request */
	FLEXIO_EnableShifterStatusDMA(s_FlexioMemsDevice.flexioBase, (1U << s_FlexioMemsDevice.rxShifterIndex), true);
	// up to here we have initialize the FlexIo shifter and timer interface
	MEMS_EDMA_Init();
	EDMA_CreateHandle(&g_EDMA_MEMS_Handle, DMA0, FLEXIO_MEMS_DMA_CHANNEL);
	EDMA_SetCallback(&g_EDMA_MEMS_Handle, MEMS_Handler, NULL);				// Set the private callback for reception

    s_TcdMemoryPtrMEMSToFrame->SADDR = (uint32_t) FLEXIO_MEMS_RxGetDataRegisterAddress(&s_FlexioMemsDevice); 		//
    s_TcdMemoryPtrMEMSToFrame->SOFF = 0;                              		// capturing a FlexIO 32 bit Buffer always
    s_TcdMemoryPtrMEMSToFrame->ATTR = DMA_ATTR_SSIZE(kEDMA_TransferSize4Bytes) | DMA_ATTR_DSIZE(kEDMA_TransferSize4Bytes);   // 32 bit S&D
    s_TcdMemoryPtrMEMSToFrame->NBYTES = 4;                              // one 32 bit sample every minor loop
    s_TcdMemoryPtrMEMSToFrame->SLAST = 0;		        				// source address will always be newly written before each new start  DMA_TCD0_DADDR
    s_TcdMemoryPtrMEMSToFrame->DADDR = (uint32_t) &PDMRxData[0][0];    	// the PING PONG buffer destination
    s_TcdMemoryPtrMEMSToFrame->DOFF = 4;
    s_TcdMemoryPtrMEMSToFrame->CITER = PDM_TRANSFER_SIZE/4;
    s_TcdMemoryPtrMEMSToFrame->DLAST_SGA = 0;						// no final last adjustment ( does not move )	we handle it in EDMA callback
    s_TcdMemoryPtrMEMSToFrame->CSR = DMA_CSR_INTMAJOR_MASK;   		// interrupt when done
    s_TcdMemoryPtrMEMSToFrame->BITER = PDM_TRANSFER_SIZE/4;				// no chan links, total samples ( 512 )
    // don't forget the channel +1
    EDMA_ChannelTransferInit(DMA0, FLEXIO_MEMS_DMA_CHANNEL, (s_FlexioMemsDevice.rxShifterIndex + 1U), s_TcdMemoryPtrMEMSToFrame);

}


void MEMS_Handler(edma_handle_t *handle, void *param, bool transferDone, uint32_t tcds)
{
	EDMA_ClearChannelStatusFlags(DMA0, FLEXIO_MEMS_DMA_CHANNEL, kEDMA_InterruptFlag);
	if(!gpfMemsFrameCb(Recording_Buff_A)){		// this signal the last ready buffer with PDM data and returns true in case upper layer wants to stop the MIC
		if(Recording_Buff_A){
			Recording_Buff_A = FALSE;
			DMA0->TCD[FLEXIO_MEMS_DMA_CHANNEL].DADDR = (uint32_t) &PDMRxData[Recording_Buff_A][0];
		}else{
			Recording_Buff_A = TRUE;
			DMA0->TCD[FLEXIO_MEMS_DMA_CHANNEL].DADDR = (uint32_t) &PDMRxData[Recording_Buff_A][0];
		}
	}else{	// we should stop MIC
		MEMS_EDMA_Stop();
	}
	return;
}

#endif

void registerRecordingDeviceCb(tpfMemsFrameReadyCb pfMemsFrameCb){
	gpfMemsFrameCb = pfMemsFrameCb;
}


