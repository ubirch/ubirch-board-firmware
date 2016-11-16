/*****************************************************************************
 * (c) Copyright 2010, Freescale Semiconductor Inc.
 * ALL RIGHTS RESERVED.
 ***************************************************************************//*!
 *
 * @file      audio_dma.c
 *
 * @author    b01800
 *
 * @version   1.0.2.0
 *
 * @date      Mar-23-2012
 *
 * @brief
 ******************************************************************************/

#include <stdlib.h>
#include "fsl_common.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_edma.h"
#include "fsl_dmamux.h"
#include "fsl_port.h"

//to support dac and vref
#include "fsl_vref.h"
#include "fsl_dac.h"
// support ftm
#include "fsl_ftm.h"
#include "fsl_pit.h"
#include "audio_dma.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define TRUE 	1
#define FALSE	0
volatile tpfAudioDmaCb		gpfAudioDmaCb;
#define DAC_DMA_CHANNEL (0U)			// for periodic trigger DMA should be in the channels from 0 - 3 range (see K82F RM DMAMUX page 529)
#define DAC_DMA_SOURCE 	(31U)			// PIT2 is triggering DAC DMA

/*! @brief Pointers to FTM clocks for each instance. */
static const clock_ip_name_t s_ftmClocks[] = FTM_CLOCKS;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void Audio_Handler(edma_handle_t *handle, void *param, bool transferDone, uint32_t tcds);

/*******************************************************************************
 * Variables
 ******************************************************************************/

static  __attribute__((aligned(32))) edma_tcd_t s_TcdMemoryPtrAudioToFrame[1];
// 16 bit audio samples - not volatile because only read by interrupts/hw
int16_t __attribute__(( aligned(32), section(".myRAM") )) Audio_Source_Blk_A[AUDIO_SIZE];
int16_t __attribute__((aligned(32), section(".myRAM"))) Audio_Source_Blk_B[AUDIO_SIZE];

// to be used during mute
int16_t __attribute__((aligned(32), section(".myRAM") )) Audio_Silence[AUDIO_SIZE];

volatile int Playing_Buff_A;

edma_handle_t 	g_EDMA_Audio_Handle;

/*******************************************************************************
 * Code
 ******************************************************************************/


/***************************************************************************//*!
* @function ClearAudioBuffers
*
* @brief    Clear Audio buffers used by DMA channel for transfering to output
****************************************************************************/
void ClearAudioBuffers(void)
{
  int i;
  int16_t *p1,*p2,*p3;

  p1 = Audio_Source_Blk_A;
  p2 = Audio_Source_Blk_B;
  p3 = Audio_Silence;

  for (i=0 ; i < AUDIO_SIZE; i++)
    {
    *p1++ = 0;   // mute initially
    *p2++ = 0;   // mute initially
    *p3++ = 0;   // mute forever
    }
 }


void ClearAudioBuf(int16_t *p){
	for (int i=0 ; i < AUDIO_SIZE; i++)
	{
		*p++ = 0;   // mute initially
	}
}
/****************************************************************************/

/***************************************************************************//*!
* @function Init_Audio
*
* @brief    Setup Timer, DMA channels and DMA complete IRQ for audio task
****************************************************************************/
void Init_Audio(void)
{

  ClearAudioBuffers();

  Set_Audio_Playback_Dma();

  Playing_Buff_A = TRUE;

#if (AUDIO_DMA_OUTPUT == USE_PWM)

  Init_FTM2(); // pwm starts running - audio playback DMA must be already setup

#elif (AUDIO_DMA_OUTPUT == USE_I2S)

  // Not implemented yet for this project but leave it here for future ports.

#elif (AUDIO_DMA_OUTPUT == USE_DAC)
  Set_VREF();     // VREF for DAC
  Init_DAC();     // DACs ON
  Init_FTM2();
//  Init_PIT();

#endif

}
/****************************************************************************/

/***************************************************************************//*!
* @function Start_Audio
*
* @brief    Clears the buffers and enable DMA Audio channel feed
****************************************************************************/

void Start_Audio(void)
{
	ClearAudioBuffers();
	DMA0->ERQ |= DMA_ERQ_ERQ0_MASK;			// this is fix to channel 0
}


void restart_Audio(void){
	Playing_Buff_A = TRUE;
	DMA0->TCD[DAC_DMA_CHANNEL].SADDR = (uint32_t) Audio_Source_Blk_A ;
	DMA0->ERQ |= DMA_ERQ_ERQ0_MASK;			// this is fix to channel 0
	gpfAudioDmaCb();				// Force Buffer B reload before A finish
}

/****************************************************************************/

/***************************************************************************//*!
* @function Stop_Audio
*
* @brief    Clears the buffers and disable DMA Audio channel feed
****************************************************************************/
void Stop_Audio( void)
{
	edma_tcd_t *tcdRegs;
	tcdRegs = (edma_tcd_t *)&g_EDMA_Audio_Handle.base->TCD[g_EDMA_Audio_Handle.channel];
	tcdRegs->SADDR = (uint32_t) Audio_Silence; // silence feed for DMA0
	ClearAudioBuffers();
	DMA0->ERQ &= ~DMA_ERQ_ERQ0_MASK;		// this is fix to channel 0
//	DMA0->ERQ &= ~DMA_ERQ_ERQ2_MASK;		// this is fix to channel 0
}
/****************************************************************************/


static void EDMA_AudioTransferInitTCD(DMA_Type *base, uint32_t channel, edma_tcd_t *tcd)
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

static void EDMA_ChannelTransferInit(DMA_Type *EDMAUsed, uint8_t channelNum, uint8_t source, edma_tcd_t *tcd)
{

	DMAMUX_DisableChannel(DMAMUX0, channelNum);
    DMAMUX_SetSource(DMAMUX0, channelNum, source);
    EDMA_AudioTransferInitTCD(EDMAUsed, channelNum, tcd);
    EDMA_SetModulo(EDMAUsed, channelNum, kEDMA_ModuloDisable, kEDMA_ModuloDisable);			// no circular addressing S&D
    EDMA_ClearChannelStatusFlags(EDMAUsed, channelNum, kEDMA_DoneFlag);
    EDMA_EnableChannelRequest(EDMAUsed, channelNum);
    DMAMUX_EnableChannel(DMAMUX0, channelNum);
}

/***************************************************************************//*!
* @function Set_Audio_Playback_Dma
*
* @brief    SetUp Audio DMA Playback, enable DMA major loop complete IRQ
*
* @note
****************************************************************************/
void Set_Audio_Playback_Dma(void)
{
//	edma_tcd_t *tcdRegs;
	edma_channel_Preemption_config_t 	audio_preemption_cfg;

	// Enable clock for DMAMUX and DMA
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
    /* Configure DMAMUX */
    DMAMUX_Init(DMAMUX0);
    DMAMUX0->CHCFG[DAC_DMA_CHANNEL] = DMAMUX_CHCFG_ENBL_MASK;


    /* Configure DMA */
    edma_config_t edmaConfig;
    EDMA_GetDefaultConfig(&edmaConfig);
    edmaConfig.enableDebugMode = true;					// DMA_CR = 0 | DMA_CR_EDBG_MASK // Stall DMA transfers when debugger is halted (avoid noise)
    EDMA_Init(DMA0, &edmaConfig);						// no minor loop mapping
    EDMA_CreateHandle(&g_EDMA_Audio_Handle, DMA0, DAC_DMA_CHANNEL);
    audio_preemption_cfg.channelPriority = 0;
    audio_preemption_cfg.enablePreemptAbility = false;
    audio_preemption_cfg.enableChannelPreemption = true;
    EDMA_SetChannelPreemptionConfig(DMA0, DAC_DMA_CHANNEL, &audio_preemption_cfg);
    EDMA_SetCallback(&g_EDMA_Audio_Handle, Audio_Handler, NULL);

    s_TcdMemoryPtrAudioToFrame->SADDR = (uint32_t) Audio_Source_Blk_A; 		// alternated with Audio_Source_Blk_B
    s_TcdMemoryPtrAudioToFrame->SOFF = 2;                              		// 2 byte offset
    s_TcdMemoryPtrAudioToFrame->ATTR = DMA_ATTR_SSIZE(kEDMA_TransferSize2Bytes) | DMA_ATTR_DSIZE(kEDMA_TransferSize2Bytes);   // 16 bit S&D
    s_TcdMemoryPtrAudioToFrame->NBYTES = 2;                              // one 16 bit sample every minor loop
    s_TcdMemoryPtrAudioToFrame->SLAST = 0;	//-(AUDIO_SIZE*2);         // source address will always be newly written before each new start  DMA_TCD0_DADDR
#if (AUDIO_DMA_OUTPUT == USE_PWM)
    s_TcdMemoryPtrAudioToFrame->DADDR = (uint32_t) &FTM2_C0V;    // the FTM Channel 0 duty value		! NOT PORTED TO K82F
#elif (AUDIO_DMA_OUTPUT == USE_I2S)
    s_TcdMemoryPtrAudioToFrame->DADDR = (uint32_t) &I2S0_TX0;    // the FTM Channel 0 duty value		! NOT PORTED TO K82F
#elif (AUDIO_DMA_OUTPUT == USE_DAC)
    s_TcdMemoryPtrAudioToFrame->DADDR = (uint32_t) &DAC0->DAT[0];    		// the FTM Channel 0 duty value			// I think before it was a number so they use &...
    s_TcdMemoryPtrAudioToFrame->DOFF = 0;
    s_TcdMemoryPtrAudioToFrame->CITER = AUDIO_SIZE;				// total samples (512)
    s_TcdMemoryPtrAudioToFrame->DLAST_SGA = 0;						// no final last adjustment ( does not move )
    s_TcdMemoryPtrAudioToFrame->CSR = DMA_CSR_INTMAJOR_MASK;   		// interrupt when done				//DMA_CSR_DREQ_MASK |
    s_TcdMemoryPtrAudioToFrame->BITER = AUDIO_SIZE;				// no chan links, total samples ( 512 )
#endif //DMA_OUTPUT

    // configure DMA_MUX to trigger DMA channel 2  with PIT channel 2
    EDMA_ChannelTransferInit(DMA0, DAC_DMA_CHANNEL, DAC_DMA_SOURCE, s_TcdMemoryPtrAudioToFrame);

    // now enable chan0 for HW triggers
//    DMA0->ERQ = DMA_ERQ_ERQ0_MASK ;


}

/***************************************************************************//*!
* @function  Init_PIT
*
* @brief     This function configures the PIT timer for trigger DMA at 8000Hz
*
* @note
****************************************************************************/

void Init_PIT(void){
    /* Set timer period for channel 0 */
    PIT_SetTimerPeriod(PIT, kPIT_Chnl_2, USEC_TO_COUNT(500U, PIT_SOURCE_CLOCK));
//    DisableIRQ(PIT_IRQ2_ID);
//    PIT_DisableInterrupts(PIT, kPIT_Chnl_2, kPIT_TimerInterruptEnable);
    EnableIRQ(PIT_IRQ2_ID);
    PIT_EnableInterrupts(PIT, kPIT_Chnl_2, kPIT_TimerInterruptEnable);
//    PIT_StartTimer(PIT, kPIT_Chnl_2);

}


/*	Handles PIT interrupt if enabled
 *
 * 	Starts conversion in ADC0 with single ended channel 8 (PTB0) as input
 *
 * */

void PIT2_IRQHandler(void)
{
	// Clear interrupt
	PIT_ClearStatusFlags(PIT, kPIT_Chnl_2, PIT_TFLG_TIF_MASK);
	PIT_StopTimer(PIT, kPIT_Chnl_2);
//	PIT_DisableInterrupts(PIT, kPIT_Chnl_2, kPIT_TimerInterruptEnable);
//	DisableIRQ(PIT_IRQ2_ID);
//	uint32_t primask = DisableGlobalIRQ();
	if(gpfAudioDmaCb)
		gpfAudioDmaCb();
//	EnableGlobalIRQ(primask);
}

/***************************************************************************//*!
* @function  Init_FTM2
*
* @brief     This function configures the flextimer to generate run a pwm
*            for the audio system We will aim for 8000Hz
* @note
****************************************************************************/

void Init_FTM2(void)
{
	/* Ungate the FTM clock*/
	CLOCK_EnableClock(kCLOCK_Ftm2);
  // PWM section not ported
#if (AUDIO_DMA_OUTPUT == USE_PWM)
	PORTB_PCR18  = PORT_PCR_MUX(3);         // FTM2_CH1 output PTB18   TWR-Elevator - B66
	//PORTB_PCR17  = PORT_PCR_MUX(3);         // FTM2_CH1 output PTB17   TWR-Elevator - B67
	FTM2_C0SC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK ;  // PWM, true high pulses
#endif

	FTM2->MODE |= FTM_MODE_WPDIS_MASK;
	FTM2->CONTROLS[1].CnSC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK | FTM_CnSC_CHIE_MASK | FTM_CnSC_DMA_MASK ;  // PWM, true high pulses - generates DMA req
	uint32_t BSP_BUS_CLOCK = CLOCK_GetFreq(kCLOCK_FlexBusClk);
	FTM2->MOD = (uint32_t)(BSP_BUS_CLOCK/(SAMPLERATE)); //  75MHz / 16000
//	FTM2->CONTROLS[0].CnV = 1100;                  // any initial random value
	FTM2->CONTROLS[1].CnV = 50;    // this value does NOT change -it is used to generate audio reload DMA request
	FTM2->SC |= FTM_SC_CLKS(0x01) | FTM_SC_PS(0x00); // | FTM_SC_TOIE_MASK;       // Bus Clock, divide-by-1
}
/****************************************************************************/


/***************************************************************************//*!
* @function  Set_VREF
*
* @brief     Sets the VREF module, light regulation without VREF_OUT signal
*
* @note      need to have also Vref enabled
****************************************************************************/
void Set_VREF(void)
{
	SIM->SCGC4 |= SIM_SCGC4_VREF_MASK ;     							// enable VRERF clock

	// VREF->TRM = 32;    // mid trim value  - trim register removed

	VREF->SC = VREF_SC_VREFEN_MASK | VREF_SC_REGEN_MASK ;              // enable the BANDGAP, and regulator, but remain in bandgap-only mode
	while ((VREF->SC & VREF_SC_VREFST_MASK) != VREF_SC_VREFST_MASK );  // wait for bandgap to be ready
	VREF->SC |= VREF_SC_MODE_LV(1);                                    // now switch to lowpower buffered mode

}


/***************************************************************************//*!
* @function  Init_DAC
*
* @brief     This function configures the DAC0, use VREF as ref
*
* @note      need to have also Vref enabled
****************************************************************************/
void Init_DAC(void)
{

	SIM->SCGC2 |= SIM_SCGC2_DAC0_MASK ; // | SIM_SCGC2_DAC1_MASK ;     // enable DAC clocks				CHECK PLEASE

// under reset conditions the DACs will use Vref as reference , high power mode, no FIFO (work in normal mode)
// to be tested : low power mode ( lower slew rate so lower high frequency content on output )


//	DAC0->C1 |= DAC_C1_DMAEN_MASK;

	DAC0->C0 |=  DAC_C0_DACEN_MASK;
	DAC0->C0 |=  DAC_C0_DACRFS_MASK;

	//DAC1->C0 |=  DAC_C0_DACEN_MASK ;           // DACs now enabled,

}

/***************************************************************************//*!
* @ User callback function for EDMA transfer.
*
* @brief    Swap the playback buffers and signal the app one of them is free
*
* @param    none
*
* @return   none
*
* @note
****************************************************************************/

void Audio_Handler(edma_handle_t *handle, void *param, bool transferDone, uint32_t tcds)
{
	EDMA_ClearChannelStatusFlags(DMA0, DAC_DMA_CHANNEL, kEDMA_InterruptFlag);
	if (Playing_Buff_A) {                        // finished playing buffer A
		Playing_Buff_A = FALSE;
		DMA0->TCD[DAC_DMA_CHANNEL].SADDR = (uint32_t) Audio_Source_Blk_B ;
	}
	else{
		Playing_Buff_A = TRUE;
		DMA0->TCD[DAC_DMA_CHANNEL].SADDR = (uint32_t) Audio_Source_Blk_A ;
	}

//	PIT_SetTimerPeriod(PIT, kPIT_Chnl_2, USEC_TO_COUNT(500U, PIT_SOURCE_CLOCK));
//    EnableIRQ(PIT_IRQ2_ID);
//    PIT_EnableInterrupts(PIT, kPIT_Chnl_2, kPIT_TimerInterruptEnable);
//    PIT_StartTimer(PIT, kPIT_Chnl_2);

	//This interrupt thing works but when FlexIO camera DMA enter the game it breaks everything.

	if(gpfAudioDmaCb)
		gpfAudioDmaCb();			// DMA finished playback an ready for a new buffer, inform upper layer



}

void FTM2_IRQHandler(void)
{
	FTM2->SC &= ~FTM_SC_TOF_MASK;
	if(FTM2->CONTROLS[1].CnSC & FTM_CnSC_CHF_MASK){
		// got interrupt of channel 1
		FTM2->CONTROLS[1].CnSC &= ~FTM_CnSC_CHF_MASK;
	}

}


/****************************************************************************/


void registerAudioCallback(tpfAudioDmaCb pfAudioDmaCb){
	gpfAudioDmaCb = pfAudioDmaCb;
}

