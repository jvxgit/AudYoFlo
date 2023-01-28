#include "jvx-apvq.h"
#include "jvx-apvq_prv.h"


// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// START ================== ALL MACROS FOR OPERATION MODES =======================

//#define CREATE_VERIFICATION_FILE




//#define USE_OPTIMAL_BITALLOCATION_THEORY

//#define FULL_SEARCH

// END ================== ALL MACROS FOR OPERATION MODES =======================
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


// END ================== MISC TYPEDEFS =======================
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


// END ================== HELPING FUNCTIONS =======================
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// START ================== LOCAL STATIC FUNCTIONS =======================

// END ================== LOCAL STATIC FUNCTIONS =======================
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// START ================== MACRO FUNCTIONS =======================
// END ================== MACRO FUNCTIONS =======================
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<



// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
// INTERFACE FUNCTIONS
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

jvxErrorType jvx_apvq_initCfg(jvxApvq* apvq)
{
	if (apvq)
	{
		apvq->init_param.avalue = 1000;
		apvq->init_param.dimensionVQ = 11;
		apvq->init_param.fillupIndices = false;
		apvq->init_param.maximumNumberCandidateExclusion = 8;
		apvq->init_param.numberBitsTreeStore = JVX_NUMBER_MAX_BITS_STORE_TREE;
		apvq->init_param.overallBitrate = 22;
		apvq->init_param.radiusQCorrectionFactor = 1.0;
		apvq->init_param.useBitBounds = false;
		apvq->init_param.radius_and_sphere_q = true;

		apvq->derived_param.handleMisc = NULL;
		apvq->derived_param.handleRAM = NULL;
		apvq->derived_param.handleROM = NULL;
		apvq->derived_param.numBitsRAM = 0;
		apvq->derived_param.numBitsROM = 0;
		apvq->derived_param.maximumNumberCandidateExclusion = 0;
		apvq->derived_param.maximumNumberCandidateWeights = 0;

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

/**
 * Entry function to allocate and initialize the apple-peeling quantizer.
 * Note that the functionlity is different for different operation modes!
 * INPUT:
 * - dimensionVQ: Vector dimension
 * - overallBitrate: Number of bits for each vector to be quantized
 * - AVALUE: A-value constant
 * - numberBitsTreeStore: Number of bits per entry in coding tree
 * - maximumNumberCandidateExclusion: Maximum candidates to be considered in candidate exclusion
 * - useBitBounds: Use bit boundaries to separate radius and sphere on bit level
 * OUTPUT:
 * - handleROM: Handle to the allocated ROM (do not access data but pass to interface!)
 * - handleRAM: Handle to the allocated RAM (do not access data but pass to interface!)
 * - handleMISC: Handle to the allocated MISC structure (do not access data but pass to interface!)
 * - numBitsROM: Pointer to a jvxInt64 field to return the number of BITS required for ROM
 * - numBitsRAM: Pointer to a jvxInt64 field to return the number of BITS required for RAM
 *///====================================================================
jvxErrorType
jvx_apvq_init(jvxApvq* apvq)
{
	applePeelingROM* allocatedROM = NULL;
	applePeelingRAM* allocatedRAM = NULL;
	applePeelingMISC* allocatedMISC = NULL;

	jvxInt64 bitsROM_codingTree = 0;
	jvxUInt64 bitsROM_trigoLookup = 0;
	jvxInt64 bitsRAM_containers = 0;


	// alpha for nominator for radius estimation
	jvxData vol1_nom = 0.0;

	// alpha for denominator for radius estimation
	jvxData vol1_den = 0.0;

	jvxData vol1 = 0.0;

	jvxData sphereRadiusConstant =0.0;

	jvxUInt64 overallNumberIndices_NT;

	jvxInt32 numberIndicesRadius = 0;
	jvxInt32 numberIndicesRadiusNextAttempt = 0;

	jvxUInt64 numberIndicesSphere_NT = 0;
	jvxUInt64 numberIndicesRadiusAndSphere_NT = 0;
	jvxUInt64 numberIndicesSphereNextAttempt_NT = 0;
	jvxUInt64 numberIndicesRadiusAndSphereNextAttempt_NT = 0;

	jvxInt16 parameterNSP = 0;
	jvxInt16 parameterNSPNextAttempt = 0;

	jvxCBool* useStepsCosSinPi = NULL;
	jvxCBool* useStepsCosSin2Pi = NULL;

	jvxUInt64 cntrVerifyList;
	jvxUInt64 cntrVerifyExt;

#ifdef CREATE_VERIFICATION_FILE
	FILE* hd;
#endif

	jvxUInt16 i;

	jvxData tmp;

	//============== END OF VARIABLE DEFINITIONS =============================

	apvq->derived_param.numBitsROM = 0;
	apvq->derived_param.numBitsRAM = 0;


	// Set operation mode (do this later!)
	//usedOperationMode = this->SPHERE_OPERATION_MODE_UNINITIALIZED;
	allocatedROM = (applePeelingROM*)malloc(sizeof(applePeelingROM));

	memset(allocatedROM, 0, sizeof(applePeelingROM));
	allocatedROM->elementSize = sizeof(applePeelingROM);
	allocatedROM->quantize_sphere_gain = apvq->init_param.radius_and_sphere_q;

	assert(apvq->init_param.numberBitsTreeStore <= JVX_NUMBER_MAX_BITS_STORE_TREE);

	allocatedROM->dimensionVQ = apvq->init_param.dimensionVQ;
	apvq->derived_param.numBitsROM += sizeof(jvxInt16);

	allocatedROM->overallBitrate = apvq->init_param.overallBitrate;
	apvq->derived_param.numBitsROM += sizeof(jvxInt16);

	allocatedMISC = (applePeelingMISC*)malloc(sizeof(applePeelingMISC));
	
	memset(allocatedMISC, 0, sizeof(applePeelingMISC));
	allocatedMISC->elementSize = sizeof(applePeelingMISC);

	allocatedMISC->avalue = apvq->init_param.avalue;
	allocatedMISC->maxAmplitude = MAX_AMPLITUDE;
	
	allocatedMISC->energyOriginal = 0.0;
	allocatedMISC->energyDifference = 0.0;
	allocatedMISC->energyRadius = 0.0;
	allocatedMISC->energyDiffRadius = 0.0;
	allocatedMISC->energyDiffSphere = 0.0;

	allocatedMISC->numRadiusLog = 0;
	allocatedMISC->numRadiusLin = 0;
	allocatedMISC->overload = 0;

	/*
	 * Start the real initialization..
	 */

	if (apvq->init_param.radius_and_sphere_q)
	{
		// Find the volume of a D-dimensional Hypersphere with radius 1.0
		// Approximate the gamma function by factorials
		if (allocatedROM->dimensionVQ % 2)
		{
			vol1_nom = (jvxData)(1 << allocatedROM->dimensionVQ)*pow(M_PI, ((allocatedROM->dimensionVQ - 1) / 2))*factorial(((jvxInt32)allocatedROM->dimensionVQ - 1) / 2);
			vol1_den = factorial((jvxInt32)allocatedROM->dimensionVQ);
		}
		else
		{
			vol1_nom = pow(M_PI, allocatedROM->dimensionVQ / 2);
			vol1_den = factorial((jvxInt32)allocatedROM->dimensionVQ / 2);
		}

		// Here we got the volume
		vol1 = vol1_nom / vol1_den;

		// Find the maximum radius here. The constraint is: Volume of the hypercube with maximum
		// edge lengths equal to that of hypersphere -> maximum radius
		allocatedMISC->maxRadius = (allocatedMISC->maxAmplitude * 2.0) / pow(vol1, (1.0 / (jvxData)allocatedROM->dimensionVQ));
		sphereRadiusConstant = 1.0 / (1.0 + log(allocatedMISC->avalue));

		// Final checks:
#ifdef MODE_32BIT
		if (overallBitrate > 31)
		{
			return(-1);
		}
#endif

		// Determine the number of indices available within the maximum number of bits per vector
		overallNumberIndices_NT = (jvxUInt64)1;
		overallNumberIndices_NT <<= allocatedROM->overallBitrate;

		// Find sphere covering with zentroids
		numberIndicesRadius = 0;
		numberIndicesRadiusNextAttempt = 0;
		numberIndicesRadiusAndSphere_NT = 0;
		numberIndicesSphere_NT = 0;
		numberIndicesSphereNextAttempt_NT = 0;
		numberIndicesRadiusAndSphereNextAttempt_NT = 0;

		parameterNSP = 0;

		while (1)
		{
			parameterNSPNextAttempt = (parameterNSP + 1);
#ifndef USE_OPTIMAL_BITALLOCATION_THEORY
			numberIndicesRadiusNextAttempt = (jvxInt32)(1.0 / (sphereRadiusConstant*M_PI)*(jvxData)parameterNSPNextAttempt);
#endif
			determineNumberNodes((allocatedROM->dimensionVQ - 1), 1.0, (M_PI / (jvxData)parameterNSPNextAttempt), &numberIndicesSphereNextAttempt_NT);

#ifdef USE_OPTIMAL_BITALLOCATION_THEORY
			numberIndicesRadiusNextAttempt = (jvxInt16)ceil(radiusQCorrectionFactor*calcNumberRadius(allocatedROM->dimensionVQ,
				allocatedMISC->avalue, (jvxData)numberIndicesSphereNextAttempt_NT));
#endif
			numberIndicesRadiusAndSphereNextAttempt_NT = ((jvxUInt64)numberIndicesRadiusNextAttempt) * numberIndicesSphereNextAttempt_NT;

#ifdef APVQ_VERBOSE_MODE
			rtpPrintf("APVQ: Parameter NSP: %d\n", parameterNSPNextAttempt);
			rtpPrintf("APVQ: Number Indices Radius: %d\n", numberIndicesRadiusNextAttempt);
			rtpPrintf("APVQ: Number Indices Sphere: %d\n", numberIndicesSphereNextAttempt_NT);
#endif

#ifdef NOT_EXTRA_ZERO
			if (numberIndicesRadiusAndSphereNextAttempt_NT < overallNumberIndices_NT)
			{
				parameterNSP = parameterNSPNextAttempt;
				numberIndicesSphere_NT = numberIndicesSphereNextAttempt_NT;
				numberIndicesRadius = numberIndicesRadiusNextAttempt;
			}
			else
			{
				break;
			}
#else
			if ((numberIndicesRadiusAndSphereNextAttempt_NT + 1) < overallNumberIndices)
			{
				parameterNSP = parameterNSPNextAttempt;
				numberIndicesSphere_NT = numberIndicesSphereNextAttempt_NT;
				numberIndicesRadius = numberIndicesRadiusNextAttempt;
			}
			else
			{
				break;
			}
#endif
		}

#ifdef APVQ_VERBOSE_MODE
		rtpPrintf("APVQ: Sphere design completed, Nsp = %d; ", parameterNSP);
		rtpPrintf("APVQ: Msp = %d; ", numberIndicesSphere_NT);
		rtpPrintf("APVQ: Mr = %d\n", numberIndicesRadius);
#endif

		//= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
		//= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
		// ALLOCATE ROM =====================================================
		//= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
		//= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 

		allocatedROM->numberNodesMax_NT = numberIndicesSphere_NT;
		apvq->derived_param.numBitsROM += sizeof(jvxUInt64);
		allocatedROM->numberNodesValid_NT = numberIndicesSphere_NT;
		apvq->derived_param.numBitsROM += sizeof(jvxUInt64);

		if (apvq->init_param.useBitBounds)
		{
			allocatedROM->numBitsSpherePart = C32_16((jvxInt32)ceil(log10((jvxData)allocatedROM->numberNodesValid_NT) / log10(2.0)));
			apvq->derived_param.numBitsROM += sizeof(jvxInt16);

			allocatedROM->numBitsRadiusPart = allocatedROM->overallBitrate - allocatedROM->numBitsSpherePart;
			apvq->derived_param.numBitsROM += sizeof(jvxInt16);

			allocatedROM->numberNodesMax_NT = ((jvxInt64)1) << allocatedROM->numBitsSpherePart;

			// If extra zero is required, it is part of the spherical code!
			allocatedROM->numberIndicesRadius = CU32_16(((jvxInt32)1) << allocatedROM->numBitsRadiusPart);
			apvq->derived_param.numBitsROM += sizeof(jvxInt16);

			allocatedMISC->effectiveBitrate = ((jvxData)allocatedROM->numBitsSpherePart + allocatedROM->numBitsRadiusPart) / ((jvxData)allocatedROM->dimensionVQ);

#ifdef APVQ_VERBOSE_MODE
			rtpPrintf("APVQ: ## Apple-peeling in bitBounds mode, \n number of bits sphere:%d; number of bits radius: %d\n", allocatedROM->numBitsSpherePart, allocatedROM->numBitsRadiusPart);
			rtpPrintf("APVQ: ## Number available indices sphere: %d\n", allocatedROM->numberNodesMax_NT);
			rtpPrintf("APVQ: ## Number valid indices sphere: %d\n", allocatedROM->numberNodesValid_NT);
			rtpPrintf("APVQ: ## Number indices radius: %d\n", allocatedROM->numberIndicesRadius);
			rtpPrintf("APVQ: ## Effective Bitrate: %f\n", allocatedMISC->effectiveBitrate);
			rtpPrintf("APVQ: ## Effective Bitrate Sphere: %f\n", log10((jvxData)allocatedROM->numberNodesValid_NT) / log10(2.0) / allocatedROM->dimensionVQ);
#endif
		}
		else
		{
			allocatedROM->numBitsSpherePart = -1;
			allocatedROM->numBitsRadiusPart = -1;
			if (apvq->init_param.fillupIndices)
			{
				// This information is not required for encoding/decoding..
#ifdef NOT_EXTRA_ZERO
#ifdef USE_OPTIMAL_BITALLOCATION_THEORY
				allocatedROM->numberIndicesRadius = (jvxInt16)ceil(radiusQCorrectionFactor*calcNumberRadius(allocatedROM->dimensionVQ,
					allocatedMISC->avalue, (jvxData)allocatedROM->numberNodesValid_NT));
#else
				allocatedROM->numberIndicesRadius = C32_16((jvxInt32)(floor((jvxData)overallNumberIndices_NT / (jvxData)allocatedROM->numberNodesValid_NT)));
				apvq->derived_param.numBitsROM += sizeof(jvxInt16);
#endif
#else
#ifdef USE_OPTIMAL_BITALLOCATION_THEORY
				allocatedROM->numberIndicesRadius = (jvxInt16)ceil(radiusQCorrectionFactor*calcNumberRadius(allocatedROM->dimensionVQ,
					allocatedMISC->avalue, (jvxData)allocatedROM->numberNodesValid_NT));
#else
			// Be sure that we have a zero index as reserve!
				allocatedROM->numberIndicesRadius = C32_16((jvxInt32)(floor((jvxData)(overallNumberIndices_NT - 1) / (jvxData)allocatedROM->numberNodesValid)));
				apvq->derived_param.numBitsROM += SIZE_jvxInt16;
#endif
#endif
			}
			else
			{
				allocatedROM->numberIndicesRadius = C32_16(numberIndicesRadius);
			}
			allocatedMISC->effectiveBitrate = (log10((jvxData)allocatedROM->numberNodesValid_NT) + log10((jvxData)allocatedROM->numberIndicesRadius)) / log10(2.0) / ((jvxData)allocatedROM->dimensionVQ);
#ifdef APVQ_VERBOSE_MODE
			rtpPrintf("APVQ: ## Apple-peeling in normal mode, number of valid indices sphere: %d\n", allocatedROM->numberNodesValid_NT);
			rtpPrintf("APVQ:  number of indices radius: %d\n", allocatedROM->numberIndicesRadius);
			rtpPrintf("APVQ: ## Effective Bitrate: %f\n", allocatedMISC->effectiveBitrate);
			rtpPrintf("APVQ: ## Effective Bitrate Sphere: %f\n", log10((jvxData)allocatedROM->numberNodesValid_NT) / log10(2.0) / allocatedROM->dimensionVQ);
#endif

		}

		allocatedROM->radiusFldBoundsSquared = (jvxData*)malloc(sizeof(jvxData)*(allocatedROM->numberIndicesRadius + 1));
		apvq->derived_param.numBitsROM += sizeof(jvxData)*(allocatedROM->numberIndicesRadius + 1);

		allocatedROM->radiusReconstruct = (jvxData*)malloc(sizeof(jvxData)*allocatedROM->numberIndicesRadius);
		apvq->derived_param.numBitsROM += sizeof(jvxData)*(allocatedROM->numberIndicesRadius);

		// Design of the quantizer for the radius
		allocatedROM->radiusFldBoundsSquared[0] = -FLT_MAX;
		allocatedROM->radiusFldBoundsSquared[allocatedROM->numberIndicesRadius] = +FLT_MAX;

		// Set the radius quantization zentroids and the bounds inbetween for a faster decision
		for (i = 0; i < allocatedROM->numberIndicesRadius; i++)
		{
			allocatedROM->radiusReconstruct[i] = readOutRadiusIndex(i, allocatedROM->numberIndicesRadius, 0, allocatedMISC->maxRadius);
			allocatedROM->radiusReconstruct[i] = convertLog2LinRadius(allocatedROM->radiusReconstruct[i], allocatedMISC->maxRadius,
				allocatedMISC->avalue, sphereRadiusConstant);

			printf("APVQ: Radius reconstruction Value[%d] = %f\n", i, allocatedROM->radiusReconstruct[i]);
			if (i != 0)
			{
				//fldBounds[i-1] = (fldZentrRadius[i] + fldZentrRadius[i-1])/2;
				tmp = (allocatedROM->radiusReconstruct[i] + allocatedROM->radiusReconstruct[i - 1]) / 2;
				allocatedROM->radiusFldBoundsSquared[i] = tmp * tmp;//fldBounds[i-1]*fldBounds[i-1];
			}
		}
	}
	else
	{
		allocatedMISC->maxRadius = 1;
		sphereRadiusConstant = 1.0;

		// Final checks:
#ifdef MODE_32BIT
		if (overallBitrate > 31)
		{
			return(-1);
		}
#endif

		if (JVX_CHECK_SIZE_SELECTED(allocatedROM->overallBitrate))
		{
			// Determine the number of indices available within the maximum number of bits per vector
			overallNumberIndices_NT = (jvxUInt64)1;
			overallNumberIndices_NT <<= allocatedROM->overallBitrate;

			// Find sphere covering with zentroids
			numberIndicesRadius = 0;
			numberIndicesRadiusNextAttempt = 0;
			numberIndicesRadiusAndSphere_NT = 0;
			numberIndicesSphere_NT = 0;
			numberIndicesSphereNextAttempt_NT = 0;
			numberIndicesRadiusAndSphereNextAttempt_NT = 0;

			parameterNSP = 0;

			while (1)
			{
				parameterNSPNextAttempt = (parameterNSP + 1);
				numberIndicesRadiusNextAttempt = 1;
				determineNumberNodes((allocatedROM->dimensionVQ - 1), 1.0, (M_PI / (jvxData)parameterNSPNextAttempt), &numberIndicesSphereNextAttempt_NT);
				numberIndicesRadiusAndSphereNextAttempt_NT = ((jvxUInt64)numberIndicesRadiusNextAttempt) * numberIndicesSphereNextAttempt_NT;

#ifdef APVQ_VERBOSE_MODE
				rtpPrintf("APVQ: Parameter NSP: %d\n", parameterNSPNextAttempt);
				rtpPrintf("APVQ: Number Indices Radius: %d\n", numberIndicesRadiusNextAttempt);
				rtpPrintf("APVQ: Number Indices Sphere: %d\n", numberIndicesSphereNextAttempt_NT);
#endif

#ifdef NOT_EXTRA_ZERO
				if (numberIndicesRadiusAndSphereNextAttempt_NT < overallNumberIndices_NT)
				{
					parameterNSP = parameterNSPNextAttempt;
					numberIndicesSphere_NT = numberIndicesSphereNextAttempt_NT;
					numberIndicesRadius = numberIndicesRadiusNextAttempt;
				}
				else
				{
					break;
				}
#else
				if ((numberIndicesRadiusAndSphereNextAttempt_NT + 1) < overallNumberIndices)
				{
					parameterNSP = parameterNSPNextAttempt;
					numberIndicesSphere_NT = numberIndicesSphereNextAttempt_NT;
					numberIndicesRadius = numberIndicesRadiusNextAttempt;
				}
				else
				{
					break;
				}
#endif
			}
		}
		else
		{
			parameterNSP = apvq->init_param.num_delta_steps;
			numberIndicesRadiusNextAttempt = 1;
			determineNumberNodes((allocatedROM->dimensionVQ - 1), 1.0, (M_PI / (jvxData)parameterNSP), &numberIndicesSphere_NT);
		}
		allocatedROM->numberNodesMax_NT = numberIndicesSphere_NT;
		apvq->derived_param.numBitsROM += sizeof(jvxUInt64);
		allocatedROM->numberNodesValid_NT = numberIndicesSphere_NT;
		apvq->derived_param.numBitsROM += sizeof(jvxUInt64);
		allocatedROM->numberIndicesRadius = 1;
		allocatedROM->radiusFldBoundsSquared = NULL;
		allocatedROM->radiusReconstruct = NULL;
	}


	// Allocate the coding tree. We directly use the compact representation..
	allocatedROM->linearListTree = NULL;
	allocatedMISC->numberElementsLinearList = 0;

	allocatedROM->externalReferencesTree = NULL;
	allocatedMISC->numberElementsExternalReferences = 0;

	allocatedMISC->numberBitsToStoreExternalElements = 0;
	allocatedMISC->numberStepsPi = 0;
	allocatedMISC->numberSteps2Pi = 0;

	// Step I: Determine the length of the coding in a linear list, at the same time determine the 
	// number of values that will have to be stored in an external field and the maximum number of sin/cos values required
	
	allocatedMISC->numberElementsLinearList = determineCodingTreeLength((allocatedROM->dimensionVQ-1), 
		1.0, (M_PI/(jvxData)parameterNSP), (allocatedROM->dimensionVQ-1), &allocatedMISC->numberElementsExternalReferences, 
		apvq->init_param.numberBitsTreeStore, &allocatedMISC->numberBitsToStoreExternalElements,
		&allocatedMISC->numberStepsPi, &allocatedMISC->numberSteps2Pi, 0);

	allocatedROM->linearListTree = (jvxTreeListType*)malloc(sizeof(jvxTreeListType)*allocatedMISC->numberElementsLinearList);
	bitsROM_codingTree = apvq->init_param.numberBitsTreeStore*allocatedMISC->numberElementsLinearList;

	allocatedROM->externalReferencesTree = (jvxUInt64*) malloc(sizeof(jvxUInt64)*allocatedMISC->numberElementsExternalReferences);
	bitsROM_codingTree += allocatedMISC->numberBitsToStoreExternalElements*allocatedMISC->numberElementsExternalReferences;

	apvq->derived_param.numBitsROM += bitsROM_codingTree;

	cntrVerifyList = allocatedMISC->numberElementsLinearList;
	cntrVerifyExt = allocatedMISC->numberElementsExternalReferences;

	// Determine the size of the codebook for coding tree
//	bitsROM = (newSearchStructROM.numberOfElmsInLinearList* newSearchStructROM.numberBitsDataElementsTree);
//	bitsROM += (newSearchStructROM.numberExternalValues* newSearchStructROM.numberBitsDataElementsExternal);
	
	allocatedMISC->numberElementsLinearList = 0;
	allocatedMISC->numberElementsExternalReferences = 0;

	// Test all grid in cos and sinus whether they are required in coding tree or not
	useStepsCosSinPi = (jvxCBool*)malloc(sizeof(jvxCBool)*allocatedMISC->numberStepsPi);
	useStepsCosSin2Pi = (jvxCBool*)malloc(sizeof(jvxCBool)*allocatedMISC->numberSteps2Pi);

	memset(useStepsCosSinPi, 0, sizeof(jvxCBool)*allocatedMISC->numberStepsPi);
	memset(useStepsCosSin2Pi, 0, sizeof(jvxCBool)*allocatedMISC->numberSteps2Pi);

	//! Now fill the coding tree AND indicate cos and sin grids used
	allocatedMISC->numberElementsLinearList = determineCodingTree(
		allocatedROM->linearListTree, 
		allocatedROM->externalReferencesTree, (allocatedROM->dimensionVQ-1), 1.0, (M_PI/(jvxData)parameterNSP), 
		(allocatedROM->dimensionVQ-1), &allocatedMISC->numberElementsExternalReferences, 
		apvq->init_param.numberBitsTreeStore, &allocatedMISC->numberBitsToStoreExternalElements,
		useStepsCosSinPi, allocatedMISC->numberStepsPi, 
		useStepsCosSin2Pi, allocatedMISC->numberSteps2Pi);

	assert(cntrVerifyList == allocatedMISC->numberElementsLinearList);
	assert(cntrVerifyExt == allocatedMISC->numberElementsExternalReferences);

	bitsROM_trigoLookup = 0;
	allocateFieldCosSin(&allocatedROM->angleFieldPi, &allocatedROM->cosFieldPi, &allocatedROM->sinFieldPi, useStepsCosSinPi, M_PI,
		allocatedMISC->numberStepsPi,  &bitsROM_trigoLookup);
	allocateFieldCosSin(&allocatedROM->angleField2Pi, &allocatedROM->cosField2Pi, &allocatedROM->sinField2Pi, useStepsCosSin2Pi, M_PI*2.0,
		allocatedMISC->numberSteps2Pi,  &bitsROM_trigoLookup);
	
	free(useStepsCosSinPi);
	free(useStepsCosSin2Pi);

	apvq->derived_param.numBitsROM += bitsROM_trigoLookup;

#ifdef CREATE_VERIFICATION_FILE
	hd = fopen("verList_new.bin", "wb");
	fwrite(allocatedROM->linearListTree, sizeof(jvxUInt16), allocatedMISC->numberElementsLinearList, hd);
	fclose(hd);
	hd = fopen("verExt_new.bin", "wb");
	fwrite(allocatedROM->externalReferencesTree, sizeof(jvxUInt64), allocatedMISC->numberElementsExternalReferences,hd);
	fclose(hd);
#endif

	allocatedROM->maxNumberCandidates = CU32_16(1 << (allocatedROM->dimensionVQ - 2));
	if (JVX_CHECK_SIZE_SELECTED(apvq->init_param.maximumNumberCandidateExclusion))
	{
		allocatedROM->maxNumberCandidates = JVX_MIN(allocatedROM->maxNumberCandidates,
			CU32_16(apvq->init_param.maximumNumberCandidateExclusion));
	}
	apvq->derived_param.maximumNumberCandidateExclusion = allocatedROM->maxNumberCandidates;
	apvq->derived_param.maximumNumberCandidateWeights = apvq->derived_param.maximumNumberCandidateExclusion * 2;

	//= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	//= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	// ALLOCATZE RAM ====================================================
	//= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	//= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 

	apvq->derived_param.numBitsRAM = 0;
	allocatedRAM = (applePeelingRAM*)malloc(sizeof(applePeelingRAM));
	memset(allocatedRAM, 0, sizeof(applePeelingRAM));
	allocatedRAM->elementSize = sizeof(applePeelingRAM);

	// Allocate the MAX*2 candidates
	allocatedRAM->candidates = (candidateSearchStruct*)malloc(sizeof(candidateSearchStruct)*(allocatedROM->maxNumberCandidates * 2));
	bitsRAM_containers = (allocatedROM->maxNumberCandidates * 2)*(sizeof(jvxData) + (sizeof(jvxData) + sizeof(jvxData*) + allocatedROM->dimensionVQ *
		sizeof(jvxData) + sizeof(jvxData*) + sizeof(jvxUInt64) +
		sizeof(jvxInt16) + sizeof(jvxInt16) + sizeof(jvxData) + sizeof(jvxInt16) + sizeof(jvxInt16) + sizeof(jvxInt16) + sizeof(jvxUInt64)));

	allocatedRAM->usedContainers = (candidateSearchStruct**)malloc(sizeof(candidateSearchStruct*)*allocatedROM->maxNumberCandidates);
	apvq->derived_param.numBitsRAM += sizeof(jvxInt16*)*allocatedROM->maxNumberCandidates;
	allocatedRAM->availableContainers = (candidateSearchStruct**)malloc(sizeof(candidateSearchStruct*)*allocatedROM->maxNumberCandidates * 2);
	apvq->derived_param.numBitsRAM += sizeof(jvxInt16*)*allocatedROM->maxNumberCandidates * 2;

	allocatedRAM->candidatesExcluded = (candidateSearchStruct**)malloc(sizeof(candidateSearchStruct*)*allocatedROM->maxNumberCandidates);
	apvq->derived_param.numBitsRAM += sizeof(jvxInt16*)*allocatedROM->maxNumberCandidates;

	allocatedRAM->candidatesNotExcluded = (candidateSearchStruct**)malloc(sizeof(candidateSearchStruct*)*allocatedROM->maxNumberCandidates);
	apvq->derived_param.numBitsRAM += sizeof(jvxInt16*)*allocatedROM->maxNumberCandidates;

	// Initialiuze all containers and set pointer references in table of available containers
	for (i = 0; i < allocatedROM->maxNumberCandidates * 2; i++)
	{
		allocatedRAM->candidates[i].fieldDest = (jvxData*)malloc(sizeof(jvxData)*allocatedROM->dimensionVQ);
		bitsRAM_containers += sizeof(jvxData) * allocatedROM->dimensionVQ;
		allocatedRAM->availableContainers[i] = &allocatedRAM->candidates[i];
	}

	apvq->derived_param.numBitsRAM += bitsRAM_containers;

	allocatedRAM->fieldConvOutputUnQ = (jvxData*)malloc(sizeof(jvxData)* allocatedROM->dimensionVQ);
	apvq->derived_param.numBitsRAM += sizeof(jvxData) * allocatedROM->dimensionVQ;
	allocatedRAM->elementSize = sizeof(applePeelingRAM);

#ifdef APVQ_VERBOSE_MODE
	rtpPrintf("APVQ: ROM Size: %f [kByte]\n", ((jvxData)*ptrROM) / (8.0*1024.0));
	rtpPrintf("APVQ: --> ROM Size Coding trees: %f [kByte]\n", ((jvxData)bitsROM_codingTree) / (8.0*1024.0));
	rtpPrintf("APVQ: --> ROM Size Trigonometric Lookups: %f [kByte]\n", ((jvxData)bitsROM_trigoLookup) / (8.0*1024.0));
	rtpPrintf("APVQ: RAM Size: %f [kByte]\n", ((jvxData)*ptrRAM) / (8.0*1024.0));
	rtpPrintf("APVQ: --> RAM Size Containers: %f [kByte]\n", ((jvxData)bitsRAM_containers) / (8.0*1024.0));
#endif
	

	apvq->derived_param.handleMisc = (void*)allocatedMISC;
	apvq->derived_param.handleROM = (void*)allocatedROM;
	apvq->derived_param.handleRAM = (void*)allocatedRAM;

	return JVX_NO_ERROR;
}

#if 0
jvxErrorType
jvx_apvq_dumprom(jvxApvq* apvq, const char* filenameDump)
{
	FILE* fPtr = NULL;
	applePeelingROM* allocatedROM = (applePeelingROM*)apvq->derived_param.handleROM;
	const char tmpStr[9] = AP_VERSION;

	// Open file
	fopen_s(&fPtr, filenameDump, "wb");

	if(!&fPtr)
	{
		// Error
		printf("Error: File %s could not be opened for dump!\n", filenameDump);
		return(-1);
	}

	fwrite(tmpStr, 8,1, fPtr);

	fclose(fPtr);
	return(JVX_NO_ERROR);
}
#endif

jvxErrorType
jvx_apvq_terminate(jvxApvq* apvq)
{
	jvxInt16 i;

	jvxData snrRadius= 0;
	jvxData snrVectors = 0;

	applePeelingROM* allocatedROM = (applePeelingROM*)apvq->derived_param.handleROM;
	applePeelingRAM* allocatedRAM = (applePeelingRAM*)apvq->derived_param.handleRAM;
	applePeelingMISC* allocatedMISC = (applePeelingMISC*)apvq->derived_param.handleMisc;

	assert(allocatedROM->elementSize == sizeof(applePeelingROM));
	assert(allocatedMISC->elementSize == sizeof(applePeelingMISC));
	if (allocatedRAM)
	{
		assert(allocatedRAM->elementSize == sizeof(applePeelingRAM));
	}

	snrRadius = 10*log10(allocatedMISC->energyRadius/(allocatedMISC->energyDiffRadius+0.000000001));
	snrVectors = 10*log10(allocatedMISC->energyOriginal/(allocatedMISC->energyDifference+0.000000001));

	for (i = 0; i < allocatedROM->maxNumberCandidates * 2; i++)
	{
		free(allocatedRAM->candidates[i].fieldDest);
	}
	free(allocatedRAM->usedContainers);
	free(allocatedRAM->availableContainers);
	free(allocatedRAM->candidatesExcluded);
	free(allocatedRAM->candidatesNotExcluded);
	free(allocatedRAM->fieldConvOutputUnQ);
	free(allocatedRAM->candidates);

	free(allocatedRAM);

	deallocateFieldCosSin(allocatedROM->angleFieldPi, allocatedROM->cosFieldPi, allocatedROM->sinFieldPi, allocatedMISC->numberStepsPi);
	deallocateFieldCosSin(allocatedROM->angleField2Pi, allocatedROM->cosField2Pi, allocatedROM->sinField2Pi, allocatedMISC->numberSteps2Pi);

	free(allocatedROM->linearListTree);
	free(allocatedROM->externalReferencesTree);

	if (allocatedROM->quantize_sphere_gain)
	{
		free(allocatedROM->radiusReconstruct);
		free(allocatedROM->radiusFldBoundsSquared);
	}

	memset(allocatedROM, 0, sizeof(applePeelingROM));
	free(allocatedROM);

	memset(allocatedMISC, 0, sizeof(applePeelingMISC));
	free(allocatedMISC);
	
	apvq->derived_param.handleROM = NULL;
	apvq->derived_param.handleRAM = NULL;
	apvq->derived_param.handleMisc = NULL;

	return JVX_NO_ERROR;
}

void 
jvx_apvq_quantize_cl(jvxApvq* apvq, jvxData* targetVector,  jvxData* impulseResp,
					  jvxUInt64* indexVector_NT, jvxUInt64* indexRadius, 
					  jvxSize vectorLength, jvxData* vectorQ, jvxCBool doRequantization)
{
	jvxUInt16 i,j;

	jvxData val1, val2;

	jvxData metricNum, metricDen;

	applePeelingROM* allocatedROM = (applePeelingROM*)apvq->derived_param.handleROM;
	applePeelingRAM* allocatedRAM = (applePeelingRAM*)apvq->derived_param.handleRAM;
	applePeelingMISC* allocatedMISC = (applePeelingMISC*)apvq->derived_param.handleMisc;

	jvxUInt16 cnt = 0;
	jvxData realRadius = 0; //EPSILON_RADIUS;
	jvxData radiusQ, radiusOpt;

	assert(allocatedROM->elementSize == sizeof(applePeelingROM));
	assert(allocatedRAM->elementSize == sizeof(applePeelingRAM));
	assert(allocatedMISC->elementSize == sizeof(applePeelingMISC));

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

	*indexRadius = getIndexRadius(realRadius, allocatedROM);

	// Read out the radius
	radiusQ = allocatedROM->radiusReconstruct[*indexRadius];

	allocatedMISC->energyDiffRadius += (radiusQ- sqrt(realRadius))*(radiusQ- sqrt(realRadius));

	// Search the best matching vector for normalized input in a closed loop manner, 
	// use the OL vectors for a preselection and the target signal for a CL search
	// on the reduced set of vectors
	// It is not important whether the signal is normalized prior to search as
	// the radius is removed in the search procedure anyway

	// Employ sphere quantizer
	quantizeVectorSphereCL(targetVector, radiusQ, impulseResp, indexVector_NT, 
						  realRadius+EPSILON_RADIUS, allocatedROM, allocatedRAM, allocatedMISC);
	
	decodeVectorSphere(vectorQ, *indexVector_NT, allocatedROM, true);
	if(doRequantization)
	{
		metricNum = 0.0;
		metricDen = 0.0000000001;
		for(i = 0; i < vectorLength; i++)
		{
			val1 = 0.0;
			val2 = 0.0;

			for(j = 0; j <= i; j++)
			{
				val1 += impulseResp[i-j] * vectorQ[j];
				val2 += impulseResp[i-j] * targetVector[j];
			}
			metricNum += val1*val2;
			metricDen += val1*val1;
		}
		radiusOpt = metricNum/metricDen;
		realRadius = radiusOpt*radiusOpt;
		*indexRadius = getIndexRadius(realRadius, allocatedROM);
		radiusQ = allocatedROM->radiusReconstruct[*indexRadius];
	}
	for(i = 0; i < vectorLength; i++)
	{
		allocatedMISC->energyDiffSphere += (targetVector[i]/sqrt(realRadius+0.000000001)-vectorQ[i])*(targetVector[i]/sqrt(realRadius+0.000000001)-vectorQ[i]);
		vectorQ[i] *= radiusQ;
	}
}

jvxErrorType 
jvx_apvq_quantize(jvxApvq* apvq, jvxData* targetVector, jvxUInt64* indexVector_NT, jvxUInt64* indexRadius,
	jvxSize vectorLength, jvxData* vectorQ, jvxCBool doRequantization,
	jvxData* metric, jvxApvq_interpol* ipol, jvxApvq_ipolweights* ipolwghts)
	//,
	//jvxUInt64* neighborsI, jvxData* neighborsW, jvxSize nNeighbors)
{
	jvxUInt16 i;

	jvxData metricNum, metricDen;

	applePeelingROM* allocatedROM = (applePeelingROM*)apvq->derived_param.handleROM;
	applePeelingRAM* allocatedRAM = (applePeelingRAM*)apvq->derived_param.handleRAM;
	applePeelingMISC* allocatedMISC = (applePeelingMISC*)apvq->derived_param.handleMisc;

	jvxUInt16 cnt = 0;
	jvxData realRadius = 0; //EPSILON_RADIUS;
	jvxData radiusQ, radiusOpt;

#ifdef FULL_SEARCH

	jvxUInt64 i64;
	jvxData metricMin = FLT_MAX;
	jvxData metricLocal;
	jvxUInt64 idSelect = 0;
#endif
	assert(allocatedROM->elementSize == sizeof(applePeelingROM));
	assert(allocatedRAM->elementSize == sizeof(applePeelingRAM));
	assert(allocatedMISC->elementSize == sizeof(applePeelingMISC));

	allocatedMISC->numberFrames++;
	/*	ULINT cnt; 
	double realRadius = 0;//EPSILON_RADIUS;
	ULINT useIndex = 0;
*/
	for(cnt = 0; cnt < vectorLength; cnt++)
	{
		realRadius += (targetVector[cnt] * targetVector[cnt]);
	}

	if (!allocatedROM->quantize_sphere_gain)
	{
		if (sqrt(realRadius) > allocatedMISC->maxRadius)
			allocatedMISC->overload++;

		if (sqrt(realRadius) < allocatedMISC->maxRadius / allocatedMISC->avalue)
		{
			allocatedMISC->numRadiusLin++;
		}
		else
		{
			allocatedMISC->numRadiusLog++;
		}
	}

	// For performance measuring, apply 
	allocatedMISC->energyRadius += realRadius;

	if (allocatedROM->quantize_sphere_gain)
	{
		*indexRadius = getIndexRadius(realRadius, allocatedROM);

		// Read out the radius
		radiusQ = allocatedROM->radiusReconstruct[*indexRadius];
	}
	else
	{
		radiusQ = sqrt(realRadius);
	}

	allocatedMISC->energyDiffRadius += (radiusQ- sqrt(realRadius))*(radiusQ- sqrt(realRadius));

	// Search the best matching vector for normalized input in a closed loop manner, 
	// use the OL vectors for a preselection and the target signal for a CL search
	// on the reduced set of vectors
	// It is not important whether the signal is normalized prior to search as
	// the radius is removed in the search procedure anyway
#ifdef FULL_SEARCH

	for(i64 = 0; i64 < allocatedROM->numberNodesValid_NT; i64++)
	{
		decodeVectorSphere(vectorQ, i64, allocatedROM);
		metricLocal = 0.0;
		for(i = 0; i < allocatedROM->dimensionVQ; i++)
		{
			metricLocal += (vectorQ[i]*radiusQ-targetVector[i])*(vectorQ[i]*radiusQ-targetVector[i]);
		}
		if(metricLocal < metricMin)
		{
			idSelect = i64;
			metricMin = metricLocal;
		}
	}
	*indexVector_NT = idSelect;

	allocatedMISC->energyOriginal  += realRadius;
	allocatedMISC->energyDifference += metricMin;

#else

	// Employ sphere quantizer
	quantizeVectorSphereOL(targetVector, radiusQ, 
		realRadius + EPSILON_RADIUS, allocatedROM, 
		allocatedRAM, allocatedMISC,
		indexVector_NT, metric, ipol, ipolwghts);
#endif	

	if (vectorQ)
	{
		decodeVectorSphere(vectorQ, *indexVector_NT, allocatedROM, true);

		if (allocatedROM->quantize_sphere_gain)
		{
			if (doRequantization)
			{
				metricNum = 0.0;
				metricDen = 0.0000000001;
				for (i = 0; i < vectorLength; i++)
				{
					metricNum += vectorQ[i] * targetVector[i];
					metricDen += vectorQ[i] * vectorQ[i];
				}
				radiusOpt = metricNum / metricDen;
				realRadius = radiusOpt * radiusOpt;
				*indexRadius = getIndexRadius(realRadius, allocatedROM);
				radiusQ = allocatedROM->radiusReconstruct[*indexRadius];
			}
		}

		for (i = 0; i < vectorLength; i++)
		{
			allocatedMISC->energyDiffSphere += (targetVector[i] / sqrt(realRadius + 0.000000001) - vectorQ[i])*(targetVector[i] / sqrt(realRadius + 0.000000001) - vectorQ[i]);
			vectorQ[i] *= radiusQ;
		}
	}
	return JVX_NO_ERROR;
}
//#endif


//#endif

jvxErrorType
jvx_apvq_decode(jvxApvq* apvq, jvxData* vectorOutput, jvxSize vectorLength, jvxUInt64 indexVector_NT, jvxUInt32 indexRadius, jvxBool outCart)
{
	jvxUInt16 i;
	applePeelingROM* allocatedROM = (applePeelingROM*)apvq->derived_param.handleROM;
	jvxData radiusQ = 1.0;
	if (allocatedROM->quantize_sphere_gain)
	{
		jvxData radiusQ = allocatedROM->radiusReconstruct[indexRadius];
	}
	decodeVectorSphere(vectorOutput, indexVector_NT, allocatedROM, outCart);
	if (outCart)
	{
		for (i = 0; i < vectorLength; i++)
		{
			vectorOutput[i] *= radiusQ;
		}
	}
	else
	{
		// If in cartesian, the variables are all set
		// vectorOutput[allocatedROM->dimensionVQ - 1] = radiusQ;
	}
	return JVX_NO_ERROR;
}

/**
  * Access functions to get post processing information
  *///===================================================
jvxErrorType 
jvx_apvq_performance(jvxApvq* apvq,
	jvxSize* num_overload, jvxSize* num_lin, jvxSize* num_log_area,
	jvxData*snr_signal_db, jvxData* snr_sphere_db, jvxData* snr_radius_db, jvxData* eff_birate)
{
	applePeelingROM* allocatedROM = (applePeelingROM*)apvq->derived_param.handleROM;
	applePeelingMISC* allocatedMISC = (applePeelingMISC*)apvq->derived_param.handleMisc;
	if(snr_signal_db)
		*snr_signal_db = 10 * log10((allocatedMISC->energyOriginal + 0.000001) / (allocatedMISC->energyDifference + 0.000001));
	if (snr_sphere_db)
		*snr_sphere_db = 10 * log10(((jvxData)allocatedMISC->numberFrames + 0.000001) / (allocatedMISC->energyDiffSphere + 0.000001));
	if (snr_radius_db)
		*snr_radius_db = 10 * log10((allocatedMISC->energyRadius + 0.000001) / (allocatedMISC->energyDiffRadius + 0.000001));
	if (eff_birate)
		*eff_birate = allocatedMISC->effectiveBitrate;

	if (num_overload)
		*num_overload = allocatedMISC->overload;
	if (num_lin)
		*num_lin = allocatedMISC->numRadiusLin;
	if (num_log_area)
		*num_log_area = allocatedMISC->numRadiusLog;

	return JVX_NO_ERROR;
}


jvxErrorType
jvx_apvq_number_indices(jvxApvq* apvq, 
	jvxUInt64 *numNodes,
	jvxUInt64 *numIndicesRadius)
{
	applePeelingROM *allocatedROM = (applePeelingROM *)apvq->derived_param.handleROM;
	if (numNodes)
	{
		*numNodes = allocatedROM->numberNodesMax_NT;
	}

	if (numIndicesRadius)
	{
		*numIndicesRadius = allocatedROM->numberIndicesRadius;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
jvx_apvq_number_candidates(jvxApvq* apvq, jvxSize* numCandMax)
{
	if (numCandMax)
	{
		*numCandMax = apvq->derived_param.maximumNumberCandidateWeights;
	}
	return JVX_NO_ERROR;
}
