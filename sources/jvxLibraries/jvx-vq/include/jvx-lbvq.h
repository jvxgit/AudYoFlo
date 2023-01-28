#ifndef _LOWBITRATEVQ_H__
#define _LOWBITRATEVQ_H__

#include "jvx.h"

typedef struct
{
	jvxInt16** positionsGrid;
	jvxInt16* maxNumberPositionsGrid;
	jvxInt16* numberPulsesGrid;
	jvxInt16 numberGrids;
} codebookConfiguration_LBVQ;

typedef struct
{
	codebookConfiguration_LBVQ* codebooks;
	jvxInt16 numberCodebooks;
} quantizerConfiguration_LBVQ;

extern "C"
{
void lbvq_sc_initializeAllocate(jvxInt16 dimensionVQ, 
							 quantizerConfiguration_LBVQ* configQ,
							 jvxData AVALUE, 
							 void** handleROM, 
							 void** handleRAM, 
							 void** handleMISC, 
							 jvxSize* numBitsROM, 
							 jvxSize* numBitsRAM);

void lbvq_sc_terminateDeallocate(void* handleROM, void* handleRAM, void* handleMISC);


void lbvq_sc_quantizeVectorOL(const jvxData* targetVector, jvxData* outputVectorQ,  
	jvxUInt64* indexVector_NT, jvxUInt64* indexRadius,
						   jvxUInt16 vectorLength,  jvxCBool doRequantization,
					       void* handleROM, void* handleRAM, void* handleMISC);

void lbvq_sc_getNumIndices(jvxSize *numNodes, jvxUInt16 *numIndicesRadius, void *handleROM); 
void lbvq_sc_decodeVector(jvxData *vector, jvxUInt16 vectorLength, jvxUInt64 index, jvxUInt64 indexRadius, void *handleROM, void *handleRAM);

jvxSize lbvq_sc_getNumberOverload(void* handleMisc);
jvxSize lbvq_sc_getNumberLinearArea(void* handleMisc);
jvxSize lbvq_sc_getNumberLogArea(void* handleMisc);
jvxSize lbvq_sc_getNumberFrames(void* handleMisc);
jvxData lbvq_sc_getSNRSignaldB(void* handleMisc);
jvxData lbvq_sc_getSNRSpheredB(void* handleMisc);
jvxData lbvq_sc_getSNRRadiusdB(void* handleMisc);
jvxData lbvq_sc_getEffectiveBitrate(void* handleMISC);
jvxData lbvq_sc_getEffectiveBitrateCELP(void* handleMISC);

};
#endif