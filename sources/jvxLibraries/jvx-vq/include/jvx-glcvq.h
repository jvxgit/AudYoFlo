#ifndef __JVX_GLCVQ_H__
#define __JVX_GLCVQ_H__

#include "jvx.h"


/**
 * Initialization function: Specify vector dimension, bitrate and A for
 * A-law quantization. The function allocates all memory and returns the 
 * memory handles. Bit bounds and another option can be used in addition.
 * INPUT:
 * - dimensionVQ: Vector dimension, note that only 8 is optimal for the gosset lattice
 * - overallBitrate: Target bitrate per vector. This rate is an upper bound since not all
 *					 bit rate can be reached.
 * - AVALUE: Constant A for A-law quantization of radius.
 * - useBitBounds: Bool to activate design of VQ such that the bits can be assigned to 
 *				   sphere and radius on bit level.
 * - fillUpIndices: Bool to fill up the indices of the quantizer of the radius so that 
 *					the overall bitrate is reached as close as possible´.
 * OUTPUT:
 * - handleROM, handleRAM, handleMISC: Data handle to operate different quantizers in parallel.
 * numBitsRAM, numBitsROM: Field to return the number of bits required for ROM and RAM.
 * Return 0 if success, -1 if failed
 *///===================================================================================
int 
jvx_sc_glcvq_init(jvxInt16 dimensionVQ, jvxInt16 numberVectors, jvxInt16 overallBitrate, jvxData AVALUE, void** handleROM, 
				   void** handleRAM, void** handleMISC, jvxSize* numBitsROM, jvxSize* numberBitsRAM, 
				   jvxData radiusQCorrectionFactor, jvxInt16 useBitBounds, jvxInt16 fillupIndices, jvxInt16 modeOnlySphere);

/**
 * Termination and deallocation function. All memory fields obtained by a caller in initialization 
 * will be freed by call of this function. Access to the fields after call will result in segmentation violation.
 *///======================================================================================0
void 
jvx_sc_glcvq_terminate(void* handleROM, void* handleRAM, void* handleMISC);

/**
 * Quantization function. Use the memory fields to quantize the targetVector and return index fpr
 * sphere and radius AND the quantized vector outputVectorQ.
 * INPUT:
 * - targetVector: Input vector of right dimension.
 * - vectorLength: Vector dimension
 * - handleROM, handleRAM, handleMISC: Datastructs for RAM, ROM and MISC.
 * doRequantization: Switch for slightly enhanced performance and complexity due to the requantization 
 * OUTPUT:
 * - indexVector_NT: Index of spherical vector on return.
 * - indexRadius: Index of radius quantizer on return.
 * - outputVectorQ: Field filled with quantized vector on return.
 *///========================================================================================
void 
jvx_sc_glcvq_quantize(const jvxData* targetVector, jvxData* outputVectorQ,  jvxUInt64* indexVector_NT, jvxUInt64* indexRadius,
					  jvxUInt16 vectorLength,  jvxCBool doRequantization,
					  void* handleROM, void* handleRAM, void* handleMISC, jvxInt16 modeQuantization);






/**
 * Quantization function. Use the memory fields to quantize the targetVector and return index fpr
 * sphere and radius AND the quantized vector outputVectorQ.
 * INPUT:
 * - targetVector: Input vector of right dimension.
 * - vectorLength: Vector dimension
 * - handleROM, handleRAM, handleMISC: Datastructs for RAM, ROM and MISC.
 * doRequantization: Switch for slightly enhanced performance and complexity due to the requantization 
 * OUTPUT:
 * - indexVector_NT: Index of spherical vector on return.
 * - indexRadius: Index of radius quantizer on return.
 * - outputVectorQ: Field filled with quantized vector on return.
 *///========================================================================================
void 
jvx_sc_glcvq_quantize(const jvxData* targetVector, jvxData* outputVectorQ,  
						jvxSize* indexVector_NT, jvxUInt16* indexRadius, 
						jvxUInt16 vectorLength,  jvxUInt16 doRequantization,
						void* handleROM, void* handleRAM, void* handleMISC, jvxInt16 modeQuantization);

void 
jvx_sc_glcvq_quantize_full(const jvxData* targetVector, jvxData* outputVectorQ, 
	jvxUInt64* indexVector_NT, jvxUInt64* indexRadius,
							jvxUInt16 vectorLength, jvxCBool doRequantization,
							void* handleROM, void* handleRAM, void* handleMISC);

/**
 * Decode function. Use the memory fields to reconstruct a quantized vector based on
 * an index for the sphere part and one for the radius.
 * INPUT:
 * - index: Quantization index for sphere
 * - indexRadius: Quantization index radius
 * - handleROM, handleRAM, handleMISC: Datastructs for RAM, ROM and MISC.
 * OUTPUT:
 * - vector: Reconstructed signal vector.
 *///========================================================================================
void 
jvx_sc_glcvq_decode(jvxData *vector, jvxUInt64* index, jvxUInt64 indexRadius, void *handleROM, void *handleRAM, void* handleMISC);

jvxData 
	jvx_sc_glcvq_signal_snr_dB(void* handleMisc);
jvxData 
	jvx_sc_glcvq_sphere_signal_snr_dB(void* handleMisc);
jvxData 
	jvx_sc_glcvq_radius_snr_dB(void* handleMisc);
jvxData 
	jvx_sc_glcvq_effective_bitrate(void* handleMISC);

jvxSize 
	jvx_sc_glcvq_number_lin_area(void* handleMisc);
jvxSize 
	jvx_sc_glcvq_number_log_area(void* handleMisc);
jvxSize 
	jvx_sc_glcvq_number_overload(void* handleMisc);

void 
jvx_sc_glcvq_number_indices(jvxUInt64 *numNodes, jvxUInt64 *numIndicesRadius, void *handleROM);

void 
jvx_sc_glcvq_generate_code_allocation(const char *structname, void *handleROM, void 
							 *handleRAM, void *handleMISC);


#endif