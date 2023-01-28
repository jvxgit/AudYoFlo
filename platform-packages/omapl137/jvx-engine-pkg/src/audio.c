/*
 * ==============================================
 * Filename: audio.c
 * ==============================================
 * Purpose: This file contains all functionality to
 *			the DKSDA830 extension board attached to
 *			the OMAP-L137 evaluation board (only DSP)
 *			The audio chip and the serial protocol is initialized.
 * 			Afterwards, audio I/O is completely DMA based, that,
 *			buffers of audio samples are copied to the DSP 
 * 			without using the CPU. Once the DMA trasnfer is complete, 
 *			a HWI is triggered. Arriving input buffers are transformed to
 *			float format and copied to one out of two buffers (Two buffers are needed
 * 			since there is a temporal offset between input and output DMA transfer completion).
 *			Once the DMA is complete for output, the HWI service routine is used to
 *			map this event to a software interrupt. In the software interrupt,
 *			the input buffers are processed in the user routine to produce output samples
 *			Once finished, the float format output samples are converted into 24 bit fixed point format.
 * 			If the transform has been completed before the next DMA buffer complete interrupt occurs,
 * 			real-time constraints are fulfilled. 
 *			Note that input and output DMA buffers are double buffers: While the DMA transfer fills/
 *			reads samples in one half of the double-buffer, the other half is available for the user.
 * 			Channels are mapped to the user buffers as defined in the config file user_config.h. However,
 * 			always all channels are moved in the DMA transfers snce I have not succeded to
 * 			use only part of teh McAsp serializers.
 * 			 
 * 			COPYRIGHT HK, Javox, 2011
 */
 
// Some DSP Bios references

#include "dskda830.h"
#include "dskda830_edma3.h"
#include "dskda830_mcasp.h"
#include "systemConfig.h"
#include "math.h"

extern void postSwi_audio();
extern void postSwi_uarttrans();


// ===============================================
// Switch on and off some additional debugging features realized in code
// ===============================================
//#define ADDITIONAL_DEBUGGING
// ===============================================

static MCASP_Handle mcasp;

// ===========================================================================
// Algorithm User API
#include "user_audioProcessing.h"

// ===========================================================================
// ===========================================================================

// ===========================================================================
// Extern functions from board support packages developed by Spectrum Digital
// File dc_audio.c
// ===========================================================================
extern void ak4588_init( );
extern Int16 ak4588_rset(Uint16 id, Uint16 regnum, Uint16 regval);
extern Int16 ak4588_rget(Uint16 id, Uint16 regnum, Uint16* regval);
// ===========================================================================

// Define PI 
#define M_PI 3.141592654

// Every input sample is 32 bit integer. This normalization factor
// normalizes to the range -1..1.0
#define NORM_FLT_32BIT ((float)1.0/(float)0x80000000)

// Every input sample is 32 bit integer. This normalization factor
// denormalizes the range -1..1.0 to fill 32 bit integers
#define DENORM_FLT_32BIT ((float)0x80000000)

// =============================================
// Struct to store some data to derive the current audio load
// (does not use bios)
// =============================================
static struct 
{
	int sampleCountOnSwiStart;
	int sampleCountOnSwitStop;
	int bufferMissedCounter;
} theStat;
// =============================================

// =============================================
// Struct to manage the buffer selection in SW interrupt
// service routine
// =============================================
static struct 
{
	int idxWriteToDMA;
	int idxReadFromSoftBuffer;
	int isBeingServed;
} theStructSwitch;


// ==================================================================
// ALL BUFFERS STORED IN MEMORY
// ==================================================================

// DMA transfer buffers used in DMA cycling 
Int32 dma_output[NUM_SERIALIZERS*NUM_CHANNELS_PER_SERIALIZER*AUDIO_BUFFERSIZE*2];
Int32 dma_input[NUM_SERIALIZERS*NUM_CHANNELS_PER_SERIALIZER*AUDIO_BUFFERSIZE*2];

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
// Allocate buffers for software side, double input buffering due to input/output delay
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
#if (defined(INPUT_CHANNELSET_1_4))

float buf_cs1_in_left[AUDIO_BUFFERSIZE * 2];
float buf_cs1_in_right[AUDIO_BUFFERSIZE * 2];
float buf_cs2_in_left[AUDIO_BUFFERSIZE * 2];
float buf_cs2_in_right[AUDIO_BUFFERSIZE * 2];
float buf_cs3_in_left[AUDIO_BUFFERSIZE * 2];
float buf_cs3_in_right[AUDIO_BUFFERSIZE * 2];
float buf_cs4_in_left[AUDIO_BUFFERSIZE * 2];
float buf_cs4_in_right[AUDIO_BUFFERSIZE * 2];
float buf_cs1_out_left[AUDIO_BUFFERSIZE];
float buf_cs1_out_right[AUDIO_BUFFERSIZE];
float buf_cs2_out_left[AUDIO_BUFFERSIZE];
float buf_cs2_out_right[AUDIO_BUFFERSIZE];
float buf_cs3_out_left[AUDIO_BUFFERSIZE];
float buf_cs3_out_right[AUDIO_BUFFERSIZE];
float buf_cs4_out_left[AUDIO_BUFFERSIZE];
float buf_cs4_out_right[AUDIO_BUFFERSIZE];

// Group input channels in one struct
float* bufsIn_0[NUM_INPUT_CHANNELS_AUDIO] = 
{
	buf_cs1_in_left,
	buf_cs1_in_right,
	buf_cs2_in_left,
	buf_cs2_in_right,
	buf_cs3_in_left,
	buf_cs3_in_right,
	buf_cs4_in_left,
	buf_cs4_in_right
};
	
// Group input channels in one struct
float* bufsIn_1[NUM_INPUT_CHANNELS_AUDIO] = 
{
	buf_cs1_in_left + AUDIO_BUFFERSIZE,
	buf_cs1_in_right + AUDIO_BUFFERSIZE,
	buf_cs2_in_left + AUDIO_BUFFERSIZE,
	buf_cs2_in_right + AUDIO_BUFFERSIZE,
	buf_cs3_in_left + AUDIO_BUFFERSIZE,
	buf_cs3_in_right + AUDIO_BUFFERSIZE,
	buf_cs4_in_left + AUDIO_BUFFERSIZE,
	buf_cs4_in_right + AUDIO_BUFFERSIZE
};

// Group input channel structs in one meta-struct
float** bufsIn[2] =
{
	bufsIn_0,
	bufsIn_1
};

#elif (defined(INPUT_CHANNELSET_1_3))

float buf_cs1_in_left[AUDIO_BUFFERSIZE * 2];
float buf_cs1_in_right[AUDIO_BUFFERSIZE * 2];
float buf_cs2_in_left[AUDIO_BUFFERSIZE * 2];
float buf_cs2_in_right[AUDIO_BUFFERSIZE * 2];
float buf_cs3_in_left[AUDIO_BUFFERSIZE * 2];
float buf_cs3_in_right[AUDIO_BUFFERSIZE * 2];
float buf_cs1_out_left[AUDIO_BUFFERSIZE];
float buf_cs1_out_right[AUDIO_BUFFERSIZE];
float buf_cs2_out_left[AUDIO_BUFFERSIZE];
float buf_cs2_out_right[AUDIO_BUFFERSIZE];
float buf_cs3_out_left[AUDIO_BUFFERSIZE];
float buf_cs3_out_right[AUDIO_BUFFERSIZE];

// Group input channels in one struct
float* bufsIn_0[NUM_INPUT_CHANNELS_AUDIO] = 
{
	buf_cs1_in_left,
	buf_cs1_in_right,
	buf_cs2_in_left,
	buf_cs2_in_right,
	buf_cs3_in_left,
	buf_cs3_in_right,
};
	
// Group input channels in one struct
float* bufsIn_1[NUM_INPUT_CHANNELS_AUDIO] = 
{
	buf_cs1_in_left + AUDIO_BUFFERSIZE,
	buf_cs1_in_right + AUDIO_BUFFERSIZE,
	buf_cs2_in_left + AUDIO_BUFFERSIZE,
	buf_cs2_in_right + AUDIO_BUFFERSIZE,
	buf_cs3_in_left + AUDIO_BUFFERSIZE,
	buf_cs3_in_right + AUDIO_BUFFERSIZE,
};

// Group input channel structs in one meta-struct
float** bufsIn[2] =
{
	bufsIn_0,
	bufsIn_1
};

#elif (defined(INPUT_CHANNELSET_1_2))

float buf_cs1_in_left[AUDIO_BUFFERSIZE * 2];
float buf_cs1_in_right[AUDIO_BUFFERSIZE * 2];
float buf_cs2_in_left[AUDIO_BUFFERSIZE * 2];
float buf_cs2_in_right[AUDIO_BUFFERSIZE * 2];
float buf_cs1_out_left[AUDIO_BUFFERSIZE];
float buf_cs1_out_right[AUDIO_BUFFERSIZE];
float buf_cs2_out_left[AUDIO_BUFFERSIZE];
float buf_cs2_out_right[AUDIO_BUFFERSIZE];

// Group input channels in one struct
float* bufsIn_0[NUM_INPUT_CHANNELS_AUDIO] = 
{
	buf_cs1_in_left,
	buf_cs1_in_right,
	buf_cs2_in_left,
	buf_cs2_in_right,
};
	
// Group input channels in one struct
float* bufsIn_1[NUM_INPUT_CHANNELS_AUDIO] = 
{
	buf_cs1_in_left + AUDIO_BUFFERSIZE,
	buf_cs1_in_right + AUDIO_BUFFERSIZE,
	buf_cs2_in_left + AUDIO_BUFFERSIZE,
	buf_cs2_in_right + AUDIO_BUFFERSIZE,
};

// Group input channel structs in one meta-struct
float** bufsIn[2] =
{
	bufsIn_0,
	bufsIn_1
};

#else

float buf_cs1_in_left[AUDIO_BUFFERSIZE * 2];
float buf_cs1_in_right[AUDIO_BUFFERSIZE * 2];
float buf_cs1_out_left[AUDIO_BUFFERSIZE];
float buf_cs1_out_right[AUDIO_BUFFERSIZE];

// Group input channels in one struct
float* bufsIn_0[NUM_INPUT_CHANNELS_AUDIO] = 
{
	buf_cs1_in_left,
	buf_cs1_in_right,
};
	
// Group input channels in one struct
float* bufsIn_1[NUM_INPUT_CHANNELS_AUDIO] = 
{
	buf_cs1_in_left + AUDIO_BUFFERSIZE,
	buf_cs1_in_right + AUDIO_BUFFERSIZE,
};

// Group input channel structs in one meta-struct
float** bufsIn[2] =
{
	bufsIn_0,
	bufsIn_1
};

#endif

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
// Allocate buffers for software side, output
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
#if (defined(OUTPUT_CHANNELSET_1_4))

// Allocate the buffers
float buf_cs1_out_left[AUDIO_BUFFERSIZE];
float buf_cs1_out_right[AUDIO_BUFFERSIZE];
float buf_cs2_out_left[AUDIO_BUFFERSIZE];
float buf_cs2_out_right[AUDIO_BUFFERSIZE];
float buf_cs3_out_left[AUDIO_BUFFERSIZE];
float buf_cs3_out_right[AUDIO_BUFFERSIZE];
float buf_cs4_out_left[AUDIO_BUFFERSIZE];
float buf_cs4_out_right[AUDIO_BUFFERSIZE];

// Group the buffers in one struct
float* bufsOut[NUM_OUTPUT_CHANNELS_AUDIO] = 
{
	buf_cs1_out_left,
	buf_cs1_out_right,
	buf_cs2_out_left,
	buf_cs2_out_right,
	buf_cs3_out_left,
	buf_cs3_out_right,
	buf_cs4_out_left,
	buf_cs4_out_right
};

#elif (defined(OUTPUT_CHANNELSET_1_3))

// Allocate the buffers
float buf_cs1_out_left[AUDIO_BUFFERSIZE];
float buf_cs1_out_right[AUDIO_BUFFERSIZE];
float buf_cs2_out_left[AUDIO_BUFFERSIZE];
float buf_cs2_out_right[AUDIO_BUFFERSIZE];
float buf_cs3_out_left[AUDIO_BUFFERSIZE];
float buf_cs3_out_right[AUDIO_BUFFERSIZE];

// Group the buffers in one struct
float* bufsOut[NUM_OUTPUT_CHANNELS_AUDIO] = 
{
	buf_cs1_out_left,
	buf_cs1_out_right,
	buf_cs2_out_left,
	buf_cs2_out_right,
	buf_cs3_out_left,
	buf_cs3_out_right,
};

#elif (defined(OUTPUT_CHANNELSET_1_3))

// Allocate the buffers
float buf_cs1_out_left[AUDIO_BUFFERSIZE];
float buf_cs1_out_right[AUDIO_BUFFERSIZE];
float buf_cs2_out_left[AUDIO_BUFFERSIZE];
float buf_cs2_out_right[AUDIO_BUFFERSIZE];

// Group the buffers in one struct
float* bufsOut[NUM_OUTPUT_CHANNELS_AUDIO] = 
{
	buf_cs1_out_left,
	buf_cs1_out_right,
	buf_cs2_out_left,
	buf_cs2_out_right,
};

#else 

// Allocate the buffers
float buf_cs1_out_left[AUDIO_BUFFERSIZE];
float buf_cs1_out_right[AUDIO_BUFFERSIZE];

// Group the buffers in one struct
float* bufsOut[NUM_OUTPUT_CHANNELS_AUDIO] = 
{
	buf_cs1_out_left,
	buf_cs1_out_right,
};

#endif

// ALL BUFFERS END ALL BUFFERS END ALL BUFFERS END
//============================================================== 

//============================================================== 
// Control the two buffers for input which we need for high 
// performance
//============================================================== 
static struct
{
	int idBufIn_dma;
	int idBufIn_soft;
} inputBufferControl = { 0,1 };
//============================================================== 

//============================================================== 
// paRAM stuct pointers for DMA cycling
//============================================================== 
static struct
{
	edma_paRAM* paramX;
	edma_paRAM* paramR;
} dmaControl = {0,0};
//============================================================== 


#ifdef ADDITIONAL_DEBUGGING

//============================================================== 
// Misc variables
//============================================================== 
static struct 
{
	float factor_cpuCyclesPerHtime;
} miscData;
//============================================================== 

// Reference timing to measure from one frame to the next (to 
// compute the samplingrate)
static LgUns tStampRef = 0;

// Buffer to store timestamps
static struct
{
	LgUns oneTimestamp;
	LgUns oneTimestampDiff;
	Int32 eventType; // 1: X IRQ, 2: R IRQ, 3: SWI-Enter, 4: SWI-Leave
} allTimeStamps[1024];
static int cntTimeStamp = 0; 	

// =====================================================

// DMA debugging
typedef struct
{
	Uint32 idxUseBuffer;
	Uint32 idxUseSample;
	Uint32 idxUseSample_cross;
	Uint32 iprOnEnter;
} oneField;

// Debug buffers
oneField dmaSave_receive[1024];
Uint32 cntDma_receive = 0;

// Debug buffers
oneField dmaSave_transmit[1024];
Uint32 cntDma_transmit = 0;

// =====================================================


#endif

//Uint32 fldDMATest[NUM_SERIALIZERS*NUM_CHANNELS_PER_SERIALIZER*AUDIO_BUFFERSIZE*2];

// ======================================================
// Runtime memory for handling audio I/O.
// userData is to be allocated by the user and might be
// in every kind of memory.
// ======================================================
static struct
{
	// User handle data
	void* userData;
} theProcessingData;
// ======================================================

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
// END OF VARIABLES END OF VARIABLES END OF VARIABLES
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

/**
 * Function to init all buffers, that is, set all memory spaces to zero.
 *///==================================================================
Int16 
audio_initBuffers()
{
	int i,j,k;
	int idx = 0;

	// Init the buffers for DMA
	for(i = 0; i < AUDIO_BUFFERSIZE*2; i++)
	{
		for(j = 0; j < NUM_CHANNELS_PER_SERIALIZER; j++)
		{
			for(k = 0; k < NUM_SERIALIZERS; k++)
			{
				dma_input[idx] = 0;
				dma_output[idx++] = 0;
			}
		}
	}

	// Init the software buffer 
	for(i = 0; i < AUDIO_BUFFERSIZE; i++)
	{

#if (defined(OUTPUT_CHANNELSET_1_4))
		buf_cs1_out_left[i] = 0.0;
		buf_cs1_out_right[i] = 0.0;
		buf_cs2_out_left[i] = 0.0;
		buf_cs2_out_right[i] = 0.0;
		buf_cs3_out_left[i] = 0.0;
		buf_cs3_out_right[i] = 0.0;
		buf_cs4_out_left[i] = 0.0;
		buf_cs4_out_right[i] = 0.0;
#elif (defined(OUTPUT_CHANNELSET_1_3))
		buf_cs1_out_left[i] = 0.0;
		buf_cs1_out_right[i] = 0.0;
		buf_cs2_out_left[i] = 0.0;
		buf_cs2_out_right[i] = 0.0;
		buf_cs3_out_left[i] = 0.0;
		buf_cs3_out_right[i] = 0.0;
#elif (defined(OUTPUT_CHANNELSET_1_4))
		buf_cs1_out_left[i] = 0.0;
		buf_cs1_out_right[i] = 0.0;
		buf_cs2_out_left[i] = 0.0;
		buf_cs2_out_right[i] = 0.0;
#else
		buf_cs1_out_left[i] = 0.0;
		buf_cs1_out_right[i] = 0.0;
#endif
	}
	
	for(i = 0; i < 2*AUDIO_BUFFERSIZE; i++)
	{
#if (defined(INPUT_CHANNELSET_1_4))
		buf_cs1_in_left[i] = 0.0;
		buf_cs1_in_right[i] = 0.0;
		buf_cs2_in_left[i] = 0.0;
		buf_cs2_in_right[i] = 0.0;
		buf_cs3_int_left[i] = 0.0;
		buf_cs3_in_right[i] = 0.0;
		buf_cs4_in_left[i] = 0.0;
		buf_cs4_in_right[i] = 0.0;
#elif (defined(OUTPUT_CHANNELSET_1_3))
		buf_cs1_in_left[i] = 0.0;
		buf_cs1_in_right[i] = 0.0;
		buf_cs2_in_left[i] = 0.0;
		buf_cs2_in_right[i] = 0.0;
		buf_cs3_int_left[i] = 0.0;
		buf_cs3_in_right[i] = 0.0;
#elif (defined(OUTPUT_CHANNELSET_1_4))
		buf_cs1_in_left[i] = 0.0;
		buf_cs1_in_right[i] = 0.0;
		buf_cs2_in_left[i] = 0.0;
		buf_cs2_in_right[i] = 0.0;
#else
		buf_cs1_in_left[i] = 0.0;
		buf_cs1_in_right[i] = 0.0;
#endif
	}

	theStat.sampleCountOnSwiStart = 0;
	theStat.sampleCountOnSwitStop = 0;
	theStat.bufferMissedCounter = 0;

#ifdef ADDITIONAL_DEBUGGING
	miscData.factor_cpuCyclesPerHtime = -1;
#endif

	// ===============================================
	// Call user prepare function
	// ===============================================
	theProcessingData.userData = 0;
	user_prepareProcessing(&theProcessingData.userData);
	// ===============================================

	return(0);
}
		
/**
 * Prepare all data and registers for the transmit and the receive DMA streams.
 * All DMA traffic is setup manually. One might think about using the LLC library once
 *///==============================================================================
void
initDMATransfers()
{
	int i,j,k;
	int idx = 0;

	edma_paRAM* paramX1 = 0;
	edma_paRAM* paramX2 = 0;

	edma_paRAM* paramR1 = 0;
	edma_paRAM* paramR2 = 0;

	// Set the dma buffers to zero
	for(i = 0; i < AUDIO_BUFFERSIZE*2; i++)
	{
		for(j = 0; j < NUM_CHANNELS_PER_SERIALIZER; j++)
		{
			for(k = 0; k < NUM_SERIALIZERS; k++)
			{
				dma_output[idx++] = 0;
			}
		}
	}

	// Enable the region 1 DMA access for McAsp transmit and receive
	EDMA3CC_DRAE1 = 0;

#ifdef USE_AUDIO

	EDMA3CC_DRAE1 = (1 << X_DMA_NUM_S0) | (1 << R_DMA_NUM_S0); // Enable event ER-X_DMA_NUM_S0 

	// Clear SER if set (normally, this will not be set)
	EDMA3CC_REGION1_SECR = (1 << X_DMA_NUM_S0)| (1 << R_DMA_NUM_S0);

	// Enable the interrupt reporting for McAsp trasnmit and receive
	EDMA3CC_REGION1_IESR =  (1 << X_DMA_NUM_S0) | (1 << R_DMA_NUM_S0); // Enable interrupt X_DMA_NUM_S0
	//EDMA3CC_REGION1_IESR =  (1 << R_DMA_NUM_S0); // Enable interrupt X_DMA_NUM_S0

	// Setup the paRAM structs for transmit
	dmaControl.paramX = EDMA3_PARAMS_RAM;
	paramX1 = EDMA3_PARAMS_RAM;
	paramX2 = EDMA3_PARAMS_RAM;
	
	// Setup the paRAM structs for receive
	dmaControl.paramR = EDMA3_PARAMS_RAM;
	paramR1 = EDMA3_PARAMS_RAM;
	paramR2 = EDMA3_PARAMS_RAM;

	// ====================================
	// First, the transmit side
	// ====================================

	// McAsp1: Transmit event is routed to channel 3
	dmaControl.paramX += X_DMA_NUM_S0;
	paramX1 += X_DMA_NUM_S1;
	paramX2 += X_DMA_NUM_S2;

	// IRQ triggered for DMA transfer complete event
	dmaControl.paramX->opt = 0x00100000 | (X_DMA_NUM_S0 << 12) | (1 << 2); // ITCCHEN, TCCHEN, ITCINTEN = 0, TCINTEN = 1, TCC = 3 (as DMA channel), TCCMODE = 0, FWID = 0, STATIC = 0, SYNCDIM = 1 (AB), DAM, SAM = 0

	// Set the memory to stream data to DMA output
	dmaControl.paramX->src = (Uint32)dma_output;//fldDMA; // Set the DMA field address pointer

	// One event means to feed data for 4 serializers, for each 4 byte
	dmaControl.paramX->a_b_cnt = (4 << 16) | (4);

	// Destination address: Transfer buffer register
	dmaControl.paramX->dst = MCASP1_XBUF_PTR;

	// On memory side, move pointer forward, on McAsp, do not move it
	dmaControl.paramX->src_dst_bidx = (0 << 16) | 4;

	// New link address: Link to self, always reload B to 4
	dmaControl.paramX->link_bcntrld = (4 << 16) | (((Uint32)paramX1) & 0xFFFF);

	// Shift forward the source address by 4 a-frames (for each serializer), do not shift forward the destnation address 
	dmaControl.paramX->src_dst_cidx = (0 << 16) | (NUM_SERIALIZERS * NUM_BYTES_SAMPLE);

	// Stop data transfer after each complete frame
	dmaControl.paramX->ccnt = AUDIO_BUFFERSIZE * NUM_CHANNELS_PER_SERIALIZER;

	// Set the two other paRAM struct to loop forever
	memcpy(paramX1, dmaControl.paramX, sizeof(edma_paRAM));
	memcpy(paramX2, dmaControl.paramX, sizeof(edma_paRAM));

	// Associate second half DMA buffer with second paRAM
	paramX1->src += NUM_SERIALIZERS*NUM_CHANNELS_PER_SERIALIZER*AUDIO_BUFFERSIZE * NUM_BYTES_SAMPLE;

	// Set address to following paRAM
	paramX1->link_bcntrld = (4 << 16) | (((Uint32)paramX2) & 0xFFFF);

	// ====================================
	// Second, the receive side
	// ====================================

	// McAsp1: Receive event is routed to channel 2

	// Compute pointers to paRAM structs
	dmaControl.paramR += R_DMA_NUM_S0;
	paramR1 += R_DMA_NUM_S1;
	paramR2 += R_DMA_NUM_S2;

	// Set interrupt to trigger after complete frame
	dmaControl.paramR->opt = 0x00100000 | (R_DMA_NUM_S0 << 12) | (1 << 2); // ITCCHEN, TCCHEN, ITCINTEN = 0, TCINTEN = 1, TCC = 3 (as DMA channel), TCCMODE = 0, FWID = 0, STATIC = 0, SYNCDIM = 1 (AB), DAM, SAM = 0

	// Pointer to destination memory region
	dmaControl.paramR->dst = (Uint32)dma_input;// Set the DMA field address pointer

	// One event means to feed data for 4 serializers, for each 4 byte
	dmaControl.paramR->a_b_cnt = (4 << 16) | (4);

	// Destination address: Transfer buffer register
	dmaControl.paramR->src = MCASP1_RBUF_PTR;

	// All output from McAsp by reading from one register
	dmaControl.paramR->src_dst_bidx = (4 << 16) | 0;

	// New link address: Link to self, always reload B to 4
	dmaControl.paramR->link_bcntrld = (4 << 16) | (((Uint32)paramR1) & 0xFFFF);

	// Shift forward the source address by 4 a-frames (for each serializer), do not shift forward the destnation address 
	dmaControl.paramR->src_dst_cidx = ((NUM_SERIALIZERS * NUM_BYTES_SAMPLE) << 16) | 0;

	//paramX->src_dst_cidx = (0 << 16) | (NUM_SERIALIZERS * NUM_BYTES_SAMPLE);
	dmaControl.paramR->ccnt = AUDIO_BUFFERSIZE * NUM_CHANNELS_PER_SERIALIZER;

	// Prepare two other paRAM
	memcpy(paramR1, dmaControl.paramR, sizeof(edma_paRAM));
	memcpy(paramR2, dmaControl.paramR, sizeof(edma_paRAM));

	// Set second paRAM to seconde half of DMA buffer
	paramR1->dst += NUM_SERIALIZERS*NUM_CHANNELS_PER_SERIALIZER*AUDIO_BUFFERSIZE * NUM_BYTES_SAMPLE;

	// Link paRAM structs
	paramR1->link_bcntrld = (4 << 16) | (((Uint32)paramR2) & 0xFFFF);

	// ====================================================
	// Enable the events: receive and transmit
	// ====================================================
	EDMA3CC_REGION1_EESR = (1 << X_DMA_NUM_S0)| (1 << R_DMA_NUM_S0);
	//EDMA3CC_REGION1_EESR = (1 << X_DMA_NUM_S0)| (1 << R_DMA_NUM_S0);
#endif
}

/**
 *
 * Prepare audio processing. We have mostly copied the code from Spectrum Digital to
 * configure the ak4588.
 * Note here some information not given by Spectrum Digital:
 * 1) The ADCs are instances of the chip type PCM1802. The PCM1802 are operated in slace mode and
 * 	  are configured by hardware wires (set the pins to "1" or "0")
 * 2) The DACs are one instance of type ak4588. The docs for ak4588 are horrible and Spectrum
 *    Digital does not comment a word on what is intended to be done. The ak4588 is setup via SPI interface in 4-wire
 *    mode. Close to the end of the ak4588 docs, there is an explanation of the expected bits in 4-wire mode which matches the 
 *    ak4588_rset and ak4588_rget functions.
 * 3) The McAsp1 is used to stream the audio samples to the ak4588 via serial protocol. We use four serializers (4 stereo inputs/outputs)
 *    in a two-slot mode (left & right).
 *///============================================================================  
Int16 
audio_prepareAudio()
{
	int i;
//	Uint32 tmpXSTAT = 0;
//	Uint32 tmpRSTAT = 0;
//	Uint32 rfifosts = 0;
	Uint32 dummy = 0;
 	Uint16 dummy16 = 0;

    /* Initialize SPI and codec */
    ak4588_init( );

    /* Configure AK4588 registers */
	// There are two register maps, addressed by the first argument to this function,
	// the id=1 is the first register map from the ak4588 docs whereas
	// the id=0 is the second register map 
    ak4588_rset(1,0,0x0C);    // 24-bit I2S mode
    ak4588_rset(1,1,0x80);
	ak4588_rset(1,9,0x01);    // Take AK4588 DAC out of reset
#ifdef OUTPUT_ONLY
	ak4588_rset(0,0,0x00);    // Reset DIR
#endif
    ak4588_rset(0,0,0x7B);    // Double AHCLKX1 frequency

	// Set the deemphasis filter
#ifdef SAMPLERATE_32000
	ak4588_rset(0,1,0x66);
#else 
#ifdef SAMPLERATE_48000
	ak4588_rset(0,1,0x64);
#else
#error "No samplerate specified, to be edited in user_config.h"
#endif
#endif
	ak4588_rget(0,1,&dummy16);

	// Some additional modifications of the registers on the ak4588,
	// mostly for testing
    ak4588_rset(1,2,0x00);    // Set channel volume
    ak4588_rset(1,3,0x00);    // Set channel volume
    ak4588_rset(1,4,0x00);    // Set channel volume
    ak4588_rset(1,5,0x00);    // Set channel volume
    ak4588_rset(1,6,0x00);    // Set channel volume
    ak4588_rset(1,7,0x00);    // Set channel volume

	// Get some information about the IRQ0 and IRQ1 bit masks
	/*
	ak4588_rget(0, 4, &maskIrq0);// Get the    IRQ0    error  mask   
	ak4588_rget(0, 5, &maskIrq1);// Get the    IRQ0    error  mask   
	ak4588_rget(0, 6, &recStat0);// Get the receiver status register 0
	ak4588_rget(0, 7, &recStat1);// Get the receiver status register 1
	*/

    /* Initialize MCASP1 */
	PINMUX11 = PINMUX11 | 0x11100000;  // Set pins for their McASP functions
    mcasp = &MCASP_MODULE_1;           // Choose McASP1

    /* ---------------------------------------------------------------- *
     *                                                                  *
     *  McASP is in MASTER mode.                                        *
     *      BCLK & WCLK come from McASP                                 *
     *                                                                  *
     * ---------------------------------------------------------------- */
    mcasp->regs->GBLCTL  = 0;       // Reset
    mcasp->regs->RGBLCTL = 0;       // Reset RX
    mcasp->regs->XGBLCTL = 0;       // Reset TX
    mcasp->regs->PWRDEMU = 1;       // Free-running

	//===============================================================
	// If desired start the FIFOs
	//===============================================================

	// Setup write FIFO
	MCASP1_WFIFOCTL = 0x00010000 | (4 << 8) | (4); // Set bit 16 to enable FIFO and set the number of serializers to 4

	// Setup read FIFO
	MCASP1_RFIFOCTL = 0x00010000  | (4 << 8) | (4); // Set bit 16 to enable FIFO and set the number of serializers to 4

	// This somehow is necessary to start audio...
	while(MCASP1_RFIFOSTS)
	{
		dummy = MCASP1_RBUF;
		dummy++; // <- avoid warning from compiler
	}

	//===============================================================
    // Setup the input side (McAsp receiver serializers)
	//===============================================================

    mcasp->regs->RMASK      = 0xffffffff; // No padding used
    mcasp->regs->RFMT       = 0x000180f0; // MSB 32bit, 1-delay, no pad, DATABus
    mcasp->regs->AFSRCTL    = 0x00000112; // 2TDM, Rising, INTERNAL FS, word
#ifdef SAMPLERATE_32000
    mcasp->regs->ACLKRCTL   = 0x000000AB; // Rising INTRERNAL CLK, div-by-12
#else 
#ifdef SAMPLERATE_48000
    mcasp->regs->ACLKRCTL   = 0x000000A7; // Rising INTRERNAL CLK, div-by-12
#else
#error "No samplerate specified, to be edited in user_config.h"
#endif
#endif
	// Important note here: This is where the samplerate is setup: the value in bit
	// 4..0 is (divisor N - 1). If you setup 3 here, this means that the divisor is
	// 4. I did not see that for a while..
	// What we compute here is: 24.765 MHz / 64 / N = 48000 for N = 8 and 32000 for N = 12
	// The 64 comes from the fact that we have two time slots and 32 bit words on the serial 
	// output line
    mcasp->regs->AHCLKRCTL  = 0x00000000; // EXTERNAL CLK
    mcasp->regs->RTDM       = 0x00000003; // Slots 0,1
    mcasp->regs->RINTCTL    = 0x00000000; // Not used
	MCASP1_REVTCTL  	= 0x00000000;
	//mcasp->regs->RINTCTL    = 0x00000020; // Enable data available interrupt

    mcasp->regs->RCLKCHK    = 0x00FF0008; // 255-MAX 0-MIN, div-by-256

	//===============================================================
    // Setup the output side (McAsp transmit serializers)
	//===============================================================
    mcasp->regs->XMASK      = 0xffffffff; // No padding used

    mcasp->regs->XFMT       = 0x000180f0; // MSB 32bit, 1-delay, no pad, DATABus
    mcasp->regs->AFSXCTL    = 0x00000112; // 2TDM, Rising edge INTERNAL FS, word
 #ifdef SAMPLERATE_32000
    mcasp->regs->ACLKXCTL   = 0x000000EB; // ASYNC, Rising INTERNAL CLK, div-by-12
#else 
#ifdef SAMPLERATE_48000
    mcasp->regs->ACLKXCTL   = 0x000000E7; // ASYNC, Rising INTERNAL CLK, div-by-12
#else
#error "No samplerate specified, to be edited in user_config.h"
#endif
#endif
	// Important note here: This is where the samplerate is setup: the value in bit
	// 4..0 is (divisor N - 1). If you setup 3 here, this means that the divisor is
	// 4. I did not see that for a while..
	// What we compute here is: 24.765 MHz / 64 / N = 48000 for N = 8 and 32000 for N = 12
	// The 64 comes from the fact that we have two time slots and 32 bit words on the serial 
	// output line
    mcasp->regs->AHCLKXCTL  = 0x00000000; // EXTERNAL CLK
    mcasp->regs->XTDM       = 0x00000003; // Slots 0,1
    mcasp->regs->XINTCTL    = 0x00000000; // Not used
	MCASP1_XEVTCTL  	= 0x00000000;
    mcasp->regs->XCLKCHK    = 0x00FF0008; // 255-MAX 0-MIN, div-by-256
   
    /* Outputs to DAC */
    mcasp->regs->SRCTL5     = 0x000D;     // MCASP1.AXR1[5] --> SDTI1 [Rb/Lb]
    mcasp->regs->SRCTL6     = 0x000D;     // MCASP1.AXR1[6] --> SDTI2 [SW/CTR]
    mcasp->regs->SRCTL7     = 0x000D;     // MCASP1.AXR1[7] --> SDTI3 [Rs/Ls]
    mcasp->regs->SRCTL8     = 0x000D;     // MCASP1.AXR1[8] --> SDTI4 [Rf/Lf]
    /* Inputs from ADC */

    mcasp->regs->SRCTL0     = 0x000E;     // MCASP1.AXR1[0] <-- DOUT [0:1]
    mcasp->regs->SRCTL1     = 0x000E;     // MCASP1.AXR1[1] <-- DOUT [2:3]
    mcasp->regs->SRCTL2     = 0x000E;     // MCASP1.AXR1[2] <-- DOUT [4:5]
    mcasp->regs->SRCTL10    = 0x000E;     // MCASP1.AXR1[10]<-- DOUT [6:7]

    mcasp->regs->PFUNC      = 0;          // All MCASPs
    mcasp->regs->PDIR       = 0xB40001E0; // All inputs except AXR1[5], AXR1[6], AXR1[7], AXR1[8], ACLKX1, AFSX1

    mcasp->regs->DITCTL     = 0x00000000; // Not used
    mcasp->regs->DLBCTL     = 0x00000000; // Not used
    mcasp->regs->AMUTE      = 0x00000000; // Not used


    /* Starting sections of the McASP*/
    mcasp->regs->XGBLCTL |= GBLCTL_XHCLKRST_ON;                                    // HS Clk
    while ( ( mcasp->regs->XGBLCTL & GBLCTL_XHCLKRST_ON ) != GBLCTL_XHCLKRST_ON );  

    mcasp->regs->RGBLCTL |= GBLCTL_RHCLKRST_ON;                                    // HS Clk
    while ( ( mcasp->regs->RGBLCTL & GBLCTL_RHCLKRST_ON ) != GBLCTL_RHCLKRST_ON );
   
    mcasp->regs->XGBLCTL |= GBLCTL_XCLKRST_ON;                                     // Clk
    while ( ( mcasp->regs->XGBLCTL & GBLCTL_XCLKRST_ON ) != GBLCTL_XCLKRST_ON );

    mcasp->regs->RGBLCTL |= GBLCTL_RCLKRST_ON;                                     // Clk
    while ( ( mcasp->regs->RGBLCTL & GBLCTL_RCLKRST_ON ) != GBLCTL_RCLKRST_ON );

	// ==========================================================
	// Init the DMA transfers
	// ==========================================================
	initDMATransfers();
	//rfifosts = MCASP1_RFIFOSTS;
	
	// ==========================================================

    mcasp->regs->XSTAT = 0x0000ffff;        // Clear all
    mcasp->regs->RSTAT = 0x0000ffff;        // Clear all

    mcasp->regs->XGBLCTL |= GBLCTL_XSRCLR_ON;                                   // Serialize
    while ( ( mcasp->regs->XGBLCTL & GBLCTL_XSRCLR_ON ) != GBLCTL_XSRCLR_ON );

    mcasp->regs->RGBLCTL |= GBLCTL_RSRCLR_ON;                                      // Serialize
    while ( ( mcasp->regs->RGBLCTL & GBLCTL_RSRCLR_ON ) != GBLCTL_RSRCLR_ON );

    /* Write a 0, so that no underrun occurs after releasing the state machine */

	// Fill transmit FIFO with a lot of zeros
    for(i = 0; i < NUMBER_ZEROS_FIFO; i++)
    {
    	MCASP1_XBUF = 0;
    	MCASP1_XBUF = 0;
    	MCASP1_XBUF = 0;
    	MCASP1_XBUF = 0;
	}
    //mcasp->regs->RBUF0 = 0;

    mcasp->regs->XGBLCTL |= GBLCTL_XSMRST_ON;                                       // State Machine
    while ( ( mcasp->regs->XGBLCTL & GBLCTL_XSMRST_ON ) != GBLCTL_XSMRST_ON );
 
    mcasp->regs->RGBLCTL |= GBLCTL_RSMRST_ON;                                       // State Machine
    while ( ( mcasp->regs->RGBLCTL & GBLCTL_RSMRST_ON ) != GBLCTL_RSMRST_ON );

    mcasp->regs->XGBLCTL |= GBLCTL_XFRST_ON;                                        // Frame Sync
    while ( ( mcasp->regs->XGBLCTL & GBLCTL_XFRST_ON ) != GBLCTL_XFRST_ON );

    mcasp->regs->RGBLCTL |= GBLCTL_RFRST_ON;                                        // Frame Sync
    while ( ( mcasp->regs->RGBLCTL & GBLCTL_RFRST_ON ) != GBLCTL_RFRST_ON );

	//===============================================================
    // Start by sending a 0
	//===============================================================
	return(0);
	// Prepare complete
}

/**
 * Rest the audio engine, currently not used.
 *///=======================================================
Int16 postProcessAudio()
{
    /* Close Codec */ 
	ak4588_rset(1,9,0x00);   // Reset DAC
    
    /* Close McASP */
    mcasp->regs->SRCTL0 = 0; // Serializers
    mcasp->regs->SRCTL1 = 0;
    mcasp->regs->SRCTL2 = 0;
    mcasp->regs->SRCTL3 = 0;
    mcasp->regs->SRCTL5 = 0;
    mcasp->regs->SRCTL6 = 0;
    mcasp->regs->SRCTL7 = 0;
    mcasp->regs->SRCTL8 = 0;
    mcasp->regs->SRCTL10 = 0;
    mcasp->regs->GBLCTL = 0;  // Global Reset

    return 0;
}

/**
 * DMA complete interrupt. This function is the triggering sync event
 * once a complete buffer has been trasnmitted to McAsp periphery.
 * If a new input buffer is available, the input samples are copied
 * to the software buffer directly. If a new output buffer is detected, 
 * nothing happens at first except for mapping the HWI to a SWI.
 * The rest of the effort is done in the SWI servce routine. If the next buffer
 * arrives, the flag should indicate the buffer is no longer served, otherwise
 * we will observe an audio dropout. 
 *///=================================================================
void serveEdmaMcasp1_transmit()
{
	int i,k;
	int idxSlot = 0;
	int idxSerializer = 0;
	int idxDma = 0;
	int tmpSwap = 0;

	float** bufsInDma = 0;
	Uint32 bufSoft_idx = 0;
	Int32* bufSoft = 0;
	float* ptrTmp_copyTo = 0;
	Int32* ptrTmp_copyFrom = 0;
	float tmpFlt = 0.0;

	Uint32 idxBufDMA_receive = 0;
#ifdef ADDITIONAL_DEBUGGING	
	Uint32 idxSampleDMA_receive = 0;
	Uint32 idxSampleDMA_receive2 = 0;
#endif
	
	Uint32 idxBufDMA_transmit = 0;
#ifdef ADDITIONAL_DEBUGGING	
	Uint32 idxSampleDMA_transmit = 0;
#endif

	edma_paRAM* ptrT = 0;
#ifdef ADDITIONAL_DEBUGGING	
	Uint32 iprOnEnter = 0;

	LgUns xTimestamp = 0;
	LgUns rTimestamp = 0;

	if(miscData.factor_cpuCyclesPerHtime == -1)
	{
		miscData.factor_cpuCyclesPerHtime = CLK_cpuCyclesPerHtime();
	}
	iprOnEnter = EDMA3CC_REGION1_IPR;
#endif

	while(1)
	{
		if(EDMA3CC_REGION1_IPR & (1 << X_DMA_NUM_S0))
		{
#ifdef ADDITIONAL_DEBUGGING
			xTimestamp = CLK_gethtime();
			if(tStampRef > 0)
			{
				allTimeStamps[cntTimeStamp].oneTimestampDiff = xTimestamp - tStampRef;
				if(allTimeStamps[cntTimeStamp].oneTimestampDiff > 0)
				{
					STS_add(&framePeriodMeasure, allTimeStamps[cntTimeStamp].oneTimestampDiff);
				}
			}
			else
			{
				allTimeStamps[cntTimeStamp].oneTimestampDiff = 0;
			}
			allTimeStamps[cntTimeStamp].eventType = 1;
			allTimeStamps[cntTimeStamp].oneTimestamp = xTimestamp;

			cntTimeStamp++;
			if(cntTimeStamp == 1024)
			{
				cntTimeStamp = 0;
			}
			tStampRef = xTimestamp;
#endif

			// Transmit DMA event incicates only starting point. From here on we have one frame of time to
			// transform the input into the output

			ptrT = (EDMA3_PARAMS_RAM + X_DMA_NUM_S1);
			idxBufDMA_transmit = (dmaControl.paramX->link_bcntrld & 0xFFFF) - ((Uint32)ptrT & 0xFFFF);
			idxBufDMA_transmit = idxBufDMA_transmit >> 5;

#ifdef ADDITIONAL_DEBUGGING	
			idxSampleDMA_transmit = (AUDIO_BUFFERSIZE * NUM_CHANNELS_PER_SERIALIZER) - dmaControl.paramX->ccnt;
			idxSampleDMA_transmit = idxSampleDMA_transmit >> 1;
			idxSampleDMA_receive = (AUDIO_BUFFERSIZE * NUM_CHANNELS_PER_SERIALIZER) - dmaControl.paramR->ccnt;
			idxSampleDMA_receive = idxSampleDMA_receive >> 1;
			dmaSave_transmit[cntDma_transmit].idxUseBuffer = idxBufDMA_transmit;
			dmaSave_transmit[cntDma_transmit].idxUseSample = idxSampleDMA_transmit;
			dmaSave_transmit[cntDma_transmit].idxUseSample_cross = idxSampleDMA_receive;
			dmaSave_transmit[cntDma_transmit].iprOnEnter = iprOnEnter;
			cntDma_transmit++;
			if(cntDma_transmit == 1024)
			{
				cntDma_transmit = 0;	
			}
#endif
			if(theStructSwitch.isBeingServed == 1)
			{
				theStat.bufferMissedCounter++;
			}
			else
			{
				// Store some data for the software interrupt
				theStructSwitch.idxWriteToDMA = (idxBufDMA_transmit+1)%2;
				theStructSwitch.idxReadFromSoftBuffer = inputBufferControl.idBufIn_soft;
				theStructSwitch.isBeingServed = 1;
			}

			postSwi_audio();
			//SWI_post(&swiNewFrameReady);

			// Mark the interrupt as being handled 
			EDMA3CC_REGION1_ICR = (1 << X_DMA_NUM_S0);
		}
		else if(EDMA3CC_REGION1_IPR & (1 << R_DMA_NUM_S0))
		{
#ifdef ADDITIONAL_DEBUGGING
			rTimestamp = CLK_gethtime();
			if(tStampRef > 0)
			{
				allTimeStamps[cntTimeStamp].oneTimestampDiff = rTimestamp - tStampRef;
			}
			else
			{
				allTimeStamps[cntTimeStamp].oneTimestampDiff = 0;
			}
			allTimeStamps[cntTimeStamp].eventType = 2;
			allTimeStamps[cntTimeStamp].oneTimestamp = rTimestamp;
			cntTimeStamp++;
			if(cntTimeStamp == 1024)
			{
				cntTimeStamp = 0;
			}
#endif

			ptrT = (EDMA3_PARAMS_RAM + R_DMA_NUM_S1);
			idxBufDMA_receive = (dmaControl.paramR->link_bcntrld & 0xFFFF) - ((Uint32)ptrT & 0xFFFF);
			idxBufDMA_receive = idxBufDMA_receive >> 5;

#ifdef ADDITIONAL_DEBUGGING	
			idxSampleDMA_receive = (AUDIO_BUFFERSIZE * NUM_CHANNELS_PER_SERIALIZER) - dmaControl.paramR->ccnt;
			idxSampleDMA_receive = idxSampleDMA_receive >> 1;
			idxSampleDMA_transmit = (AUDIO_BUFFERSIZE * NUM_CHANNELS_PER_SERIALIZER) - dmaControl.paramX->ccnt;
			idxSampleDMA_transmit = idxSampleDMA_transmit >> 1;
			dmaSave_receive[cntDma_receive].idxUseBuffer = idxBufDMA_receive;
			dmaSave_receive[cntDma_receive].idxUseSample = idxSampleDMA_receive;
			dmaSave_receive[cntDma_receive].idxUseSample_cross = idxSampleDMA_transmit;
			dmaSave_receive[cntDma_receive].iprOnEnter = iprOnEnter;
			cntDma_receive++;
			if(cntDma_receive == 1024)
			{
				cntDma_receive = 0;	
			}
#endif
			// Obtain the id which software buffer to use
			bufsInDma = bufsIn[inputBufferControl.idBufIn_dma];

			// Find out which DMA buffer is currently NOT in use
			bufSoft_idx = (idxBufDMA_receive+1)%2 ;

			// Get the pointer entry to DMA input buffer
			bufSoft = dma_input + bufSoft_idx * (NUM_SERIALIZERS*NUM_CHANNELS_PER_SERIALIZER*AUDIO_BUFFERSIZE);

			// Loop over all active input channels
			for(i = 0; i < NUM_INPUT_CHANNELS_AUDIO; i++)//*NUM_CHANNELS_PER_SERIALIZER
			{

				// From the left->right->left->right indexing, trasnform into a serializer and a slot id
				idxSerializer = (i >> 2);
				idxSlot = i % 2;

				// Compute the right start Int32 value in DMA buffer
				idxDma = idxSlot * NUM_SERIALIZERS + idxSerializer;

				// Target software buffer
				ptrTmp_copyTo = bufsInDma[i];

				// Source buffer (dma buffer with a serializer + channel shift)
				ptrTmp_copyFrom = bufSoft + idxDma;

				// loop over all samples in buffer
				for(k = 0; k < AUDIO_BUFFERSIZE; k++)
				{
					// Convert from INT32 (the lowest 8 bits are zero) to -1..1.0 float values
					tmpFlt = (float)*ptrTmp_copyFrom;

					// Increment the source pointer (that is, skip 8 Int32)
					ptrTmp_copyFrom += NUM_SERIALIZERS*NUM_CHANNELS_PER_SERIALIZER;

					// Normalize to -1.0 to 1.0
					tmpFlt = tmpFlt * NORM_FLT_32BIT;

					// Store the normalized float
					*ptrTmp_copyTo++ = tmpFlt;
				}
			}

			// Manage input buffer indices (one to write, one to work with)
			tmpSwap = inputBufferControl.idBufIn_soft;
			inputBufferControl.idBufIn_soft = inputBufferControl.idBufIn_dma;
			inputBufferControl.idBufIn_dma = tmpSwap;

			// Mark the interrupt as being handled 
			EDMA3CC_REGION1_ICR = (1 << R_DMA_NUM_S0);
		}
		else if(EDMA3CC_REGION1_IPR & (Uint32)(1 << X_DMA_UART))
		{
			// Do something
			postSwi_uarttrans();
			//SWI_post(&swiUartTransmitComplete);
			// Mark the interrupt as being handled 
			EDMA3CC_REGION1_ICR = (Uint32)(1 << X_DMA_UART);
		}
		else
		{
			// If we reach here, all interrupts have been processed..
			break;
		}
	}
	// Here, the transmit interrupt is complete
}
		
/**
 * Callback function to serve software interrupt which was triggered by 
 * the DMA hardware interrupt
 *///===============================================================
void serveFrameCallback()
{
	int i = 0, k= 0;
	int idxSerializer;
	int	idxSlot;
	int idxDma;
	float** inputBuffers = 0;	
	float** outputBuffers = 0;
	Int32* dmaOutputBuffer = 0;
	float* ptrTmp_copyFrom = 0;
	Int32* ptrTmp_copyTo = 0;
	Int32 tmpInt32;
	float tmpFlt;
#ifdef ADDITIONAL_DEBUGGING
	LgUns leavSwiTimestamp;
#endif
	//LgUns startTStampLoad = 0;
	//LgUns stopTStampLoad = 0;
	//LgUns entSwiTimestamp = CLK_gethtime();

#ifdef ADDITIONAL_DEBUGGING	
	HWI_disable();
	if(tStampRef > 0)
	{
		allTimeStamps[cntTimeStamp].oneTimestampDiff = entSwiTimestamp - tStampRef;
	}
	else
	{
		allTimeStamps[cntTimeStamp].oneTimestampDiff = 0;
	}
	
	allTimeStamps[cntTimeStamp].eventType = 3;
	allTimeStamps[cntTimeStamp].oneTimestamp = entSwiTimestamp;
	cntTimeStamp++;
	if(cntTimeStamp == 1024)
	{
		cntTimeStamp = 0;
	}
	HWI_enable();
#endif

	theStat.sampleCountOnSwiStart = (AUDIO_BUFFERSIZE * NUM_CHANNELS_PER_SERIALIZER) - dmaControl.paramX->ccnt;
 	theStat.sampleCountOnSwiStart = theStat.sampleCountOnSwiStart >> 1;
	
	inputBuffers = bufsIn[theStructSwitch.idxReadFromSoftBuffer];
	outputBuffers = bufsOut;

	//startTStampLoad = CLK_gethtime();
	user_process(theProcessingData.userData, inputBuffers, outputBuffers);
	//stopTStampLoad = CLK_gethtime();

	//STS_add(&userLoadStat, (stopTStampLoad - startTStampLoad));

	dmaOutputBuffer = dma_output + theStructSwitch.idxWriteToDMA * (NUM_SERIALIZERS*NUM_CHANNELS_PER_SERIALIZER*AUDIO_BUFFERSIZE);

	// Loop over all active output channels
	for(i = 0; i < NUM_OUTPUT_CHANNELS_AUDIO; i++)//*NUM_CHANNELS_PER_SERIALIZER
	{
		// From the left->right->left->right indexing, trasnform into a serializer and a slot id
		idxSerializer = (NUM_SERIALIZERS -1) - (i >> 2);// IMPORTANT: The output side is connected in reverse order
		idxSlot = i % 2;

		// Compute the right start Int32 value in DMA buffer
		idxDma = idxSlot * NUM_SERIALIZERS + idxSerializer;

		// Source software buffer
		ptrTmp_copyFrom = outputBuffers[i];

		// Source buffer (dma buffer with a serializer + channel shift)
		ptrTmp_copyTo = dmaOutputBuffer + idxDma;

		// loop over all samples in buffer
		for(k = 0; k < AUDIO_BUFFERSIZE; k++)
		{
			// Convert from INT32 (the lowest 8 bits are zero) to -1..1.0 float values
			tmpFlt = ((*ptrTmp_copyFrom++) * DENORM_FLT_32BIT);
			tmpInt32 = (Int32) tmpFlt;

			*ptrTmp_copyTo = tmpInt32;

			// Increment the dest pointer (that is, skip 8 Int32)
			ptrTmp_copyTo += NUM_SERIALIZERS*NUM_CHANNELS_PER_SERIALIZER;
		}
	}
	
	theStat.sampleCountOnSwitStop = (AUDIO_BUFFERSIZE * NUM_CHANNELS_PER_SERIALIZER) - dmaControl.paramX->ccnt;
	theStat.sampleCountOnSwitStop = theStat.sampleCountOnSwitStop >> 1;

	theStructSwitch.isBeingServed = 0;

#ifdef ADDITIONAL_DEBUGGING	
	leavSwiTimestamp = CLK_gethtime();
	HWI_disable();
	if(tStampRef > 0)
	{
		allTimeStamps[cntTimeStamp].oneTimestampDiff = leavSwiTimestamp - tStampRef;
	}
	else
	{
		allTimeStamps[cntTimeStamp].oneTimestampDiff = 0;
	}
	
	allTimeStamps[cntTimeStamp].eventType = 4;
	allTimeStamps[cntTimeStamp].oneTimestamp = leavSwiTimestamp;
	cntTimeStamp++;
	if(cntTimeStamp == 1024)
	{
		cntTimeStamp = 0;
	}
	HWI_enable();
#endif
}
