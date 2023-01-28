#define _USE_MATH_DEFINES 
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <iostream>
#include <string>

#include "rtpL_sourceCoding.h"


// If you want to write some data for debugging purposes
#ifdef WRITE_OUT_FILE
static FILE* dbgOut = fopen("out.txt", "w");
#endif

// Macros
#define MAX_AMPLITUDE 1.0
#define EPSILON_U_Q 0.00000001
#define EPSILON_RADIUS 0.00000000001
#define MAX_NUMBER_WEIGHTS 3 // Corresponding to +1, 0, -1

// Search routine options
#define INTELLIGENT_SEARCH
//#define FULL_SEARCH
#define ONLY_PULSES

//==================================================================
// Rounding operators with overflow protection

static 
MAX_WIDTH_TYPE rndREAL2MAXWIDTHTYPE(DOUBLE input)
{
#ifdef MODE_64BIT
	assert(abs(input) < pow(2.0, 63));
#else
	assert(abs(input) < pow(2.0, 31));
#endif
	return((MAX_WIDTH_TYPE)floor(input+0.5));
}

static 
WORD32 rndREAL2WORD32(DOUBLE input)
{
	assert(abs(input) < pow(2.0, 31));
	return((WORD32)floor(input+0.5));
}

//==================================================================
// Static functions: Factorial in DOUBLE
static DOUBLE factorialDbl(WORD16 input)
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
static DOUBLE factorialDblStSt(WORD16 inputStart, WORD16 inputStop)
{
	WORD16 i;
	DOUBLE res = 1;
	for(i = inputStop; i >= inputStart; i--)
		res *= (DOUBLE)i/(DOUBLE)inputStart;
	return(res);
}

//==================================================================
// Structs: RAM struct for one single grid
typedef struct
{
	DOUBLE* excVector;
	WORD16* selectedVector;
	WORD16 idxPermutation;
	WORD16* copyWeights;

	//WORD16* searchWeights;
	//	DOUBLE* errorVector;<- for other search modes
}codebookGridRAM;

//==================================================================
// Structs: RAM struct for all grids, the number of grids is stored in ROM
typedef struct 
{
	codebookGridRAM* gridRAM;
} codebookRAM;

//==================================================================
// Structs: ROM element for each grid in a codebook
typedef struct
{
	// Configuration from initialization
	codebookConfiguration_LBVQ configuration;

	// Number of different sign permutations
	WORD16* numberSignPermutations;

	MAX_WIDTH_TYPE** numberIndicesSignPermutations;

	MAX_WIDTH_TYPE* numberIndicesGrid;

 // For each sign permutation: All weight-fields, maximum number length: 3 (+1 0 -1)
	WORD16*** weights;

	// For each sign permutation: All weight-fields, maximum number length: 3 (+1 0 -1)
	DOUBLE*** values;

	// For each sign permutation: Number of valid entries in weight- and value-fields, maximum value: 3 (+1 0 -1)
	WORD16** maxNumberWeights;

	// Division factor to separate indices from codebooks
	DOUBLE* divisorCodebookPartitions;

	// Multiplication factor to separate indices from codebooks
	DOUBLE* factorCodebookPartitions;

	// Amplitude to multiply pulses with (related to the number of pulses in codebook
	DOUBLE amplitudePulses;

#ifndef INTELLIGENT_SEARCH
	MAX_WIDTH_TYPE* numberPulsPermutations;
#endif

	// Should be located in MISC not in ROM
	WORD16* numberBitsConvCELP;
	// Overall number of pulses per codebook
	WORD16 numberPulsesCodebook;
} codebookConfiguration_LBVQ_local;

//==================================================================
// Structs: ROM elements for codebooks
typedef struct
{
	// List of codebooks
	codebookConfiguration_LBVQ_local* codebooks;

	// Number of codebooks
	WORD16 numberCodebooks;

	// Offset indices for codebook separation
	MAX_WIDTH_TYPE* numberIndicesCodebooks;

	// Should be located in MISC not ROM
	// Number mode bits for conv. CELP addressing
	WORD16 numberModeBitsCELP;
	// Number bits for conv. CELP addressing
	MAX_WIDTH_TYPE* bitsCodebooksCELP;
} quantizerConfiguration_LBVQ_local;

//==================================================================
// ROM
typedef struct
{
	// Element size for test
	size_t elementSize;

	// Link to all codebooks
	quantizerConfiguration_LBVQ_local codebooks;

	// VQ dimension
	WORD16 dimensionVQ;

	// Number indices for radius Q
	WORD16 numberIndicesRadius;

	// Number vector indices for sphere
	MAX_WIDTH_TYPE numberVectors;

	// Field for radius Q
	DOUBLE* radiusFldBoundsSquared;

	// Field for radius reconstruction
	DOUBLE* radiusReconstruct;
} lbvqROM;

//==================================================================
// RAM
typedef struct
{
	// Test
	size_t elementSize;

	// RAM for operations
	codebookRAM* fldRAMCodebook;
 
	//DOUBLE* testVector;
} lbvqRAM;

//==================================================================
// MISC
typedef struct
{
	size_t elementSize;
	DOUBLE aValue;
	DOUBLE maxRadius;
	DOUBLE maxAmplitude;
	DOUBLE overallBitrate;
	DOUBLE overallBitrateCELP;

	DOUBLE energyOriginal;
	DOUBLE energyDifference;
	DOUBLE energyRadius;
	DOUBLE energyDiffRadius;
	DOUBLE energyDiffSphere;

	DOUBLE effectiveBitrate;
	DOUBLE effectiveBitrateCELP;

	MAX_WIDTH_TYPE numRadiusLog;
	MAX_WIDTH_TYPE numRadiusLin;
	MAX_WIDTH_TYPE overload;
	MAX_WIDTH_TYPE numberFrames;
} lbvqMISC;

//==================================================================
// Static functions for quantization
//==================================================================
static MAX_WIDTH_TYPE calcNumberVectorsLBVQ(WORD16 numberPulses, WORD16 numberPositions);
static double gammaN2(WORD16 n);
static double calcNumberRadius(WORD16 n, double A, double Ks);
static DOUBLE multinomialDbl(WORD16* fld, WORD16 lField);
static DOUBLE readOutRadiusIndex(UWORD32 index, UWORD32 numberSteps, DOUBLE minV, DOUBLE maxV);
static DOUBLE convertLog2LinRadius(DOUBLE radiusLogQ, DOUBLE r_0, DOUBLE a_value, DOUBLE c);
static WORD16 getIndexRadius(DOUBLE realRadius, lbvqROM* allocatedROM);
static MAX_WIDTH_TYPE schalkwijkEncoder(const DOUBLE* field, WORD16 dimensionVQ, WORD16* fieldWeights, 
										DOUBLE* fieldAmplitudes, WORD16 numberWeightsUsed, WORD16* copyWeights);
static void schalkwijkDecoder(DOUBLE* vecQ, MAX_WIDTH_TYPE idx, WORD16 dimensionVQ, WORD16* fieldWeights, 
							  DOUBLE* fieldAmplitudes, WORD16 numberWeightsUsed, WORD16* copyWeights);
static MAX_WIDTH_TYPE calcNumberVectorsLBVQ(WORD16 numberPulses, WORD16 numberPositions, WORD16** weights, DOUBLE** values, 
											WORD16* maxNumberWeights, MAX_WIDTH_TYPE* offsetSignPermutations);
static DOUBLE calculatePermutationsDblNew(WORD16* weights, WORD16 numWeights);
static void quantizeVectorSphereOL(const DOUBLE* inputVector, DOUBLE* outputVector, DOUBLE radiusQ, MAX_WIDTH_TYPE* indexVector_NT, 
						  DOUBLE realRadius, lbvqROM* allocatedROM, lbvqRAM* allocatedRAM,
						  lbvqMISC* allocatedMISC);
static void generateVector(MAX_WIDTH_TYPE kk, DOUBLE* excVector, WORD16 weightsTemp[2]);

/**
 * Initialization function, configure quantizer and allocate all required memory
 *///=============================================================================
void lbvq_initializeAllocate(WORD16 dimensionVQ, 
							 quantizerConfiguration_LBVQ* configQ,
							 DOUBLE AVALUE, 
							 void** handleROM, 
							 void** handleRAM, 
							 void** handleMISC, 
							 MAX_WIDTH_TYPE* numBitsROM, 
							 MAX_WIDTH_TYPE* numBitsRAM)
{
	WORD16 i,ii,j,k;
#ifndef INTELLIGENT_SEARCH
	WORD16 weightsTemp[2];
#ifdef FULL_SEARCH
	std::cout << "Full Search active." << std::endl;
#else
	std::cout << "Non-intelligent Search active." << std::endl;
#endif
#else
	std::cout << "Intelligent Search active." << std::endl;

#endif

	DOUBLE numBitsConventionalCELP;
	MAX_WIDTH_TYPE numberPermutationsTracks, numberPermutationsLocal,numberVectorsForCurrentCodebook;

	DOUBLE vol1_nom = 0.0;
	DOUBLE vol1_den = 0.0;
	DOUBLE vol1 = 0.0;

	WORD16 numBitsCELPCurrentCodebook = 0;
	WORD16 maxBitsCELPCodebook = 0;
	DOUBLE sphereRadiusConstant, tmp;
	
	lbvqROM* allocatedROM = (lbvqROM*)malloc(sizeof(lbvqROM));
	lbvqRAM* allocatedRAM = (lbvqRAM*)malloc(sizeof(lbvqRAM));
	lbvqMISC* allocatedMISC = (lbvqMISC*)malloc(sizeof(lbvqMISC));

	allocatedROM->elementSize = sizeof(lbvqROM);
	allocatedRAM->elementSize = sizeof(lbvqRAM);
	allocatedMISC->elementSize = sizeof(lbvqMISC);

	allocatedROM->dimensionVQ = dimensionVQ;
	allocatedMISC->aValue = AVALUE;
	allocatedMISC->maxAmplitude = MAX_AMPLITUDE;

	allocatedROM->codebooks.codebooks = 
		(codebookConfiguration_LBVQ_local*)malloc(sizeof(codebookConfiguration_LBVQ_local)*configQ->numberCodebooks);
	allocatedROM->codebooks.numberCodebooks = configQ->numberCodebooks;
	allocatedROM->codebooks.numberIndicesCodebooks = (MAX_WIDTH_TYPE*)malloc(sizeof(MAX_WIDTH_TYPE)*allocatedROM->codebooks.numberCodebooks);
	allocatedROM->codebooks.bitsCodebooksCELP = (MAX_WIDTH_TYPE*)malloc(sizeof(MAX_WIDTH_TYPE)*allocatedROM->codebooks.numberCodebooks);

	std::cout << "Low bit rate (sparse) vector quantizer, dimension Lv=" << allocatedROM->dimensionVQ << std::endl;
	std::cout << "Number of codebooks:" << allocatedROM->codebooks.numberCodebooks << std::endl;

	allocatedROM->numberVectors = 0;

	numBitsCELPCurrentCodebook = 0;
	allocatedROM->codebooks.numberModeBitsCELP = (WORD16)ceil(log10((DOUBLE)allocatedROM->codebooks.numberCodebooks)/log10(2.0));

	allocatedRAM->fldRAMCodebook = (codebookRAM*)malloc(sizeof(codebookRAM)*allocatedROM->codebooks.numberCodebooks);
//	allocatedRAM->testVector = (DOUBLE*)malloc(sizeof(DOUBLE)*allocatedROM->dimensionVQ);

	for(i = 0; i < allocatedROM->codebooks.numberCodebooks; i++)
	{
		numBitsCELPCurrentCodebook = 0;
		std::cout << "Codebook Nr." << i << std::endl;
		allocatedROM->codebooks.codebooks[i].configuration.numberGrids = 
			configQ->codebooks[i].numberGrids;
		
		allocatedRAM->fldRAMCodebook[i].gridRAM = (codebookGridRAM*)malloc(sizeof(codebookGridRAM)*allocatedROM->codebooks.codebooks[i].configuration.numberGrids);

		std::cout << "---> Number grids:" << allocatedROM->codebooks.codebooks[i].configuration.numberGrids << std::endl;

		allocatedROM->codebooks.codebooks[i].configuration.maxNumberPositionsGrid = (WORD16*)malloc(sizeof(WORD16)*allocatedROM->codebooks.codebooks[i].configuration.numberGrids);
		allocatedROM->codebooks.codebooks[i].configuration.numberPulsesGrid = (WORD16*)malloc(sizeof(WORD16)*allocatedROM->codebooks.codebooks[i].configuration.numberGrids);
		allocatedROM->codebooks.codebooks[i].configuration.positionsGrid = (WORD16**)malloc(sizeof(WORD16*)*allocatedROM->codebooks.codebooks[i].configuration.numberGrids);
		allocatedROM->codebooks.codebooks[i].divisorCodebookPartitions = (DOUBLE*)malloc(sizeof(DOUBLE)*allocatedROM->codebooks.codebooks[i].configuration.numberGrids);
		allocatedROM->codebooks.codebooks[i].factorCodebookPartitions = (DOUBLE*)malloc(sizeof(DOUBLE)*allocatedROM->codebooks.codebooks[i].configuration.numberGrids);

		numberPermutationsTracks = 1;
		numberVectorsForCurrentCodebook = 0;

		allocatedROM->codebooks.codebooks[i].weights = (WORD16***)malloc(sizeof(WORD16**)*allocatedROM->codebooks.codebooks[i].configuration.numberGrids);
		allocatedROM->codebooks.codebooks[i].values = (DOUBLE***)malloc(sizeof(DOUBLE**)*allocatedROM->codebooks.codebooks[i].configuration.numberGrids);
		allocatedROM->codebooks.codebooks[i].maxNumberWeights = (WORD16**)malloc(sizeof(WORD16*)*allocatedROM->codebooks.codebooks[i].configuration.numberGrids);
		allocatedROM->codebooks.codebooks[i].numberSignPermutations = (WORD16*)malloc(sizeof(WORD16)*allocatedROM->codebooks.codebooks[i].configuration.numberGrids);
		allocatedROM->codebooks.codebooks[i].numberIndicesSignPermutations = (MAX_WIDTH_TYPE**)malloc(sizeof(MAX_WIDTH_TYPE*)*allocatedROM->codebooks.codebooks[i].configuration.numberGrids);
		allocatedROM->codebooks.codebooks[i].numberIndicesGrid = (MAX_WIDTH_TYPE*)malloc(sizeof(MAX_WIDTH_TYPE)*allocatedROM->codebooks.codebooks[i].configuration.numberGrids);

#ifndef INTELLIGENT_SEARCH
		allocatedROM->codebooks.codebooks[i].numberPulsPermutations = (MAX_WIDTH_TYPE*)malloc(sizeof(MAX_WIDTH_TYPE)*allocatedROM->codebooks.codebooks[i].configuration.numberGrids);
#endif

		allocatedROM->codebooks.codebooks[i].numberBitsConvCELP = (WORD16*)malloc(sizeof(WORD16)*allocatedROM->codebooks.codebooks[i].configuration.numberGrids);
		allocatedROM->codebooks.codebooks[i].numberPulsesCodebook = 0;

		for(ii = 0; ii < allocatedROM->codebooks.codebooks[i].configuration.numberGrids; ii++)
		{
			std::cout << "Codebook Nr." << i << ", grid Nr. " << ii << std::endl;
			allocatedROM->codebooks.codebooks[i].configuration.maxNumberPositionsGrid[ii] = configQ->codebooks[i].maxNumberPositionsGrid[ii];
			allocatedROM->codebooks.codebooks[i].configuration.numberPulsesGrid[ii] = configQ->codebooks[i].numberPulsesGrid[ii];
			allocatedROM->codebooks.codebooks[i].numberPulsesCodebook += allocatedROM->codebooks.codebooks[i].configuration.numberPulsesGrid[ii];
			allocatedROM->codebooks.codebooks[i].configuration.positionsGrid[ii] = (WORD16*)malloc(sizeof(WORD16)*allocatedROM->codebooks.codebooks[i].configuration.maxNumberPositionsGrid[ii]);
			memcpy(allocatedROM->codebooks.codebooks[i].configuration.positionsGrid[ii], configQ->codebooks[i].positionsGrid[ii], sizeof(WORD16)*allocatedROM->codebooks.codebooks[i].configuration.maxNumberPositionsGrid[ii]);
			
#ifndef INTELLIGENT_SEARCH
			weightsTemp[0] = allocatedROM->codebooks.codebooks[i].configuration.numberPulsesGrid[ii];
			weightsTemp[1] = allocatedROM->codebooks.codebooks[i].configuration.maxNumberPositionsGrid[ii]-
				allocatedROM->codebooks.codebooks[i].configuration.numberPulsesGrid[ii];
			
			allocatedROM->codebooks.codebooks[i].numberPulsPermutations[ii] = rndREAL2MAXWIDTHTYPE(calculatePermutationsDblNew(weightsTemp, 2));
#endif

			allocatedRAM->fldRAMCodebook[i].gridRAM[ii].excVector = (DOUBLE*)malloc(sizeof(DOUBLE)*allocatedROM->codebooks.codebooks[i].configuration.maxNumberPositionsGrid[ii]);
			allocatedRAM->fldRAMCodebook[i].gridRAM[ii].selectedVector = (WORD16*)malloc(sizeof(WORD16)*allocatedROM->codebooks.codebooks[i].configuration.maxNumberPositionsGrid[ii]);
			allocatedRAM->fldRAMCodebook[i].gridRAM[ii].copyWeights = (WORD16*)malloc(sizeof(WORD16)*MAX_NUMBER_WEIGHTS);
						
			allocatedROM->codebooks.codebooks[i].factorCodebookPartitions[ii] = (DOUBLE)numberPermutationsTracks;
			allocatedROM->codebooks.codebooks[i].divisorCodebookPartitions[ii] = 1.0/(DOUBLE)numberPermutationsTracks;
			
			std::cout << "------> Positions in grid" <<  std::flush;
			for(j = 0; j < allocatedROM->codebooks.codebooks[i].configuration.maxNumberPositionsGrid[ii]; j++)
			{
				std::cout << " " << allocatedROM->codebooks.codebooks[i].configuration.positionsGrid[ii][j] << std::flush;
			}
			std::cout << std::endl;
			std::cout << "------> Number pulses in grid: " << allocatedROM->codebooks.codebooks[i].configuration.numberPulsesGrid[ii] << std::endl;

			// There are (N+1) different +/- permutations
			allocatedROM->codebooks.codebooks[i].numberSignPermutations[ii] = allocatedROM->codebooks.codebooks[i].configuration.numberPulsesGrid[ii]+1;
				
			allocatedROM->codebooks.codebooks[i].maxNumberWeights[ii] = (WORD16*) malloc(sizeof(WORD16)*allocatedROM->codebooks.codebooks[i].numberSignPermutations[ii]);
			allocatedROM->codebooks.codebooks[i].weights[ii] = (WORD16**)malloc(sizeof(WORD16*)*allocatedROM->codebooks.codebooks[i].numberSignPermutations[ii]);
			allocatedROM->codebooks.codebooks[i].values[ii]  = (DOUBLE**)malloc(sizeof(DOUBLE*)*allocatedROM->codebooks.codebooks[i].numberSignPermutations[ii]);
			allocatedROM->codebooks.codebooks[i].numberIndicesSignPermutations[ii] = (MAX_WIDTH_TYPE*)malloc(sizeof(MAX_WIDTH_TYPE)*allocatedROM->codebooks.codebooks[i].numberSignPermutations[ii]);

			for(k = 0; k < allocatedROM->codebooks.codebooks[i].numberSignPermutations[ii]; k++)
			{
				allocatedROM->codebooks.codebooks[i].maxNumberWeights[ii][k] = MAX_NUMBER_WEIGHTS;
				allocatedROM->codebooks.codebooks[i].values[ii][k] = (DOUBLE*)malloc(sizeof(DOUBLE)*allocatedROM->codebooks.codebooks[i].maxNumberWeights[ii][k]);
				allocatedROM->codebooks.codebooks[i].weights[ii][k] = (WORD16*)malloc(sizeof(WORD16)*allocatedROM->codebooks.codebooks[i].maxNumberWeights[ii][k]);
			}

			numberPermutationsLocal = calcNumberVectorsLBVQ(allocatedROM->codebooks.codebooks[i].configuration.numberPulsesGrid[ii],
				allocatedROM->codebooks.codebooks[i].configuration.maxNumberPositionsGrid[ii],allocatedROM->codebooks.codebooks[i].weights[ii],
				allocatedROM->codebooks.codebooks[i].values[ii],allocatedROM->codebooks.codebooks[i].maxNumberWeights[ii],
				allocatedROM->codebooks.codebooks[i].numberIndicesSignPermutations[ii]);
			allocatedROM->codebooks.codebooks[i].numberIndicesGrid[ii] = numberPermutationsLocal;
			numberPermutationsTracks *= numberPermutationsLocal;
			std::cout << "Local number of indices in bits per sample:" << /*indCalculatedLocal << "::" <<*/ log10((DOUBLE)numberPermutationsLocal)/log10(2.0)/dimensionVQ << std::endl;
			numBitsConventionalCELP = ceil(log10((DOUBLE)allocatedROM->codebooks.codebooks[i].configuration.maxNumberPositionsGrid[ii])/log10(2.0))*(DOUBLE)allocatedROM->codebooks.codebooks[i].configuration.numberPulsesGrid[ii] +1;
			allocatedROM->codebooks.codebooks[i].numberBitsConvCELP[ii] = (WORD16)numBitsConventionalCELP;
			//pow((DOUBLE)allocatedROM->codebooks.codebooks[i].configuration.maxNumberPositionsGrid[ii], (DOUBLE)allocatedROM->codebooks.codebooks[i].configuration.numberPulsesGrid[ii])*2;
			std::cout << "Conventional number of indices in bits:" << (DOUBLE)numBitsConventionalCELP/(DOUBLE)dimensionVQ /*<< "::" << log10((DOUBLE)conventional)/log10(2.0)*/ << std::endl;
			numberVectorsForCurrentCodebook = numberPermutationsTracks;
			numBitsCELPCurrentCodebook += (WORD16)numBitsConventionalCELP;
		}
		allocatedROM->codebooks.codebooks[i].amplitudePulses = 1.0/(sqrt((DOUBLE)allocatedROM->codebooks.codebooks[i].numberPulsesCodebook));
		std::cout << "--->Amplitude pulses:" << allocatedROM->codebooks.codebooks[i].amplitudePulses << std::endl;
		allocatedROM->codebooks.numberIndicesCodebooks[i] = numberVectorsForCurrentCodebook;
		allocatedROM->codebooks.bitsCodebooksCELP[i] = numBitsCELPCurrentCodebook;
		if(numBitsCELPCurrentCodebook > maxBitsCELPCodebook)
			maxBitsCELPCodebook = numBitsCELPCurrentCodebook;

		allocatedROM->numberVectors += numberVectorsForCurrentCodebook;
	}
	allocatedROM->numberIndicesRadius = (WORD16)rndREAL2MAXWIDTHTYPE(calcNumberRadius(allocatedROM->dimensionVQ, allocatedMISC->aValue, 
		(DOUBLE)allocatedROM->numberVectors));

	std::cout << "Number vectors Sphere: " << allocatedROM->numberVectors << std::endl;

	allocatedMISC->overallBitrate = log10((DOUBLE)((MAX_WIDTH_TYPE)allocatedROM->numberIndicesRadius*allocatedROM->numberVectors))/log10(2.0);
	allocatedMISC->overallBitrateCELP = log10((DOUBLE)(allocatedROM->numberIndicesRadius))/log10(2.0)+ allocatedROM->codebooks.numberModeBitsCELP+maxBitsCELPCodebook;

	std::cout << "Overall bitrate:" << allocatedMISC->overallBitrate << std::endl;
	std::cout << "Overall bitrate CELP:" << allocatedMISC->overallBitrateCELP << std::endl;
	allocatedMISC->effectiveBitrate = (DOUBLE)allocatedMISC->overallBitrate/(DOUBLE)allocatedROM->dimensionVQ;
	allocatedMISC->effectiveBitrateCELP = (DOUBLE)allocatedMISC->overallBitrateCELP/(DOUBLE)allocatedROM->dimensionVQ;
	sphereRadiusConstant = 1.0/(1.0+log(allocatedMISC->aValue));;
	
	// === DETERIMINE THE MAXIMUM RADIUS ====
	// Find the volume of a D-dimensional Hypersphere with radius 1.0
	// Approximate the gamma function by factorials
	if(allocatedROM->dimensionVQ%2)
	{
		vol1_nom = (DOUBLE)(1<<allocatedROM->dimensionVQ)*pow(M_PI, ((allocatedROM->dimensionVQ-1)/2))*
			factorialDbl((allocatedROM->dimensionVQ-1)/2);
		vol1_den = (DOUBLE)factorialDbl(allocatedROM->dimensionVQ);
	}
	else
	{
		vol1_nom = pow(M_PI, allocatedROM->dimensionVQ/2);
		vol1_den = (DOUBLE)factorialDbl(allocatedROM->dimensionVQ/2);
	}
	
	// Here we got the volume
	vol1 = vol1_nom/vol1_den;
	allocatedMISC->maxRadius = (allocatedMISC->maxAmplitude * 2.0)/pow(vol1, (1.0/(DOUBLE)allocatedROM->dimensionVQ));

	// Now create the quantizer for radius
	allocatedROM->radiusFldBoundsSquared = (DOUBLE*)malloc(sizeof(DOUBLE)*(allocatedROM->numberIndicesRadius+1));

	allocatedROM->radiusReconstruct = (DOUBLE*)malloc(sizeof(DOUBLE)*allocatedROM->numberIndicesRadius);

	// Design of the quantizer for the radius
	allocatedROM->radiusFldBoundsSquared[0] = -FLT_MAX;
	allocatedROM->radiusFldBoundsSquared[allocatedROM->numberIndicesRadius] = +FLT_MAX;

	// Set the radius quantization zentroids and the bounds inbetween for a faster decision
	for(i = 0; i < allocatedROM->numberIndicesRadius; i++)
	{
		allocatedROM->radiusReconstruct[i] = readOutRadiusIndex(i, allocatedROM->numberIndicesRadius, 0, allocatedMISC->maxRadius);
		allocatedROM->radiusReconstruct[i] = convertLog2LinRadius(allocatedROM->radiusReconstruct[i], allocatedMISC->maxRadius, 
			allocatedMISC->aValue, sphereRadiusConstant);

		printf("Rq=%f}\n", allocatedROM->radiusReconstruct[i]);//printf("Value=%f}\n", allocatedROM->radiusReconstruct[i]);

		if( i != 0)
		{
			//fldBounds[i-1] = (fldZentrRadius[i] + fldZentrRadius[i-1])/2;
			tmp = (allocatedROM->radiusReconstruct[i] + allocatedROM->radiusReconstruct[i-1])/2;
			printf("Cq=%f}\n", tmp);
			allocatedROM->radiusFldBoundsSquared[i] = tmp*tmp;//fldBounds[i-1]*fldBounds[i-1];
		}
	}

	// Initialize the performance feedback variables
	allocatedMISC->energyOriginal = 0.0;
	allocatedMISC->energyDifference = 0.0;
	allocatedMISC->energyRadius = 0.0;
	allocatedMISC->energyDiffRadius = 0.0;
	allocatedMISC->energyDiffSphere = 0.0;

	allocatedMISC->numRadiusLog = 0;
	allocatedMISC->numRadiusLin = 0;
	allocatedMISC->overload = 0;
	allocatedMISC->numberFrames = 0;


	if(handleROM)
		*handleROM = allocatedROM;
	if(handleRAM)
		*handleRAM = allocatedRAM;
	if(handleROM)
		*handleMISC = allocatedMISC;
}

/**
 * Quantization entry function
 *///=========================================================================0
void lbvq_quantizeVectorOL(const DOUBLE* targetVector, DOUBLE* outputVectorQ,  
						   MAX_WIDTH_TYPE* indexVector_NT, UWORD16* indexRadius, 
						   UWORD16 vectorLength,  UWORD16 doRequantization,
					       void* handleROM, void* handleRAM, void* handleMISC)
{
	UWORD16 i;

	DOUBLE metricNum, metricDen;

	DOUBLE tmpp;
	
//	MAX_WIDTH_TYPE idV64;
	lbvqROM* allocatedROM = (lbvqROM*)handleROM;
	lbvqRAM* allocatedRAM = (lbvqRAM*)handleRAM;
	lbvqMISC* allocatedMISC = (lbvqMISC*)handleMISC;

	UWORD16 cnt = 0;
	DOUBLE realRadius = 0; //EPSILON_RADIUS;
	DOUBLE radiusQ, radiusOpt;

	assert(allocatedROM->elementSize == sizeof(lbvqROM));
	assert(allocatedRAM->elementSize == sizeof(lbvqRAM));
	assert(allocatedMISC->elementSize == sizeof(lbvqMISC));

	allocatedMISC->numberFrames++;

	/*	ULINT cnt; 
	double realRadius = 0;//EPSILON_RADIUS;
	ULINT useIndex = 0;
*/
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

	tmpp = sqrt(realRadius);
	//DBLOUT(&tmpp, 1, "radius.dat");

	*indexRadius = getIndexRadius(realRadius, allocatedROM);

	// Read out the radius
	radiusQ = allocatedROM->radiusReconstruct[*indexRadius];

	tmpp = radiusQ;
	//DBLOUT(&tmpp, 1, "radiusQ.dat");
	//radiusQ = sqrt(realRadius);

	// Search the best matching vector for normalized input in a closed loop manner, 
	// use the OL vectors for a preselection and the target signal for a CL search
	// on the reduced set of vectors
	// It is not important whether the signal is normalized prior to search as
	// the radius is removed in the search procedure anyway

#ifdef FULL_SEARCH

	DOUBLE metricMin = FLT_MAX;
	MAX_WIDTH_TYPE i64;
	MAX_WIDTH_TYPE i64Min = 0;
	DOUBLE metricLocal =0.0;

	for(i64 = 0; i64 < allocatedROM->numberVectors; i64++)
	{
		metricLocal =0.0;
		lbvq_decodeVector(outputVectorQ, allocatedROM->dimensionVQ, i64, *indexRadius, handleROM, handleRAM);
		//std::cout << "Idx" << i64 << "::" << std::flush;
		for(i = 0; i < allocatedROM->dimensionVQ; i++)
		{
			//std::cout << outputVectorQ[i] << " " << std::flush;
			metricLocal += (outputVectorQ[i]-targetVector[i])*(outputVectorQ[i]-targetVector[i]);
		}
		//std::cout << std::endl;
		if(metricLocal < metricMin)
		{ 
			metricMin = metricLocal;
			i64Min = i64;
		}		
	}
	
	//idV64 = i64Min;
	*indexVector_NT = i64Min;
#else

	// Employ sphere quantizer
	quantizeVectorSphereOL(targetVector, outputVectorQ, radiusQ, indexVector_NT, 
						  realRadius+EPSILON_RADIUS, allocatedROM, allocatedRAM, allocatedMISC);
	
/*	if(*indexVector_NT != idV64)
	{
		std::cout << "Hallo!" << std::endl;
	}*/
#endif
	//decodeVectorSphere(vectorQ, *indexVector_NT, allocatedROM);
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

	double tmp = 0.0;
	for(i = 0; i < vectorLength; i++)
	{
		allocatedMISC->energyDiffSphere += (targetVector[i]/(sqrt(realRadius)+0.000000001)-outputVectorQ[i])*(targetVector[i]/(sqrt(realRadius)+0.000000001)-outputVectorQ[i]);
#ifdef DONT_QUANTIZE_RADIUS
		outputVectorQ[i] *= sqrt(realRadius);//radiusQ;
#else
		outputVectorQ[i] *= radiusQ;
#endif
		allocatedMISC->energyDifference += (targetVector[i]-outputVectorQ[i])*(targetVector[i]-outputVectorQ[i]);
		tmp += (targetVector[i]-outputVectorQ[i])*(targetVector[i]-outputVectorQ[i]);
		
	}

#ifndef DONT_QUANTIZE_RADIUS
	lbvq_sc_decodeVector(outputVectorQ, allocatedROM->dimensionVQ, *indexVector_NT, *indexRadius, handleROM, handleRAM);
#endif
#ifdef WRITE_OUT_FILE
	fprintf(dbgOut, "Metric Vector Out: %f\n", tmp);
#endif
}

/**
 * Quantization sub function to find the spherical excitation vector
 *///===================================================================
void quantizeVectorSphereOL(const DOUBLE* inputVector, DOUBLE* outputVector, DOUBLE radiusQ, MAX_WIDTH_TYPE* indexVector_NT, 
						  DOUBLE realRadius, lbvqROM* allocatedROM, lbvqRAM* allocatedRAM,
						  lbvqMISC* allocatedMISC)
{
	WORD16 i,j,k;
	DOUBLE metricCodebook, metricGrid, singleVal, tempVal;
	DOUBLE metricCodebookMin = FLT_MAX;
	WORD16 idxCodebook = 0;
	MAX_WIDTH_TYPE idxVector = 0;
	
	WORD16 numberZeros = 0;
	WORD16 numberPlus = 0;
	WORD16 numberMinus = 0;

	MAX_WIDTH_TYPE indexVectorLocal;
	MAX_WIDTH_TYPE numberPermutations = 0;

#ifndef INTELLIGENT_SEARCH
	WORD16 weightsTemp[2];
	MAX_WIDTH_TYPE kk,ll;
	DOUBLE metricTemp;
	DOUBLE valMax;
	WORD16 l,selected;
#endif

	static int cnt = 0;
	cnt++;

	// Browse through all codebooks...
	for(i = 0; i < allocatedROM->codebooks.numberCodebooks; i++)
	{
		// Each codebokk results into a certain metric, among all codebooks that with the lowest metric is chosen
		metricCodebook = 0;

#ifdef INTELLIGENT_SEARCH

		for(j =0; j < allocatedROM->codebooks.codebooks[i].configuration.numberGrids; j++)
		{
			numberZeros = 0;
			numberPlus = 0;
			numberMinus = 0;

			metricGrid = 0.0;
			WORD16 pulsesToSet = allocatedROM->codebooks.codebooks[i].configuration.numberPulsesGrid[j];
			WORD16 zerosToSet = (allocatedROM->codebooks.codebooks[i].configuration.maxNumberPositionsGrid[j]-allocatedROM->codebooks.codebooks[i].configuration.numberPulsesGrid[j]);
#ifdef ONLY_PULSES
			zerosToSet = 0;
			numberZeros = (allocatedROM->codebooks.codebooks[i].configuration.maxNumberPositionsGrid[j]-allocatedROM->codebooks.codebooks[i].configuration.numberPulsesGrid[j]);
			memset(allocatedRAM->fldRAMCodebook[i].gridRAM[j].excVector,0, sizeof(DOUBLE)*allocatedROM->codebooks.codebooks[i].configuration.maxNumberPositionsGrid[j]);
#endif

			for(k = 0; k < allocatedROM->codebooks.codebooks[i].configuration.maxNumberPositionsGrid[j]; k++)
			{
				allocatedRAM->fldRAMCodebook[i].gridRAM[j].selectedVector[k] = 0;
			}
			while(pulsesToSet+zerosToSet)
			{
				bool specifyZero = false;
				bool specifyPuls = false;

				DOUBLE maxVal = -FLT_MAX;
				WORD16 maxIdx = 0;

				DOUBLE singleValZero;
				DOUBLE singleValPuls;

				for(k = 0; k < allocatedROM->codebooks.codebooks[i].configuration.maxNumberPositionsGrid[j]; k++)
				{
					if(!allocatedRAM->fldRAMCodebook[i].gridRAM[j].selectedVector[k])
					{
						singleValZero = abs(inputVector[allocatedROM->codebooks.codebooks[i].configuration.positionsGrid[j][k]]);
						singleValZero *= singleValZero;
						tempVal = radiusQ*allocatedROM->codebooks.codebooks[i].amplitudePulses;
						singleValPuls = abs(inputVector[allocatedROM->codebooks.codebooks[i].configuration.positionsGrid[j][k]]) - tempVal;
						singleValPuls *= singleValPuls;
						
						if(zerosToSet)
						{
							if(singleValPuls > maxVal)
							{
								specifyPuls = false;
								specifyZero = true;

								maxIdx = k;
								maxVal = singleValPuls;
							}
						}
						if(pulsesToSet)
						{
							if(singleValZero > maxVal)
							{
								specifyPuls = true;
								specifyZero = false;

								maxIdx = k;
								maxVal = singleValZero;
							}
						}
					}
				}
				
				if(specifyPuls)
				{
					allocatedRAM->fldRAMCodebook[i].gridRAM[j].excVector[maxIdx] = 
						RTP_SIGN(inputVector[allocatedROM->codebooks.codebooks[i].configuration.positionsGrid[j][maxIdx]]);

					if(allocatedRAM->fldRAMCodebook[i].gridRAM[j].excVector[maxIdx] > 0.5)
						numberPlus++;
					else
						numberMinus++;
					
					pulsesToSet--;
				}
				else
				{
					allocatedRAM->fldRAMCodebook[i].gridRAM[j].excVector[maxIdx] = 0.0;
					zerosToSet--;
					numberZeros++;
				}

				tempVal = allocatedRAM->fldRAMCodebook[i].gridRAM[j].excVector[maxIdx]*radiusQ*allocatedROM->codebooks.codebooks[i].amplitudePulses;
				singleVal = inputVector[allocatedROM->codebooks.codebooks[i].configuration.positionsGrid[j][maxIdx]];
				metricGrid += (singleVal-tempVal)*(singleVal-tempVal);
				allocatedRAM->fldRAMCodebook[i].gridRAM[j].selectedVector[maxIdx] = 1;
			}

			// Store the number of minus pulses for schalkwijk (choice of weights and values)
			allocatedRAM->fldRAMCodebook[i].gridRAM[j].idxPermutation = numberPlus;
			metricCodebook += metricGrid;
		}
#else	
		for(j =0; j < allocatedROM->codebooks.codebooks[i].configuration.numberGrids; j++)
		{
			weightsTemp[0] = allocatedROM->codebooks.codebooks[i].configuration.numberPulsesGrid[j];
			weightsTemp[1] = (allocatedROM->codebooks.codebooks[i].configuration.maxNumberPositionsGrid[j]-allocatedROM->codebooks.codebooks[i].configuration.numberPulsesGrid[j]);
			
			// Test all puls permutations
			metricGrid = FLT_MAX;

			idxVector = 0;
			for(kk = 0; kk < allocatedROM->codebooks.codebooks[i].numberPulsPermutations[j]; kk++)
			{
				generateVector(kk, allocatedRAM->fldRAMCodebook[i].gridRAM[j].excVector, weightsTemp);
				metricTemp = 0;
				for(k = 0; k < allocatedROM->codebooks.codebooks[i].configuration.maxNumberPositionsGrid[j]; k++)
				{
					singleVal = abs(inputVector[allocatedROM->codebooks.codebooks[i].configuration.positionsGrid[j][k]]);
					tempVal = allocatedRAM->fldRAMCodebook[i].gridRAM[j].excVector[k]*radiusQ*allocatedROM->codebooks.codebooks[i].amplitudePulses;
					metricTemp += (tempVal-singleVal)*(tempVal-singleVal);
				}
				if(metricTemp < metricGrid)
				{
					metricGrid = metricTemp;
					idxVector = kk;
				}
			}
			generateVector(idxVector, allocatedRAM->fldRAMCodebook[i].gridRAM[j].excVector, weightsTemp);

			numberZeros = 0;
			numberPlus = 0;
			numberMinus = 0;

			// Add all error metric values, set the excitation vector accordingly
			for(k = 0; k < allocatedROM->codebooks.codebooks[i].configuration.maxNumberPositionsGrid[j];k++)
			{
				singleVal = inputVector[allocatedROM->codebooks.codebooks[i].configuration.positionsGrid[j][k]];
				if(singleVal < 0.0)
				{
					allocatedRAM->fldRAMCodebook[i].gridRAM[j].excVector[k] *= -1;
				}
				if(allocatedRAM->fldRAMCodebook[i].gridRAM[j].excVector[k] > 0.5)
				{
					numberPlus++;
				}
				else if(allocatedRAM->fldRAMCodebook[i].gridRAM[j].excVector[k] < -0.5)
				{
					numberMinus++;
				}
				else
				{
					numberZeros++;
				}
			}

			// Store the number of minus pulses for schalkwijk (choice of weights and values)
			allocatedRAM->fldRAMCodebook[i].gridRAM[j].idxPermutation = numberPlus;
			metricCodebook += metricGrid;
		}
#endif

		// Make selection of codebook
#ifdef WRITE_OUT_FILE
		fprintf(dbgOut, "Metric Codebook %d: %f\n", i, metricCodebook);
#endif
		if(metricCodebook < metricCodebookMin)
		{
			metricCodebookMin = metricCodebook;
			idxCodebook = i;
		}
	}

//	if(idxCodebook == 1)
//		std::cout << "Hallo!" << std::endl;

	*indexVector_NT = 0;
	MAX_WIDTH_TYPE multFac = 1;
	for(j =0; j < allocatedROM->codebooks.codebooks[idxCodebook].configuration.numberGrids; j++)
	{
		// Encode all grid indices for selected codebook.
		indexVectorLocal = 0;

		// Different offsets for sign permutations
		for(k = 0; k < allocatedRAM->fldRAMCodebook[idxCodebook].gridRAM[j].idxPermutation; k++)
			indexVectorLocal += allocatedROM->codebooks.codebooks[idxCodebook].numberIndicesSignPermutations[j][k];

		// Local vector in this permutation
		indexVectorLocal += schalkwijkEncoder(allocatedRAM->fldRAMCodebook[idxCodebook].gridRAM[j].excVector, 
			allocatedROM->codebooks.codebooks[idxCodebook].configuration.maxNumberPositionsGrid[j], 
			allocatedROM->codebooks.codebooks[idxCodebook].weights[j][allocatedRAM->fldRAMCodebook[idxCodebook].gridRAM[j].idxPermutation], 
			allocatedROM->codebooks.codebooks[idxCodebook].values[j][allocatedRAM->fldRAMCodebook[idxCodebook].gridRAM[j].idxPermutation],
			allocatedROM->codebooks.codebooks[idxCodebook].maxNumberWeights[j][allocatedRAM->fldRAMCodebook[idxCodebook].gridRAM[j].idxPermutation], 
			allocatedRAM->fldRAMCodebook[idxCodebook].gridRAM[j].copyWeights);

		(*indexVector_NT) += multFac*indexVectorLocal;
		multFac *= allocatedROM->codebooks.codebooks[idxCodebook].numberIndicesGrid[j];

#ifdef WRITE_OUT_FILE
		fprintf(dbgOut,"Frame id %d, idxCodebook: %d:: Idx: %d::\n", cnt, idxCodebook, *indexVector_NT);
#endif
//		DOUBLE radiusInNormREM = 0.0;
//		DOUBLE radiusOutNormREM = 0.0;
		for(k = 0; k < allocatedROM->codebooks.codebooks[idxCodebook].configuration.maxNumberPositionsGrid[j];k++)
		{
			outputVector[allocatedROM->codebooks.codebooks[idxCodebook].configuration.positionsGrid[j][k]] = 
				allocatedROM->codebooks.codebooks[idxCodebook].amplitudePulses * allocatedRAM->fldRAMCodebook[idxCodebook].gridRAM[j].excVector[k];
//			radiusOutNormREM += outputVector[allocatedROM->codebooks.codebooks[idxCodebook].configuration.positionsGrid[j][k]]*
//				outputVector[allocatedROM->codebooks.codebooks[idxCodebook].configuration.positionsGrid[j][k]];
//			radiusInNormREM += (inputVector[allocatedROM->codebooks.codebooks[idxCodebook].configuration.positionsGrid[j][k]]/(sqrt(realRadius)+0.0000001))*
//				(inputVector[allocatedROM->codebooks.codebooks[idxCodebook].configuration.positionsGrid[j][k]]/(sqrt(realRadius)+0.0000001));
		}
//		std::cout << radiusOutNormREM << "::" << radiusInNormREM << std::endl;

	}
	for(i=0; i < idxCodebook; i++)
	{
		*indexVector_NT += allocatedROM->codebooks.numberIndicesCodebooks[i];
	}
}


//%===========================================================
//% Schalkwijk Encoder Function, based on the paper
//% "An Algorithm for Source Coding", by J. Pieter and M. Schalkwijk,
//% IEEE Transactions IT, it-18, no 3, 05/1972
//% Note that in comparison to (5) the following modification was introduced:
//% (n-k)!/((wkd-1)!*prod(wki!)[i ~=d]) = multinomial(wk)*wkd/sum(wki)
//% with (n-k)! = (sum(wki)-1)! = (sum(wki))!/sum(wki)
//% and (wkd-1)!*prod(wki)[i ~= d] = prod(wki!)/wkd
//% Hauke Krüger, RWTH Aachen, 19.7.2007
//% input: 
//% 1) vector consisting W different amplitudes
//% 2) weights containing W entries, each for a possible amplitude and
//%    corresponding weight
//% = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
//% Example: vector [2 2 1 1 0 0 1 2 1 0], weights = [2, 3; 1, 4; 0,3]
// Adaptation for C/C++: The weights are passed in two independent arrays
//%===========================================================
MAX_WIDTH_TYPE schalkwijkEncoder(const DOUBLE* field, WORD16 dimensionVQ, WORD16* fieldWeights, 
								 DOUBLE* fieldAmplitudes, WORD16 numberWeightsUsed, WORD16* copyWeights)
{
	WORD16 i,j;
	DOUBLE C;
    MAX_WIDTH_TYPE index = 0;
	DOUBLE den = dimensionVQ;
	memcpy(copyWeights, fieldWeights, sizeof(WORD16)*numberWeightsUsed);

	for(i = 0; i < dimensionVQ; i ++)
	{
        // Calculate constant based on multinomial coefficient and normalize
        C = multinomialDbl(copyWeights, numberWeightsUsed)/(DOUBLE)den;

        // loop over all weights until the right one is found
        j = 0;
        while((fieldAmplitudes[j] != field[i])&&(j < numberWeightsUsed))
		{
            // Calculate the index BEFORE the matching weight is found
            index += rndREAL2MAXWIDTHTYPE(C*copyWeights[j]);
            j++;
		}
		assert(fieldAmplitudes[j] == field[i]);

        // Update weight because one is now reduced (from dim to (dim-1))
        copyWeights[j] -= 1;

        // Update normalization
        den = den-1;
	}
    return(index);
}
  
//%===========================================================
//% Schalkwijk Decoder Function, based on the paper
//% "An Algorithm for Source Coding", by J. Pieter and M. Schalkwijk,
//% IEEE Transactions IT, it-18, no 3, 05/1972
//% Note that in comparison to (5) the following modification was introduced:
//% (n-k)!/((wkd-1)!*prod(wki!)[i ~=d]) = multinomial(wk)*wkd/sum(wki)
//% with (n-k)! = (sum(wki)-1)! = (sum(wki))!/sum(wki)
//% and (wkd-1)!*prod(wki)[i ~= d] = prod(wki!)/wkd
//% Hauke Krüger, RWTH Aachen, 19.7.2007
//% input: 
//% 1) index to address a vector
//% 2) weights containing W entries, each for a possible amplitude and
//%    corresponding weight
//% = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
//% Example: vector [2 2 1 1 0 0 1 2 1 0], weights = [2, 3; 1, 4; 0,3]
//%===========================================================
void schalkwijkDecoder(DOUBLE* vecQ, MAX_WIDTH_TYPE idx, WORD16 dimensionVQ, WORD16* fieldWeights, 
					   DOUBLE* fieldAmplitudes, WORD16 numberWeightsUsed, WORD16* copyWeights)
{
	WORD16 i,j;
	DOUBLE C;
	MAX_WIDTH_TYPE idxCompare;
	DOUBLE den = dimensionVQ;
	MAX_WIDTH_TYPE maxNumPermutations;
	memcpy(copyWeights, fieldWeights, sizeof(WORD16)*numberWeightsUsed);
	
	maxNumPermutations = rndREAL2MAXWIDTHTYPE(multinomialDbl(copyWeights,numberWeightsUsed));

    assert((idx < maxNumPermutations)||(idx >= 0));
    
    //% Loop over all coordinates
	for(i = 0; i < dimensionVQ; i++)
	{
        // Calculate constant based on multinomial coefficient and normalize
        C = multinomialDbl(copyWeights, numberWeightsUsed)/(DOUBLE)den;

        idxCompare = 0;
        
		j = 0;        
        while((idx >= 0))
		{            
            // Calculate the index BEFORE the matching weight is found
            idxCompare = rndREAL2MAXWIDTHTYPE(C*copyWeights[j]);
			idx = idx - idxCompare;
            j++;
		}
        
        idx = idx + idxCompare;
        j = j-1;
        
		vecQ[i] = fieldAmplitudes[j];
        
        // Update weight because one is now reduced (from dim to (dim-1))
        copyWeights[j] -= 1;

        // Update normalization
        den = den-1;

	}
}

/**
 * Function to calculate the number of vectors in codebook
 *///=====================================================
MAX_WIDTH_TYPE
calcNumberVectorsLBVQ(WORD16 numberPulses, WORD16 numberPositions, WORD16** weightsFld, DOUBLE** valuesFld, WORD16* maxNumberWeights,
					  MAX_WIDTH_TYPE* offsetPermutations)
{
	WORD16 i,ii;
	
	WORD16 numberPlusPulses;
	WORD16 numberMinusPulses;
	WORD16 numberZeros = numberPositions-numberPulses;

	WORD16 numWeights;

	WORD16 cnt;

	MAX_WIDTH_TYPE maxNumberVecs = 0;
	MAX_WIDTH_TYPE localNumberVecs;

	std::string txt = "";

	for(i = 0; i <= numberPulses; i++)
	{

		numWeights = 0; 
		numberPlusPulses = i;
		numberMinusPulses = (numberPulses-numberPlusPulses);
		if(numberZeros)
			numWeights++;
		if(numberPlusPulses)
			numWeights++;
		if(numberMinusPulses)
			numWeights++;

		txt = "";
		for(ii = 0; ii < numberPlusPulses; ii++)
			txt += "+";
		for(ii = 0; ii < numberMinusPulses; ii++)
			txt += "-";
		for(ii = 0; ii < numberZeros; ii++)
			txt += "0";

		cnt = 0;
		if(numberZeros)
		{		
			weightsFld[i][cnt] = numberZeros;
			valuesFld[i][cnt] = 0.0;
			cnt++;
		}
		if(numberPlusPulses)
		{
			weightsFld[i][cnt] = numberPlusPulses;
			valuesFld[i][cnt] = 1.0;
			cnt++;
		}
		if(numberMinusPulses)
		{
			weightsFld[i][cnt] = numberMinusPulses;
			valuesFld[i][cnt] = -1.0;
			cnt++;
		}

		assert(cnt <= maxNumberWeights[i]);
		maxNumberWeights[i] = cnt;

		localNumberVecs = rndREAL2MAXWIDTHTYPE(calculatePermutationsDblNew(weightsFld[i], maxNumberWeights[i]));
		offsetPermutations[i] = localNumberVecs;
		maxNumberVecs += localNumberVecs;
		std::cout << "Vector: " << txt << ", number perm.:" << localNumberVecs << std::endl;
	}
	return(maxNumberVecs);
}


//===============================================
double gammaN2(WORD16 n)
{
	assert(n > 1);

	if(n%2 == 0)
	{
		// even case:
		return(factorialDbl(n/2-1));
	}

	return((sqrt(M_PI)*factorialDbl(n-2))/(pow(2.0, n-2)*factorialDbl((n-3)/2)));
}

double calcNumberRadius(WORD16 n, double A, double Ks)
{
	double g1 = gammaN2(n+1);
	double g2 = gammaN2(n);
	double csh = ((DOUBLE)n-1.0)/((DOUBLE)n+1.0)*pow((2*sqrt(M_PI)*g1/g2),(2.0/((DOUBLE)n-1.0)));
    double c = 1/(1+log(A));
	double Kr;

    Kr =  pow(Ks,(1.0/((DOUBLE)n-1.0)))*pow((12.0*c*c*csh/((DOUBLE)n-1.0)),(-0.5));
	return(Kr);
}


/*
MAX_WIDTH_TYPE calculatePermutations(weights& fldWeight)
{
	MAX_WIDTH_TYPE num = 0;
	MAX_WIDTH_TYPE den = 1;
	WORD16 i=0,number = 0;

	for(i = 0; i < fldWeight.numEntries; i++)
	{
		num += fldWeight.occurrence[i];
		den = den* factorial(fldWeight.occurrence[i]);
	}
	return(factorial((WORD16)num)/den);
}
*/
/*
DOUBLE calculatePermutationsDbl(WORD16* weights, WORD16 numWeights)
{
	DOUBLE num = 0;
	DOUBLE den = 1;
	WORD16 i=0,number = 0;

	for(i = 0; i < numWeights; i++)
	{

		num += (DOUBLE)weights[i];
		den = den* factorialDbl(weights[i]);
	}
	return(factorialDbl((WORD16)num)/den);
}
*/
DOUBLE calculatePermutationsDblNew(WORD16* weights, WORD16 numWeights)
{
	DOUBLE num = 0;
	DOUBLE den = 1;
	WORD16 i=0;

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
	return(fractionAll);
}


DOUBLE readOutRadiusIndex(UWORD32 index, UWORD32 numberSteps, DOUBLE minV, DOUBLE maxV)
{
	DOUBLE value_quantized = 0.0;
	DOUBLE range = maxV-minV+2*EPSILON_U_Q;

	if(index >= numberSteps)
	{
		// Index out of range for radius quantization
		printf("Error Radius!");
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

WORD16
getIndexRadius(DOUBLE realRadius, lbvqROM* allocatedROM)
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

/**
 * Function to calculate the multinomial which is used very often in Schalkwijk
 *///=======================================================================
/*
DOUBLE multinomial(WORD16* fld, WORD16 lField)
{
	WORD16 i;
	WORD16 num = 0;
	DOUBLE den = 1.0;

	for(i = 0; i < lField; i++)
	{
		num += fld[i];
		den *= factorial(fld[i]);
	}
	return((DOUBLE)factorial(num)/den);
}
*/
DOUBLE multinomialDbl(WORD16* weights, WORD16 numWeights)
{
	return(calculatePermutationsDblNew(weights, numWeights));
/*
	DOUBLE num = 0;
	DOUBLE den = 1;
	WORD16 i=0;

	WORD16 cntFacNum = 0;
	
	DOUBLE fraction = 0.0;
	DOUBLE fractionAll = 1.0;

	for(i = 0; i < numWeights; i++)
	{
		fraction = factorialDblStSt(cntFacNum+1,cntFacNum+weights[i])/factorial(weights[i]);
		cntFacNum += weights[i];
		fractionAll *= fraction;
	}
	return(fractionAll);*/
}

MAX_WIDTH_TYPE 
lbvq_getNumberOverload(void* handleMisc)
{
	lbvqMISC* allocatedMISC = (lbvqMISC*) handleMisc;
	return(allocatedMISC->overload);
}

MAX_WIDTH_TYPE 
lbvq_getNumberLinearArea(void* handleMisc)
{
	lbvqMISC* allocatedMISC = (lbvqMISC*) handleMisc;
	return(allocatedMISC->numRadiusLin);
}

MAX_WIDTH_TYPE 
lbvq_getNumberLogArea(void* handleMisc)
{
	lbvqMISC* allocatedMISC = (lbvqMISC*) handleMisc;
	return(allocatedMISC->numRadiusLog);
}

MAX_WIDTH_TYPE 
lbvq_getNumberFrames(void* handleMisc)
{
	lbvqMISC* allocatedMISC = (lbvqMISC*) handleMisc;
	return(allocatedMISC->numberFrames);
}

DOUBLE lbvq_getSNRSignaldB(void* handleMisc)
{
	lbvqMISC* allocatedMISC = (lbvqMISC*) handleMisc;
	return(10*log10((allocatedMISC->energyOriginal+0.000001)/(allocatedMISC->energyDifference+0.000001)));
}

DOUBLE lbvq_getSNRSpheredB(void* handleMisc)
{
	lbvqMISC* allocatedMISC = (lbvqMISC*) handleMisc;
	return(10*log10(((DOUBLE)allocatedMISC->numberFrames+0.000001)/(allocatedMISC->energyDiffSphere+0.000001)));
}

DOUBLE lbvq_getSNRRadiusdB(void* handleMisc)
{
	lbvqMISC* allocatedMISC = (lbvqMISC*) handleMisc;
	return(10*log10((allocatedMISC->energyRadius+0.000001)/(allocatedMISC->energyDiffRadius+0.000001)));
}

DOUBLE lbvq_getEffectiveBitrate(void* handleMISC)
{
	lbvqMISC* allocatedMISC = (lbvqMISC*) handleMISC;
	return(allocatedMISC->effectiveBitrate);
}

DOUBLE lbvq_getEffectiveBitrateCELP(void* handleMISC)
{
	lbvqMISC* allocatedMISC = (lbvqMISC*) handleMISC;
	return(allocatedMISC->effectiveBitrateCELP);
}

void lbvq_terminateDeallocate(void* handleROM, void* handleRAM, void* handleMISC)
{
	WORD16 i,ii,k;

	lbvqROM* allocatedROM = (lbvqROM*)handleROM;
	lbvqRAM* allocatedRAM = (lbvqRAM*)handleRAM;
	lbvqMISC* allocatedMISC = (lbvqMISC*)handleMISC;

	for(i = 0; i < allocatedROM->codebooks.numberCodebooks; i++)
	{
		for(ii = 0; ii < allocatedROM->codebooks.codebooks[i].configuration.numberGrids; ii++)
		{
			for(k = 0; k < allocatedROM->codebooks.codebooks[i].numberSignPermutations[ii]; k++)
			{
				free(allocatedROM->codebooks.codebooks[i].values[ii][k]);
				free(allocatedROM->codebooks.codebooks[i].weights[ii][k]);
			}

			free(allocatedROM->codebooks.codebooks[i].maxNumberWeights[ii]);
			free(allocatedROM->codebooks.codebooks[i].weights[ii]);
			free(allocatedROM->codebooks.codebooks[i].values[ii]);
			free(allocatedROM->codebooks.codebooks[i].configuration.positionsGrid[ii]);
//			free(allocatedRAM->fldRAMCodebook[i].gridRAM[ii].errorVector);
			free(allocatedRAM->fldRAMCodebook[i].gridRAM[ii].excVector);
			free(allocatedRAM->fldRAMCodebook[i].gridRAM[ii].selectedVector);
			free(allocatedRAM->fldRAMCodebook[i].gridRAM[ii].copyWeights);
			free(allocatedROM->codebooks.codebooks[i].numberIndicesSignPermutations[ii]);
		}
		
		free(allocatedROM->codebooks.codebooks[i].configuration.maxNumberPositionsGrid);
		free(allocatedROM->codebooks.codebooks[i].configuration.numberPulsesGrid);
		free(allocatedROM->codebooks.codebooks[i].configuration.positionsGrid);
		free(allocatedROM->codebooks.codebooks[i].divisorCodebookPartitions);
		free(allocatedROM->codebooks.codebooks[i].factorCodebookPartitions);
		free(allocatedROM->codebooks.codebooks[i].numberIndicesSignPermutations);
		free(allocatedROM->codebooks.codebooks[i].numberIndicesGrid);

		free(allocatedRAM->fldRAMCodebook[i].gridRAM);

		free(allocatedROM->codebooks.codebooks[i].weights);
		free(allocatedROM->codebooks.codebooks[i].values);
		free(allocatedROM->codebooks.codebooks[i].maxNumberWeights);
		free(allocatedROM->codebooks.codebooks[i].numberSignPermutations);

		free(allocatedROM->codebooks.codebooks[i].numberBitsConvCELP);
	}
	free(allocatedROM->codebooks.numberIndicesCodebooks);
	free(allocatedROM->codebooks.bitsCodebooksCELP);
	
	free(allocatedROM->codebooks.codebooks);
	free(allocatedRAM->fldRAMCodebook);

	free(allocatedROM->radiusFldBoundsSquared);
	free(allocatedROM->radiusReconstruct);

	memset(allocatedROM, 0, sizeof(allocatedROM));
	free(allocatedROM);
	memset(allocatedRAM, 0, sizeof(allocatedRAM));
	free(allocatedRAM);
	memset(allocatedMISC, 0, sizeof(allocatedMISC));
	free(allocatedMISC);
}


void 
generateVector(MAX_WIDTH_TYPE kk, DOUBLE* excVector, WORD16 weightsTemp[2])
{
	DOUBLE valuesTemp[2] = {1.0, 0.0};
	WORD16 copyWeights[2];
	schalkwijkDecoder(excVector, kk, weightsTemp[0] + weightsTemp[1], weightsTemp, valuesTemp, 2, copyWeights);
}

void 
lbvq_getNumIndices(MAX_WIDTH_TYPE *numNodes, UWORD16 *numIndicesRadius, void *handleROM)
{
	lbvqROM* allocatedROM = (lbvqROM*)handleROM;
	if(numNodes)
		*numNodes = allocatedROM->numberVectors;
	if(numIndicesRadius)
		*numIndicesRadius = allocatedROM->numberIndicesRadius;
}

void lbvq_decodeVector(DOUBLE *vector, UWORD16 vectorLength, MAX_WIDTH_TYPE index, UWORD16 indexRadius, void *handleROM, void *handleRAM)
{
	lbvqROM* allocatedROM = (lbvqROM*)handleROM;
	lbvqRAM* allocatedRAM = (lbvqRAM*)handleRAM;

	WORD16 idxCodebook = 0;
	MAX_WIDTH_TYPE offset = 0;
	WORD16 i,j,k;

	DOUBLE radiusQ = allocatedROM->radiusReconstruct[indexRadius];

	while(1)
	{
		offset = allocatedROM->codebooks.numberIndicesCodebooks[idxCodebook];
		if((index-offset) < 0)
			break;
		index = index-offset;
		idxCodebook++;
	}

	for(i=(allocatedROM->codebooks.codebooks[idxCodebook].configuration.numberGrids-1); i > 0 ; i--)
	{
		MAX_WIDTH_TYPE multFac = 1;
		for(k = 0; k < i; k++)
		{
			multFac *= allocatedROM->codebooks.codebooks[idxCodebook].numberIndicesGrid[k];
		}

		MAX_WIDTH_TYPE indexLocal = index/multFac;
		index = index%multFac;

		offset = 0;
		WORD16 idxPermutation = 0;

		while(1)
		{
			offset = allocatedROM->codebooks.codebooks[idxCodebook].numberIndicesSignPermutations[i][idxPermutation];
			if(indexLocal-offset < 0)
			{
				break;
			}
			indexLocal = indexLocal-offset;
			idxPermutation++;
		}

		// Find idxPermutations
		schalkwijkDecoder(allocatedRAM->fldRAMCodebook[idxCodebook].gridRAM[i].excVector, 
			indexLocal, allocatedROM->codebooks.codebooks[idxCodebook].configuration.maxNumberPositionsGrid[i],
			allocatedROM->codebooks.codebooks[idxCodebook].weights[i][idxPermutation], 
			allocatedROM->codebooks.codebooks[idxCodebook].values[i][idxPermutation],
			allocatedROM->codebooks.codebooks[idxCodebook].maxNumberWeights[i][idxPermutation], 
			allocatedRAM->fldRAMCodebook[idxCodebook].gridRAM[i].copyWeights);

		for(j = 0; j < allocatedROM->codebooks.codebooks[idxCodebook].configuration.maxNumberPositionsGrid[i];j++)
		{
			vector[allocatedROM->codebooks.codebooks[idxCodebook].configuration.positionsGrid[i][j]] = 
				allocatedRAM->fldRAMCodebook[idxCodebook].gridRAM[i].excVector[j]*
				allocatedROM->codebooks.codebooks[idxCodebook].amplitudePulses*radiusQ;
		}
	}

	offset = 0;
	WORD16 idxPermutation = 0;

	while(1)
	{
		offset = allocatedROM->codebooks.codebooks[idxCodebook].numberIndicesSignPermutations[0][idxPermutation];
		if(index-offset < 0)
		{			
			break;
		}
		index = index-offset;
		idxPermutation++;
	}

	// Find idxPermutations
	schalkwijkDecoder(allocatedRAM->fldRAMCodebook[idxCodebook].gridRAM[0].excVector, 
		index, allocatedROM->codebooks.codebooks[idxCodebook].configuration.maxNumberPositionsGrid[0],
		allocatedROM->codebooks.codebooks[idxCodebook].weights[0][idxPermutation], 
		allocatedROM->codebooks.codebooks[idxCodebook].values[0][idxPermutation],
		allocatedROM->codebooks.codebooks[idxCodebook].maxNumberWeights[0][idxPermutation], 
		allocatedRAM->fldRAMCodebook[idxCodebook].gridRAM[0].copyWeights);

	for(j = 0; j < allocatedROM->codebooks.codebooks[idxCodebook].configuration.maxNumberPositionsGrid[0];j++)
	{
		//DOUBLE tmp = 
		vector[allocatedROM->codebooks.codebooks[idxCodebook].configuration.positionsGrid[0][j]] = 
					allocatedRAM->fldRAMCodebook[idxCodebook].gridRAM[0].excVector[j]*
			allocatedROM->codebooks.codebooks[idxCodebook].amplitudePulses*radiusQ;
		/*
			if(tmp != vector[allocatedROM->codebooks.codebooks[idxCodebook].configuration.positionsGrid[idxGrid][j]])
			std::cout << "ohOh!" << std::endl;*/
	}
}
	
	
	

