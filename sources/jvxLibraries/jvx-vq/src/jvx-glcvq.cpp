#define _USE_MATH_DEFINES 
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include "rtpL_sourceCoding.h"

//#include "outputDebug.h"

// ============================================================
// Activate / deactivate old version of Schalkwijk implementation
//#define OLD_SCHALKWIJK
// ============================================================

// ============================================================
// Define to activate new multinomial calculation. The new version
// has only advantages and its use is mandatory for high bit rates 
// and vector dimensions for float precision
#define NEW_MULTINOMIAL
// ============================================================

// ============================================================
// Define to indicate that vector dimension is a multiple of 8. If this is not the case, GLCVQ will not deliver the high quality
#define GRAN_8
// ============================================================

// ============================================================
// GLCVQ works only for 64 bit data types
#ifndef RTPMODE64BIT
#error "GLCVQ requires 64 Bit data types!"
#endif
// ============================================================

// I do not know what this was good for, just leave it undefined
//#define NEW_SPHERE_Q

// Quantization Metric Calculations:
//==============================================
// METRIC: i=arg max(c^T*c_i) <-- only this is supported 
// ----------------------------------------------------------
//==============================================

//===============================================
// General Macros for gosset lattice VQ
#define MAX_AMPLITUDE 1.0
#define EPSILON_U_Q 0.00000001
#define EPSILON_RADIUS 0.00000000001

#define SIZE_WORD16 16
#define SIZE_WORD32 32
#define SIZE_WORD64 64
#define SIZE_DOUBLE 64
#define SIZE_POINTER 32
#define SIZE_MAX_WIDTH_TYPE SIZE_WORD64

//#define USE_OPTIMAL_BITALLOCATION_THEORY

//=========================================================
// STATIC HELPERS
//=========================================================

/**
 * Function to calculate a MAXWODTHTYPE with rounding ((INT) xx+0.5)
 *///===============================================================
static 
MAX_WIDTH_TYPE rndREAL2MAXWIDTHTYPE(DOUBLE input)
{
	return((MAX_WIDTH_TYPE)floor(input+0.5));
}

//=========================================================
// DEFINITION OF DATATYPES
//=========================================================

/**
 * Datastruct for ROM, contain only constant data
 *///==============================================
typedef struct
{
	WORD32 elementSize;
	MAX_WIDTH_TYPE numberNodesValid;
	MAX_WIDTH_TYPE numberNodesMax;
	WORD16 numBitsSpherePart;
	WORD16 numBitsRadiusPart;

	// Quantization of the radius
	WORD16 numberIndicesRadius;
	DOUBLE* radiusFldBoundsSquared;
	DOUBLE* radiusReconstruct;

	// Dimension for quantization: D, Number of Class Root Leaders: N
	// Number weights max (without zeros!): M
	WORD16 dimensionVQ;									// <-D
	WORD32 numberClassRootLeadersCMPR;					// <-N
	WORD16** fieldWeightsRootCMPR;						// [N][M]
	WORD16* validWeightsRootCMPR;						// <- M_i <= M
	WORD16** fieldAmplitudesRootCMPR;					// [N][M]
	WORD16* modeClassRootLeaderCMPR;					// [N]
	WORD16* signParityClassRootLeaderCMPR;				// [N]
	WORD16* numberZerosRootCMPR;						// [N]
	MAX_WIDTH_TYPE* offsetIdxClassRootsCMPR;			// [N]
	WORD32* numberOffsetFieldClassRootsCMPR;			// [N] <- P_i
	MAX_WIDTH_TYPE** offsetFieldClassRootsCMPR;			// [N][P_i]
	MAX_WIDTH_TYPE numberPermutationsPermutationsCMPR;	// ??
	DOUBLE normalizationRadiusCMPR;						// 1/sqrt(8*M) (Factor 2 due to %4 Condition in Gosset)
	WORD16 numberWeightsMaxCMPR;
	UWORD32* divisorSchalkwijkUW32;
} gossetROM;

/**
 * Datastruct for RAM, contain all data that must be modified during 
 * encoding/decoding
 *///===============================================================
typedef struct
{
	WORD32 elementSize;				
	WORD16* reorderMatrix;			// [D] Matrix to control reordering
	DOUBLE* reorderedEntries;		// [D] Reordered entries for quantization
	WORD16* copyWeightsSchalkwijk;	// [M] Field to copy all weights for schalkwijk Algo
	WORD16* copyWeightsPermCMPR;	// [2*M+1] <- Assumption: Given weights [a b c] + d zeros, 
									// maximum of [a_+ b_+ c_+ d c_- b_- a_-] possible 
	WORD16* copyAmplsPermCMPR;		// [2*M+1] <- Assumption: Given weights [a b c] + d zeros, 
									// maximum of [a_+ b_+ c_+ d c_- b_- a_-] possible 
	WORD16* numberSignsPermCMPR;	// [M] <- Number of signs per weight
	DOUBLE* copyEntriesNoSignCMPR;	// [D] Copy of the input vector, sign removed
	WORD16* copyEntriesSignsCMPR;	// [D] Vector for the Signs
	WORD16* signsCMPR;				// [D] Reordered signs
	DOUBLE* errVecCMPR;				// [D] Error vector for late correction in case mode=1 and sign parity mismatch
	WORD16* synthVectorUnsortedCMPR;// [D] Sorted WORD16, output for quantizer
} gossetRAM;

/**
 * Datastruct for everything that is necessarily required but makes use
 * of the quantizer more convenient.
 *///===============================================================
typedef struct
{
	WORD32 elementSize;

	WORD16 modeOnlySphere;// Mode to not consider radius quantization
	WORD16 numberVectors; // Number of spherical vectors per SVQ vector, 3xD=8 instead of D=24

	WORD16 overallBitrate;
	DOUBLE maxRadius;
	DOUBLE maxAmplitude;
	DOUBLE aValue;
	WORD16 sphereIndex;
	DOUBLE effectiveBitrate;
	DOUBLE effectiveBitratePositive;
	DOUBLE energyOriginal;
	DOUBLE energyDifference;
	DOUBLE energyRadius;
	DOUBLE energyDiffRadius;
	DOUBLE energyDiffSphere;
	DOUBLE deltaOverall;
	MAX_WIDTH_TYPE numRadiusLog;
	MAX_WIDTH_TYPE numRadiusLin;
	MAX_WIDTH_TYPE overload;
	MAX_WIDTH_TYPE numberFrames;

	DOUBLE* vqOutVerify;
} gossetMISC;



//==================================================================
// Functions to assure that no problem arises due to short word lengths
//==================================================================

#define MAX32B 2147483647
#define MAX32BU 4294967295
#define MAX16B 32767
#define MAX16BU 65535

static WORD16 C32_16(WORD32 in)
{
	assert(in <= MAX16B);
	return((WORD16)in);
}

static UWORD16 CU32_16(UWORD32 in)
{
	assert(in <= MAX16BU);
	return((UWORD16)in);
}

static WORD32 C64_32(WORD64 in)
{
	assert(in <= MAX32B);
	return((WORD32)in);
}

static UWORD32 CU64_32(UWORD64 in)
{
	assert(in <= MAX32BU);
	return((UWORD32)in);
}

//==================================================================

//=========================================================================
// USED STATIC FUNCTIONS USED STATIC FUNCTIONS USED STATIC FUNCTIONS
//					Declaration of static functions
//=========================================================================

//==============================================================================
static double gammaN2(WORD16 n);
//==============================================================================

//==============================================================================
static double calcNumberRadius(WORD16 n, double A, double Ks);
//==============================================================================

//==============================================================================
MAX_WIDTH_TYPE powMWT(MAX_WIDTH_TYPE in, WORD16 exp);
//==============================================================================

//==============================================================================
static DOUBLE readOutRadiusIndex(UWORD32 index, UWORD32 numberSteps, DOUBLE minV, DOUBLE maxV);
//==============================================================================

//==============================================================================
static DOUBLE convertLog2LinRadius(DOUBLE radiusLogQ, DOUBLE r_0, DOUBLE a_value, DOUBLE c);
//==============================================================================

//==============================================================================
static WORD16 getIndexRadius(DOUBLE realRadius, gossetROM* allocatedROM);
//==============================================================================

//==============================================================================
static MAX_WIDTH_TYPE schalkwijkEncoderW16(const WORD16* field, WORD16 dimensionVQ, WORD16* fieldWeights, 
									WORD16* fieldAmplitudes, WORD16 numberWeightsUsed, gossetRAM* allocatedRAM,
									MAX_WIDTH_TYPE numberPermutations, UWORD32* divisor_1_DV);
//==============================================================================

//==============================================================================
static void schalkwijkDecoderW16(WORD16* vecQ, MAX_WIDTH_TYPE idx, WORD16 dimensionVQ, 
								 WORD16* fieldWeights, WORD16* fieldAmplitudes, WORD16 numberWeightsUsed, 
								 gossetRAM* allocatedRAM, MAX_WIDTH_TYPE numberPermutations, UWORD32* divisor_1_DV);
//==============================================================================

//==============================================================================
static WORD32 calculateClassLeaderNew(WORD16 sphereIndexTest, WORD16* numberWeightsMax, WORD16 dimensionVQ, WORD16 funcCallMode,
							   WORD32* idxClassLeader, /*WORD16** clsLVectors,*/ WORD16* validWeights, WORD16** weights, 
							   WORD16** ampls, WORD16* zeros, WORD16* mode, WORD16* signs);
//==============================================================================

//==============================================================================
static WORD32 calculateClassLeaderNewRecursive(WORD16* vector, WORD16 curDim, WORD16 dimensionVQ, WORD16 mode, WORD16 modExpr, 
										WORD32 radiusSQR, WORD16 xVal, WORD16* numberWeightsMax, WORD16 funcCallMode,
										WORD32* idxClassLeader, /*WORD16** clsLVectors,*/ WORD16* validWeights, WORD16** weights, WORD16** ampls,
										WORD16* numberZeros, WORD16* modes, WORD16* signs);
//==============================================================================

//==============================================================================
static WORD16 testCondition1(WORD16* vector, WORD16 curDim, WORD16 mode, WORD32 radiusSQR);
//==============================================================================

//==============================================================================
static WORD16 testCondition2(WORD16* vector, WORD16 curDim, WORD16 mode, WORD32 radiusSQR);
//==============================================================================

//==============================================================================
static WORD16 testCondition3(WORD16* vector, WORD16 curDim, WORD16 mode, WORD16 modExpr, WORD16*chosenSign);
//==============================================================================

//==============================================================================
static WORD16 calculateWeightsZeros(WORD16* vector, WORD16* weightsLocal, WORD16 dimVQ, WORD16 mode, WORD16 functionCallMode,
							  /*WORD16* clsLVectors,*/ WORD16* weights, WORD16* ampls, WORD16* zeros);
//==============================================================================

//==============================================================================
static WORD32 calcClassLeaderPermutations(WORD16* weights, WORD16 numWeights, WORD16 mode, WORD16 sign,
								   WORD16 functionCallMode, WORD32* numberPermutationsClassRoot,
								   WORD16* copyWeightsPermCMPRRAM, WORD16* numberSignsPermCMPRRAM,
								   WORD16 numberZeros, MAX_WIDTH_TYPE* offsetIdxClassRootsCMPR,
								   MAX_WIDTH_TYPE* offsetFieldClassRootsCMPR, MAX_WIDTH_TYPE* offsetIndices,
								   //MAX_WIDTH_TYPE* offsetFieldClassRootsCMPRPositive, MAX_WIDTH_TYPE* offsetIndicesPositive,MAX_WIDTH_TYPE* offsetIndicesPositiveOld, 
								   MAX_WIDTH_TYPE *numberPossibleIndices );
//==============================================================================

//==============================================================================
static WORD32 calcClassLeaderPermutationsShort(WORD16* weights, WORD16 numWeights, WORD16 mode, WORD16 sign);
//==============================================================================

//==============================================================================
static WORD32 calcClassLeaderPermutationsShort_CPLX(WORD16* weights, WORD16 numWeights, WORD16 mode, WORD16 sign);
//==============================================================================

//==============================================================================
static void transformIdxSigns(WORD32 idx, WORD16 mode, WORD16 sign, WORD16* weights, WORD16 numWeights, WORD16* numberSignsPermCMPR);
//==============================================================================

//==============================================================================
static void allocateGossetSVQ_CMPR(WORD16 sphereIndex, gossetROM* allocatedROM, gossetRAM* allocatedRAM, 
							WORD32* numberClassRootLeaders, MAX_WIDTH_TYPE* numAllCodevectors, WORD16 fctID, 
							MAX_WIDTH_TYPE* numBitsROM, MAX_WIDTH_TYPE* numBitsRAM, WORD16 numberVectors, MAX_WIDTH_TYPE numberPossibleIndices);
//==============================================================================

//==============================================================================
static void deallocateGossetSVQ_CMPR(gossetROM* allocatedROM, gossetRAM* allocatedRAM, WORD32* numberClassRootLeaders, WORD16 fctID);
//==============================================================================

//==============================================================================
static void rtp_glcvq_quantize_sphere(const DOUBLE* inputVector, DOUBLE* vecQ, DOUBLE radiusQ, MAX_WIDTH_TYPE* indexVector_NT, 
						  DOUBLE realRadius, gossetROM* allocatedROM, gossetRAM* allocatedRAM,
						  gossetMISC* allocatedMISC);
//==============================================================================

//==============================================================================
static WORD32 convertSignsToPermutationNumber(WORD16* fieldSigns, WORD16* weights,
									   WORD16 numWeights, WORD16 signParity, WORD16 mode);
//==============================================================================

//==============================================================================
static WORD32 convertSignsToPermutationNumber_CPLX(WORD16* fieldSigns, WORD16* weights,
									   WORD16 numWeights, WORD16 signParity, WORD16 mode);
//==============================================================================

//==============================================================================
static void decodeVectorSphereNew(DOUBLE* outputVector, MAX_WIDTH_TYPE indexVector_NT, gossetROM* allocatedROM, 
						gossetRAM* allocatedRAM);
//==============================================================================

//==============================================================================
static void rtp_glcvq_decode_sphere(DOUBLE* outputVector, MAX_WIDTH_TYPE indexVector_NT, gossetROM* allocatedROM, 
						gossetRAM* allocatedRAM);
//==============================================================================

//==============================================================================
static void decodeVectorSphereNewPositive(DOUBLE* outputVector, MAX_WIDTH_TYPE indexVector_NT, gossetROM* allocatedROM, 
						gossetRAM* allocatedRAM);
//==============================================================================

//==============================================================================
static DOUBLE calculateDelta(gossetROM* allocatedROM, gossetRAM* allocatedRAM);
//==============================================================================

//==============================================================================
static DOUBLE calculatePermutationsDblNew(WORD16* weights, WORD16 numWeights);
//==============================================================================

//==============================================================================
static DOUBLE factorialDbl(WORD16 input);
//==============================================================================

//==============================================================================
static DOUBLE factorialDblStSt(WORD16 inputStart, WORD16 inputStop);
//==============================================================================

//==============================================================================
static DOUBLE multinomialDbl(WORD16* weights, WORD16 numWeights);
//==============================================================================

//========================================================================
// API FUNCTIONS API FUNCTIONS API FUNCTIONS API FUNCTIONS API FUNCTIONS
//========================================================================

/** 
 * Initialization and memory allocation function, ROM, RAM and MISC.
 * The quantizer is configured by the information on bitrate, vector dimension
 * and Avalue. Sphere shells will be tested until the one with the maximum number which does
 * NOT exceed the bit bound is found. All codebooks are allocated and will be returned to the caller.
 * Return 0 in case of success, -1 otherwise
 *///=========================================================================================
int 
FUNCTIONNAME(rtp_sc_glcvq_init)(WORD16 dimensionVQ, WORD16 numberVectors, WORD16 overallBitrate, DOUBLE AVALUE, void** handleROM, 
				   void** handleRAM, void** handleMISC, MAX_WIDTH_TYPE* numBitsROM, MAX_WIDTH_TYPE* numBitsRAM, 
				   DOUBLE radiusQCorrectionFactor, WORD16 useBitBounds, WORD16 fillupIndices, WORD16 modeOnlySphere)
{
	// == DEFINITION OF ALL VARIABLES ===
	WORD16 i;
	gossetROM* allocatedROM = NULL;
	gossetRAM* allocatedRAM = NULL;
	gossetMISC* allocatedMISC = NULL;
	
	MAX_WIDTH_TYPE numberPossibleIndices = 0;
	MAX_WIDTH_TYPE numberPossibleIndicesSystem = 0;
	MAX_WIDTH_TYPE numberIndicesUsed = 0;
	MAX_WIDTH_TYPE numberIndicesTest = 0;

	WORD16 sphereIndex = 0;
	WORD16 sphereIndexTest = 0;
	
	WORD16 numberClassLeadersTest = 0;
	WORD16 verifyNumberClassLeadersTest = 0;
	WORD16 numberWeightsTest = 0;
	WORD16 numberIndicesRadiusNextTest = 0;
	
	WORD16 minBitrate = 0;

	DOUBLE sphereRadiusConstant;
	DOUBLE vol1_nom = 0.0;
	DOUBLE vol1_den = 0.0;
	DOUBLE vol1 = 0.0;
	DOUBLE tmp;

	//========================================================================
	WORD32 numClassRootLeaders = 0;
	//WORD16 numberWeightsMax = 0;
	WORD32 idxClassLeader = 0;
	MAX_WIDTH_TYPE numberVectorPermutationsCMPR = 0;

	// Return value arguments must be non-zero
	assert(numBitsROM);
	assert(numBitsRAM);
	assert(handleROM);
	assert(handleRAM);
	assert(handleMISC);

	*numBitsROM = 0;
	*numBitsRAM = 0;
		
	// ==== ALLOCATE ACCESS HANDLES ====
	allocatedROM = (gossetROM*)malloc(sizeof(gossetROM));
	allocatedROM->elementSize = sizeof(gossetROM);

	allocatedRAM = (gossetRAM*)malloc(sizeof(gossetRAM));
	allocatedRAM->elementSize = sizeof(gossetRAM);

	allocatedMISC = (gossetMISC*)malloc(sizeof(gossetMISC));
	allocatedMISC->elementSize = sizeof(gossetMISC);

	allocatedMISC->modeOnlySphere = modeOnlySphere;
	allocatedMISC->numberVectors = numberVectors;

	// START COPYING CONTENTS
	allocatedROM->dimensionVQ = dimensionVQ;
	*numBitsROM += SIZE_WORD16;

	allocatedMISC->overallBitrate = overallBitrate;
	allocatedMISC->aValue = AVALUE;
	allocatedMISC->maxAmplitude = MAX_AMPLITUDE;
	
	// Calculate radius constant for determination or Q error on 1.0 sphere
	sphereRadiusConstant = 1.0/(1.0+log(allocatedMISC->aValue));

	// === DETERIMINE THE MAXIMUM RADIUS ====
	// Find the volume of a D-dimensional Hypersphere with radius 1.0
	// Approximate the gamma function by factorials
	if(allocatedROM->dimensionVQ%2)
	{
		vol1_nom = (DOUBLE)(1<<allocatedROM->dimensionVQ)*pow(M_PI, ((allocatedROM->dimensionVQ-1)/2))*factorialDbl((allocatedROM->dimensionVQ-1)/2);
		vol1_den = (DOUBLE)factorialDbl(allocatedROM->dimensionVQ);
	}
	else
	{
		vol1_nom = pow(M_PI, allocatedROM->dimensionVQ/2);
		vol1_den = (DOUBLE)factorialDbl(allocatedROM->dimensionVQ/2);
	}
	
	// Here we got the volume of the hyper sphere
	vol1 = vol1_nom/vol1_den;

	// Compute the radius of the hypersphere such that it has the same volume as a hypercube of 
	// length 2 (+1 -> -1)
	allocatedMISC->maxRadius = (allocatedMISC->maxAmplitude * 2.0)/pow(vol1, (1.0/(DOUBLE)allocatedROM->dimensionVQ));

	// Update for the concatenation of sphere vectors
	allocatedMISC->maxRadius = sqrt((DOUBLE)allocatedMISC->numberVectors)*allocatedMISC->maxRadius;

	// Check that in 32 bit mode, overflow problems can not happen

	numberPossibleIndicesSystem = 0xFFFFFFFFFFFFFFFF;
	if(overallBitrate >= 64)
	{
		rtpPrintf("GLCVQ: Warning: Number of bits too high, quantizer has been configured for 64 bit.\n");
		rtpPrintf("GLCVQ: Using 2^64-1 number of codevectors (maximum to be addressed in 64 bits!\n");
		numberPossibleIndices = (MAX_WIDTH_TYPE)0xFFFFFFFFFFFFFFFF;
	}
	else
	{
		numberPossibleIndices = ((MAX_WIDTH_TYPE)1)<<allocatedMISC->overallBitrate;
#ifdef GLCVQ_VERBOSE_MODE
		rtpPrintf("GLCVQ: Number of possible indices: %llu\n", numberPossibleIndices);
#endif
	}

	// Start looping over spheres with the minimum sphereIndex 0
	sphereIndex = 0;


	//========================================================================
	// PART I of Sphere design process:
	// For all sphere numbers generate the number of codevectors and test
	// for which sphere the overall bit rate will be exceded
	while(numberIndicesUsed < numberPossibleIndices)
	{
		// Increment sphere index for testing
		sphereIndexTest = sphereIndex+1;

#ifdef GLCVQ_VERBOSE_MODE
		rtpPrintf("GLCVQ: =============================================== \n", sphereIndexTest);
		rtpPrintf("GLCVQ: ===== Treating Gosset Shell with index %d ===== \n", sphereIndexTest);
		rtpPrintf("GLCVQ: =============================================== \n", sphereIndexTest);
#endif
		// Allocate memory for current Gosset shell
		allocateGossetSVQ_CMPR(sphereIndexTest, allocatedROM, allocatedRAM, &allocatedROM->numberClassRootLeadersCMPR,
			&allocatedROM->numberPermutationsPermutationsCMPR,0, NULL, NULL, allocatedMISC->numberVectors, numberPossibleIndicesSystem);

#ifdef GLCVQ_VERBOSE_MODE
		//rtpPrintf("GLCVQ: Number vectors in codebook: %llu\n", allocatedROM->numberPermutationsPermutationsCMPR);
#endif
		//rtpPrintf("Number idx: %d, Number vectors only positive: %f\n",sphereIndexTest, (DOUBLE) allocatedROM->numberNodesPositiveValid);
		
		// Calculate the delta for current Gosset shell
		allocatedMISC->deltaOverall = sqrt((DOUBLE)allocatedMISC->numberVectors)*calculateDelta(allocatedROM, allocatedRAM);

		// We can deallocate the memory again..
		deallocateGossetSVQ_CMPR(allocatedROM, allocatedRAM, &allocatedROM->numberClassRootLeadersCMPR,0);

		if(!allocatedMISC->modeOnlySphere)
		{
			// From the delta for sphere calculate the number of quantization intervals for radius
#ifdef USE_OPTIMAL_BITALLOCATION_THEORY
			numberIndicesRadiusNextTest = (WORD16)ceil(radiusQCorrectionFactor*calcNumberRadius(allocatedROM->dimensionVQ, allocatedMISC->aValue, 
				(DOUBLE)allocatedROM->numberPermutationsPermutationsCMPR));
#else
			numberIndicesRadiusNextTest = (WORD16)(radiusQCorrectionFactor*(1.0/(sphereRadiusConstant*allocatedMISC->deltaOverall)));
#endif
		}
		else
		{
			numberIndicesRadiusNextTest = 1;
		}
#ifdef GLCVQ_VERBOSE_MODE
		rtpPrintf("GLCVQ: Target bit rate:%f; current bit rate:%f\n", (DOUBLE)allocatedMISC->overallBitrate/((DOUBLE)allocatedMISC->numberVectors*(DOUBLE)allocatedROM->dimensionVQ), 
			log10((DOUBLE)numberIndicesRadiusNextTest*(DOUBLE)powMWT(allocatedROM->numberPermutationsPermutationsCMPR, allocatedMISC->numberVectors))/log10(2.0)/((DOUBLE)allocatedROM->dimensionVQ*(DOUBLE)allocatedMISC->numberVectors));
#endif

		minBitrate = (WORD16)ceil(log10((DOUBLE)numberIndicesRadiusNextTest*(DOUBLE)powMWT(allocatedROM->numberPermutationsPermutationsCMPR, allocatedMISC->numberVectors))/log10(2.0));

		// Check the end condition: Do we have less Q-points than we can addres by the given maximum number of indices?
		if((MAX_WIDTH_TYPE)numberIndicesRadiusNextTest*powMWT(allocatedROM->numberPermutationsPermutationsCMPR,allocatedMISC->numberVectors) < 
				numberPossibleIndices)
			sphereIndex = sphereIndexTest;
		else
		{
#ifdef GLCVQ_VERBOSE_MODE

			rtpPrintf("GLCVQ: ---------------------------------------------- \n", sphereIndexTest);
			rtpPrintf("GLCVQ: The current shell index bit rate is too high.. \n", sphereIndexTest);
			rtpPrintf("GLCVQ: ---------------------------------------------- \n", sphereIndexTest);
#endif
			break;
		}
	}
	// END of part I of code generation
	//========================================================================

	if(sphereIndex == 0)
	{
		rtpPrintf("GLCVQ: Sphere index is zero, chosen bit rate is too low!\n");
		rtpPrintf("GLCVQ: You must at least specify a vector bit rate of %d bits per vector, specified rate is %d\n", minBitrate, overallBitrate);
		return(-1);
	}

	//========================================================================
	// PART II: Generate the actual codebook
	// Here we have found the right sphereIndex that fits into the codewords.
	// Store the resulting sphereIndex that will be used.
	allocatedMISC->sphereIndex = sphereIndex;

#ifdef GLCVQ_VERBOSE_MODE
	rtpPrintf("GLCVQ: =============================================== \n", sphereIndexTest);
	rtpPrintf("GLCVQ: ===== ==== Generating used codebook ===== ===== \n", sphereIndexTest);
	rtpPrintf("GLCVQ: =============================================== \n", sphereIndexTest);
#endif

	// Allocate the handles for memory
	allocateGossetSVQ_CMPR(allocatedMISC->sphereIndex, allocatedROM, allocatedRAM, &allocatedROM->numberClassRootLeadersCMPR,
		&allocatedROM->numberPermutationsPermutationsCMPR,1, numBitsROM, numBitsRAM, allocatedMISC->numberVectors, numberPossibleIndices);

	// Design the codeboooks right here, actually for a second time
	allocatedMISC->deltaOverall = sqrt((DOUBLE)allocatedMISC->numberVectors)*calculateDelta(allocatedROM, allocatedRAM);

	// From the delta, calculate the number of indices for the radius
	if(!allocatedMISC->modeOnlySphere)
	{
#ifdef USE_OPTIMAL_BITALLOCATION_THEORY
		allocatedROM->numberIndicesRadius = (WORD16)ceil(radiusQCorrectionFactor*calcNumberRadius(allocatedROM->dimensionVQ, allocatedMISC->aValue, 
			(DOUBLE)allocatedROM->numberNodesValid));
#else
		allocatedROM->numberIndicesRadius = C32_16((WORD32)(radiusQCorrectionFactor*1.0/(sphereRadiusConstant*allocatedMISC->deltaOverall)));
#endif
	}
	else
	{
		allocatedROM->numberIndicesRadius = 1;
	}	
	*numBitsROM += SIZE_WORD16;

	allocatedROM->numberNodesValid = powMWT(allocatedROM->numberPermutationsPermutationsCMPR, 
		allocatedMISC->numberVectors);
	allocatedROM->numberNodesMax = allocatedROM->numberNodesValid;
	*numBitsROM += SIZE_MAX_WIDTH_TYPE;

	// Options for quantization.
	if(useBitBounds)
	{
		// Calculate the number of bits to address the spherical part, round up towards next integer
		allocatedROM->numBitsSpherePart = C32_16((WORD32)ceil(log10((DOUBLE)allocatedROM->numberNodesValid)/log10(2.0)));
		*numBitsROM += SIZE_WORD16;

		// Calculate the number of bits for radius
		allocatedROM->numBitsRadiusPart = allocatedMISC->overallBitrate-allocatedROM->numBitsSpherePart;
		*numBitsROM += SIZE_WORD16;

		// Transform both bit numbers into maximum index
		allocatedROM->numberNodesMax = ((MAX_WIDTH_TYPE)1) << allocatedROM->numBitsSpherePart;
		allocatedROM->numberIndicesRadius = CU32_16(((WORD32)1) << allocatedROM->numBitsRadiusPart);

		// Calculate the effective bit rate per sample(integer!)
		allocatedMISC->effectiveBitrate = (allocatedROM->numBitsSpherePart+allocatedROM->numBitsRadiusPart)/allocatedROM->dimensionVQ;

#ifdef GLCVQ_VERBOSE_MODE
		rtpPrintf("GLCVQ: =============================================== \n", sphereIndexTest);
		rtpPrintf("GLCVQ: Gosset Lattice in bitBounds mode, \n number of bits sphere:%d; number of bits radius: %d\n", allocatedROM->numBitsSpherePart,allocatedROM->numBitsRadiusPart);
		rtpPrintf("GLCVQ: Number available indices sphere: %d\n", allocatedROM->numberNodesMax);
		rtpPrintf("GLCVQ: Number valid indices sphere: %d\n", allocatedROM->numberNodesValid);
		rtpPrintf("GLCVQ: Number indices radius: %d\n", allocatedROM->numberIndicesRadius);
		rtpPrintf("GLCVQ: Effective Bitrate: %f\n", allocatedMISC->effectiveBitrate);
		rtpPrintf("GLCVQ: =============================================== \n", sphereIndexTest);
#endif
	}
	else 
	{
		allocatedROM->numBitsSpherePart = -1;
		*numBitsROM += SIZE_WORD16;

		allocatedROM->numBitsRadiusPart = -1;
		*numBitsROM += SIZE_WORD16;

		if(fillupIndices)
		{
			// This information is not required for encoding/decoding..
			allocatedROM->numberIndicesRadius = C32_16((WORD32)(floor((DOUBLE)numberPossibleIndices/(DOUBLE)allocatedROM->numberNodesValid)));
		}
		
		allocatedMISC->effectiveBitrate = (log10((DOUBLE)allocatedROM->numberNodesValid) + log10((DOUBLE)allocatedROM->numberIndicesRadius))/log10(2.0)/
			((DOUBLE)allocatedROM->dimensionVQ*(DOUBLE)allocatedMISC->numberVectors);
#ifdef GLCVQ_VERBOSE_MODE
		rtpPrintf("GLCVQ: =============================================== \n", sphereIndexTest);
		rtpPrintf("GLCVQ: Gosset Lattice in normal mode, number of valid indices sphere: %d\n", allocatedROM->numberNodesValid);
		rtpPrintf("GLCVQ: Number of indices radius: %d\n", allocatedROM->numberIndicesRadius);
		rtpPrintf("GLCVQ: Effective Bitrate: %f\n", allocatedMISC->effectiveBitrate);
		rtpPrintf("GLCVQ: Bitrate only Sphere: %f\n", log10((DOUBLE)allocatedROM->numberNodesValid)/log10(2.0)/((DOUBLE)allocatedMISC->numberVectors*(DOUBLE)allocatedROM->dimensionVQ));
		rtpPrintf("GLCVQ: =============================================== \n", sphereIndexTest);
#endif

	}

#ifdef GLCVQ_VERBOSE_MODE
	rtpPrintf("GLCVQ: =============================================== \n", sphereIndexTest);
	rtpPrintf("GLCVQ: Sphere design completed, Shell Nr. = %d; ", sphereIndex);
	rtpPrintf("GLCVQ: Msp = %d; ", allocatedROM->numberNodesValid);
	rtpPrintf("GLCVQ: Mr = %d\n", allocatedROM->numberIndicesRadius);
	rtpPrintf("GLCVQ: =============================================== \n", sphereIndexTest);
#endif

#ifdef GLCVQ_VERBOSE_MODE
	rtpPrintf("GLCVQ: =============================================== \n", sphereIndexTest);
	rtpPrintf("GLCVQ: Designing Codebook for quantization of radius.\n", allocatedROM->numberIndicesRadius);
	rtpPrintf("GLCVQ: =============================================== \n", sphereIndexTest);
#endif

	// Now create the quantizer for radius
	allocatedROM->radiusFldBoundsSquared = (DOUBLE*)malloc(sizeof(DOUBLE)*(allocatedROM->numberIndicesRadius+1));
	*numBitsROM += SIZE_DOUBLE*(allocatedROM->numberIndicesRadius+1);

	allocatedROM->radiusReconstruct = (DOUBLE*)malloc(sizeof(DOUBLE)*allocatedROM->numberIndicesRadius);
	*numBitsROM += SIZE_DOUBLE*(allocatedROM->numberIndicesRadius);

	// Design of the quantizer for the radius
	allocatedROM->radiusFldBoundsSquared[0] = -FLT_MAX;
	allocatedROM->radiusFldBoundsSquared[allocatedROM->numberIndicesRadius] = +FLT_MAX;

	// Set the radius quantization zentroids and the bounds inbetween for a faster decision
	for(i = 0; i < allocatedROM->numberIndicesRadius; i++)
	{
		allocatedROM->radiusReconstruct[i] = readOutRadiusIndex(i, allocatedROM->numberIndicesRadius, 0, allocatedMISC->maxRadius);
		allocatedROM->radiusReconstruct[i] = convertLog2LinRadius(allocatedROM->radiusReconstruct[i], allocatedMISC->maxRadius, 
			allocatedMISC->aValue, sphereRadiusConstant);

#ifdef GLCVQ_VERBOSE_MODE
		rtpPrintf("GLCVQ: ---> Reconstruction level[%d] = %f\n", i, allocatedROM->radiusReconstruct[i]);//rtpPrintf("Value=%f}\n", allocatedROM->radiusReconstruct[i]);
#endif
		if( i != 0)
		{
			//fldBounds[i-1] = (fldZentrRadius[i] + fldZentrRadius[i-1])/2;
			tmp = (allocatedROM->radiusReconstruct[i] + allocatedROM->radiusReconstruct[i-1])/2;
#ifdef GLCVQ_VERBOSE_MODE
			rtpPrintf("GLCVQ: --->Decision Bound[%d;%d]=%f\n", i-1, i, tmp);
#endif
			allocatedROM->radiusFldBoundsSquared[i] = tmp*tmp;//fldBounds[i-1]*fldBounds[i-1];
		}
	}

	// Start performance data measuring also
	allocatedMISC->energyOriginal = 0.0;
	allocatedMISC->energyDifference = 0.0;
	allocatedMISC->energyRadius = 0.0;
	allocatedMISC->energyDiffRadius = 0.0;
	allocatedMISC->energyDiffSphere = 0.0;
	allocatedMISC->numRadiusLog = 0;
	allocatedMISC->numRadiusLin = 0;
	allocatedMISC->overload = 0;
	allocatedMISC->numberFrames = 0;
	allocatedMISC->vqOutVerify = (DOUBLE*)malloc(sizeof(DOUBLE)*allocatedROM->dimensionVQ);
	
	// End of allocation function, quantizer is now ready for use
	*handleROM = allocatedROM;
	*handleRAM = allocatedRAM;
	*handleMISC = allocatedMISC;

	return(0); // success
}

/** 
 * Function to create a file to allocate all required structs for GLCVQ statically (used in G.729.1 ext)
 *///=========================================================================================
void rtp_sc_glcvq_generate_code_allocation(const char *structname, void *handleROM, void *handleRAM, void *handleMISC)
{

	WORD16 i, j;
	FILE *fp;
	char filename[256];

	gossetROM* allocatedROM = (gossetROM*) handleROM;
	gossetRAM* allocatedRAM = (gossetRAM*) handleRAM;
	gossetMISC* allocatedMISC = (gossetMISC*) handleMISC;

	assert(allocatedROM->elementSize == sizeof(gossetROM));
	assert(allocatedRAM->elementSize == sizeof(gossetRAM));
	assert(allocatedMISC->elementSize == sizeof(gossetMISC));

	assert(strlen(structname) < 254); 


	/* generate header file */
	sprintf(filename, "%s.h", structname);
	fp = fopen(filename,"w");
	fprintf(fp, "#ifndef __%s_H__\n", structname);
	fprintf(fp, "#define __%s_H__\n", structname);
	fprintf(fp, "#include \"gossetLatticeLCVQ.h\"\n");
	fprintf(fp, "gossetMISC %s_misc;\n", structname);
	fprintf(fp, "gossetRAM %s_ram;\n", structname);
	fprintf(fp, "gossetROM %s_rom;\n", structname);
	fprintf(fp, "void %s_init();\n", structname);
	fprintf(fp, "#endif\n");
	fclose(fp);

	/* generate c file */
	sprintf(filename, "%s.c", structname);
	fp = fopen(filename,"w");


	fprintf(fp, "#include \"%s.h\"\n", structname);

	/* RAM arrays */
	fprintf(fp, "WORD16 %s_reorderMatrix[%i];\n", structname, 
		allocatedROM->dimensionVQ);
	fprintf(fp, "REAL %s_reorderedEntries[%i];\n", structname, 
		allocatedROM->dimensionVQ);
	fprintf(fp, "WORD16 %s_copyWeightsSchalkwijk[%i];\n", structname, 
		allocatedROM->numberWeightsMaxCMPR);
	fprintf(fp, "WORD16 %s_copyWeightsPermCMPR[%i];\n", structname, 
		2*allocatedROM->numberWeightsMaxCMPR+1);
	fprintf(fp, "WORD16 %s_copyAmplsPermCMPR[%i];\n", structname, 
		2*allocatedROM->numberWeightsMaxCMPR+1);
	fprintf(fp, "WORD16 %s_numberSignsPermCMPR[%i];\n", structname, 
		allocatedROM->numberWeightsMaxCMPR);
	fprintf(fp, "REAL %s_copyEntriesNoSignCMPR[%i];\n", structname, 
		allocatedROM->dimensionVQ);
	fprintf(fp, "WORD16 %s_copyEntriesSignsCMPR[%i];\n", structname, 
		allocatedROM->dimensionVQ);
	fprintf(fp, "WORD16 %s_signsCMPR[%i];\n", structname, 
		allocatedROM->dimensionVQ);
	fprintf(fp, "REAL %s_errVecCMPR[%i];\n", structname, 
		allocatedROM->dimensionVQ);
	fprintf(fp, "REAL %s_synthVectorUnsortedCMPR[%i];\n", structname, 
		allocatedROM->dimensionVQ);


	/* ROM arrays */
	fprintf(fp, "WORD16 %s_validWeightsRootCMPR[%li] = {\n%i", 
		structname, allocatedROM->numberClassRootLeadersCMPR, 
		allocatedROM->validWeightsRootCMPR[0]);
	for( i = 1; i < allocatedROM->numberClassRootLeadersCMPR; i++ ) {
		fprintf(fp, ", %i", allocatedROM->validWeightsRootCMPR[i]);
		if( (i % 10) == 0 ) fprintf(fp, "\n");        
	}
	fprintf(fp, "};\n");

	fprintf(fp, "WORD16 %s_modeClassRootLeaderCMPR[%li] = {\n%i", 
		structname, allocatedROM->numberClassRootLeadersCMPR, 
		allocatedROM->modeClassRootLeaderCMPR[0]);
	for( i = 1; i < allocatedROM->numberClassRootLeadersCMPR; i++ ) {
		fprintf(fp, ", %i", allocatedROM->modeClassRootLeaderCMPR[i]);
		if( (i % 10) == 0 ) fprintf(fp, "\n");        
	}
	fprintf(fp, "};\n");

	fprintf(fp, "WORD16 %s_signParityClassRootLeaderCMPR[%li] = {\n%i", 
		structname, allocatedROM->numberClassRootLeadersCMPR, 
		allocatedROM->signParityClassRootLeaderCMPR[0]);
	for( i = 1; i < allocatedROM->numberClassRootLeadersCMPR; i++ ) {
		fprintf(fp, ", %i", allocatedROM->signParityClassRootLeaderCMPR[i]);
		if( (i % 10) == 0 ) fprintf(fp, "\n");        
	}
	fprintf(fp, "};\n");

	fprintf(fp, "WORD16 %s_numberZerosRootCMPR[%li] = {\n%i", 
		structname, allocatedROM->numberClassRootLeadersCMPR, 
		allocatedROM->numberZerosRootCMPR[0]);
	for( i = 1; i < allocatedROM->numberClassRootLeadersCMPR; i++ ) {
		fprintf(fp, ", %i", allocatedROM->numberZerosRootCMPR[i]);
		if( (i % 10) == 0 ) fprintf(fp, "\n");        
	}
	fprintf(fp, "};\n");

	fprintf(fp, "MAX_WIDTH_TYPE %s_offsetIdxClassRootsCMPR[%li] = {\n%lli", 
		structname, allocatedROM->numberClassRootLeadersCMPR, 
		allocatedROM->offsetIdxClassRootsCMPR[0]);
	for( i = 1; i < allocatedROM->numberClassRootLeadersCMPR; i++ ) {
		fprintf(fp, ", %lli", allocatedROM->offsetIdxClassRootsCMPR[i]);
		if( (i % 10) == 0 ) fprintf(fp, "\n");        
	}
	fprintf(fp, "};\n");

	fprintf(fp, "WORD32 %s_numberOffsetFieldClassRootsCMPR[%li] = {\n%li", 
		structname, allocatedROM->numberClassRootLeadersCMPR, 
		allocatedROM->numberOffsetFieldClassRootsCMPR[0]);
	for( i = 1; i < allocatedROM->numberClassRootLeadersCMPR; i++ ) {
		fprintf(fp, ", %li", allocatedROM->numberOffsetFieldClassRootsCMPR[i]);
		if( (i % 10) == 0 ) fprintf(fp, "\n");        
	}
	fprintf(fp, "};\n");

	fprintf(fp, "REAL %s_radiusFldBoundsSquared[%i] = {\n%f", 
		structname, allocatedROM->numberIndicesRadius+1, 
		allocatedROM->radiusFldBoundsSquared[0]);
	for( i = 1; i < allocatedROM->numberIndicesRadius+1; i++ ) {
		fprintf(fp, ", %f", allocatedROM->radiusFldBoundsSquared[i]);
		if( (i % 10) == 0 ) fprintf(fp, "\n");        
	}
	fprintf(fp, "};\n");

	fprintf(fp, "REAL %s_radiusReconstruct[%i] = {\n%f", 
		structname, allocatedROM->numberIndicesRadius, 
		allocatedROM->radiusReconstruct[0]);
	for( i = 1; i < allocatedROM->numberIndicesRadius; i++ ) {
		fprintf(fp, ", %f", allocatedROM->radiusReconstruct[i]);
		if( (i % 10) == 0 ) fprintf(fp, "\n");        
	}
	fprintf(fp, "};\n");


	/* multi dimensional ROM arrays */

	for( i = 0; i < allocatedROM->numberClassRootLeadersCMPR; i++ ) 
	{
		fprintf(fp, "WORD16 %s_fieldWeightsRootCMPR_%i[%i] = { %i", structname, i, 
			allocatedROM->validWeightsRootCMPR[i], allocatedROM->fieldWeightsRootCMPR[i]
		[0]);
		for( j = 1; j < allocatedROM->validWeightsRootCMPR[i]; j++ ) 
		{
			fprintf(fp, ", %i", allocatedROM->fieldWeightsRootCMPR[i][j]);
			assert(allocatedROM->fieldWeightsRootCMPR[i][j]<=16);
		}
		fprintf(fp," };\n");
	}
	fprintf(fp, "WORD16 *%s_fieldWeightsRootCMPR[%li] = {\n %s_fieldWeightsRootCMPR_0", structname, allocatedROM->numberClassRootLeadersCMPR, structname );
	for( i = 1; i <  allocatedROM->numberClassRootLeadersCMPR; i++ ) 
	{
		fprintf(fp, ", %s_fieldWeightsRootCMPR_%i", structname,i);
		if( (i % 4) == 0 ) fprintf(fp, "\n");        
	}
	fprintf(fp, "};\n");


	for( i = 0; i < allocatedROM->numberClassRootLeadersCMPR; i++ ) 
	{
		fprintf(fp, "WORD16 %s_fieldAmplitudesRootCMPR_%i[%i] = { %i", structname, 
			i, allocatedROM->validWeightsRootCMPR[i], 
			allocatedROM->fieldAmplitudesRootCMPR[i][0]);
		for( j = 1; j < allocatedROM->validWeightsRootCMPR[i]; j++ ) {
			fprintf(fp, ", %i", allocatedROM->fieldAmplitudesRootCMPR[i][j]);
		}
		fprintf(fp," };\n");
	}
	fprintf(fp, "WORD16 *%s_fieldAmplitudesRootCMPR[%li] = 	{\n %s_fieldAmplitudesRootCMPR_0", 
	structname, allocatedROM->numberClassRootLeadersCMPR, structname );
	for( i = 1; i < allocatedROM->numberClassRootLeadersCMPR; i++ ) {
		fprintf(fp, ", %s_fieldAmplitudesRootCMPR_%i", structname,i);
		if( (i % 4) == 0 ) fprintf(fp, "\n");        
	}
	fprintf(fp, "};\n");


	for( i = 0; i < allocatedROM->numberClassRootLeadersCMPR; i++ ) 
	{
		fprintf(fp, "MAX_WIDTH_TYPE %s_offsetFieldClassRootsCMPR_%i[%li] = { %lli", structname, i, allocatedROM->numberOffsetFieldClassRootsCMPR[i], 
		allocatedROM->offsetFieldClassRootsCMPR[i][0]);
		for( j = 1; j < allocatedROM->numberOffsetFieldClassRootsCMPR[i]; j++ ) 
		{
			fprintf(fp, ", %lli", allocatedROM->offsetFieldClassRootsCMPR[i][j]);
		}
		fprintf(fp," };\n");
		}
		fprintf(fp, "MAX_WIDTH_TYPE *%s_offsetFieldClassRootsCMPR[%li] = {\n %s_offsetFieldClassRootsCMPR_0", 
		structname, allocatedROM->numberClassRootLeadersCMPR, structname );
		for( i = 1; i < allocatedROM->numberClassRootLeadersCMPR; i++ ) {
			fprintf(fp, ", %s_offsetFieldClassRootsCMPR_%i", structname,i);
			if( (i % 4) == 0 ) fprintf(fp, "\n");        
		}
		fprintf(fp, "};\n");

		/* init function */
		fprintf(fp, "void %s_init() {\n", structname);

		/* init misc */
		fprintf(fp, "%s_misc.elementSize      = %li;\n", structname, 
			allocatedMISC->elementSize);
		fprintf(fp, "%s_misc.modeOnlySphere   = %i;\n", structname, 
			allocatedMISC->modeOnlySphere);
		fprintf(fp, "%s_misc.numberVectors    = %i;\n", structname, 
			allocatedMISC->numberVectors);
		fprintf(fp, "%s_misc.overallBitrate   = %i;\n", structname, 
			allocatedMISC->overallBitrate);
		fprintf(fp, "%s_misc.maxRadius        = %f;\n", structname, 
			allocatedMISC->maxRadius);
		fprintf(fp, "%s_misc.maxAmplitude     = %f;\n", structname, 
			allocatedMISC->maxAmplitude);
		fprintf(fp, "%s_misc.aValue           = %f;\n", structname, 
			allocatedMISC->aValue);
		fprintf(fp, "%s_misc.sphereIndex      = %i;\n", structname, 
			allocatedMISC->sphereIndex);
		fprintf(fp, "%s_misc.effectiveBitrate = %f;\n", structname, 
			allocatedMISC->effectiveBitrate);
		fprintf(fp, "%s_misc.energyOriginal   = %f;\n", structname, 
			allocatedMISC->energyOriginal);
		fprintf(fp, "%s_misc.energyDifference = %f;\n", structname, 
			allocatedMISC->energyDifference);
		fprintf(fp, "%s_misc.energyRadius     = %f;\n", structname, 
			allocatedMISC->energyRadius);
		fprintf(fp, "%s_misc.energyDiffRadius = %f;\n", structname, 
			allocatedMISC->energyDiffRadius);
		fprintf(fp, "%s_misc.energyDiffSphere = %f;\n", structname, 
			allocatedMISC->energyDiffSphere);
		fprintf(fp, "%s_misc.deltaOverall     = %f;\n", structname, 
			allocatedMISC->deltaOverall);
		fprintf(fp, "%s_misc.numRadiusLog     = %lli;\n", structname, 
			allocatedMISC->numRadiusLog);
		fprintf(fp, "%s_misc.numRadiusLin     = %lli;\n", structname, 
			allocatedMISC->numRadiusLin);
		fprintf(fp, "%s_misc.overload         = %lli;\n", structname, 
			allocatedMISC->overload);
		fprintf(fp, "%s_misc.numberFrames     = %lli;\n", structname, 
			allocatedMISC->numberFrames);

		/* init rom */
		fprintf(fp, "%s_rom.elementSize         = %li;\n", structname, 
			allocatedROM->elementSize);
		fprintf(fp, "%s_rom.numberNodesValid    = %lli;\n", structname, 
			allocatedROM->numberNodesValid);
		fprintf(fp, "%s_rom.numberNodesMax      = %lli;\n", structname, 
			allocatedROM->numberNodesMax);
		fprintf(fp, "%s_rom.numBitsSpherePart   = %i;\n", structname, 
			allocatedROM->numBitsSpherePart);
		fprintf(fp, "%s_rom.numBitsRadiusPart   = %i;\n", structname, 
			allocatedROM->numBitsRadiusPart);
		fprintf(fp, "%s_rom.numberIndicesRadius = %i;\n", structname, 
			allocatedROM->numberIndicesRadius);
		fprintf(fp, "%s_rom.radiusReconstruct = (REAL *)%s_radiusReconstruct;\n", 
			structname, structname);
		fprintf(fp, "%s_rom.radiusFldBoundsSquared = (REAL *)%s_radiusFldBoundsSquared;\n", structname, structname);
			fprintf(fp, "%s_rom.dimensionVQ         = %i;\n", structname, 
			allocatedROM->dimensionVQ);
		fprintf(fp, "%s_rom.numberClassRootLeadersCMPR  = %li;\n", structname, 
			allocatedROM->numberClassRootLeadersCMPR);
		fprintf(fp, "%s_rom.numberWeightsMaxCMPR  = %i;\n", structname, 
			allocatedROM->numberWeightsMaxCMPR);
		fprintf(fp, "%s_rom.fieldWeightsRootCMPR = (WORD16 	**)%s_fieldWeightsRootCMPR;\n", structname, structname);
			fprintf(fp, "%s_rom.validWeightsRootCMPR = (WORD16 	*)%s_validWeightsRootCMPR;\n", structname, structname);
			fprintf(fp, "%s_rom.fieldAmplitudesRootCMPR = (WORD16 	**)%s_fieldAmplitudesRootCMPR;\n", structname, structname);
			fprintf(fp, "%s_rom.modeClassRootLeaderCMPR = (WORD16 	*)%s_modeClassRootLeaderCMPR;\n", structname, structname);
			fprintf(fp, "%s_rom.signParityClassRootLeaderCMPR = (WORD16 *)%s_signParityClassRootLeaderCMPR;\n", structname, structname);
			fprintf(fp, "%s_rom.numberZerosRootCMPR = (WORD16 *)%s_numberZerosRootCMPR;	\n", structname, structname);
			fprintf(fp, "%s_rom.offsetIdxClassRootsCMPR = (MAX_WIDTH_TYPE *)%s_offsetIdxClassRootsCMPR;\n", structname, structname);
			fprintf(fp, "%s_rom.numberOffsetFieldClassRootsCMPR = (WORD32 *)%s_numberOffsetFieldClassRootsCMPR;\n", structname, structname);
			fprintf(fp, "%s_rom.offsetFieldClassRootsCMPR = (MAX_WIDTH_TYPE **)%s_offsetFieldClassRootsCMPR;\n", structname, structname);
			fprintf(fp, "%s_rom.numberPermutationsPermutationsCMPR = %lli;\n", 
			structname, allocatedROM->numberPermutationsPermutationsCMPR);
		fprintf(fp, "%s_rom.normalizationRadiusCMPR = %f;\n", structname, 
			allocatedROM->normalizationRadiusCMPR);


		/* init ram */
		fprintf(fp, "%s_ram.elementSize         = %li;\n", structname, 
			allocatedRAM->elementSize);
		fprintf(fp, "%s_ram.reorderMatrix       = (WORD16 *)%s_reorderMatrix;\n", 
			structname, structname);
		fprintf(fp, "%s_ram.reorderedEntries    = (REAL *)%s_reorderedEntries;\n", 
			structname, structname);
		fprintf(fp, "%s_ram.copyWeightsSchalkwijk = (WORD16 *)%s_copyWeightsSchalkwijk;\n", structname, structname);
			fprintf(fp, "%s_ram.copyWeightsPermCMPR = (WORD16 *)%s_copyWeightsPermCMPR;\n", structname, structname);
			fprintf(fp, "%s_ram.copyAmplsPermCMPR = (WORD16 *)%s_copyAmplsPermCMPR;\n", 
			structname, structname);
		fprintf(fp, "%s_ram.numberSignsPermCMPR = (WORD16 *)%s_numberSignsPermCMPR;\n", structname, structname);
			fprintf(fp, "%s_ram.copyEntriesNoSignCMPR = (REAL *)%s_copyEntriesNoSignCMPR;\n", structname, structname);
			fprintf(fp, "%s_ram.copyEntriesSignsCMPR = (WORD16 *)%s_copyEntriesSignsCMPR;\n", structname, structname);
			fprintf(fp, "%s_ram.signsCMPR = (WORD16 *)%s_signsCMPR;\n", structname, 
			structname);
		fprintf(fp, "%s_ram.errVecCMPR = (REAL *)%s_errVecCMPR;\n", structname, 
			structname);
		fprintf(fp, "%s_ram.synthVectorUnsortedCMPR = (WORD16 *)%s_synthVectorUnsortedCMPR;\n", structname, structname);


			fprintf(fp, "}\n");
		fclose(fp);
}

/** 
 * Terminate quantizer and deallocate all memory.
 * Return 0 in case of success, -1 otherwise
 *///=========================================================================================
void 
FUNCTIONNAME(rtp_sc_glcvq_terminate)(void* handleROM, void* handleRAM, void* handleMISC)
{
	gossetROM* allocatedROM = (gossetROM*) handleROM;
	gossetRAM* allocatedRAM = (gossetRAM*) handleRAM;
	gossetMISC* allocatedMISC = (gossetMISC*) handleMISC;

	assert(allocatedROM->elementSize == sizeof(gossetROM));
	assert(allocatedRAM->elementSize == sizeof(gossetRAM));
	assert(allocatedMISC->elementSize == sizeof(gossetMISC));

	free(allocatedROM->radiusFldBoundsSquared);
	free(allocatedROM->radiusReconstruct);
	free(allocatedMISC->vqOutVerify);

	deallocateGossetSVQ_CMPR(allocatedROM, allocatedRAM, &allocatedROM->numberClassRootLeadersCMPR,1);

	memset(allocatedROM, 0, sizeof(gossetROM));
	memset(allocatedMISC, 0, sizeof(gossetMISC));

	free(allocatedMISC->vqOutVerify);
	free(allocatedROM);
	free(allocatedRAM);
	free(allocatedMISC);
//	rtpPrintf("Complexity for functioncall, reserved for encoder: %f; reserved for decoder: %f \n", 
//		(DOUBLE) maxNumOperations_encoder, (DOUBLE) maxNumOperations_decoder);

}

/** 
 * Function to quantize a signal vector employing the fast search
 *///=========================================================================================
void 
FUNCTIONNAME(rtp_sc_glcvq_quantize)(const DOUBLE* targetVector, DOUBLE* outputVectorQ, 
						  MAX_WIDTH_TYPE* indexVector_NT, UWORD16* indexRadius, 
					  UWORD16 vectorLength, UWORD16 doRequantization,
					  void* handleROM, void* handleRAM, void* handleMISC, WORD16 modeOperation)
{
	UWORD16 i,j;

	DOUBLE metricNum, metricDen;


	gossetROM* allocatedROM = (gossetROM*)handleROM;
	gossetRAM* allocatedRAM = (gossetRAM*)handleRAM;
	gossetMISC* allocatedMISC = (gossetMISC*)handleMISC;

	UWORD16 cnt = 0;
	DOUBLE realRadius = 0; //EPSILON_RADIUS;
	DOUBLE radiusQ, radiusOpt;

	DOUBLE absValVec = 0.0;
	DOUBLE absValVecQ = 0.0;
	DOUBLE radiusIn = 0.0;

	const DOUBLE *ptrD1;

	MAX_WIDTH_TYPE cntEncoder = 0;
	MAX_WIDTH_TYPE cntDecoder = 0;

	assert(allocatedROM->elementSize == sizeof(gossetROM));
	assert(allocatedRAM->elementSize == sizeof(gossetRAM));
	assert(allocatedMISC->elementSize == sizeof(gossetMISC));
	assert(vectorLength == (allocatedROM->dimensionVQ*allocatedMISC->numberVectors));

	allocatedMISC->numberFrames++;

	/*	ULINT cnt; 
	double realRadius = 0;//EPSILON_RADIUS;
	ULINT useIndex = 0;
*/
	ptrD1 = targetVector;
	realRadius = 0;

	for(cnt = 0; cnt < vectorLength; cnt++)
	{
		realRadius += (*ptrD1 * *ptrD1);
		ptrD1++;
	}

	// COMPLEXITY: Things we do not need!
	radiusIn = sqrt(realRadius+0.00000001);

	// For performance measuring, apply 
	// COMPLEXITY: Things we do not need!
	allocatedMISC->energyRadius += realRadius;
	
	// COMPLEXITY: Things we do not need!
	allocatedMISC->energyOriginal += realRadius;

	// COMPLEXITY: Things we do not need!
	if(sqrt(realRadius) > allocatedMISC->maxRadius)
	{
		allocatedMISC->overload++;
	}

	// COMPLEXITY: Things we do not need!
	if(sqrt(realRadius) < allocatedMISC->maxRadius/allocatedMISC->aValue)
	{
		allocatedMISC->numRadiusLin++;
	}
	else
	{
		allocatedMISC->numRadiusLog++;
	}


	// Search the best matching vector for normalized input in a closed loop manner, 
	// use the OL vectors for a preselection and the target signal for a CL search
	// on the reduced set of vectors
	// It is not important whether the signal is normalized prior to search as
	// the radius is removed in the search procedure anyway

	// We might set this to 1.0 in G.729.1 SWB because the input vectors are known 
	// to be normalized
	if(allocatedMISC->modeOnlySphere)
	{
		radiusQ = sqrt(realRadius);
	}
	else
	{
		*indexRadius = getIndexRadius(realRadius, allocatedROM);

		// Read out the radius
		radiusQ = allocatedROM->radiusReconstruct[*indexRadius];
	}

	// Employ sphere quantizer
	for(i = 0; i < allocatedMISC->numberVectors;i++)
	{
		rtp_glcvq_quantize_sphere(&targetVector[i*allocatedROM->dimensionVQ], 
			//&outputVectorQ[i*allocatedROM->dimensionVQ], 
			allocatedMISC->vqOutVerify,
			radiusQ, &indexVector_NT[i], 
			realRadius+EPSILON_RADIUS, allocatedROM, allocatedRAM, allocatedMISC);

		rtp_glcvq_decode_sphere(&outputVectorQ[i*allocatedROM->dimensionVQ], 
			indexVector_NT[i], allocatedROM, allocatedRAM);

		for(j=0; j < allocatedROM->dimensionVQ;j++)
		{
			if(fabs(allocatedMISC->vqOutVerify[j] - outputVectorQ[i*allocatedROM->dimensionVQ+j]) > 0.000001)
				rtpPrintf("%f vs. %f \n",allocatedMISC->vqOutVerify[j],outputVectorQ[i*allocatedROM->dimensionVQ+j]);
		}
		
	}
	//decodeVectorSphere(vectorQ, *indexVector_NT, allocatedROM);

#ifdef NEW_SPHERE_Q
	temp1 = 0.0;
	for(i = 0; i < vectorLength; i++)
	{
		temp1+= (targetVector[i]/radiusIn)*outputVectorQ[i];
	}
#endif
	
	if(doRequantization)
	{
		metricNum = 0.0;
		metricDen = 0.0000000001;
		for(i = 0; i < vectorLength; i++)
		{
			metricNum += outputVectorQ[i]*targetVector[i];
			metricDen += outputVectorQ[i]*outputVectorQ[i];
		}
		radiusOpt = metricNum/metricDen;
		realRadius = radiusOpt*radiusOpt;
		*indexRadius = getIndexRadius(realRadius, allocatedROM);
		radiusQ = allocatedROM->radiusReconstruct[*indexRadius];
	}

	allocatedMISC->energyDiffRadius += (radiusQ- sqrt(realRadius))*(radiusQ- sqrt(realRadius));

	for(i = 0; i < vectorLength; i++)
	{
		absValVec += targetVector[i]/radiusIn*targetVector[i]/radiusIn;
		absValVecQ += (outputVectorQ[i]*outputVectorQ[i]);

#ifdef NEW_SPHERE_Q
		allocatedMISC->energyDiffSphere += (targetVector[i]/radiusIn-temp1*outputVectorQ[i])*
			(targetVector[i]/radiusIn-temp1*outputVectorQ[i]);
#else
		allocatedMISC->energyDiffSphere += (targetVector[i]/radiusIn-outputVectorQ[i])*(targetVector[i]/radiusIn-outputVectorQ[i]);
#endif

#ifdef DONT_QUANTIZE_RADIUS
		outputVectorQ[i] *= sqrt(realRadius);//radiusQ;
#else
		outputVectorQ[i] *= radiusQ;
#endif
		allocatedMISC->energyDifference += (targetVector[i]-outputVectorQ[i])*(targetVector[i]-outputVectorQ[i]);
	}
}

/** 
 * Function to quantize a signal vector employing a FULL vector codebook search
 *///=========================================================================================
void 
rtp_sc_glcvq_quantize_full(const DOUBLE* targetVector, DOUBLE* outputVectorQ, 
							MAX_WIDTH_TYPE* indexVector_NT, UWORD16* indexRadius, 
							UWORD16 vectorLength, UWORD16 doRequantization,
							void* handleROM, void* handleRAM, void* handleMISC)
{
	UWORD16 i;
	MAX_WIDTH_TYPE i64, iQ64;

	DOUBLE metricNum, metricDen;
	DOUBLE metricMax;

	gossetROM* allocatedROM = (gossetROM*)handleROM;
	gossetRAM* allocatedRAM = (gossetRAM*)handleRAM;
	gossetMISC* allocatedMISC = (gossetMISC*)handleMISC;
	
	UWORD16 cnt = 0;
	DOUBLE realRadius = 0; //EPSILON_RADIUS;
	DOUBLE radiusQ, radiusOpt;

	DOUBLE absValVec = 0.0;
	DOUBLE absValVecQ = 0.0;

	assert(allocatedROM->elementSize == sizeof(gossetROM));
	assert(allocatedRAM->elementSize == sizeof(gossetRAM));
	assert(allocatedMISC->elementSize == sizeof(gossetMISC));
	assert(allocatedMISC->numberVectors == 1);
	assert(vectorLength == (allocatedROM->dimensionVQ*allocatedMISC->numberVectors));

	for(cnt = 0; cnt < vectorLength; cnt++)
	{
		realRadius += (targetVector[cnt] * targetVector[cnt]);
	}

	// For performance measuring, apply 
	allocatedMISC->energyRadius += realRadius;
	allocatedMISC->energyOriginal += realRadius;

	if(sqrt(realRadius) > allocatedMISC->maxRadius)
		allocatedMISC->overload++;

	if(sqrt(realRadius) < allocatedMISC->maxRadius/allocatedMISC->aValue)
	{
		allocatedMISC->numRadiusLin++;
	}
	else
	{
		allocatedMISC->numRadiusLog++;
	}

	*indexRadius = getIndexRadius(realRadius, allocatedROM);

	// Read out the radius
	radiusQ = allocatedROM->radiusReconstruct[*indexRadius];

	//radiusQ = sqrt(realRadius);
	metricMax = -FLT_MAX;
	iQ64 = 0;

	allocatedMISC->numberFrames++;

	for(i64=0; i64 < allocatedROM->numberNodesValid; i64++)
	{
		metricNum = 0.0;
		metricDen = 0.0;

		decodeVectorSphereNew(outputVectorQ, i64, allocatedROM, allocatedRAM);

		for(i=0; i < allocatedROM->dimensionVQ; i++)
		{
			metricNum += (targetVector[i]/sqrt(realRadius))*outputVectorQ[i];
			//metricDen += outputVectorQ[i]*outputVectorQ[i];
		}
		if(metricNum > metricMax)
		{
			metricMax = metricNum;
			iQ64 = i64;
		}
	}

	indexVector_NT[0] = iQ64;

	decodeVectorSphereNew(outputVectorQ, 
			indexVector_NT[0], allocatedROM, allocatedRAM);

	if(doRequantization)
	{
		metricNum = 0.0;
		metricDen = 0.0000000001;
		for(i = 0; i < vectorLength; i++)
		{
			metricNum += outputVectorQ[i]*targetVector[i];
			metricDen += outputVectorQ[i]*outputVectorQ[i];
		}
		radiusOpt = metricNum/metricDen;
//		DBLOUT(&radiusOpt, 1, "radiusOpt.dat");
		realRadius = radiusOpt*radiusOpt;
		*indexRadius = getIndexRadius(realRadius, allocatedROM);
		radiusQ = allocatedROM->radiusReconstruct[*indexRadius];
//		radiusQ = radiusOpt;
//		DBLOUT(&radiusQ, 1, "radiusQ.dat");
	}

	allocatedMISC->energyDiffRadius += (radiusQ- sqrt(realRadius))*(radiusQ- sqrt(realRadius));

	for(i = 0; i < vectorLength; i++)
	{
		absValVec += targetVector[i]/sqrt(realRadius+0.000000001)*targetVector[i]/sqrt(realRadius+0.000000001);
		absValVecQ += (outputVectorQ[i]*outputVectorQ[i]);

		allocatedMISC->energyDiffSphere += (targetVector[i]/sqrt(realRadius+0.000000001)-outputVectorQ[i])*(targetVector[i]/sqrt(realRadius+0.000000001)-outputVectorQ[i]);
#ifdef DONT_QUANTIZE_RADIUS
		outputVectorQ[i] *= sqrt(realRadius);//radiusQ;
#else
		outputVectorQ[i] *= radiusQ;
#endif
		allocatedMISC->energyDifference += (targetVector[i]-outputVectorQ[i])*(targetVector[i]-outputVectorQ[i]);
	}
}

/** 
 * Function to quantize a normalized isgnal vector.
 *///=========================================================================================
void rtp_glcvq_quantize_sphere(const DOUBLE* inputVector, DOUBLE* outputVector, DOUBLE radiusQ, MAX_WIDTH_TYPE* indexVector_NT, 
  						   DOUBLE realRadius, gossetROM* allocatedROM, gossetRAM* allocatedRAM,
						   gossetMISC* allocatedMISC)
{
	static int cntAll = 0;
	WORD16 i, j,k;
	WORD16 idxMin, maxIdx;
	DOUBLE maxVal, metric;
	DOUBLE metricMax;
	WORD16 tempValErrIdx;
	WORD16 cnt,val;
	DOUBLE singleVal = 0.0;
	WORD16 singleValW16;
	WORD16 signParity = 0;
	DOUBLE metricMinNew =0;
	WORD32 permutationsClassRoot;
	WORD16 positionSignModify;
	MAX_WIDTH_TYPE idxOffset = 0;
	MAX_WIDTH_TYPE idxVecLocal = 0;
	MAX_WIDTH_TYPE numPerms;

	DOUBLE* ptrD1, *ptrD2, *ptrD3, *ptrD4;
	WORD16* ptrS1, *ptrS2,*ptrS3,*ptrS4,*ptrS5;
	//	DOUBLE tt1, tt2, tt3;

	//========================================
	// Separate absolute vector and signs
	//========================================
	ptrD1 = (DOUBLE*)inputVector;
	ptrD2 = allocatedRAM->copyEntriesNoSignCMPR;	// <- input vector but with signs removed
	ptrD4 = allocatedRAM->reorderedEntries;			// <- sign removed vector but in descending order
	ptrS1 = allocatedRAM->signsCMPR;				// <- signs of the input vector
	ptrS2 = allocatedRAM->copyEntriesSignsCMPR;		// <- Copy of signs in which the right order of the amplitudes is taken into account
	ptrS3 = allocatedRAM->reorderMatrix;			// <- Reorder matrix: From the ordered vector, how to reconstruct the not-ordered version

	signParity = 0;
	for(i=0; i < allocatedROM->dimensionVQ;i++)
	{
		// Absolute value of input vector
		*ptrD2++ = fabs(*ptrD1); // I1

		// Store the sign
		*ptrS1 = RTP_SIGN(*ptrD1++); // I2

		// Derive sign parity (not yet modulo 2)
		signParity += (*ptrS1++ < 0); // I3
	}

	// Do the ordering of the vector containing the absolute values. 
	// DIMENSIONVQ maximum values are searched. Once a maximum is found,
	// it is stored in the next free position in the target vector. The
	// value taken from the unordered vector is set to -FLTMAX and is hence never
	// going to be a maximum again.
	for(i=0; i < allocatedROM->dimensionVQ;i++)
	{
		maxVal = -FLT_MAX;
		maxIdx = -1;
		ptrD3 = allocatedRAM->copyEntriesNoSignCMPR;
		for(j = 0; j < allocatedROM->dimensionVQ; j++) // I4
		{
			if(*ptrD3 > maxVal)
			{
				maxVal = *ptrD3;
				maxIdx = j;
			}
			ptrD3++;
		}
		assert(maxIdx != -1);

		// I5
		*ptrD4++ = maxVal;

		*ptrS2++ = *(allocatedRAM->signsCMPR+maxIdx);
		*ptrS3++ = maxIdx;
		*(allocatedRAM->copyEntriesNoSignCMPR+maxIdx) = - FLT_MAX;
	}


	// Calculate the sign parity
	signParity = signParity%2;


	// Among all classleader root vectors, search the one with the MAXIMUM metric..
	metricMax = -FLT_MAX;
	idxMin = -1;
	positionSignModify = -1;

	for(i = 0; i < allocatedROM->numberClassRootLeadersCMPR; i++)
	{
		metric = 0.0;	

		// pointer to all non-zero amplitudes
		ptrS1 = *(allocatedROM->fieldAmplitudesRootCMPR+i);

		// pointer to the vector containing the ordered amplitudes
		ptrD1 = allocatedRAM->reorderedEntries;
			
		// Loop over all non-zero weights
		for(j = 0; j < allocatedROM->validWeightsRootCMPR[i]; j++)
		{

			// Loop over all amplitudes with the same amplitude
			for(k = 0; k < allocatedROM->fieldWeightsRootCMPR[i][j]; k++)
			{
				// Metric over all non-zero amplitudes
				metric += (DOUBLE)(*ptrS1) * (*ptrD1++);
			}

			// Skip to next type of amplitude/weight
			ptrS1++;
		}

		// Here we have the rule for the metric BEFORE applying the special rule 
		if((allocatedROM->modeClassRootLeaderCMPR[i] == 0)||
				((allocatedROM->modeClassRootLeaderCMPR[i] == 1)&&(allocatedROM->signParityClassRootLeaderCMPR[i]==signParity)))
		{

			// No sign modification is required
			tempValErrIdx = -1;
		}
		else
		{
			// Here the sign manipulation: The input vector and the classleader root vector
			// have a sign parity mismatch.
			// We go back to the last coordinate and consider the sign change in the metric as well as
			// we store the position at which the sign change will be done.
			// NOTE: We look for the last NON-zero coordinate in the classleader root vector.
			// However, there is NO ZERO coordinate in class B type vectors, therefore, it is always the 
			// DIMVQ-1 entry!
			ptrS1--;
			ptrD1--;
			metric -= 2* (DOUBLE)(*ptrS1) * (*ptrD1);
			tempValErrIdx = allocatedROM->dimensionVQ-1;
		}

		// Compare to max metric found so far
		if(metric > metricMax)
		{
			metricMax = metric;
			idxMin = i;
			positionSignModify = tempValErrIdx;
		}
	}
	assert(idxMin != -1);

	// If required, modify the ONE sign to match the sign parity constraint
	if(positionSignModify >= 0)
	{
		// Invert the sign for best candidate
		*(allocatedRAM->copyEntriesSignsCMPR + positionSignModify) *= -1; 
	}

	// ============= ============= ============= ============= =============
	// We have found the optimal vector. Next, transform what we found into an index
	// ============= ============= ============= ============= =============

	// Count the signs for each weight entry
	ptrS1 = allocatedRAM->copyEntriesSignsCMPR;
	ptrS2 = allocatedRAM->numberSignsPermCMPR;

	// Loop over weights/amplitudes
	for(j = 0; j < allocatedROM->validWeightsRootCMPR[idxMin]; j++)
	{
		*ptrS2 = 0;

		// Loop over all positions for the current weight
		for(k = 0; k < allocatedROM->fieldWeightsRootCMPR[idxMin][j]; k++)
		{
			*ptrS2 += ((*ptrS1++) == -1);
		}
		ptrS2++;
	}

	// Transform the sign distribution into an index and hence the choice of a classleader vector
	permutationsClassRoot = convertSignsToPermutationNumber_CPLX(allocatedRAM->numberSignsPermCMPR,
		allocatedROM->fieldWeightsRootCMPR[idxMin], allocatedROM->validWeightsRootCMPR[idxMin],
		allocatedROM->signParityClassRootLeaderCMPR[idxMin], allocatedROM->modeClassRootLeaderCMPR[idxMin]);
	
	// Derive the index offset for this classleader vector
	idxOffset = allocatedROM->offsetFieldClassRootsCMPR[idxMin][permutationsClassRoot];

	// Put the signs to the right position and consider reorder matrix
	// synthVectorUnsortedCMPR will hold the UNSORTED WORD16 codebook vector
	ptrS1 = *(allocatedROM->fieldAmplitudesRootCMPR+idxMin);
	ptrS2 = allocatedRAM->reorderMatrix;
	ptrS3 = allocatedRAM->copyEntriesSignsCMPR;

	for(j = 0; j < allocatedROM->validWeightsRootCMPR[idxMin]; j++)
	{
		singleValW16 = *ptrS1++;	
		for(k = 0; k < allocatedROM->fieldWeightsRootCMPR[idxMin][j]; k++)
		{
			// Combine the amplitudes with the signs by while considering reorder matrix
			allocatedRAM->synthVectorUnsortedCMPR[*ptrS2++] = singleValW16*(*ptrS3++);
		}
	}

	// Fill up the synth vector with zeros (where the input vector has its lowest amplitudes
	for(k = 0; k < allocatedROM->numberZerosRootCMPR[idxMin]; k++)
	{
		allocatedRAM->synthVectorUnsortedCMPR[*ptrS2++] = 0;
	}

	// Compute the weight vectors under consideration of the sign distribution
	cnt = 0;
	val = 0;
	ptrS1 = allocatedRAM->copyWeightsPermCMPR;
	ptrS2 = allocatedRAM->copyAmplsPermCMPR;
	ptrS3 = allocatedROM->fieldAmplitudesRootCMPR[idxMin];
	ptrS4 = allocatedROM->fieldWeightsRootCMPR[idxMin];
	ptrS5 = allocatedRAM->numberSignsPermCMPR;

	// Positive amplitudes
	for(j = 0; j < (allocatedROM->validWeightsRootCMPR[idxMin]); j++)
	{
		val = *ptrS4++ - *ptrS5++;
		if(val != 0)
		{
			*ptrS1++ = val;
			*ptrS2++ = *ptrS3;
			cnt++;
		}
		ptrS3++;
	}
	
	// Zeros
	if(allocatedROM->numberZerosRootCMPR[idxMin] != 0)
	{
		*ptrS1++ = allocatedROM->numberZerosRootCMPR[idxMin];
		*ptrS2++ = 0;
		cnt++;
	}

	// Negative amplitudes
	for(j = (allocatedROM->validWeightsRootCMPR[idxMin]-1); j >= 0 ; j--)
	{
		val = *(--ptrS5);
		ptrS3--;
		if(val != 0)
		{
			*ptrS1++ = val;
			*ptrS2++ = -(*ptrS3);
			cnt++;
		}
	}

	// Transform the unsorted vector and the weights/amplitudes into an index by means of the Schalkwijk Algorithm
#ifdef OLD_SCHALKWIJK
	idxVecLocal = schalkwijkEncoderW16(allocatedRAM->synthVectorUnsortedCMPR, allocatedROM->dimensionVQ, 
		allocatedRAM->copyWeightsPermCMPR, allocatedRAM->copyAmplsPermCMPR, 
		cnt, allocatedRAM, allocatedROM->divisorSchalkwijk);
#else

	// Calculate the number of permutations for Schalkwijk Encoder from stored offset fields.
	// By doing this, we can safe complexity since this number is known when entering the Schalkwijk algorithm!
	numPerms = allocatedROM->numberNodesValid;
	if(permutationsClassRoot == (allocatedROM->numberOffsetFieldClassRootsCMPR[idxMin]-1))
	{
		if(idxMin != allocatedROM->numberClassRootLeadersCMPR-1)
		{
			numPerms = allocatedROM->offsetFieldClassRootsCMPR[idxMin+1][0];
		}
	}
	else
	{
		numPerms = allocatedROM->offsetFieldClassRootsCMPR[idxMin][permutationsClassRoot+1];
	}
	numPerms -= allocatedROM->offsetFieldClassRootsCMPR[idxMin][permutationsClassRoot];

	// Enter Schalkwijk algorithm here
	idxVecLocal = schalkwijkEncoderW16(allocatedRAM->synthVectorUnsortedCMPR, allocatedROM->dimensionVQ, 
		allocatedRAM->copyWeightsPermCMPR, allocatedRAM->copyAmplsPermCMPR, 
		cnt, allocatedRAM, numPerms, allocatedROM->divisorSchalkwijkUW32);
#endif

	// This is the codevector for the sphere vector
	*indexVector_NT = idxOffset + idxVecLocal;

	// Construct the output vector
	// For complexity: We can store the vectors also in normalized version, in this case this division is not necessary!
	for(i = 0; i < allocatedROM->dimensionVQ; i++)
	{
		outputVector[i] = (DOUBLE)allocatedRAM->synthVectorUnsortedCMPR[i]*(1.0/allocatedROM->normalizationRadiusCMPR);
	}
}

/** 
 * Function to return the number of vectors located in the overload region of the quantizer.
 * Overload is if the radius is too large, the spherical part has no overload region
 *///=========================================================================================
MAX_WIDTH_TYPE 
FUNCTIONNAME(rtp_sc_glcvq_number_overload)(void* handleMisc)
{
	gossetMISC* allocatedMISC = (gossetMISC*) handleMisc;
	return(allocatedMISC->overload);
}

/** 
 * Function to return the number of vectors located in the linear region of the quantizer (kind of underload).
 * Underload is if the radius is too small, the spherical part has no overload region
 *///=========================================================================================
MAX_WIDTH_TYPE 
FUNCTIONNAME(rtp_sc_glcvq_number_lin_area)(void* handleMisc)
{
	gossetMISC* allocatedMISC = (gossetMISC*) handleMisc;
	return(allocatedMISC->numRadiusLin);
}

/** 
 * Function to return the number of vectors located in the logarithmic region of the quantizer (best region for quantization since snr is const).
 *///=========================================================================================
MAX_WIDTH_TYPE 
FUNCTIONNAME(rtp_sc_glcvq_number_log_area)(void* handleMisc)
{
	gossetMISC* allocatedMISC = (gossetMISC*) handleMisc;
	return(allocatedMISC->numRadiusLog);
}

/** 
 * Return the num ber of quantized frames
 *///=========================================================================================
MAX_WIDTH_TYPE 
gossetLCP_getNumberFrames(void* handleMisc)
{
	gossetMISC* allocatedMISC = (gossetMISC*) handleMisc;
	return(allocatedMISC->numberFrames);
}

/** 
 * Function to return the quantization SNR in dB
 *///=========================================================================================
DOUBLE 
FUNCTIONNAME(rtp_sc_glcvq_signal_snr_dB)(void* handleMisc)
{
	gossetMISC* allocatedMISC = (gossetMISC*) handleMisc;
	return(10*log10((allocatedMISC->energyOriginal+0.000001)/(allocatedMISC->energyDifference+0.000001)));
}

/** 
 * Function to return the quantization SNR in dB - only the normalized vectors (sphere signal)
 *///=========================================================================================
DOUBLE
FUNCTIONNAME(rtp_sc_glcvq_sphere_signal_snr_dB)(void* handleMisc)
{
	gossetMISC* allocatedMISC = (gossetMISC*) handleMisc;
	return(10*log10(((DOUBLE)allocatedMISC->numberFrames+0.000001)/(allocatedMISC->energyDiffSphere+0.000001)));
}

/** 
 * Function to return the quantization SNR in dB - only the quantization of the radius.
 *///=========================================================================================
DOUBLE 
FUNCTIONNAME(rtp_sc_glcvq_radius_snr_dB)(void* handleMisc)
{
	gossetMISC* allocatedMISC = (gossetMISC*) handleMisc;
	return(10*log10((allocatedMISC->energyRadius+0.000001)/(allocatedMISC->energyDiffRadius+0.000001)));
}

/** 
 * Function to return the effective bit rate for the overall quantizer
 *///=========================================================================================
DOUBLE
FUNCTIONNAME(rtp_sc_glcvq_effective_bitrate)(void* handleMISC)
{
	gossetMISC* allocatedMISC = (gossetMISC*) handleMISC;
	return(allocatedMISC->effectiveBitrate);
}

/** 
 * Function to return the number of reconstruction vectors/levels for sphere and radius, respectively.
 *///=========================================================================================
void 
FUNCTIONNAME(rtp_glcvq_number_indices)(MAX_WIDTH_TYPE *numNodes,
		      UWORD16 *numIndicesRadius,
		      void *handleROM) 
{
  gossetROM *allocatedROM = (gossetROM *)handleROM;
  *numNodes = allocatedROM->numberNodesValid;
  *numIndicesRadius = allocatedROM->numberIndicesRadius;
}

/** 
 * Function to decode one signal vector given the index for the sphere vector and the index for the radius.
 *///=========================================================================================
void 
FUNCTIONNAME(rtp_glcvq_decode)(DOUBLE *vector, 
		     MAX_WIDTH_TYPE* index, UWORD16 indexRadius, 
			 void *handleROM, void *handleRAM, void* handleMISC)
{
	int i;
	gossetROM *allocatedROM = (gossetROM *)handleROM;
	gossetRAM *allocatedRAM = (gossetRAM *)handleRAM;
	gossetMISC *allocatedMISC = (gossetMISC *)handleMISC;
	DOUBLE radius = allocatedROM->radiusReconstruct[indexRadius]; 
	
	// Decode one spherical vector after the other
	for(i = 0; i < allocatedMISC->numberVectors; i++)
	{
		rtp_glcvq_decode_sphere(&vector[i*allocatedROM->dimensionVQ], index[i], allocatedROM, allocatedRAM);
	}
	for(i = 0; i < allocatedROM->dimensionVQ; i++)
	{
		vector[i] *= radius;
	}
}

//============ ============ ============ ============ ============ ============
//============ ============ ============ ============ ============ ============
//============ ============ ============ ============ ============ ============
//============ ============ ============ ============ ============ ============
//============ ============ ============ ============ ============ ============

//=========================================================================
// HELPER FUNCTIONS
//=========================================================================

static double gammaN2(WORD16 n)
{
	assert(n > 1);

	if(n%2 == 0)
	{
		// even case:
		return((DOUBLE)factorialDbl(n/2-1));
	}

	return((sqrt(M_PI)*factorialDbl(n-2))/(pow(2.0, n-2)*factorialDbl((n-3)/2)));
}

static double calcNumberRadius(WORD16 n, double A, double Ks)
{
	double g1 = gammaN2(n+1);
	double g2 = gammaN2(n);
	double csh = ((DOUBLE)n-1.0)/((DOUBLE)n+1.0)*pow((2*sqrt(M_PI)*g1/g2),(2.0/((DOUBLE)n-1.0)));
    double c = 1/(1+log(A));
	double Kr;

    Kr =  pow(Ks,(1.0/((DOUBLE)n-1.0)))*pow((12.0*c*c*csh/((DOUBLE)n-1.0)),(-0.5));
	return(Kr);
}

MAX_WIDTH_TYPE powMWT(MAX_WIDTH_TYPE in, WORD16 exp)
{
	WORD16 i;
	MAX_WIDTH_TYPE res = 1;
	for(i = 0; i < exp;i++)
	{
		res *= in;
	}
	return(res);
}


DOUBLE readOutRadiusIndex(UWORD32 index, UWORD32 numberSteps, DOUBLE minV, DOUBLE maxV)
{
	DOUBLE value_quantized = 0.0;
	DOUBLE range = maxV-minV+2*EPSILON_U_Q;

	if(index >= numberSteps)
	{
		// Index out of range for radius quantization
		rtpPrintf("Error Radius!");
		assert(0);
	}

	value_quantized = minV+(((DOUBLE)2*index+1)/(2.0*(DOUBLE)numberSteps))*range;
	return(value_quantized);
}
	
DOUBLE convertLog2LinRadius(DOUBLE radiusLogQ, DOUBLE r_0, DOUBLE a_value, DOUBLE c)
{
	DOUBLE radiusQ = 0.0;
	if(radiusLogQ > r_0*c)
	//if(radiusLogQ > r_0/a_value)
	{
		radiusQ = r_0 * exp(((radiusLogQ/r_0)-1)/c);
	}
	else
	{
		radiusQ = radiusLogQ/(a_value*c);
	}
	return(radiusQ);
}

/*WORD16
getIndexRadius(DOUBLE realRadius, gossetROM* allocatedROM)
{
	UWORD16 startIndexLower = 0;
	UWORD16 startIndexUpper = 0;
	UWORD16 useIndex;
	DOUBLE entrySquaredBound = 0.0;

	startIndexLower = 0;
	startIndexUpper = allocatedROM->numberIndicesRadius;

	 // Bisection to find the radius index
	while((startIndexUpper-startIndexLower) != 1) 
	{
		useIndex = (startIndexUpper + startIndexLower)/2;
		entrySquaredBound = allocatedROM->radiusFldBoundsSquared[useIndex];
		if(realRadius > entrySquaredBound)
		{
			startIndexLower = useIndex;
		}
		else
		{
			startIndexUpper = useIndex;
		}
	}

	return(startIndexLower);
}
*/
/**
 * Given a radius to be quantized, tis function returns the 
 * quantization index!
 *///=========================================================
WORD16
getIndexRadius(DOUBLE realRadius, gossetROM* allocatedROM)
{
	UWORD16 startIndexLower = 0;
	UWORD16 startIndexUpper = 0;
	UWORD16 useIndex;
	DOUBLE entrySquaredBound = 0.0;

	startIndexLower = 0;	
	startIndexUpper = allocatedROM->numberIndicesRadius;

	 // Bisection to find the radius index
	while((startIndexUpper-startIndexLower) != 1) 
	{
		// While condition
		useIndex = ((startIndexUpper + startIndexLower)>>1);
		entrySquaredBound = *(allocatedROM->radiusFldBoundsSquared + useIndex);
		if(realRadius > entrySquaredBound)
		{			
			startIndexLower = useIndex;
		}
		else
		{			
			startIndexUpper = useIndex;
		}		
	}

	return(startIndexLower);
}

//=======================================================================
// >>>>>>>>>>>   Implementations of the SCHALKWIJK ALGORITHM <<<<<<<<<<<<
//=======================================================================

//%===========================================================
//% Schalkwijk Encoder Function, based on the paper
//% "An Algorithm for Source Coding", by J. Pieter and M. Schalkwijk,
//% IEEE Transactions IT, it-18, no 3, 05/1972
// This version is realized a little bit in a different way,
// employing the maximum number of permutations (multinomial coefficient)
// which is taken from lookup tables to save complexity.
// An update of the permutation number from dimension to dimension leads to a 
// really low complexity.
//% 1) vector consisting W different amplitudes
//% 2) weights containing W entries, each for a possible amplitude and
//%    corresponding weight
//% = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
//% Example: vector [2 2 1 1 0 0 1 2 1 0], weights = [2, 3; 1, 4; 0,3]
// Adaptation for C/C++: The weights are passed in two independent arrays
//%===========================================================
MAX_WIDTH_TYPE schalkwijkEncoderW16(const WORD16* field, WORD16 dimensionVQ, WORD16* fieldWeights, 
									WORD16* fieldAmplitudes, WORD16 numberWeightsUsed, gossetRAM* allocatedRAM,
									MAX_WIDTH_TYPE numberPermutations, UWORD32* divisor_1_DV)
{
	WORD16 i,j;
    MAX_WIDTH_TYPE index = 0;
	WORD16 den = dimensionVQ;
	WORD16 *ptrS1, *ptrS2;
	MAX_WIDTH_TYPE idxInc1 = 0;
	MAX_WIDTH_TYPE idxInc2 = 0;
	MAX_WIDTH_TYPE tmp =0;

	
	ptrS1 = fieldWeights;

	
	ptrS2 = allocatedRAM->copyWeightsSchalkwijk;

	
	for(i = 0; i < numberWeightsUsed; i++)
	{
		*ptrS2++ = *ptrS1++;
		//allocatedRAM->copyWeightsSchalkwijk[i] = fieldWeights[i];
	}

	// Just a check!
	assert(fabs(multinomialDbl(allocatedRAM->copyWeightsSchalkwijk, numberWeightsUsed) - (DOUBLE) numberPermutations) <= 0.0001);

	// always the same!
//	ptrD1 = divisor_1_DV+(dimensionVQ-1);

	
	for(i = 0; i < dimensionVQ; i ++)
	{
		// loop over all weights until the right one is found
        
		j = 0;

		
		ptrS1 = allocatedRAM->copyWeightsSchalkwijk;

		
		ptrS2 = fieldAmplitudes;

		
        while(((*ptrS2++) != field[i])&&(j < numberWeightsUsed))
		{
			
			
			
			
			
			
			
			 // Check while condition

            // Calculate the index BEFORE the matching weight is found
#ifdef SCHALKWIJK_FIXEDPOINT
			// For fixed point, this routine is deigned for 32 bit operation only!
			assert(numberPermutations <= (UWORD32)0xFFFFFFFF);
			index += (MAX_WIDTH_TYPE)mult32_32_16_48_round((UWORD32)numberPermutations, divisor_1_DV[dimensionVQ-i-1], allocatedRAM->copyWeightsSchalkwijk[j]);
#else
			index += rndREAL2MAXWIDTHTYPE((DOUBLE)numberPermutations * (DOUBLE)allocatedRAM->copyWeightsSchalkwijk[j] /((DOUBLE)(dimensionVQ-i)));
#endif

			
            j++;
		}
		assert(fieldAmplitudes[j] == field[i]);

#ifdef SCHALKWIJK_FIXEDPOINT
		assert(numberPermutations <= (UWORD32)0xFFFFFFFF);
		numberPermutations = (MAX_WIDTH_TYPE)mult32_32_16_48_round((UWORD32)numberPermutations, divisor_1_DV[dimensionVQ-i-1], allocatedRAM->copyWeightsSchalkwijk[j]);
#else
		numberPermutations = rndREAL2MAXWIDTHTYPE((DOUBLE)numberPermutations * (DOUBLE)allocatedRAM->copyWeightsSchalkwijk[j] /((DOUBLE)(dimensionVQ-i)));
#endif
        // Update weight because one is now reduced (from dim to (dim-1))
        
		
		
		
		*(allocatedRAM->copyWeightsSchalkwijk+j) -= 1;

        // Update normalization
        //den = den-1;
	}
    return(index);
}

//%===========================================================
//% Schalkwijk Decoder Function, based on the paper
//% "An Algorithm for Source Coding", by J. Pieter and M. Schalkwijk,
//% IEEE Transactions IT, it-18, no 3, 05/1972
// This version is realized a little bit in a different way,
// employing the maximum number of permutations (multinomial coefficient)
// which is taken from lookup tables to save complexity.
// An update of the permutation number from dimension to dimension leads to a 
// really low complexity.
//% = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
//% Example: vector [2 2 1 1 0 0 1 2 1 0], weights = [2, 3; 1, 4; 0,3]
// Adaptation for C/C++: The weights are passed in two independent arrays
//%===========================================================
void schalkwijkDecoderW16(WORD16* vecQ, MAX_WIDTH_TYPE idx, WORD16 dimensionVQ, WORD16* fieldWeights, 
					   WORD16* fieldAmplitudes, WORD16 numberWeightsUsed, gossetRAM* allocatedRAM, 
					   MAX_WIDTH_TYPE numberPermutations, UWORD32* divisor_1_DV)
{
	WORD16 i,j;
	MAX_WIDTH_TYPE idxOffset;
	MAX_WIDTH_TYPE idxCount;

	WORD16* ptrS1, *ptrS2;

	ptrS1 = allocatedRAM->copyWeightsSchalkwijk;
	ptrS2 = fieldWeights;
		
	for( i = 0; i < numberWeightsUsed; i++)
	{
		*ptrS1++ = *ptrS2++;
	}
	
    assert((idx < numberPermutations)||(idx >= 0));
	//	ptrD1 = &divisor_1_DV[dimensionVQ-1];
	ptrS2 = vecQ;
    
	//% Loop over all coordinates
	for(i = 0; i < dimensionVQ; i++)
	{
        // Calculate constant based on multinomial coefficient and normalize
		
        idxOffset = 0;
		j = 0;    
       
#ifdef SCHALKWIJK_FIXEDPOINT
      assert(numberPermutations <= (UWORD32)0xFFFFFFFF);
      idxCount = (MAX_WIDTH_TYPE)mult32_32_16_48_round((UWORD32)numberPermutations,
						       divisor_1_DV[dimensionVQ-i-1],
						       allocatedRAM->copyWeightsSchalkwijk[j]);
#else
      idxCount = rndREAL2MAXWIDTHTYPE ((DOUBLE)numberPermutations * (DOUBLE)allocatedRAM->copyWeightsSchalkwijk[j] /((DOUBLE)(dimensionVQ-i)));
#endif
      
	  while(idxCount <= idx)
	  {     
		  j++;

		  idxOffset = idxCount;

#ifdef SCHALKWIJK_FIXEDPOINT
		  assert(numberPermutations <= (UWORD32)0xFFFFFFFF);

		  idxCount += (MAX_WIDTH_TYPE)mult32_32_16_48_round((UWORD32)numberPermutations,
			  divisor_1_DV[dimensionVQ-i-1],
			  allocatedRAM->copyWeightsSchalkwijk[j]);
#else
		  idxCount += rndREAL2MAXWIDTHTYPE ((DOUBLE)numberPermutations * (DOUBLE)allocatedRAM->copyWeightsSchalkwijk[j] /((DOUBLE)(dimensionVQ-i)));
#endif

	  }

	  idx -= idxOffset;

#ifdef SCHALKWIJK_FIXEDPOINT
	  assert(numberPermutations <= (UWORD32)0xFFFFFFFF);
	  numberPermutations = (MAX_WIDTH_TYPE)mult32_32_16_48_round((UWORD32)numberPermutations, divisor_1_DV[dimensionVQ-i-1], allocatedRAM->copyWeightsSchalkwijk[j]);
#else
	  numberPermutations = rndREAL2MAXWIDTHTYPE((DOUBLE)numberPermutations * (DOUBLE)allocatedRAM->copyWeightsSchalkwijk[j] /((DOUBLE)(dimensionVQ-i)));
#endif		

	  *ptrS2++ = *(fieldAmplitudes+j);

	  // Update weight because one is now reduced (from dim to (dim-1))
	  *(allocatedRAM->copyWeightsSchalkwijk+j) -= 1;
	}
}

//=======================================================================
// HELPER FUNCTIONS
//=======================================================================

DOUBLE calculatePermutationsDblNew(WORD16* weights, WORD16 numWeights)
{
	DOUBLE num = 0;
	DOUBLE den = 1;
	WORD16 i=0;

#ifdef NEW_MULTINOMIAL
	WORD16 j=0;
	WORD16 cntFacNum = weights[0];
	DOUBLE fraction = 0.0;
	DOUBLE fractionAll = 1.0;

	// From 1 to end because the first weight leads to a one anyway
	for(i = 1; i < numWeights; i++)
	{
		for(j=1; j <= weights[i];j++)
			fractionAll *= ((DOUBLE)(cntFacNum+j))/((DOUBLE)(j));
		cntFacNum += weights[i];
	}
#else
	WORD16 cntFacNum = 0;

	DOUBLE fraction = 0.0;
	DOUBLE fractionAll = 1.0;

	for(i = 0; i < numWeights; i++)
	{
		fraction = factorialDblStSt(cntFacNum+1,cntFacNum+weights[i])/factorialDbl(weights[i]);
		fraction *= pow((DOUBLE)cntFacNum+1,weights[i]); 
		cntFacNum += weights[i];
		fractionAll *= fraction;
	}
#endif
	return(fractionAll);
}

/*
DOUBLE calculatePermutationsDblNew_CPLX(WORD16* weights, WORD16 numWeights, DOUBLE* divisor_1_DV)
{
	DOUBLE num = 0;
	DOUBLE den = 1;
	WORD16 i=0;

#ifdef NEW_MULTINOMIAL

	WORD16 j=0;
	WORD16 cntFacNum = *weights;
	DOUBLE fraction = 0.0;
	DOUBLE fractionAll = 1.0;

	
	
	
	


	// From 1 to end because the first weight leads to a one anyway	
	
	for(i = 1; i < numWeights; i++)
	{
		
		for(j=1; j <= weights[i];j++)
		{
			//fractionAll *= ((DOUBLE)(cntFacNum+j))/((DOUBLE)(j));
			
			
			
			
			
			fractionAll *= ((DOUBLE)(cntFacNum+j))*divisor_1_DV[j-1];
			
			
		}

		cntFacNum += weights[i];
		
	}
#else
	WORD16 cntFacNum = 0;

	DOUBLE fraction = 0.0;
	DOUBLE fractionAll = 1.0;

	for(i = 0; i < numWeights; i++)
	{
		fraction = factorialDblStSt(cntFacNum+1,cntFacNum+weights[i])/factorialDbl(weights[i]);
		fraction *= pow((DOUBLE)cntFacNum+1,weights[i]); 
		cntFacNum += weights[i];
		fractionAll *= fraction;
	}
#endif
	return(fractionAll);
}
*/

DOUBLE multinomialDbl(WORD16* weights, WORD16 numWeights)
{
	return(calculatePermutationsDblNew(weights, numWeights));
}

/*
DOUBLE multinomialDbl_CPLX(WORD16* weights, WORD16 numWeights, DOUBLE* divisor_1_DV)
{
	return(calculatePermutationsDblNew_CPLX(weights, numWeights, divisor_1_DV));
}
*/

//==================================================================
// Static functions: Factorial in DOUBLE
DOUBLE factorialDbl(WORD16 input)
{
	WORD16 i;
	DOUBLE res = 1;
	for(i = input; i > 0; i--)
		res *= (DOUBLE)i;
	return(res);
}

//==================================================================
// Static functions: Factorial with start and end, also this function 
// expects factorial(x,y) = factorialDblStSt(x,y) * pow(x, (y-x))
// for better numeric behavior for large vector lengths
DOUBLE factorialDblStSt(WORD16 inputStart, WORD16 inputStop)
{
	WORD16 i;
	DOUBLE res = 1;
	for(i = inputStop; i >= inputStart; i--)
		res *= (DOUBLE)i/(DOUBLE)inputStart;
	return(res);
}


WORD32 calculateClassLeaderNew(WORD16 sphereIndexTest, WORD16* numberWeightsMax, WORD16 dimensionVQ, 
							   WORD16 functionCallMode /*0: first call, 1: second call*/,
							   WORD32* idxClassLeader, /*WORD16** clsLVectors,*/ WORD16* validWeights, 
							   WORD16** weights, WORD16** ampls, WORD16* zeros, WORD16* modes,
							   WORD16* signs)
{
	WORD32 numVectors = 0;
	WORD32 radius = 0;
	WORD16 maxVal = 0;
	WORD16 ii;
	WORD16 tmp;
	WORD16* vector = (WORD16*)malloc(sizeof(WORD16)*dimensionVQ);
#ifdef GRAN_8
	radius = 2*sphereIndexTest;
	tmp = 0;
#else
	radius = 2*sphereIndexTest + (dimensionVQ/4)%2;
	tmp = 2*((dimensionVQ/4)%2);
#endif
	maxVal = (WORD16)floor(sqrt((DOUBLE)radius));
	for(ii = maxVal; ii >= 0; ii--)
	{
		vector[0] = ii;
		numVectors += calculateClassLeaderNewRecursive(vector, 1, dimensionVQ, 0, tmp, 4*(radius), ii, numberWeightsMax, functionCallMode,
			idxClassLeader, /*clsLVectors,*/ validWeights, weights, ampls, zeros, modes, signs);
		numVectors += calculateClassLeaderNewRecursive(vector, 1, dimensionVQ, 1, (dimensionVQ)%4, 4*radius, ii, numberWeightsMax,
			functionCallMode, idxClassLeader, /*clsLVectors,*/ validWeights, weights, ampls, zeros, modes, signs);
	}
	free(vector);
	return(numVectors);
}

WORD32 calculateClassLeaderNewRecursive(WORD16* vector, WORD16 curDim, WORD16 dimensionVQ, WORD16 mode, WORD16 modExpr, 
										WORD32 radiusSQR, WORD16 xVal, WORD16* numberWeightsMax, WORD16 functionCallMode,
										WORD32* idxClassLeader, /*WORD16** clsLVectors,*/ WORD16* validWeights, WORD16** weights, 
										WORD16** ampls, WORD16* zeros, WORD16* modes, WORD16* signs)
{
#ifdef GLCVQ_VERBOSE_MODE
	WORD16 hh;
#endif

	WORD16 ii;
	WORD32 localCount = 0;
	WORD16 weightsLocal =0;
	WORD16 sign = 0;

	if(curDim < (dimensionVQ-1))
	{
		for(ii = xVal; ii >= 0; ii--)
		{
			vector[curDim] = ii;
			if(testCondition1(vector, curDim, mode, radiusSQR) == 0)
				continue;
			else
				localCount += calculateClassLeaderNewRecursive(vector, curDim+1, dimensionVQ, 
					mode, modExpr, radiusSQR, ii, numberWeightsMax, functionCallMode, 
					idxClassLeader, /*clsLVectors,*/ validWeights, weights, ampls, zeros, modes, signs);
		}
	}
	else
	{
		for(ii = xVal; ii >= 0; ii--)
		{
			vector[curDim] = ii;

#ifdef GLCVQ_VERBOSE_MODE
			if(functionCallMode == 0)
			{
				rtpPrintf("GLCVQ: Testing vector: ");
				for(hh = 0; hh <= curDim; hh++)
				{
					if(mode == 0)
					{
						rtpPrintf("[%d]", vector[hh]*2);
					}
					else
					{
						rtpPrintf("[%d]", vector[hh]*2+1);
					}
				}
			}
#endif
			if(testCondition2(vector, curDim, mode, radiusSQR))
			{
#ifdef GLCVQ_VERBOSE_MODE
				if(functionCallMode == 0)
				{
					rtpPrintf("+Radius+cond+");
				}
#endif

				// Here, the radius has been exactly hit
				if(testCondition3(vector, curDim, mode, modExpr,&sign))
				{
#ifdef GLCVQ_VERBOSE_MODE
					if(functionCallMode == 0)
					{
						rtpPrintf("+Lattice+cond+");
					}
#endif
					/*if(functionCallMode == 0)
					{
						if((mode == 1)&&(sign == 1))
						{ 
							rtpPrintf("%%%%%%%%%%%%%%%%%%%%%Vector geht nicht!\n");
							for(hh = 0; hh <= curDim; hh++)
							{
								rtpPrintf("[%d]", vector[hh]);
							}
							rtpPrintf("\n");
						}
						else
						{ 
							rtpPrintf("%%%%%%%%%%%%%%%%%%%%%Vector geht!\n");
							for(hh = 0; hh <= curDim; hh++)
							{
								rtpPrintf("[%d]", vector[hh]);
							}
							rtpPrintf("\n");
						}
					}*/
#ifdef GLCVQ_VERBOSE_MODE
					if(functionCallMode == 0)
					{
						rtpPrintf("---> Classleader root");
					}
#endif
					// Identified new class leader vector here
					localCount++;

					// Calculate the number of zeros and other coordinates in valid classleader root vector
					if(functionCallMode == 0)
					{
						calculateWeightsZeros(vector, &weightsLocal, dimensionVQ, mode, functionCallMode,
							/*NULL,*/ NULL, NULL, NULL);
					}
					else
					{
						calculateWeightsZeros(vector, &weightsLocal, dimensionVQ, mode, functionCallMode,
							/*clsLVectors[*idxClassLeader],*/ weights[*idxClassLeader], ampls[*idxClassLeader], 
							&zeros[*idxClassLeader]);
					}

					// 
					if(functionCallMode == 1)
					{
						validWeights[*idxClassLeader] = weightsLocal;
						modes[*idxClassLeader] = mode;
						signs[*idxClassLeader] = sign;
						(*idxClassLeader)++;
					}
					if(weightsLocal > *numberWeightsMax)
						*numberWeightsMax = weightsLocal;
				}
			}
#ifdef GLCVQ_VERBOSE_MODE
			if(functionCallMode == 0)
			{
				rtpPrintf("\n");
			}
#endif
		}
	}
	return(localCount);
}

/**
 * Test the radius condition to EXCLUDE vector candidates early
 *///============================================================
// ====================================================================
WORD16 testCondition1(WORD16* vector, WORD16 curDim, WORD16 mode, WORD32 radiusSQR)
{
	WORD16 ii;
	WORD16 tempRadius = 0;

	if(mode == 0)
	{
		for(ii = 0; ii <= curDim; ii++)
		{
			tempRadius += (2*vector[ii]*2*vector[ii]);
		}
	}
	else
	{
		for(ii = 0; ii <= curDim; ii++)
		{
			tempRadius += ((2*vector[ii]+1)*(2*vector[ii]+1));
		}
	}
	if(tempRadius > radiusSQR)
		return(0);
	return(1);
}

/**
 * Test the radius condition to find valid vector candidates once all positions are known
 *///============================================================
// ====================================================================
WORD16 testCondition2(WORD16* vector, WORD16 curDim, WORD16 mode, WORD32 radiusSQR)
{
	WORD16 ii;
	WORD16 tempRadius = 0;
	if(mode == 0)
	{
		for(ii = 0; ii <= curDim; ii++)
		{
			tempRadius += (2*vector[ii])*(2*vector[ii]);
		}
	}
	else
	{
		for(ii = 0; ii <= curDim; ii++)
		{
			tempRadius += ((2*vector[ii]+1)*(2*vector[ii]+1));
		}
	}
/*
	if(mode == 0)
	{
		rtpPrintf("Radius: %d\n", tempRadius);
	}
	*/
	if(tempRadius == radiusSQR)
		return(1);
	return(0);
}

/**
 * Test the Lattice condition
 *///============================================================
// ====================================================================
WORD16 testCondition3(WORD16* vector, WORD16 curDim, WORD16 mode, WORD16 modExpr, 
					  WORD16* chosenSign)
{
	WORD16 ii;
	WORD16 tempExpr = 0;
	WORD16 invCons = 0;

	if(mode == 0)
	{
		for(ii = 0; ii <= curDim; ii++)
		{
			tempExpr += 2*vector[ii];
		}
		if(tempExpr%4 == modExpr)
		{
			*chosenSign = 0;
			return(1);
		}
	}
	else
	{
		for(ii = 0; ii <= curDim; ii++)
		{
			tempExpr += 2*vector[ii]+1;
		}
		if(tempExpr%4 == modExpr)
		{
			*chosenSign = 0;
			return(1);
		}
		tempExpr -= 2*(2*vector[curDim]+1);
		if(tempExpr%4 == modExpr)
		{
			*chosenSign = 1;
			return(1);
		}
	}
	return(0);
}

/**
 * Compute the weights for zeros and non-zero coordinates
 *///============================================================
// ====================================================================
WORD16 calculateWeightsZeros(WORD16* vector, WORD16* weightsLocal, WORD16 dimVQ, WORD16 mode, WORD16 functionCallMode,
							 /*WORD16* clsLVector,*/ WORD16* weights, WORD16* ampls, WORD16* zeros)
{
	WORD16 ii = 0;
	WORD16 oldValue = vector[0];
	WORD16 cntW = 1;

	*weightsLocal = 0;

	if(functionCallMode == 1)
	{
		// Set number zeros top 0 as default
		*zeros = 0;
		/*
		if(mode == 0)
		{
			clsLVector[0] = 2*vector[0];
		}
		else
		{
			clsLVector[0] = 2*vector[0]+1;
		}*/
	}
	for(ii = 1; ii < dimVQ; ii++)
	{
		if(functionCallMode == 1)
		{
/*
			if(mode == 0)
			{
				clsLVector[ii] = 2*vector[ii];
			}
			else
			{
				clsLVector[ii] = 2*vector[ii]+1;
			}
			*/
		}
		if(vector[ii] == oldValue)
		{
			cntW++;
		}
		else
		{
			if((mode==0)&&(oldValue == 0))
			{
				// Consider zeros in a special rule
				// Zeros only occur in mode 0
				if(functionCallMode == 1)
				{
					*zeros = cntW;
				}
			}
			else
			{
				if(functionCallMode == 1)
				{
					weights[*weightsLocal] = cntW;
					if(mode == 0)
						ampls[*weightsLocal] = 2*oldValue;
					else
						ampls[*weightsLocal] = 2*oldValue+1;
				}
				(*weightsLocal)++;
			}
			oldValue = vector[ii];
			cntW = 1;
		}
	}

	// special rule following the very last coordinate
	if((mode==0)&&(oldValue == 0))
	{
		// Consider zeros in a special rule
		// Zeros only occur in mode 0
		if(functionCallMode == 1)
		{
			*zeros = cntW;
		}
	}
	else
	{
		if(functionCallMode == 1)
		{
			weights[*weightsLocal] = cntW;
			if(mode == 0)
				ampls[*weightsLocal] = 2*oldValue;
			else
				ampls[*weightsLocal] = 2*oldValue+1;
		}
		(*weightsLocal)++;
	}
	return(1);
}

// ====================================================================
WORD32 calcClassLeaderPermutations(WORD16* weights, WORD16 numWeights, WORD16 mode, WORD16 sign,
								   WORD16 functionCallMode, WORD32* numberPermutationsClassRoot,
								   WORD16* copyWeightsPermCMPRRAM, WORD16* numberSignsPermCMPRRAM,
								   WORD16 numberZeros, MAX_WIDTH_TYPE* offsetIdxClassRootsCMPR,
								   MAX_WIDTH_TYPE* offsetFieldClassRootsCMPR, MAX_WIDTH_TYPE* offsetIndices,
								   // MAX_WIDTH_TYPE* offsetFieldClassRootsCMPRPositive, MAX_WIDTH_TYPE* offsetIndicesPositive, MAX_WIDTH_TYPE* offsetIndicesPositiveOld, 
								   MAX_WIDTH_TYPE* numberPossibleIndices)
{
	WORD16 ii,i;
	WORD32 numPerms = 1;
	WORD16 val, cnt;
	MAX_WIDTH_TYPE localPerms = 0;

	// Compute the number of permutations due to signs, e.g. weights[0] = 2; weights[1] = 3
	// ++ +++, +- +++, -- +++, 
	// ++ ++-, +- ++-, -- ++-,
	// ...
	// ++ ---, +- ---, -- --- 
	for(ii = 0; ii < numWeights; ii++)
	{
		numPerms *= (weights[ii]+1);
	}

	// If mode is 1 (class B classleader root vectors), we need to consder the sign parity
	if(mode == 1)
	{
		if(numPerms%2)
		{
			if(sign)
				numPerms = ((numPerms-1)/2);
			else
				numPerms =((numPerms+1)/2);
		}
		else
			numPerms = ((numPerms)/2);
	}

	// At first call, return the number of permutations and hence produced classleaser vectors
	if(functionCallMode == 1)
	{
		*numberPermutationsClassRoot = numPerms;
	}

	// During second call, allocate the offset table, one for each classleader vector
	if(functionCallMode == 2)
	{
#ifdef GLCVQ_VERBOSE_MODE
		rtpPrintf("GLCVQ: --> Number sign permutations: %d\n", numPerms);
#endif
		// Set offset to very first index
		*offsetIdxClassRootsCMPR = *offsetIndices;	
		for(i = 0; i < numPerms; i++)
		{
			offsetFieldClassRootsCMPR[i] = *offsetIndices;	
			transformIdxSigns(i, mode, sign, weights, numWeights,numberSignsPermCMPRRAM);

			// Transform weights for classleadr root vector into weights for classleader vector
			cnt = 0;
			val = 0;
			for(ii = 0; ii < (numWeights); ii++)
			{
				val = weights[ii] - numberSignsPermCMPRRAM[ii];
				if(val != 0)
					copyWeightsPermCMPRRAM[cnt++] = val;
			}
			if(numberZeros != 0)
			{
				copyWeightsPermCMPRRAM[cnt++] = numberZeros;
			}
			for(ii = (numWeights-1); ii >= 0 ; ii--)
			{
				val = numberSignsPermCMPRRAM[ii];
				if(val != 0)
					copyWeightsPermCMPRRAM[cnt++] = val;
			}
#ifdef GLCVQ_VERBOSE_MODE
			int iii;
			rtpPrintf("GLCVQ: ----> Weights for classleader vector[%d] (mode = %d, sign = %d):", i, mode, sign);
			for(iii = 0; iii < cnt; iii++)
			{
				rtpPrintf("[%d]", copyWeightsPermCMPRRAM[iii]);
			}
#endif

			 localPerms = rndREAL2MAXWIDTHTYPE(calculatePermutationsDblNew(copyWeightsPermCMPRRAM, cnt));
#ifdef GLCVQ_VERBOSE_MODE
			 rtpPrintf(":: Number permutations: %llu\n", localPerms);
#endif
			 // Assertion to make sure that the maximum bit width is not exceded
			 if(*numberPossibleIndices - localPerms < *offsetIndices)
			 {
				 rtpPrintf("Bit rate can not be handled by 32 bit data types during VQ generation procedure. Switch to 64 bit and try again!\n");
				 exit(0);
			 }
			 *offsetIndices += localPerms;
		}
	}
	return(numPerms);
}

WORD32 calcClassLeaderPermutationsShort(WORD16* weights, WORD16 numWeights, WORD16 mode, WORD16 sign)
{
	WORD16 ii;
	WORD32 numPerms = 1;

	for(ii = 0; ii < numWeights; ii++)
	{
		numPerms *= (weights[ii]+1);
	}
	if(mode == 1)
	{
		if(numPerms%2)
		{
			if(sign)
				numPerms = ((numPerms-1)/2);
			else
				numPerms =((numPerms+1)/2);
		}
		else
			numPerms = ((numPerms)/2);
	}
	return(numPerms);
}

WORD32 calcClassLeaderPermutationsShort_CPLX(WORD16* weights, WORD16 numWeights, WORD16 mode, WORD16 sign)
{
	WORD16 ii;
	WORD32 numPerms = 1;
	WORD16* ptrS1;

	
	ptrS1 = weights;

	
	for(ii = 0; ii < numWeights; ii++)
	{
		
		
		
		
		numPerms *= ((*ptrS1++)+1);

		
	}

	
	
	
	if(mode == 1)
	{
		
		
		
		if(numPerms%2)
		{
			
			
			if(sign)
			{
				
				
				numPerms = ((numPerms-1)>>1);
			}
			else
			{
				
				
				numPerms =((numPerms+1)>>1);
			}
		}
		else
		{
			
			numPerms = ((numPerms)>>1);
		}
	}
	return(numPerms);
}

/*
void transformIdxSigns(WORD32 idx, WORD16 mode, WORD16 sign, WORD16* weights, WORD16 numWeights, WORD16* numberSignsPermCMPR)
{
	WORD16 ii = 0;
	WORD16 newSign = 0;
	WORD32 subNumber = 0;
	WORD16 idxNew = 0;

	WORD16 grid = 1;
	if(mode == 1)
		grid = 2;
	
	// Important here: Make sure sign is 0 for Mode0!!

	if(numWeights > 1)
	{
		for(ii = 0; ii < (weights[0]+1); ii++)
		{
			newSign = (ii+sign)%grid;
			subNumber = calcClassLeaderPermutationsShort(&weights[1], (numWeights-1), mode, newSign);
			idx = idx-subNumber;
			if(idx < 0)
			{
				idx = idx+subNumber;
				break;
			}
		}
		*numberSignsPermCMPR = ii;
		transformIdxSigns(idx, mode, newSign, &weights[1], (numWeights-1), &numberSignsPermCMPR[1]);
	}
	else
	{
		*numberSignsPermCMPR = (WORD16)idx*grid+sign;
	}
}
*/		
void transformIdxSigns(WORD32 idx, WORD16 mode, WORD16 sign, WORD16* weights, WORD16 numWeights, WORD16* numberSignsPermCMPR)
{
	WORD16 ii = 0;
	WORD16 newSign = 0;
	WORD32 subNumber = 0;
	WORD16 idxNew = 0;

	WORD16 grid = 1;

	WORD16 weightCounter = 0;
	WORD16 *ptrS1, *ptrS2;

	if(mode == 1)
	{
		grid = 2;
	}

	weightCounter = 0;
	ptrS1 = weights;
	ptrS2 = numberSignsPermCMPR;

	for(weightCounter = 0; weightCounter < (numWeights-1); weightCounter++)
	{
		for(ii = 0; ii < (*ptrS1)+1; ii++)
		{
			newSign = (ii+sign)%grid;
			subNumber = calcClassLeaderPermutationsShort_CPLX((ptrS1+1), (numWeights-1-weightCounter), mode, newSign);
			idx = idx-subNumber;
			if(idx < 0)
			{
				idx = idx+subNumber;
				break;
			}
		}
		ptrS1++;
		*ptrS2++ = ii;
		sign = newSign;
	}
	*ptrS2++ = (WORD16)idx*grid+sign;
}

void allocateGossetSVQ_CMPR(WORD16 sphereIndex, gossetROM* allocatedROM, gossetRAM* allocatedRAM,
							WORD32* numberClassRootLeaders, MAX_WIDTH_TYPE* numPermPerm, WORD16 functionCallID,
							MAX_WIDTH_TYPE* numBitsROM, MAX_WIDTH_TYPE* numBitsRAM, WORD16 numberVectors, MAX_WIDTH_TYPE numberPossibleIndices)
{
	WORD16 i;//,j;

	MAX_WIDTH_TYPE bitsROM = 0;
	MAX_WIDTH_TYPE bitsRAM = 0;

	//========================================================================

//	WORD16 numberWeightsMax = 0;
	WORD32 idxClassLeader = 0;
	WORD32 classLeadersCMPR;
	MAX_WIDTH_TYPE numberVectorPermutationsCMPR = 0;
//	MAX_WIDTH_TYPE offsetIndicesPositiveOld;

	if(numBitsROM == NULL)
		numBitsROM = &bitsROM;
	if(numBitsRAM == NULL)
		numBitsRAM = &bitsRAM;

	allocatedROM->numberWeightsMaxCMPR = 0;

	allocatedROM->normalizationRadiusCMPR = sqrt((DOUBLE)numberVectors*8.0*(DOUBLE)sphereIndex);
	*numBitsROM += SIZE_MAX_WIDTH_TYPE;

	/*******************************************************
	* NEW VERSION OF GOSSET SVQ
	******************************************************/

	// - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Step I: Compute the number of classleader root vectors
	// - - - - - - - - - - - - - - - - - - - - - - - - - -
	*numberClassRootLeaders = calculateClassLeaderNew(sphereIndex, &allocatedROM->numberWeightsMaxCMPR, allocatedROM->dimensionVQ, 0,
		NULL, /*NULL,*/ NULL, NULL, NULL, NULL, NULL, NULL);

#ifdef GLCVQ_VERBOSE_MODE
	rtpPrintf("GLCVQ: Number of classleader root vectors: %d\n", *numberClassRootLeaders);
#endif

	// - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Step II: Allocate memory for second run
	// - - - - - - - - - - - - - - - - - - - - - - - - - -
	//	allocatedROM->fieldClassRootLeadersCMPR = (WORD16**)malloc(sizeof(WORD16*)* (*numberClassRootLeaders));
	allocatedROM->validWeightsRootCMPR = (WORD16*)malloc(sizeof(WORD16)*(*numberClassRootLeaders));
	*numBitsROM += SIZE_WORD16**numberClassRootLeaders;
	
	allocatedROM->fieldWeightsRootCMPR = (WORD16**)malloc(sizeof(WORD16*)*(*numberClassRootLeaders));
	*numBitsROM += SIZE_POINTER* *numberClassRootLeaders;
	
	allocatedROM->fieldAmplitudesRootCMPR = (WORD16**)malloc(sizeof(WORD16*)*(*numberClassRootLeaders));
	*numBitsROM += SIZE_POINTER* *numberClassRootLeaders;
	
	allocatedROM->numberZerosRootCMPR = (WORD16*)malloc(sizeof(WORD16)*(*numberClassRootLeaders));
	*numBitsROM += SIZE_WORD16* *numberClassRootLeaders;

	allocatedROM->modeClassRootLeaderCMPR = (WORD16*)malloc(sizeof(WORD16)*(*numberClassRootLeaders));
	*numBitsROM += SIZE_WORD16* *numberClassRootLeaders;
	
	allocatedROM->signParityClassRootLeaderCMPR = (WORD16*)malloc(sizeof(WORD16)*(*numberClassRootLeaders));
	*numBitsROM += SIZE_WORD16**numberClassRootLeaders;

	for(i =0; i < *numberClassRootLeaders;i++)
	{
		allocatedROM->fieldWeightsRootCMPR[i] = (WORD16*)malloc(sizeof(WORD16)*allocatedROM->numberWeightsMaxCMPR);
		*numBitsROM += SIZE_WORD16*allocatedROM->numberWeightsMaxCMPR;
		
		allocatedROM->fieldAmplitudesRootCMPR[i] = (WORD16*)malloc(sizeof(WORD16)*allocatedROM->numberWeightsMaxCMPR);
		*numBitsROM += SIZE_WORD16*allocatedROM->numberWeightsMaxCMPR;
//		allocatedROM->fieldClassRootLeadersCMPR[i] = (WORD16*)malloc(sizeof(WORD16)*allocatedROM->dimensionVQ);
	}

	idxClassLeader = 0;

	// - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Step III: Fill allocated memory in second run
	// - - - - - - - - - - - - - - - - - - - - - - - - - -
	*numberClassRootLeaders = calculateClassLeaderNew(sphereIndex, &allocatedROM->numberWeightsMaxCMPR, allocatedROM->dimensionVQ, 1,
		&idxClassLeader, /*allocatedROM->fieldClassRootLeadersCMPR,*/ allocatedROM->validWeightsRootCMPR,
		allocatedROM->fieldWeightsRootCMPR, allocatedROM->fieldAmplitudesRootCMPR, allocatedROM->numberZerosRootCMPR,
		allocatedROM->modeClassRootLeaderCMPR, allocatedROM->signParityClassRootLeaderCMPR);

	// - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Step IV: Prepare tables for index offset for index computations
	// - - - - - - - - - - - - - - - - - - - - - - - - - -
	allocatedROM->offsetIdxClassRootsCMPR = (MAX_WIDTH_TYPE*) malloc(sizeof(MAX_WIDTH_TYPE)*(*numberClassRootLeaders));
	*numBitsROM += SIZE_MAX_WIDTH_TYPE* *numberClassRootLeaders;
	
//	allocatedROM->offsetIdxClassRootsCMPRPositive = (MAX_WIDTH_TYPE*) malloc(sizeof(MAX_WIDTH_TYPE)*(*numberClassRootLeaders));
//	*numBitsROM += SIZE_MAX_WIDTH_TYPE* *numberClassRootLeaders;

	allocatedROM->numberOffsetFieldClassRootsCMPR = (WORD32*) malloc(sizeof(WORD32)*(*numberClassRootLeaders));
	*numBitsROM += SIZE_WORD32* *numberClassRootLeaders;
	
	allocatedROM->offsetFieldClassRootsCMPR = (MAX_WIDTH_TYPE**) malloc(sizeof(MAX_WIDTH_TYPE*)*(*numberClassRootLeaders));
	*numBitsROM += SIZE_POINTER* *numberClassRootLeaders;
	// Allocate RAM for sign permutations

	allocatedRAM->copyWeightsPermCMPR = (WORD16*) malloc(sizeof(WORD16)*(allocatedROM->numberWeightsMaxCMPR*2+1));
	*numBitsRAM += SIZE_WORD16 * (allocatedROM->numberWeightsMaxCMPR*2+1);

	allocatedRAM->copyAmplsPermCMPR = (WORD16*) malloc(sizeof(WORD16)*(allocatedROM->numberWeightsMaxCMPR*2+1));
	*numBitsRAM += SIZE_WORD16 *(allocatedROM->numberWeightsMaxCMPR*2+1);
	
	allocatedRAM->numberSignsPermCMPR = (WORD16*) malloc(sizeof(WORD16)*(allocatedROM->numberWeightsMaxCMPR));
	*numBitsRAM += SIZE_WORD16 *(allocatedROM->numberWeightsMaxCMPR);
	
	allocatedRAM->copyWeightsSchalkwijk = (WORD16*) malloc(sizeof(WORD16)*(2*allocatedROM->numberWeightsMaxCMPR+1));
	*numBitsRAM += SIZE_WORD16 *(allocatedROM->numberWeightsMaxCMPR);
	
	allocatedRAM->reorderMatrix = (WORD16*)malloc(sizeof(WORD16)*allocatedROM->dimensionVQ);
	*numBitsRAM += SIZE_WORD16 *(allocatedROM->dimensionVQ);
	
	allocatedRAM->reorderedEntries = (DOUBLE*) malloc(sizeof(DOUBLE)*allocatedROM->dimensionVQ);
	*numBitsRAM += SIZE_DOUBLE *(allocatedROM->dimensionVQ);
	
//	allocatedRAM->copyEntries = malloc(sizeof(DOUBLE)*allocatedROM->dimensionVQ);
	*numBitsRAM += SIZE_DOUBLE *(allocatedROM->dimensionVQ);
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Step V: Compute the permutations for each classleader root vector
	// - - - - - - - - - - - - - - - - - - - - - - - - - -
	classLeadersCMPR = 0;
	for(i=0; i < *numberClassRootLeaders; i++)
	{
		classLeadersCMPR += calcClassLeaderPermutations(allocatedROM->fieldWeightsRootCMPR[i],
			allocatedROM->validWeightsRootCMPR[i], allocatedROM->modeClassRootLeaderCMPR[i],
			allocatedROM->signParityClassRootLeaderCMPR[i],1,&allocatedROM->numberOffsetFieldClassRootsCMPR[i],
			NULL, NULL, 0, NULL, NULL, NULL, /*NULL, NULL, NULL,*/ NULL);
		allocatedROM->offsetFieldClassRootsCMPR[i] = (MAX_WIDTH_TYPE*) malloc(sizeof(MAX_WIDTH_TYPE)*
			allocatedROM->numberOffsetFieldClassRootsCMPR[i]);
		*numBitsROM += SIZE_MAX_WIDTH_TYPE* allocatedROM->numberOffsetFieldClassRootsCMPR[i];
	}

#ifdef GLCVQ_VERBOSE_MODE
	rtpPrintf("GLCVQ: Number of classleader vectors computed from class leader root vectors: %d\n", classLeadersCMPR);
#endif

	classLeadersCMPR = 0;
	*numPermPerm = 0;
//	allocatedROM->numberNodesPositiveValid = 0;
//	offsetIndicesPositiveOld = 0;

	for(i=0; i < *numberClassRootLeaders; i++)
	{
#ifdef GLCVQ_VERBOSE_MODE
		rtpPrintf("GLCVQ: Treating classleader root vector[%d]\n", i);
#endif
		classLeadersCMPR += calcClassLeaderPermutations(allocatedROM->fieldWeightsRootCMPR[i],
			allocatedROM->validWeightsRootCMPR[i], allocatedROM->modeClassRootLeaderCMPR[i],
			allocatedROM->signParityClassRootLeaderCMPR[i],2,NULL, allocatedRAM->copyWeightsPermCMPR, 
			allocatedRAM->numberSignsPermCMPR,allocatedROM->numberZerosRootCMPR[i],
			&allocatedROM->offsetIdxClassRootsCMPR[i], allocatedROM->offsetFieldClassRootsCMPR[i], 
			numPermPerm, 
			//&allocatedROM->offsetIdxClassRootsCMPRPositive[i], &allocatedROM->numberNodesPositiveValid, &offsetIndicesPositiveOld ,
			&numberPossibleIndices);
	}

#ifdef GLCVQ_VERBOSE_MODE
	rtpPrintf("GLCVQ: Number of GLCVQ codevectors: %llu\n", *numPermPerm);
#endif

//	rtpPrintf("Number Nodes: %f\n", (DOUBLE)allocatedROM->numberNodesPositiveValid);

	allocatedRAM->copyEntriesNoSignCMPR = (DOUBLE*)malloc(sizeof(DOUBLE)*allocatedROM->dimensionVQ);
	*numBitsRAM += SIZE_DOUBLE *(allocatedROM->dimensionVQ);
	
	allocatedRAM->copyEntriesSignsCMPR = (WORD16*)malloc(sizeof(WORD16)*allocatedROM->dimensionVQ);
	*numBitsRAM += SIZE_DOUBLE *(allocatedROM->dimensionVQ);
	
	allocatedRAM->signsCMPR = (WORD16*)malloc(sizeof(WORD16)*allocatedROM->dimensionVQ);
	*numBitsRAM += SIZE_WORD16 *(allocatedROM->dimensionVQ);
	
	allocatedRAM->errVecCMPR = (DOUBLE*)malloc(sizeof(DOUBLE)*allocatedROM->dimensionVQ);
	*numBitsRAM += SIZE_DOUBLE *(allocatedROM->dimensionVQ);
	
	allocatedRAM->synthVectorUnsortedCMPR = (WORD16*)malloc(sizeof(WORD16)*allocatedROM->dimensionVQ);
	*numBitsRAM += SIZE_WORD16 *(allocatedROM->dimensionVQ);

	// Create field for divisor in Schalkwijk algorithm
//	allocatedROM->divisorSchalkwijk = (DOUBLE*)malloc(sizeof(DOUBLE)*allocatedROM->dimensionVQ);
//	*numBitsROM += SIZE_DOUBLE*(allocatedROM->dimensionVQ);

//	for(i = 0; i < allocatedROM->dimensionVQ; i++)
//	{
//		allocatedROM->divisorSchalkwijk[i] = 1.0/(i+1);
//	}

	allocatedROM->divisorSchalkwijkUW32 = (UWORD32*)malloc(sizeof(UWORD32)*allocatedROM->dimensionVQ);
	*numBitsROM += SIZE_WORD32*(allocatedROM->dimensionVQ);

	allocatedROM->divisorSchalkwijkUW32[0] = 0xffffffff;
	for(i = 1; i < allocatedROM->dimensionVQ; i++)
	{
		allocatedROM->divisorSchalkwijkUW32[i] = (((unsigned)((1.0/(i+1))*((double)((unsigned)(1<<31))))) << 1);
	}
}

void deallocateGossetSVQ_CMPR(gossetROM* allocatedROM, gossetRAM* allocatedRAM,WORD32* numberClassRootLeaders, WORD16 functionCallID)
{
	WORD16 i;
	// Free the second part of the memory
	for(i=0; i < *numberClassRootLeaders; i++)
	{
		free(allocatedROM->offsetFieldClassRootsCMPR[i]);
		free(allocatedROM->fieldWeightsRootCMPR[i]);
		free(allocatedROM->fieldAmplitudesRootCMPR[i]);
	}

	
	free(allocatedROM->offsetIdxClassRootsCMPR);
//	free(allocatedROM->offsetIdxClassRootsCMPRPositive);
	free(allocatedROM->numberOffsetFieldClassRootsCMPR);
	free(allocatedROM->offsetFieldClassRootsCMPR);
	free(allocatedROM->validWeightsRootCMPR);
	free(allocatedROM->fieldWeightsRootCMPR);
	free(allocatedROM->fieldAmplitudesRootCMPR);
	free(allocatedROM->numberZerosRootCMPR);
	free(allocatedROM->modeClassRootLeaderCMPR);
	free(allocatedROM->signParityClassRootLeaderCMPR);

	// Create field for divisor in Schalkwijk algorithm
//	free(allocatedROM->divisorSchalkwijk);
	free(allocatedROM->divisorSchalkwijkUW32);

	free(allocatedRAM->reorderMatrix);
	free(allocatedRAM->reorderedEntries);
//	free(allocatedRAM->copyEntries);
	free(allocatedRAM->copyWeightsSchalkwijk);
	free(allocatedRAM->copyWeightsPermCMPR);
	free(allocatedRAM->copyAmplsPermCMPR);
	free(allocatedRAM->numberSignsPermCMPR);
	free(allocatedRAM->copyEntriesNoSignCMPR);
	free(allocatedRAM->copyEntriesSignsCMPR);
	free(allocatedRAM->signsCMPR);
	free(allocatedRAM->errVecCMPR);
	free(allocatedRAM->synthVectorUnsortedCMPR);
}


#if 0
void gossetLCP_decodeVector(DOUBLE *vector, 
		     MAX_WIDTH_TYPE* index, UWORD16 indexRadius, 
			 void *handleROM, void *handleRAM, void* handleMISC)
{
	int i;
	gossetROM *allocatedROM = (gossetROM *)handleROM;
	gossetRAM *allocatedRAM = (gossetRAM *)handleRAM;
	gossetMISC *allocatedMISC = (gossetMISC *)handleMISC;
	DOUBLE radius = allocatedROM->radiusReconstruct[indexRadius]; 
	
	// Decode one spherical vector after the other
	for(i = 0; i < allocatedMISC->numberVectors; i++)
	{
		decodeVectorSphereNew(&vector[i*allocatedROM->dimensionVQ], index[i], allocatedROM, allocatedRAM);
	}
	for(i = 0; i < allocatedROM->dimensionVQ; i++)
	{
		vector[i] *= radius;
	}
}
#endif


WORD32 convertSignsToPermutationNumber(WORD16* fieldSigns, WORD16* weights,
									   WORD16 numWeights, WORD16 signParity, WORD16 mode)
{
	WORD16 i;
	WORD16 grid = 1;
	WORD32 result = 0;
	WORD16 newSign;

	if(mode == 1)
		grid = 2;

	if(numWeights == 1)
	{
		result = ((fieldSigns[0]-signParity)/grid);
	}
	else
	{
		for(i = 0; i < fieldSigns[0];i++)
		{
			newSign = (signParity+i)%grid;
			result += calcClassLeaderPermutationsShort(&weights[1], (numWeights-1), mode, newSign);
		}
		newSign = (signParity+fieldSigns[0])%grid;
		result += convertSignsToPermutationNumber(&fieldSigns[1], &weights[1],
									   numWeights-1, newSign, mode);
	}
	return(result);
}

WORD32 convertSignsToPermutationNumber_CPLX(WORD16* fieldSigns, WORD16* weights,
									   WORD16 numWeights, WORD16 signParity, WORD16 mode)
{
	WORD16 i;
	WORD16 grid = 1;
	WORD16 gridShift = 0;
	WORD32 result = 0;
	WORD16 newSign;
	WORD16 ctrNumWeights;

	WORD16 * ptrS1,* ptrS2;
	WORD16 cnt;
	
	
	
	
	if(mode == 1)
	{
		
		grid = 2;

		
		gridShift = 1;
	}

	
	cnt = numWeights-1;

	
	ptrS1 = fieldSigns;

	
	ptrS2 = weights+1;

	
	
	
	for(ctrNumWeights = 0; ctrNumWeights < (numWeights-1); ctrNumWeights++)
	{
		
		
		for(i = 0; i < (*ptrS1);i++)
		{
			
			
			newSign = (signParity+i)%grid;

			
			result += calcClassLeaderPermutationsShort_CPLX(ptrS2, (cnt), mode, newSign);
			
			
		}

		
		
		
		signParity = (signParity+*ptrS1)%grid;

		
		cnt--;

		
		ptrS1++;

		
		ptrS2++;

		
	}

	
	
	
	result += (((*ptrS1)-signParity) >> gridShift);
	return(result);
}

void decodeVectorSphereNew(DOUBLE* outputVector, MAX_WIDTH_TYPE indexVector_NT, gossetROM* allocatedROM, 
						gossetRAM* allocatedRAM)
{
	WORD16 i;
	WORD16 idxClassRootVector;
	WORD32 idxPermutation;
	MAX_WIDTH_TYPE idxSchalkwijk;
	WORD16 val, cnt;
	WORD16 caseMissed = 1;
	 MAX_WIDTH_TYPE numPerms;

	// From index, find the index of the class root vector
	for(idxClassRootVector = 0; idxClassRootVector < (allocatedROM->numberClassRootLeadersCMPR-1); idxClassRootVector++)
	{
		if(allocatedROM->offsetIdxClassRootsCMPR[idxClassRootVector+1] > indexVector_NT)
			break;
	}

	// Use idxClassRootVector here to find the index for the sign permutation
	for(idxPermutation = 0; idxPermutation < (allocatedROM->numberOffsetFieldClassRootsCMPR[idxClassRootVector]-1); idxPermutation++)
	{
		if(allocatedROM->offsetFieldClassRootsCMPR[idxClassRootVector][idxPermutation+1] > indexVector_NT)
		{
			caseMissed = 0;
			idxSchalkwijk = indexVector_NT - allocatedROM->offsetFieldClassRootsCMPR[idxClassRootVector][idxPermutation];
			break;
		}
	}
	if(caseMissed)
	{
		idxSchalkwijk = indexVector_NT - allocatedROM->offsetFieldClassRootsCMPR[idxClassRootVector][idxPermutation];
	}

	transformIdxSigns(idxPermutation, allocatedROM->modeClassRootLeaderCMPR[idxClassRootVector],
		allocatedROM->signParityClassRootLeaderCMPR[idxClassRootVector], allocatedROM->fieldWeightsRootCMPR[idxClassRootVector],
		allocatedROM->validWeightsRootCMPR[idxClassRootVector], allocatedRAM->numberSignsPermCMPR);

	cnt = 0;
	val = 0;
	for(i = 0; i < (allocatedROM->validWeightsRootCMPR[idxClassRootVector]); i++)
	{
		val = allocatedROM->fieldWeightsRootCMPR[idxClassRootVector][i] - allocatedRAM->numberSignsPermCMPR[i];
		if(val != 0)
		{
			allocatedRAM->copyWeightsPermCMPR[cnt] = val;
			allocatedRAM->copyAmplsPermCMPR[cnt++] = allocatedROM->fieldAmplitudesRootCMPR[idxClassRootVector][i];
		}
	}
	if(allocatedROM->numberZerosRootCMPR[idxClassRootVector] != 0)
	{
		allocatedRAM->copyWeightsPermCMPR[cnt] = allocatedROM->numberZerosRootCMPR[idxClassRootVector];
			allocatedRAM->copyAmplsPermCMPR[cnt++] = 0;
	}
	for(i = (allocatedROM->validWeightsRootCMPR[idxClassRootVector]-1); i >= 0 ; i--)
	{
		val = allocatedRAM->numberSignsPermCMPR[i];
		if(val != 0)
		{
			allocatedRAM->copyWeightsPermCMPR[cnt] = val;
			allocatedRAM->copyAmplsPermCMPR[cnt++] = -allocatedROM->fieldAmplitudesRootCMPR[idxClassRootVector][i];
		}
	}

	// Calculate the number of permutations from weights. 
	numPerms = rndREAL2MAXWIDTHTYPE(multinomialDbl(allocatedRAM->copyWeightsPermCMPR,cnt));

	if(idxSchalkwijk == 2298819142)
		rtpPrintf("STOP");

		schalkwijkDecoderW16(allocatedRAM->synthVectorUnsortedCMPR, idxSchalkwijk, allocatedROM->dimensionVQ, 
		allocatedRAM->copyWeightsPermCMPR, 
		allocatedRAM->copyAmplsPermCMPR, cnt, allocatedRAM, numPerms, allocatedROM->divisorSchalkwijkUW32);

	for(i = 0; i < allocatedROM->dimensionVQ; i++)
		outputVector[i] = allocatedRAM->synthVectorUnsortedCMPR[i]/allocatedROM->normalizationRadiusCMPR;
}
	
void rtp_glcvq_decode_sphere(DOUBLE* outputVector, MAX_WIDTH_TYPE indexVector_NT, gossetROM* allocatedROM, gossetRAM* allocatedRAM)
{
	WORD16 i;
	WORD16 idxClassRootVector;
	WORD32 idxPermutation;
	MAX_WIDTH_TYPE idxSchalkwijk;
	WORD16 val, cnt;
	MAX_WIDTH_TYPE* ptrMW1;
	WORD16* ptrS1, *ptrS2, *ptrS3,*ptrS4, *ptrS5;
	MAX_WIDTH_TYPE numPerms;

	WORD16 caseMissed = 1;	
	ptrMW1 = allocatedROM->offsetIdxClassRootsCMPR+1;

	// From index, find the index of the class root vector
	for(idxClassRootVector = 0; idxClassRootVector < (allocatedROM->numberClassRootLeadersCMPR-1); idxClassRootVector++)
	{
		if(*ptrMW1++ > indexVector_NT)
			break;		
	}

	// Use idxClassRootVector here to find the index for the sign permutation
	ptrMW1 = allocatedROM->offsetFieldClassRootsCMPR[idxClassRootVector]+1;

	for(idxPermutation = 0; idxPermutation < (allocatedROM->numberOffsetFieldClassRootsCMPR[idxClassRootVector]-1); idxPermutation++)
	{
		if(*ptrMW1 > indexVector_NT)
		{
			caseMissed = 0;
			idxSchalkwijk = indexVector_NT - *(--ptrMW1);
			break;
		}
		
		ptrMW1++;
	}
	
	if(caseMissed)
	{
		idxSchalkwijk = indexVector_NT - *(--ptrMW1);
	}

	transformIdxSigns(idxPermutation, allocatedROM->modeClassRootLeaderCMPR[idxClassRootVector],
		allocatedROM->signParityClassRootLeaderCMPR[idxClassRootVector], allocatedROM->fieldWeightsRootCMPR[idxClassRootVector],
		allocatedROM->validWeightsRootCMPR[idxClassRootVector], allocatedRAM->numberSignsPermCMPR);

	// Calc the weights according to the sign permutation index
	cnt = 0;
	val = 0;
	ptrS1 = allocatedRAM->copyWeightsPermCMPR;
	ptrS2 = allocatedRAM->copyAmplsPermCMPR;
	ptrS3 = allocatedROM->fieldAmplitudesRootCMPR[idxClassRootVector];
	ptrS4 = allocatedROM->fieldWeightsRootCMPR[idxClassRootVector];	
	ptrS5 = allocatedRAM->numberSignsPermCMPR;

	for(i = 0; i < (allocatedROM->validWeightsRootCMPR[idxClassRootVector]); i++)
	{
		val = *ptrS4++ - *ptrS5++;
		if(val != 0)
		{
			*ptrS1++ = val;
			*ptrS2++ = *ptrS3;
			cnt++;
		}

		ptrS3++;		
	}
	if(allocatedROM->numberZerosRootCMPR[idxClassRootVector] != 0)
	{
		*ptrS1++ = allocatedROM->numberZerosRootCMPR[idxClassRootVector];
		*ptrS2++ = 0;
		cnt++;
	}
	for(i = (allocatedROM->validWeightsRootCMPR[idxClassRootVector]-1); i >= 0 ; i--)
	{
		val = *(--ptrS5);
		ptrS3--;
		if(val != 0)
		{
			*ptrS1++ = val;
			*ptrS2++ = -(*ptrS3);
			cnt++;
		}		
	}

#ifdef OLD_SCHALKWIJK
	schalkwijkDecoderW16_CPLX(allocatedRAM->synthVectorUnsortedCMPR, idxSchalkwijk, allocatedROM->dimensionVQ, 
		allocatedRAM->copyWeightsPermCMPR, 
		allocatedRAM->copyAmplsPermCMPR, cnt, allocatedRAM, allocatedROM->divisorSchalkwijk);
#else

	// Calculate the number of permutations - from offset fields!
	numPerms = allocatedROM->numberNodesValid;

	if(idxPermutation == (allocatedROM->numberOffsetFieldClassRootsCMPR[idxClassRootVector]-1))
	{
		if(idxClassRootVector != allocatedROM->numberClassRootLeadersCMPR-1)
		{
			numPerms = allocatedROM->offsetFieldClassRootsCMPR[idxClassRootVector+1][0];
		}
	}
	else
	{
		numPerms = allocatedROM->offsetFieldClassRootsCMPR[idxClassRootVector][idxPermutation+1];
	}
	numPerms -= allocatedROM->offsetFieldClassRootsCMPR[idxClassRootVector][idxPermutation];

	schalkwijkDecoderW16(allocatedRAM->synthVectorUnsortedCMPR, idxSchalkwijk, allocatedROM->dimensionVQ, 
		allocatedRAM->copyWeightsPermCMPR, 
		allocatedRAM->copyAmplsPermCMPR, cnt, allocatedRAM, numPerms, allocatedROM->divisorSchalkwijkUW32);
#endif

	// I think that we will not need this functionality (division)
	for(i = 0; i < allocatedROM->dimensionVQ; i++)
		outputVector[i] = allocatedRAM->synthVectorUnsortedCMPR[i]/allocatedROM->normalizationRadiusCMPR;
}


DOUBLE calculateDelta(gossetROM* allocatedROM, gossetRAM* allocatedRAM)
{
	WORD16 i,j,k, ii,jj;
	DOUBLE* vectorOne;
	DOUBLE* vectorTwo;
	DOUBLE deltaMin = 0.0;
	DOUBLE delta = 0.0;
	DOUBLE deltaAccu = 0.0;
	MAX_WIDTH_TYPE numIndices = 0;
	MAX_WIDTH_TYPE idxOne;
	MAX_WIDTH_TYPE idxTwo;

	vectorOne = (DOUBLE*)malloc(sizeof(DOUBLE)*allocatedROM->dimensionVQ);
	vectorTwo = (DOUBLE*)malloc(sizeof(DOUBLE)*allocatedROM->dimensionVQ);

	for( i = 0; i < allocatedROM->numberClassRootLeadersCMPR; i++)
	{
		for(j = 0; j < allocatedROM->numberOffsetFieldClassRootsCMPR[i];j++)
		{
			idxOne = allocatedROM->offsetFieldClassRootsCMPR[i][j];
			decodeVectorSphereNew(vectorOne, idxOne, allocatedROM, allocatedRAM);
		
			deltaMin = FLT_MAX;

			for( ii = 0; ii < allocatedROM->numberClassRootLeadersCMPR; ii++)
			{
				for(jj = 0; jj < allocatedROM->numberOffsetFieldClassRootsCMPR[ii];jj++)
				{
					idxTwo = allocatedROM->offsetFieldClassRootsCMPR[ii][jj];
					if(idxTwo == 2841400896)
						rtpPrintf("STOP!");
					if(idxOne != idxTwo)
					{
						decodeVectorSphereNew(vectorTwo, idxTwo, allocatedROM, allocatedRAM);
					
						delta = 0.0;
						for(k=0; k < allocatedROM->dimensionVQ;k++)
							delta += (vectorOne[k]-vectorTwo[k])*(vectorOne[k]-vectorTwo[k]);
						delta = sqrt(delta);
						if(delta < deltaMin)
							deltaMin =delta;
					}
				}
			}
			assert(deltaMin != FLT_MAX);
			if(j == allocatedROM->numberOffsetFieldClassRootsCMPR[i]-1)
			{
				if(i == allocatedROM->numberClassRootLeadersCMPR-1)
				{
					numIndices = (allocatedROM->numberPermutationsPermutationsCMPR-
					allocatedROM->offsetFieldClassRootsCMPR[i][j]);
				}
				else
				{
					numIndices = allocatedROM->offsetFieldClassRootsCMPR[i+1][0]-
						allocatedROM->offsetFieldClassRootsCMPR[i][j];
				}
			}
			else
			{
				numIndices = allocatedROM->offsetFieldClassRootsCMPR[i][j+1]-
						allocatedROM->offsetFieldClassRootsCMPR[i][j];
			}
			deltaAccu += deltaMin * (DOUBLE)numIndices;
		}
	}
	deltaAccu = deltaAccu/(DOUBLE)allocatedROM->numberPermutationsPermutationsCMPR;

	free(vectorOne);
	free(vectorTwo);
	return(deltaAccu);
}


