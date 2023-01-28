#ifndef __JVX_APVQ_H__
#define __JVX_APVQ_H__

#include "jvx.h"

/* Init parameters:
 *-dimensionVQ : Dimension of vector
 * -overallBitrate : Number of bits per vector
 * -numberBitsTreeStore : Number of bits to store each element in codebook, 1 - 31 can be chosen,
 * a good value for 2 bits per sample is e.g. 10, has an impact on ROM size
 *  -maximumNumberCandidateExclusion : Limitation for number of candidate vectors, 8 is normally good,
 *   has impact on RAM, computational complexity and performance
 *									  (small value : small RAM, small complexity low performance)
 * -useBitBounds : 0 no, 1 yes : With bit bounds, codewords can be sparated into radius and sphere part on
 *								bit level, otherwhise a division is required
 */

typedef struct
{
	jvxSize dimensionVQ;
	jvxSize overallBitrate;
	jvxData avalue;
	jvxSize numberBitsTreeStore;
	jvxSize  maximumNumberCandidateExclusion;
	jvxCBool useBitBounds;
	jvxCBool fillupIndices;
	jvxData radiusQCorrectionFactor;
	jvxCBool radius_and_sphere_q;
	jvxSize num_delta_steps;
} jvxApvq_init;

/*
 * - handleROM: Handle to allocated ROM
 * - handleRAM: Handle to used RAM (except for local variables)
 * - handleMISC: Handle to fields that hold variables for convenience and performance monitoring
 */

typedef struct 
{
	jvxHandle* handleROM;
	jvxHandle* handleRAM;
	jvxHandle* handleMisc;
	jvxSize numBitsROM;
	jvxSize numBitsRAM;
	jvxInt16 maximumNumberCandidateExclusion;
	jvxInt16 maximumNumberCandidateWeights;
} jvx_apvq_derived;

typedef struct
{
	jvxApvq_init init_param;
	jvx_apvq_derived derived_param;

} jvxApvq;

struct jvxApvq_interpol
{
	jvxData* weights;
	jvxSize nWeights;
	jvxData ipolVal;
} ;

struct jvxApvq_ipolweights
{
	jvxUInt64* neighborsI;
	jvxData* neighborsW;
	jvxSize nNeighborsMax;
	jvxSize nNeighborsInUse;
} ;

//============================================================================================

jvxErrorType jvx_apvq_initCfg(jvxApvq* apvq);

/**
 * Access to the initialization and allocation function for apple peeling quantizer(s)
 * OUTPUT:
 *///===================================================================================
jvxErrorType jvx_apvq_init(jvxApvq* apvq);

/**
 * Termination and deallocation function for apple peeling quantizer(s), all pointer passed will not be valid after function call.
 *///===================================================================================
jvxErrorType jvx_apvq_terminate(jvxApvq* apvq);

/**
 * Quantize vector (closed loop LP-SVQ mode).
 *///================================================================================
void 
jvx_apvq_quantize_cl(jvxApvq* apvq, jvxData* targetVector,  jvxData* impulseResp,
				 jvxUInt64* indexVector_NT, jvxUInt64* indexRadius, 
				 jvxUInt16 vectorLength, jvxData* vectorQ, jvxUInt16 doRequantization);

/**
 * Quantize vector (open loop SVQ mode).
 *///================================================================================
jvxErrorType jvx_apvq_quantize(jvxApvq* apvq, jvxData* targetVector,  jvxUInt64* indexVector_NT, jvxUInt64* indexRadius,
	jvxSize vectorLength, jvxData* vectorQ, jvxCBool doRequantization,
	jvxData* metric = NULL, jvxApvq_interpol* ipol = NULL, jvxApvq_ipolweights*ipolwghts = NULL);

//#endif
/**
* Spherical VQ decoder function
*///============================================================================
jvxErrorType
jvx_apvq_decode(jvxApvq* apvq, jvxData* vectorOutput, jvxSize vectorLength, jvxUInt64 indexVector_NT, jvxUInt32 indexRadius, jvxBool outCart);

 /**
  * Access functions to get post processing information
  *///===================================================
jvxErrorType 
jvx_apvq_performance(jvxApvq* apvq, 
	jvxSize* num_overload, jvxSize* num_lin, jvxSize* num_log_area,
	jvxData*snr_signal_db, jvxData* snr_sphere_db, jvxData* snr_radius_db, jvxData* eff_birate);

jvxErrorType 
jvx_apvq_number_indices(jvxApvq* apvq, jvxUInt64 *numNodes, jvxUInt64 *numIndicesRadius);

jvxErrorType
jvx_apvq_number_candidates(jvxApvq* apvq, jvxSize* numCandMax);

#endif