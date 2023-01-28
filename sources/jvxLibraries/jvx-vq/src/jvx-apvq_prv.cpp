#include "jvx-apvq_prv.h"
#include "jvx-apvq.h"

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// START ================== HELPING FUNCTIONS =======================
jvxInt32 factorial(jvxInt32 input)
{
	unsigned i;
	jvxInt32 res = 1;
	for(i = input; i > 0; i--)
		res *= i;
	return(res);
}

// Functions to assure that no problem arises due to short word lengths
//=====================================================================
#define MAX32B 0x7FFFFFFF
#define MAX32BU 0xFFFFFFFF
#define MAX16B 0x7FFF
#define MAX16BU 0xFFFF

jvxInt16 C32_16(jvxInt32 in)
{
	assert(in <= MAX16B);
	return((jvxInt16)in);
}

jvxUInt16 CU32_16(jvxUInt32 in)
{
	assert(in <= MAX16BU);
	return((jvxUInt16)in);
}

jvxInt32 C64_32(jvxInt64 in)
{
	assert(in <= MAX32B);
	return((jvxInt32)in);
}

jvxUInt32 CU64_32(jvxUInt64 in)
{
	assert(in <= MAX32BU);
	return((jvxUInt32)in);
}

jvxData factorialDbl(jvxInt16 input)
{
	jvxInt16 i;
	jvxData res = 1;
	for (i = input; i > 0; i--)
		res *= (jvxData)i;
	return(res);
}

double gammaN2(jvxInt16 n)
{
	assert(n > 1);

	if (n % 2 == 0)
	{
		// even case:
		return((jvxData)factorialDbl(n / 2 - 1));
	}

	return((sqrt(M_PI)*factorialDbl(n - 2)) / (pow(2.0, n - 2)*factorialDbl((n - 3) / 2)));
}

double calcNumberRadius(jvxInt16 n, double A, double Ks)
{
	double g1 = gammaN2(n + 1);
	double g2 = gammaN2(n);
	double csh = ((jvxData)n - 1.0) / ((jvxData)n + 1.0)*pow((2 * sqrt(M_PI)*g1 / g2), (2.0 / ((jvxData)n - 1.0)));
	double c = 1 / (1 + log(A));
	double Kr;

	Kr = pow(Ks, (1.0 / ((jvxData)n - 1.0)))*pow((12.0*c*c*csh / ((jvxData)n - 1.0)), (-0.5));
	return(Kr);
}


jvxUInt64
getIndexRadius(jvxData realRadius, applePeelingROM* allocatedROM)
{
	jvxUInt64 startIndexLower = 0;
	jvxUInt64 startIndexUpper = 0;
	jvxUInt64 useIndex;
	jvxData entrySquaredBound = 0.0;

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

void decodeVectorSphere(jvxData* vectorOutput, jvxUInt64 index_NT, applePeelingROM* allocatedROM, jvxBool outCart)
{
	// We need also negative entries!!!
	jvxTreeListType* ptrNangles = (jvxTreeListType*)allocatedROM->linearListTree;

	jvxUInt16 i;
	
	jvxTreeListType *ptr = NULL;
	jvxUInt64 numberNodesHalf_NT;
	jvxUInt64 numberNodesAll_NT;
	jvxUInt64 subNodesNew_NT = 0;
	jvxTreeListType* fieldSubTree = NULL;

	jvxUInt16 N_angles;
	jvxInt16 halfAngles;
	jvxSize iDim;
	
	jvxInt16 foundNew = 0;
	jvxInt16 mustInvertSign = 0;
	jvxData radiusQ = 1.0;

	for(iDim = allocatedROM->dimensionVQ-1; iDim > 1; iDim--)
	{
		// If we are not on the lowest level, the index must be compared to all the index ranges
		// from the subtrees. According to these ranges, decoding has to propagate to the next
		// decoding layer and subtree (branch)
	
		// Determine number of subtrees
		ptr = ptrNangles;
		numberNodesHalf_NT = READVALUEFIELD_M(ptrNangles,allocatedROM->externalReferencesTree);
		ptrNangles++;
		numberNodesAll_NT = READVALUEFIELD_M(ptrNangles,allocatedROM->externalReferencesTree);
		ptrNangles++;
		halfAngles = C32_16(C64_32(READVALUEFIELD_M(ptrNangles,allocatedROM->externalReferencesTree)));
		ptrNangles++;
				
		if(halfAngles == ID_FORWARD_TREE)
		{
			halfAngles = 1;
			N_angles = 1;
			vectorOutput[allocatedROM->dimensionVQ-1-iDim] = 0.0;
			ptrNangles = ptr;
		}
		else
		{
			// If we are not on the last layer, we need to calculate the maximum number of subtrees
			if((numberNodesHalf_NT <<1) == numberNodesAll_NT)
				N_angles = halfAngles*2;
			else
				N_angles = halfAngles*2-1;

			// If field is too small to fit a delta, set at least ONE zentroid!
			if(N_angles == 0)
			{
				printf("APVQ: ERROR: Number angles set to 0, force it to 1!\n");
				N_angles = 1;
			}

			// Now search for the subtree in which the current zentroid falls

			foundNew = 0;
			mustInvertSign = 0;
			subNodesNew_NT = 0;

			// If we have a coding tree this will speed up to find the right
			// path through decoding tree as the number of nodes in subtree are stored
			assert(index_NT < numberNodesAll_NT);
			assert(halfAngles != 0);

			// There is information available in subtree,
			// find the number of nodes in this subtree first (read out from coding tree)
			//indexSwap_NT = indexNew_NT;
			mustInvertSign = 0;

			// If we are on the lower half of the sphere, transform index to upper half
			if(index_NT >= numberNodesHalf_NT)					
			{
				index_NT = numberNodesAll_NT-1-index_NT;
				mustInvertSign = 1;
			}

			// Now find the right subtree
			assert(index_NT < numberNodesAll_NT);
			for(i = 0; i < halfAngles; i++)
			{
				// Red out the sub-nodes for each subtree:
				// currentField plus offset to associated subtree plus offset in field to number subnodes
				ptr = ((ptrNangles)+
					(jvxUInt32)READVALUEFIELD_M(ptrNangles,allocatedROM->externalReferencesTree)+1);
				subNodesNew_NT = READVALUEFIELD_M(ptr, allocatedROM->externalReferencesTree);
				//subNodesNew = *((ptrField)+*ptrField+1);

				if(subNodesNew_NT > index_NT)
				{

					// Set pointer to subtree for next recursion
					fieldSubTree = (ptrNangles)+
						(jvxUInt32)READVALUEFIELD_M(ptrNangles, allocatedROM->externalReferencesTree);

					// Determine the index offset for next sub sphere according to upper or lower half
					if(mustInvertSign)
					{
						// If we are on upper half, transform the index in the current range (because we have measured from pi)
						index_NT = subNodesNew_NT-1-index_NT;

						// Invert the angle
						//angle = (M_PI)-(2.0*(double)i+1.0)/2.0 * M_PI/(double)N_angles;
						//vectorOutput[dimension-dimensionAngles] = radius*cos(angle);
						if (outCart)
						{
							vectorOutput[allocatedROM->dimensionVQ - 1 - iDim/*dimensionAngles*/] = radiusQ *
								(-GET_COS_PI_INDEX_INDEX_MAX(i, halfAngles, N_angles));
						}
						else
						{
							vectorOutput[allocatedROM->dimensionVQ - 1 - iDim/*dimensionAngles*/] = M_PI - GET_ANGLE_PI_INDEX_INDEX_MAX(i, halfAngles, N_angles);
						}
					}
					else
					{
						// If we are on upper half everything is ready 
						//angle = (2.0*(double)i+1.0)/2.0 * M_PI/(double)N_angles;
						//vectorOutput[dimension-dimensionAngles] = radius * cos(angle);
						if (outCart)
						{
							vectorOutput[allocatedROM->dimensionVQ - 1 - iDim/*dimensionAngles*/] = radiusQ *
								GET_COS_PI_INDEX_INDEX_MAX(i, halfAngles, N_angles);
						}
						else
						{
							vectorOutput[allocatedROM->dimensionVQ - 1 - iDim/*dimensionAngles*/] = GET_ANGLE_PI_INDEX_INDEX_MAX(i, halfAngles, N_angles);
						}

					}
					foundNew = 1;
					ptrNangles = fieldSubTree;
					break;
				}
				else
				{
					// Increment the index
					index_NT-= subNodesNew_NT;

					// Set pointer to offset position for next subframe
					ptrNangles++;
				}
			}
			assert(foundNew);

			// Propagate to next dimension layer
			//dimensionAngles--;

			// Warning: Use i from loop!!
			radiusQ = radiusQ*GET_SIN_PI_INDEX_INDEX_MAX(i, halfAngles, N_angles);
			//radius = radius*sin(angle);
		}
	}
	ptrNangles++;
	N_angles = (jvxUInt16)READVALUEFIELD_M(ptrNangles,allocatedROM->externalReferencesTree);
			
	if(N_angles == 0)
	{
		printf("APVQ: ERROR: Number angles set to 0, force it to 1!\n");;
		N_angles = 1;
	}
			
	halfAngles = (N_angles+1)/2;

	assert(index_NT < (jvxUInt64)N_angles);
	//angle = ((2.0*(double)index+1.0)/2.0)*(2.0*M_PI)/(double)N_angles;
			
	if (outCart)
	{
		vectorOutput[allocatedROM->dimensionVQ - 2] = radiusQ * GET_COS_2PI_INDEX_INDEX_MAX(index_NT, halfAngles, N_angles);
		vectorOutput[allocatedROM->dimensionVQ - 1] = radiusQ * GET_SIN_2PI_INDEX_INDEX_MAX(index_NT, halfAngles, N_angles);
	}
	else
	{
		vectorOutput[allocatedROM->dimensionVQ - 2] = GET_ANGLE_2PI_INDEX_INDEX_MAX(index_NT, halfAngles, N_angles);
		vectorOutput[allocatedROM->dimensionVQ - 1] = 1.0;
	}
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
// HELPER FUNCTIONS
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
void quantizeVectorSphereCL(jvxData* inputVector, jvxData radiusQ, jvxData* impulseResp, jvxUInt64* indexVector_NT, 
						  jvxData realRadius, applePeelingROM* allocatedROM, applePeelingRAM* allocatedRAM,
						  applePeelingMISC* allocatedMISC)
{ 
	jvxUInt16 j,k,l;

	// Variables for management of candidates, mainly counters for 
	jvxUInt16 cnt_validContainersStart = 0;
	jvxUInt16 cnt_validContainers = 0;
	jvxUInt16 cnt_availableContainers = allocatedROM->maxNumberCandidates*2;

	// Working containers
	candidateSearchStruct* currentContainer = NULL;
	candidateSearchStruct* currentContainerNew = NULL;

	jvxUInt16 numberCandidatesStored = 0;
	jvxUInt16 numberCandidatesNotStored = 0;

	jvxData inVal = 0.0;
	jvxData metricLoc = 0.0;
	jvxData metricLocal = 0.0;
	jvxData metricMaxVal = 0.0;
	jvxData metricMinOverall = FLT_MAX;
	candidateSearchStruct* lastContainer = NULL;

	jvxInt16 idxMaxVal;

	jvxUInt16 positionReplaced_lower;
	jvxUInt16 positionReplaced_upper;
	jvxUInt16 notSelectedCounter;

	jvxData* ptrTarget;
	jvxData* ptrImpResp;

	jvxData angleIn = 0.0;
	jvxData angleInQDouble;

	jvxTreeListType* ptrTemp;

	jvxData tmpSynth1 = 0.0;
	jvxData tmpSynth2 = 0.0;
	jvxData tmpSynth3 = 0.0;
	jvxData tmpSynth4 = 0.0;

	jvxUInt16 useOtherHalfLower = 0;
	jvxUInt16 useOtherHalfUpper = 0;

	jvxUInt64 subNodesNew_NT = 0;
	jvxUInt64 subNodesNewLower_NT = 0;

	jvxData enerCLOL = 0;

	jvxData singleValueUpper = 0.0;
	jvxData singleValueUpperSecond = 0.0;
	jvxData singleValueLower = 0.0;
	jvxData singleValueLowerSecond = 0.0;

	assert(indexVector_NT);
	*indexVector_NT = -1;

	//=================================================================
	// Prepare the very first candidate element
	//=================================================================
	// Get the index for the first working container
	currentContainer = allocatedRAM->availableContainers[--cnt_availableContainers];
	allocatedRAM->usedContainers[cnt_validContainers++] = currentContainer;

	// Set the partial convolution output to zero
	memset(currentContainer->fieldDest, 0, sizeof(jvxData)*allocatedROM->dimensionVQ);

	// Set the reference to the destination
	currentContainer->fieldSource = currentContainer->fieldDest;

	currentContainer->localRadius = radiusQ;
	currentContainer->metricValue = 0;
	currentContainer->treePtr = allocatedROM->linearListTree;
	currentContainer->indexOffset_NT = 0;
	currentContainer->isOneTree = 0;
	currentContainer->isSecondaryField = 0;

	//=================================================================


	//==================================================================================================
	//==================================================================================================
	// ===== START SEARCHING =====		 ===== START SEARCHING =====    	 ===== START SEARCHING =====
	//==================================================================================================
	//==================================================================================================

	// Find the unquantized convolution output	
	for(k = 0; k < allocatedROM->dimensionVQ; k++)
	{
		ptrTarget = &inputVector[k];
		allocatedRAM->fieldConvOutputUnQ[k] = 0;
		ptrImpResp = impulseResp;
		for(j = 0; j <= k; j++)
		{
			allocatedRAM->fieldConvOutputUnQ[k] += (*ptrTarget--)*(*ptrImpResp++);
		}
	}

	//==================================================================================================
	// =====			LOOP OVER ALL ANGLES EXCEPT THE LAST ONE								   =====
	//==================================================================================================
	for(k = 0; k < (allocatedROM->dimensionVQ-2); k++)
	{
#ifdef DEBUG_APVQ
		printf("K=%d\n",k );
#endif
		//==================================================================================================
		// =====			FIND INPUT ANGLE														   =====
		//==================================================================================================

		//---> 1: FIND INPUT ANGLE OF TARGET SIGNAL <---
		inVal = inputVector[k]*inputVector[k];

		if(inVal < cos(M_PI/4)*cos(M_PI/4)*realRadius)
		{
			// Exactly determine arc cosine
			angleIn = acos(sqrt(inVal/realRadius));
		}
		else
		{
			inVal = realRadius-inVal;

			// Exactly determine arc sinus
			angleIn = asin(sqrt(inVal/realRadius));
		}

		// Range 0 < x^2/r^2 < 1.0 complete
		if(inputVector[k] < 0)
		{
			angleIn = M_PI-angleIn;
		}

		//==================================================================================================
		// =====USE VECTOR SEARCH HISTORY TO ENLARGE VECTOR BY ONE VALUE (make 2 out of each single vector!)		====
		//==================================================================================================

		// Reset the candidate exclusion struct
		numberCandidatesStored = 0;
		numberCandidatesNotStored = 0;

		// Loop over all currently available candidates
		cnt_validContainersStart = cnt_validContainers;

/*
		for(cntN1 = 0; cntN1 < cnt_validContainersStart; cntN1++)
			printf("Container Used[%d]:%d\n", cntN1, allocatedRAM->usedContainers[cntN1]);

		for(cntN1 = 0; cntN1 < cnt_availableContainers; cntN1++)
			printf("Container Available[%d]:%d\n", cntN1, allocatedRAM->availableContainers[cntN1]);
*/
		for(j = 0; j < cnt_validContainersStart; j++)
		{
			//=================================================================
			// Get the containers for lower (current) and upper neighbor (currentNew)
			//=================================================================

			// Get what was prepared previously and enhance for lower bound
			currentContainer = allocatedRAM->usedContainers[j];

			// Reserve a new container for upper gbbound
			currentContainerNew = allocatedRAM->availableContainers[--cnt_availableContainers];

			//printf("PointerOut1: %d; pointerOut2: %d\n", currentContainer,  currentContainerNew);
			// Get some global values
			metricLoc = currentContainer->metricValue;

			// Set the field pointers in the new field.
			//currentContainerNew->fieldSource = currentContainer->fieldSource;
			//currentContainerNew->fieldDest stores this buffer already

			ptrTemp = currentContainer->treePtr;

#ifdef DEBUG_APVQ
			printf("Offset to start: %d\n", ptrTemp-allocatedROM->linearListTree);
#endif

			// Read out the most important information from coding tree.
			currentContainer->numberNodesHalf_NT	= 
				READVALUEFIELD_M(currentContainer->treePtr,allocatedROM->externalReferencesTree);
			currentContainer->treePtr++;
			currentContainer->numberNodesAll_NT = 
				READVALUEFIELD_M(currentContainer->treePtr,allocatedROM->externalReferencesTree);
			currentContainer->treePtr++;
			currentContainer->halfAngles =
				C32_16(C64_32(READVALUEFIELD_M(currentContainer->treePtr,allocatedROM->externalReferencesTree)));

			// Check for compact tree here
			if(currentContainer->halfAngles == ID_FORWARD_TREE)
			{
#ifdef DEBUG_APVQ
				printf("OneTree!\n");
#endif
				// Compact tree means that there is only angle theta = pi/2
				currentContainer->singleValue = 0.0; // cos(pi/2)
				currentContainer->N_angles = 1;
				currentContainer->halfAngles = 1;
				currentContainer->numAngles = 0;

				// Reset the tree pointer to the beginning.
				currentContainer->treePtr = ptrTemp;

				// Determine update for metric. Metric from previous stored in metricLoc
				currentContainer->metricValue = (currentContainer->fieldSource[k]-allocatedRAM->fieldConvOutputUnQ[k]);// consider filter ringing part
				currentContainer->metricValue *= currentContainer->metricValue;
				currentContainer->metricValue += metricLoc;

				// Store status of compact tree for later
				currentContainer->isOneTree = 1;

				currentContainer->isSecondaryField = 0;

				// No copy of current candidate required, reject the candidate
				cnt_availableContainers++;

				//printf("PointerIn1: %d\n", currentContainerNew);
				// set the alternative candidate pointerto zero
				currentContainerNew = NULL;

				// = = = = = = = = = = = = = = = = = = = = = = = = = = 
				// Put the old index into the candidate exclusion field
				// = = = = = = = = = = = = = = = = = = = = = = = = = = 
				if(numberCandidatesStored >= allocatedROM->maxNumberCandidates)
				{
					// Find the value with the maximum metric first
					idxMaxVal = 0;
					metricMaxVal = allocatedRAM->candidatesNotExcluded[0]->metricValue;
					for(l = 1; l < numberCandidatesStored; l++)
					{
						metricLocal = allocatedRAM->candidatesNotExcluded[l]->metricValue;
						if(metricLocal > metricMaxVal)
						{
							idxMaxVal = l;
							metricMaxVal = metricLocal;
						}
					}

					// Compare the new value with the previously stored index with the highest metric
					if(currentContainer->metricValue < metricMaxVal)
					{
						// If the value in the table is higher, remove that value from the table and replace it 
						// by the new one
						allocatedRAM->candidatesExcluded[numberCandidatesNotStored++] = 
							allocatedRAM->candidatesNotExcluded[idxMaxVal];
						allocatedRAM->candidatesNotExcluded[idxMaxVal] = currentContainer;
					}
					else
					{
						// If the value in the table is lower, retain that value in the table and mark the new one
						// as the one to cancelled
						allocatedRAM->candidatesExcluded[numberCandidatesNotStored++] = currentContainer;
					}
				}
				else
				{
					// There is enough space to store this candidate
					allocatedRAM->candidatesNotExcluded[numberCandidatesStored++] = currentContainer;
				}
			}
			else
			{	// else Related to if(1Trees)

				currentContainer->isOneTree = 0;

				// Find indication for possible symmetrie!
				if((currentContainer->numberNodesHalf_NT <<1) == currentContainer->numberNodesAll_NT)
				{
					currentContainer->N_angles = currentContainer->halfAngles*2;
				}
				else
				{
					currentContainer->N_angles = currentContainer->halfAngles*2-1;
				}

				if(currentContainer->N_angles == 0)
				{
					printf("Forcing number angles to 1!\n");
					currentContainer->N_angles = 1;
				}

				// Determine lower and upper neighbor
				angleInQDouble = angleIn*((jvxData)currentContainer->N_angles)*C_INV_PI - 0.5;
				currentContainer->numAngles = C32_16((jvxInt32)floor(angleInQDouble)); // can be also negative						
				currentContainerNew->numAngles = C32_16((jvxInt32)ceil(angleInQDouble));

				// Find values at the boundary of sphere
				if(currentContainer->numAngles < 0)
				{
					currentContainer->numAngles = 0; // now can only be positive
				}

				if(currentContainerNew->numAngles >= currentContainer->N_angles)
				{
					currentContainerNew->numAngles = currentContainer->N_angles-1;// N_angles equal for lower and upper neighbors
				}

				// We have lower and upper neighbor at this point, find quantized neighbors and the associated excitation
				currentContainer->singleValue = GET_COS_PI_INDEX_INDEX_MAX((jvxUInt16)currentContainer->numAngles, currentContainer->halfAngles, 
					currentContainer->N_angles)*currentContainer->localRadius;
				currentContainerNew->singleValue = 
					GET_COS_PI_INDEX_INDEX_MAX((jvxUInt16)currentContainerNew->numAngles, currentContainer->halfAngles, currentContainer->N_angles)
					*currentContainer->localRadius;

				// Consider also the filter ringing (convolution part)
				// impResp[0] = 1.0
				// Find the synth value for the current sample!
				tmpSynth1 = currentContainer->fieldSource[k] + currentContainer->singleValue;
				tmpSynth2 = currentContainer->fieldSource[k] + currentContainerNew->singleValue;

				// Determine update for metric. Metric from previous stored in metricLoc
				currentContainer->metricValue = (tmpSynth1-allocatedRAM->fieldConvOutputUnQ[k]);
				currentContainerNew->metricValue = (tmpSynth2-allocatedRAM->fieldConvOutputUnQ[k]);
				currentContainer->metricValue *= currentContainer->metricValue;
				currentContainerNew->metricValue *= currentContainerNew->metricValue;
				currentContainer->metricValue += metricLoc;
				currentContainerNew->metricValue += metricLoc;

#ifdef DEBUG_APVQ
				printf("Metric Lower:%.20f\n", currentContainer->metricValue);
				printf("Metric Upper:%.20f\n", currentContainerNew->metricValue);
#endif
				//metricLower = workSpaceLower->metricValue;
				//metricUpper = workSpaceUpper->metricValue;
				//currentContainer->isOneTree = 0;

				//if(work.METRIC_SEARCH.metricLower == work.METRIC_SEARCH.metricUpper)
				if(currentContainer->numAngles == currentContainerNew->numAngles)
				{

					//================================================================
					// Now find the maxNumber best values
					if(numberCandidatesStored >= allocatedROM->maxNumberCandidates)
					{
						// Find the value with the maximum metric first
						idxMaxVal = 0;
						metricMaxVal = allocatedRAM->candidatesNotExcluded[0]->metricValue;

						for(l = 1; l < numberCandidatesStored; l++)
						{
							metricLocal = allocatedRAM->candidatesNotExcluded[l]->metricValue;
							if(metricLocal > metricMaxVal)
							{
								idxMaxVal = l;
								metricMaxVal = metricLocal;
							}							
						}

						// Compare the new value with the previously stored index with the highest metric
						if(currentContainer->metricValue < metricMaxVal)
						{
//							printf("Ptr(1):%d\n", allocatedRAM->candidatesNotExcluded[idxMaxVal]);
							// If the value in the table is higher, remove that value from the table and replace it 
							// by the new one
							allocatedRAM->candidatesExcluded[numberCandidatesNotStored++] = allocatedRAM->candidatesNotExcluded[idxMaxVal];
							allocatedRAM->candidatesNotExcluded[idxMaxVal] = currentContainer;
						}
						else
						{
//							printf("Ptr(2):%d\n", currentContainer);
							// If the value in the table is lower, retain that value in the table and mark the new one
							// as the one to cancelled
							allocatedRAM->candidatesExcluded[numberCandidatesNotStored++] = currentContainer;
						}
					}
					else
					{
						// There is enough space to store this candidate
						allocatedRAM->candidatesNotExcluded[numberCandidatesStored++] = currentContainer;
					}

					// Reject the second, new container, since both parts are identical
					cnt_availableContainers++;
					currentContainerNew = NULL;
					//printf("PointerIn1: %d\n", currentContainerNew);
					//fldIndexAvailablePtr++;IDEA FOR FUTURE!
				}
				else
				{	// else related to the comparison of identical candidates

					// Now we have to copy from first to second	candidate
					currentContainerNew->localRadius = currentContainer->localRadius;
					// currentContainerNew->metricValue has already been set..
					currentContainerNew->fieldSource = currentContainer->fieldSource;
					currentContainerNew->treePtr = currentContainer->treePtr;
					currentContainerNew->indexOffset_NT = currentContainer->indexOffset_NT;
					currentContainerNew->isOneTree = currentContainer->isOneTree;
					currentContainer->isSecondaryField = 0;	
					currentContainerNew->isSecondaryField = 1;	
					//currentContainerNew->singleValue has been set
					//currentContainerNew->numAngles has been set
					currentContainerNew->halfAngles = currentContainer->halfAngles;
					currentContainerNew->N_angles = currentContainer->N_angles;

					//currentContainerNew->numberNodesHalf_NT = currentContainer->numberNodesHalf_NT; // Not required
					currentContainerNew->numberNodesAll_NT = currentContainer->numberNodesAll_NT;

					// We prepare the candidate exclusion right here.
					// Since we have not calculated the convolution part yet and want to
					// do this only for the selected candidates, we must make sure that
					// the candidates are stored in the right order.
					// Therefore we must store the upper neighbor candidate in the list
					// BEFORE the lower neighbor. This is not alsways the case automatically and
					// sometimes we must do this explcitely by switching the positions.
					// We need the contents of the lower neighbor fields for the calculation 
					// of the higher contents field
					notSelectedCounter = 0;

					//================================================================
					// Now find the maxNumber best values
					// Second the upper neighbor
					//================================================================

					if(numberCandidatesStored >= allocatedROM->maxNumberCandidates)
					{
						// Find the value with the maximum metric first
						idxMaxVal = 0;
						metricMaxVal = allocatedRAM->candidatesNotExcluded[0]->metricValue;

						for(l = 1; l < numberCandidatesStored; l++)
						{
							metricLocal = allocatedRAM->candidatesNotExcluded[l]->metricValue;

							if(metricLocal > metricMaxVal)
							{
								idxMaxVal = l;
								metricMaxVal = metricLocal;
							}
						}

						// Compare the new value with the previously stored index with the highest metric
						if(currentContainerNew->metricValue < metricMaxVal)
						{
							// If the value in the table is higher, remove that value from the table and replace it 
							// by the new one
							allocatedRAM->candidatesExcluded[numberCandidatesNotStored++] = 
								allocatedRAM->candidatesNotExcluded[idxMaxVal];
							allocatedRAM->candidatesNotExcluded[idxMaxVal] = 
								currentContainerNew;
							positionReplaced_upper = idxMaxVal;
						}
						else
						{
							// If the value in the table is lower, retain that value in the table and mark the new one
							// as the one to cancelled
							allocatedRAM->candidatesExcluded[numberCandidatesNotStored++] = currentContainerNew;
							notSelectedCounter++;
						}
					}
					else
					{
						// There is enough space to store this candidate
						allocatedRAM->candidatesNotExcluded[numberCandidatesStored] = 
							currentContainerNew;
						positionReplaced_upper = numberCandidatesStored++;
					}

					//================================================================
					// Now find the maxNumber best values
					// First the lower neighbor
					//================================================================
					// Now find the maxNumber best values
					if(numberCandidatesStored >= allocatedROM->maxNumberCandidates)
					{
						// Find the value with the maximum metric first
						idxMaxVal = 0;
						metricMaxVal = allocatedRAM->candidatesNotExcluded[0]->metricValue;

						for(l = 1; l < numberCandidatesStored; l++)
						{
							metricLocal =allocatedRAM->candidatesNotExcluded[l]->metricValue;

							if(metricLocal > metricMaxVal)
							{
								idxMaxVal = l;
								metricMaxVal = metricLocal;
							}
						}

						// Compare the new value with the previously stored index with the highest metric
						if(currentContainer->metricValue < metricMaxVal)
						{
							// If the value in the table is higher, remove that value from the table and replace it 
							// by the new one
							allocatedRAM->candidatesExcluded[numberCandidatesNotStored++] = 
								allocatedRAM->candidatesNotExcluded[idxMaxVal];
							allocatedRAM->candidatesNotExcluded[idxMaxVal] = currentContainer;
							positionReplaced_lower = idxMaxVal;
						}
						else
						{
							// If the value in the table is lower, retain that value in the table and mark the new one
							// as the one to cancelled
							allocatedRAM->candidatesExcluded[numberCandidatesNotStored++] = currentContainer;
							notSelectedCounter++;
						}
					}
					else
					{
						// There is enough space to store this candidate
						allocatedRAM->candidatesNotExcluded[numberCandidatesStored] = currentContainer;
						positionReplaced_lower = numberCandidatesStored++;
					}

					currentContainer = NULL;
					currentContainerNew = NULL;

					// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
					// IMPORTATNT: guarantee the right order at this place, first the upper neighbor, then the
					// lower neighbor
					// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
					if(notSelectedCounter == 0)
					{
						if(positionReplaced_lower < positionReplaced_upper)
						{
							//printf("Have to switch!");
							currentContainer = allocatedRAM->candidatesNotExcluded[positionReplaced_upper];
							allocatedRAM->candidatesNotExcluded[positionReplaced_upper] = allocatedRAM->candidatesNotExcluded[positionReplaced_lower];
							allocatedRAM->candidatesNotExcluded[positionReplaced_lower] = currentContainer;
						}
					}

					currentContainer = NULL;
				}
			} // if..else.. no1Trees 
			/*
			printf("CandidatesStored:%d\n", numberCandidatesStored);
			printf("CandidatesNotStored:%d\n", numberCandidatesNotStored);
			for(l = 0; l < numberCandidatesNotStored; l++)
				printf("Pointer %d\n", allocatedRAM->candidatesExcluded[l]);
				*/
		} // End of "for all candidates"


		// std::cout << "=====================Candidate Exclusion======================" << std::endl;
		//===============================================================================
		// Execute the candidate Exclusion
		//===============================================================================
		// We have a list of valid and a list of invalid candidates.
		// Those which are valid must be processed, all others need not
		// Processing means:
		// - convolution
		// - tree update
		// - radius update (sin)

		// Build up a new database of valid containers
		for(cnt_validContainers = 0; cnt_validContainers < numberCandidatesStored; cnt_validContainers++)
		{
			// Get the next not excluded candidate
			currentContainer = allocatedRAM->candidatesNotExcluded[cnt_validContainers];

			// For the sinus function: There is none specified for one-trees in tables, therefore the branch!!
			// TODO: Catch 1Trees?
			currentContainer->localRadius *= 
				GET_SIN_PI_INDEX_INDEX_MAX((jvxUInt16)currentContainer->numAngles, currentContainer->halfAngles, currentContainer->N_angles);

			// Do the convolution
			ptrImpResp = (impulseResp+1);
			for(l = k+1; l < allocatedROM->dimensionVQ; l++)
			{
				currentContainer->fieldDest[l] = currentContainer->singleValue * *ptrImpResp++ + currentContainer->fieldSource[l];
			}

			currentContainer->fieldSource = currentContainer->fieldDest;

			// Do the tree update
			if(!currentContainer->isOneTree)
			{
				// Update the current position in coding tree for selected candidate

				// Handle symmetric properties also
				useOtherHalfLower = 0;

				// Is lower neighbor on lower half
				if(currentContainer->numAngles >= currentContainer->halfAngles)
				{
					useOtherHalfLower = 1;
					currentContainer->numAngles = currentContainer->N_angles-currentContainer->numAngles-1;
				}

				// Number of subtrees from here is equal to halfAngles!
				// Skip forward to offset for first subtree
				currentContainer->treePtr++;

				// find the number of nodes in this subtree first (read out from coding tree)
				// Now find the right subtree
				subNodesNew_NT = 0;

				// Forward to right subtree (forward
				for(l = 0; l < currentContainer->numAngles; l++)
				{
					// Read out the sub-nodes for each subtree:
					// currentField plus offset to associated subtree plus offset in field to number subnodes
					ptrTemp = ((currentContainer->treePtr)+
						(jvxInt32)READVALUEFIELD_M(currentContainer->treePtr,
						allocatedROM->externalReferencesTree)+1);
					subNodesNew_NT += READVALUEFIELD_M(ptrTemp, allocatedROM->externalReferencesTree);
					currentContainer->treePtr++;
				}

				ptrTemp = currentContainer->treePtr;

				// Store field in tree for next angle!
				currentContainer->treePtr += ((jvxUInt32)READVALUEFIELD_M(currentContainer->treePtr,allocatedROM->externalReferencesTree));

				// Arrange lower half value for upper half, we have to go one subtree further!
				if(useOtherHalfLower)
				{
					ptrTemp = ((ptrTemp)+
						READVALUEFIELD_M(ptrTemp,allocatedROM->externalReferencesTree)+1);
					subNodesNewLower_NT = subNodesNew_NT+ 
						READVALUEFIELD_M(ptrTemp, allocatedROM->externalReferencesTree);
					subNodesNewLower_NT = currentContainer->numberNodesAll_NT - 
						subNodesNewLower_NT;
				}
				else
				{
					subNodesNewLower_NT = subNodesNew_NT;
				}

				// Store offset in indexing
				currentContainer->indexOffset_NT += subNodesNewLower_NT;
			}
			//else
			//{
			// Nothing to do for one-trees
			//}

			// First copy the valid values into the valid index buffers, important to preserve the order in the buffer!!
			allocatedRAM->usedContainers[cnt_validContainers] = currentContainer;
			//printf("PointerStillOut1: %d\n", currentContainer);

			
		}

		// The excluded candidates go back to the list of available containers
		for(j = 0; j < numberCandidatesNotStored; j++)
		{
			allocatedRAM->availableContainers[cnt_availableContainers++] = allocatedRAM->candidatesExcluded[j];
			//printf("PointerIn1: %d\n", allocatedRAM->candidatesExcluded[j]);

		}		

		// Update square of real radius for open loop polar values
		realRadius = realRadius-inputVector[k]*inputVector[k];
		enerCLOL += allocatedRAM->fieldConvOutputUnQ[k]*allocatedRAM->fieldConvOutputUnQ[k];
	}

	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
	// Special rule for the last angle: we need to reconstruct two coordinates 
	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

	//==================================================================================================
	// =====			FIND INPUT ANGLE														   =====
	//==================================================================================================

	//---> 1: FIND INPUT ANGLE OF TARGET SIGNAL <---
	inVal = inputVector[allocatedROM->dimensionVQ-2]*inputVector[allocatedROM->dimensionVQ-2];
	if(inVal < cos(M_PI/4)*cos(M_PI/4)*realRadius)
	{
		angleIn = acos(sqrt(inVal/realRadius));
	}
	else
	{
		inVal = realRadius-inVal;
		angleIn = asin(sqrt(inVal/realRadius));
	}

	// Range 0 < x^2/r^2 < 1.0 complete
	if(inputVector[allocatedROM->dimensionVQ-2] < 0)
	{
		angleIn = M_PI-angleIn;
	}

	// For final angle, process the final TWO values
	//tarPtr++;

	// The final value may require a wrap around
	if(inputVector[allocatedROM->dimensionVQ-1] < 0)
	{
		angleIn = 2*M_PI - angleIn;
	}

	//==================================================================================================
	// =====USE VECTOR SEARCH HISTORY TO ENLARGE VECTOR BY ONE VALUE (make 2 out of one)			====
	//==================================================================================================

	// Store number of previously searched vectors
	cnt_validContainersStart = cnt_validContainers;

	for(j = 0; j < cnt_validContainersStart; j++)
	{
		// From the list of paths, find the current one. This one will be modified for next layer

		// Index into list of metric/history etc..
		currentContainer = allocatedRAM->usedContainers[j];

		// Always use the same available secondary container
		currentContainerNew = allocatedRAM->availableContainers[--cnt_availableContainers];

		// Metric from history!
		metricLoc = currentContainer->metricValue;
		ptrImpResp = (impulseResp+1);

		//==================================================================================================
		// =====	FINAL ANGLE: FIND METRIC FOR UPPER AND LOWER NEIGHBOR AND METRIC WITH THIS			====
		//==================================================================================================

		// Read out from coding tree

		// In this case numberNodes is equivalent to numberAngles, the number of subtrees is 0 anyway!
		currentContainer->treePtr++;
		currentContainer->N_angles = 
			CU32_16(CU64_32(READVALUEFIELD_M(currentContainer->treePtr,allocatedROM->externalReferencesTree)));

		if(currentContainer->N_angles == 0)
		{
			printf("Forcing number angles to 1!\n");
			currentContainer->N_angles = 1;
		}

		// Find lower and upper neighbor on the sphere
		angleInQDouble = angleIn*(double)currentContainer->N_angles*C_INV_2_PI - 0.5;

		// Uniform quantization, upper and lower neighbor
		currentContainer->numAngles = (jvxInt16)floor(angleInQDouble); // can be also negative here
		currentContainerNew->numAngles = (jvxInt16)ceil(angleInQDouble);

		// Need to determine halfAngles for sin and cos!
		currentContainer->halfAngles = (currentContainer->N_angles+1)/2;

		// Wrap around for angles
		if(currentContainer->numAngles < 0)
		{
			currentContainer->numAngles = currentContainer->N_angles-1; // Now can not be negative anymore
		}
		if(currentContainerNew->numAngles >= currentContainer->N_angles)
		{
			currentContainerNew->numAngles = 0;
		}

		// Determine signal for lower and upper angle, as we are in last dimensionInAngles we get two values
		// singleValueLower = cos(valueAngleLower)*fldRadiusOnSphere[indexLocal]*fldRadiusAll[indexLocal];
		singleValueLower = 
			GET_COS_2PI_INDEX_INDEX_MAX(currentContainer->numAngles, currentContainer->halfAngles, currentContainer->N_angles)
			* currentContainer->localRadius;
		singleValueLowerSecond = 
			GET_SIN_2PI_INDEX_INDEX_MAX(currentContainer->numAngles, currentContainer->halfAngles, currentContainer->N_angles)
			*currentContainer->localRadius;

		//singleValueUpper = cos(valueAngleUpper)*fldRadiusOnSphere[indexLocal]*fldRadiusAll[indexLocal];
		singleValueUpper = 
			GET_COS_2PI_INDEX_INDEX_MAX(currentContainerNew->numAngles, currentContainer->halfAngles, currentContainer->N_angles)
			*currentContainer->localRadius;
		singleValueUpperSecond = 
			GET_SIN_2PI_INDEX_INDEX_MAX(currentContainerNew->numAngles, currentContainer->halfAngles, currentContainer->N_angles)
			*currentContainer->localRadius;

		//--> Convolution of possible signalvalues for lower and upper neighbor
		// impResp[0] = 1.0!!
		tmpSynth1 = currentContainer->fieldSource[allocatedROM->dimensionVQ-2] + singleValueLower;// TODO

		// impResp[0] = 1.0!!
		tmpSynth2 = currentContainer->fieldSource[allocatedROM->dimensionVQ-2] + singleValueUpper;

		//--> Convolution of possible signalvalues for lower and upper neighbor, very last signal amplitude for value before last (ringing!)
		// Final value.. combine with impResp[1]
		tmpSynth3 = currentContainer->fieldSource[allocatedROM->dimensionVQ-1] + 
			singleValueLower* *ptrImpResp;
		tmpSynth4 = currentContainer->fieldSource[allocatedROM->dimensionVQ-1] + 
			singleValueUpper* *ptrImpResp;

		//--> Convolution of possible signalvalues for lower and upper neighbor, very last signal amplitude for last value
		// impResp[0] = 1.0!!
		tmpSynth3 += singleValueLowerSecond;

		// impResp[0] = 1.0!!
		tmpSynth4 += singleValueUpperSecond;

		//--> Find the metric for the last two values!
		// First the value before the last one
		currentContainer->metricValue = (tmpSynth1-allocatedRAM->fieldConvOutputUnQ[allocatedROM->dimensionVQ-2]);
		currentContainerNew->metricValue = (tmpSynth2-allocatedRAM->fieldConvOutputUnQ[allocatedROM->dimensionVQ-2]);// Set the ptrTargetSimple vector forwards to the last value
		currentContainer->metricValue *= currentContainer->metricValue;
		currentContainerNew->metricValue *= currentContainerNew->metricValue;

		// For the very last value
		tmpSynth1 = (tmpSynth3-allocatedRAM->fieldConvOutputUnQ[allocatedROM->dimensionVQ-1]);
		tmpSynth2 = (tmpSynth4-allocatedRAM->fieldConvOutputUnQ[allocatedROM->dimensionVQ-1]); // Set the ptrTargetSimple vector back to the value for next iterations!

		// Complete to determine the metric
		currentContainer->metricValue += tmpSynth1*tmpSynth1 + metricLoc;
		currentContainerNew->metricValue += tmpSynth2*tmpSynth2 + metricLoc;

		// Compare metric values with overall metric and find the minimum
		if(currentContainer->metricValue < metricMinOverall)
		{
			metricMinOverall = currentContainer->metricValue;
			*indexVector_NT = (jvxUInt64)(currentContainer->indexOffset_NT + currentContainer->numAngles);
		}
		if(currentContainerNew->metricValue < metricMinOverall)
		{
			metricMinOverall = currentContainerNew->metricValue;
			*indexVector_NT = (jvxUInt64)(currentContainer->indexOffset_NT + currentContainerNew->numAngles);
		}

		// Copy field for indices back to list of available fields for next frame! 
		allocatedRAM->availableContainers[cnt_availableContainers++] = currentContainer;
		allocatedRAM->availableContainers[cnt_availableContainers++] = currentContainerNew;
		//*(fldIndexAvailablePtr++) = *(--fldIndexValidPtr);IDEA FOR FUTURE!
	}

	// Consider also the final two coordinates
	enerCLOL += (allocatedRAM->fieldConvOutputUnQ[allocatedROM->dimensionVQ-2])*(allocatedRAM->fieldConvOutputUnQ[allocatedROM->dimensionVQ-2]);
	enerCLOL += (allocatedRAM->fieldConvOutputUnQ[allocatedROM->dimensionVQ-1])*(allocatedRAM->fieldConvOutputUnQ[allocatedROM->dimensionVQ-1]);
	
	// Store performance information for current vector
	allocatedMISC->energyOriginal  += enerCLOL;
	allocatedMISC->energyDifference += metricMinOverall;

#ifdef DEBUG_APVQ
	printf("Metric: %.20f\n", metricMinOverall);
#endif
	/*
	#ifdef PERFORMANCE_MEASURING
	// Update square of real radius for open loop polar values
	realRadius = realRadius-(*tarPtr)*(*tarPtr);
	tarPtr++;
	work.METRIC_SEARCH.enerCLOL += *work.TARGET_SEARCH.ptrTargetSimple * *work.TARGET_SEARCH.ptrTargetSimple;
	work.TARGET_SEARCH.ptrTargetSimple++;
	#endif
	*/
	// Finally be sure to use the mean value if the result of quantization is insufficient
	/*
	#ifdef PERFORMANCE_MEASURING
	work.METRIC_SEARCH.enerCLOL += *work.TARGET_SEARCH.ptrTargetSimple* *work.TARGET_SEARCH.ptrTargetSimple;
	this->energyDifference += work.METRIC_SEARCH.metricMinOverall;
	this->energyOriginal += work.METRIC_SEARCH.enerCLOL;
	#endif
	*/
	//std::cout << indexVector_NT << std::endl;
/*	for(j = 0; j < cnt_availableContainers; j++)
		for(k = 0; k < cnt_availableContainers; k++)
			if(k!= j)
				if(allocatedRAM->availableContainers[j] == allocatedRAM->availableContainers[k])
					printf("ALARM!");
		//printf("Container[%d]:%d\n", j, (jvxUInt32)allocatedRAM->availableContainers[j]);
*/
}


void quantizeVectorSphereOL(jvxData* inputVector, jvxData radiusQ,
	jvxData realRadius, applePeelingROM* allocatedROM, applePeelingRAM* allocatedRAM,
	applePeelingMISC* allocatedMISC,
	jvxUInt64* indexVector_NT, jvxData* metric,
	jvxApvq_interpol* ipol ,
	jvxApvq_ipolweights* ipolwghts)
{
	jvxUInt64 idx_sec = (jvxUInt64)-1;
	jvxUInt64 idxMin = (jvxUInt64)-1;
	
	jvxUInt64 idxThis = (jvxUInt64)-1;
	jvxUInt64 idxThat = (jvxUInt64)-1;
	jvxData metThis = 0;
	jvxData metThat = 0;

	jvxUInt16 j, k, l;

	jvxBool compI = false;
	jvxData metricI_num = EPS_MIN_INTERPOL;
	jvxData metricI_den = EPS_MIN_INTERPOL;

	// Variables for management of candidates, mainly counters for 
	jvxSize cnt_validContainersStart = 0;
	jvxSize cnt_validContainers = 0;
	jvxSize cnt_availableContainers = allocatedROM->maxNumberCandidates * 2;

	// Working containers
	candidateSearchStruct* currentContainer = NULL;
	candidateSearchStruct* currentContainerNew = NULL;

	jvxSize numberCandidatesStored = 0;
	jvxSize numberCandidatesNotStored = 0;

	jvxData inVal = 0.0;
	jvxData metricLoc = 0.0;
	jvxData metricLocal = 0.0;
	jvxData metricMaxVal = 0.0;
	jvxData metricMinOverall = FLT_MAX;
	jvxData metricMinOverallSec = FLT_MAX;
	candidateSearchStruct* lastContainer = NULL;

	jvxInt16 idxMaxVal;

	jvxSize positionReplaced_lower;
	jvxSize positionReplaced_upper;
	jvxSize notSelectedCounter;

	jvxData angleIn = 0.0;
	jvxData angleInQDouble;

	jvxTreeListType* ptrTemp;

	jvxData tmpSynth1 = 0.0;
	jvxData tmpSynth2 = 0.0;
	jvxData tmpSynth3 = 0.0;
	jvxData tmpSynth4 = 0.0;

	jvxUInt16 useOtherHalfLower = 0;
	jvxUInt16 useOtherHalfUpper = 0;

	jvxUInt64 subNodesNew_NT = 0;
	jvxUInt64 subNodesNewLower_NT = 0;

	jvxData enerCLOL = 0;

	jvxData singleValueUpper = 0.0;
	jvxData singleValueUpperSecond = 0.0;
	jvxData singleValueLower = 0.0;
	jvxData singleValueLowerSecond = 0.0;

	//=================================================================
	// Prepare the very first candidate element
	//=================================================================
	// Get the index for the first working container
	currentContainer = allocatedRAM->availableContainers[--cnt_availableContainers];
	allocatedRAM->usedContainers[cnt_validContainers++] = currentContainer;

	if (ipolwghts)
	{
		ipolwghts->nNeighborsInUse = 0;
	}

	currentContainer->localRadius = radiusQ;
	currentContainer->metricValue = 0;
	currentContainer->treePtr = allocatedROM->linearListTree;
	currentContainer->indexOffset_NT = 0;
	currentContainer->isOneTree = 0;
	currentContainer->isSecondaryField = 0;

	//=================================================================


	//==================================================================================================
	//==================================================================================================
	// ===== START SEARCHING =====		 ===== START SEARCHING =====    	 ===== START SEARCHING =====
	//==================================================================================================
	//==================================================================================================

	//==================================================================================================
	// =====			LOOP OVER ALL ANGLES EXCEPT THE LAST ONE								   =====
	//==================================================================================================
	for (k = 0; k < (allocatedROM->dimensionVQ - 2); k++)
	{
#ifdef DEBUG_APVQ
		printf("K=%d\n", k);
#endif
		//==================================================================================================
		// =====			FIND INPUT ANGLE														   =====
		//==================================================================================================

		//---> 1: FIND INPUT ANGLE OF TARGET SIGNAL <---
		inVal = inputVector[k] * inputVector[k];

		if (inVal < cos(M_PI / 4)*cos(M_PI / 4)*realRadius)
		{
			// Exactly determine arc cosine
			angleIn = acos(sqrt(inVal / realRadius));
		}
		else
		{
			inVal = realRadius - inVal;

			// Exactly determine arc sinus
			angleIn = asin(sqrt(inVal / realRadius));
		}

		// Range 0 < x^2/r^2 < 1.0 complete
		if (inputVector[k] < 0)
		{
			angleIn = M_PI - angleIn;
		}

		//==================================================================================================
		// =====USE VECTOR SEARCH HISTORY TO ENLARGE VECTOR BY ONE VALUE (make 2 out of each single vector!)		====
		//==================================================================================================

		// Reset the candidate exclusion struct
		numberCandidatesStored = 0;
		numberCandidatesNotStored = 0;

		// Loop over all currently available candidates
		cnt_validContainersStart = cnt_validContainers;

		/*
				for(cntN1 = 0; cntN1 < cnt_validContainersStart; cntN1++)
					printf("Container Used[%d]:%d\n", cntN1, allocatedRAM->usedContainers[cntN1]);

				for(cntN1 = 0; cntN1 < cnt_availableContainers; cntN1++)
					printf("Container Available[%d]:%d\n", cntN1, allocatedRAM->availableContainers[cntN1]);
		*/
		for (j = 0; j < cnt_validContainersStart; j++)
		{
			//=================================================================
			// Get the containers for lower (current) and upper neighbor (currentNew)
			//=================================================================

			// Get what was prepared previously and enhance for lower bound
			currentContainer = allocatedRAM->usedContainers[j];

			// Reserve a new container for upper gbbound
			currentContainerNew = allocatedRAM->availableContainers[--cnt_availableContainers];

			//printf("PointerOut1: %d; pointerOut2: %d\n", currentContainer,  currentContainerNew);
			// Get some global values
			metricLoc = currentContainer->metricValue;

			// Set the field pointers in the new field.
			//currentContainerNew->fieldSource = currentContainer->fieldSource;
			//currentContainerNew->fieldDest stores this buffer already

			ptrTemp = currentContainer->treePtr;

#ifdef DEBUG_APVQ
			printf("Offset to start: %d\n", ptrTemp - allocatedROM->linearListTree);
#endif

			// Read out the most important information from coding tree.
			currentContainer->numberNodesHalf_NT =
				READVALUEFIELD_M(currentContainer->treePtr, allocatedROM->externalReferencesTree);
			currentContainer->treePtr++;
			currentContainer->numberNodesAll_NT =
				READVALUEFIELD_M(currentContainer->treePtr, allocatedROM->externalReferencesTree);
			currentContainer->treePtr++;
			currentContainer->halfAngles =
				C32_16(C64_32(READVALUEFIELD_M(currentContainer->treePtr, allocatedROM->externalReferencesTree)));

			// Check for compact tree here
			if (currentContainer->halfAngles == ID_FORWARD_TREE)
			{
#ifdef DEBUG_APVQ
				printf("OneTree!\n");
#endif
				// Compact tree means that there is only angle theta = pi/2
				currentContainer->singleValue = 0.0; // cos(pi/2)
				currentContainer->N_angles = 1;
				currentContainer->halfAngles = 1;
				currentContainer->numAngles = 0;

				// Reset the tree pointer to the beginning.
				currentContainer->treePtr = ptrTemp;

				// Determine update for metric. Metric from previous stored in metricLoc
				currentContainer->metricValue = (inputVector[k]);// consider filter ringing part
				currentContainer->metricValue *= currentContainer->metricValue;
				currentContainer->metricValue += metricLoc;

				// Store status of compact tree for later
				currentContainer->isOneTree = 1;

				currentContainer->isSecondaryField = 0;

				// No copy of current candidate required, reject the candidate
				cnt_availableContainers++;

				//printf("PointerIn1: %d\n", currentContainerNew);
				// set the alternative candidate pointerto zero
				currentContainerNew = NULL;

				// = = = = = = = = = = = = = = = = = = = = = = = = = = 
				// Put the old index into the candidate exclusion field
				// = = = = = = = = = = = = = = = = = = = = = = = = = = 
				if (numberCandidatesStored >= allocatedROM->maxNumberCandidates)
				{
					// Find the value with the maximum metric first
					idxMaxVal = 0;
					metricMaxVal = allocatedRAM->candidatesNotExcluded[0]->metricValue;
					for (l = 1; l < numberCandidatesStored; l++)
					{
						metricLocal = allocatedRAM->candidatesNotExcluded[l]->metricValue;
						if (metricLocal > metricMaxVal)
						{
							idxMaxVal = l;
							metricMaxVal = metricLocal;
						}
					}

					// Compare the new value with the previously stored index with the highest metric
					if (currentContainer->metricValue < metricMaxVal)
					{
						// If the value in the table is higher, remove that value from the table and replace it 
						// by the new one
						allocatedRAM->candidatesExcluded[numberCandidatesNotStored++] =
							allocatedRAM->candidatesNotExcluded[idxMaxVal];
						allocatedRAM->candidatesNotExcluded[idxMaxVal] = currentContainer;
					}
					else
					{
						// If the value in the table is lower, retain that value in the table and mark the new one
						// as the one to cancelled
						allocatedRAM->candidatesExcluded[numberCandidatesNotStored++] = currentContainer;
					}
				}
				else
				{
					// There is enough space to store this candidate
					allocatedRAM->candidatesNotExcluded[numberCandidatesStored++] = currentContainer;
				}
			}
			else
			{	// else Related to if(1Trees)

				currentContainer->isOneTree = 0;

				// Find indication for possible symmetrie!
				if ((currentContainer->numberNodesHalf_NT << 1) == currentContainer->numberNodesAll_NT)
				{
					currentContainer->N_angles = currentContainer->halfAngles * 2;
				}
				else
				{
					currentContainer->N_angles = currentContainer->halfAngles * 2 - 1;
				}

				if (currentContainer->N_angles == 0)
				{
					printf("Forcing number angles to 1!\n");
					currentContainer->N_angles = 1;
				}

				// Determine lower and upper neighbor
				angleInQDouble = angleIn * ((jvxData)currentContainer->N_angles)*C_INV_PI - 0.5;
				currentContainer->numAngles = C32_16((jvxInt32)floor(angleInQDouble)); // can be also negative						
				currentContainerNew->numAngles = C32_16((jvxInt32)ceil(angleInQDouble));

				// Find values at the boundary of sphere
				if (currentContainer->numAngles < 0)
				{
					currentContainer->numAngles = 0; // now can only be positive
				}

				if (currentContainerNew->numAngles >= currentContainer->N_angles)
				{
					currentContainerNew->numAngles = currentContainer->N_angles - 1;// N_angles equal for lower and upper neighbors
				}

				// We have lower and upper neighbor at this point, find quantized neighbors and the associated excitation
				currentContainer->singleValue = GET_COS_PI_INDEX_INDEX_MAX((jvxUInt16)currentContainer->numAngles, currentContainer->halfAngles,
					currentContainer->N_angles)*currentContainer->localRadius;
				currentContainerNew->singleValue =
					GET_COS_PI_INDEX_INDEX_MAX((jvxUInt16)currentContainerNew->numAngles, currentContainer->halfAngles, currentContainer->N_angles)
					*currentContainer->localRadius;

				// Determine update for metric. Metric from previous stored in metricLoc
				currentContainer->metricValue = (currentContainer->singleValue - inputVector[k]);
				currentContainerNew->metricValue = (currentContainerNew->singleValue - inputVector[k]);
				currentContainer->metricValue *= currentContainer->metricValue;
				currentContainerNew->metricValue *= currentContainerNew->metricValue;
				currentContainer->metricValue += metricLoc;
				currentContainerNew->metricValue += metricLoc;

#ifdef DEBUG_APVQ
				printf("Metric Lower:%.20f\n", currentContainer->metricValue);
				printf("Metric Upper:%.20f\n", currentContainerNew->metricValue);
#endif
				//metricLower = workSpaceLower->metricValue;
				//metricUpper = workSpaceUpper->metricValue;
				//currentContainer->isOneTree = 0;

				//if(work.METRIC_SEARCH.metricLower == work.METRIC_SEARCH.metricUpper)
				if (currentContainer->numAngles == currentContainerNew->numAngles)
				{

					//================================================================
					// Now find the maxNumber best values
					if (numberCandidatesStored >= allocatedROM->maxNumberCandidates)
					{
						// Find the value with the maximum metric first
						idxMaxVal = 0;
						metricMaxVal = allocatedRAM->candidatesNotExcluded[0]->metricValue;

						for (l = 1; l < numberCandidatesStored; l++)
						{
							metricLocal = allocatedRAM->candidatesNotExcluded[l]->metricValue;
							if (metricLocal > metricMaxVal)
							{
								idxMaxVal = l;
								metricMaxVal = metricLocal;
							}
						}

						// Compare the new value with the previously stored index with the highest metric
						if (currentContainer->metricValue < metricMaxVal)
						{
							//							printf("Ptr(1):%d\n", allocatedRAM->candidatesNotExcluded[idxMaxVal]);
														// If the value in the table is higher, remove that value from the table and replace it 
														// by the new one
							allocatedRAM->candidatesExcluded[numberCandidatesNotStored++] = allocatedRAM->candidatesNotExcluded[idxMaxVal];
							allocatedRAM->candidatesNotExcluded[idxMaxVal] = currentContainer;
						}
						else
						{
							//							printf("Ptr(2):%d\n", currentContainer);
														// If the value in the table is lower, retain that value in the table and mark the new one
														// as the one to cancelled
							allocatedRAM->candidatesExcluded[numberCandidatesNotStored++] = currentContainer;
						}
					}
					else
					{
						// There is enough space to store this candidate
						allocatedRAM->candidatesNotExcluded[numberCandidatesStored++] = currentContainer;
					}

					// Reject the second, new container, since both parts are identical
					cnt_availableContainers++;
					currentContainerNew = NULL;
					//printf("PointerIn1: %d\n", currentContainerNew);
					//fldIndexAvailablePtr++;IDEA FOR FUTURE!
				}
				else
				{	// else related to the comparison of identical candidates

					// Now we have to copy from first to second	candidate
					currentContainerNew->localRadius = currentContainer->localRadius;
					// currentContainerNew->metricValue has already been set..
					currentContainerNew->treePtr = currentContainer->treePtr;
					currentContainerNew->indexOffset_NT = currentContainer->indexOffset_NT;
					currentContainerNew->isOneTree = currentContainer->isOneTree;
					currentContainer->isSecondaryField = 0;
					currentContainerNew->isSecondaryField = 1;
					//currentContainerNew->singleValue has been set
					//currentContainerNew->numAngles has been set
					currentContainerNew->halfAngles = currentContainer->halfAngles;
					currentContainerNew->N_angles = currentContainer->N_angles;

					//currentContainerNew->numberNodesHalf_NT = currentContainer->numberNodesHalf_NT; // Not required
					currentContainerNew->numberNodesAll_NT = currentContainer->numberNodesAll_NT;

					// We prepare the candidate exclusion right here.
					// Since we have not calculated the convolution part yet and want to
					// do this only for the selected candidates, we must make sure that
					// the candidates are stored in the right order.
					// Therefore we must store the upper neighbor candidate in the list
					// BEFORE the lower neighbor. This is not alsways the case automatically and
					// sometimes we must do this explcitely by switching the positions.
					// We need the contents of the lower neighbor fields for the calculation 
					// of the higher contents field
					notSelectedCounter = 0;

					//================================================================
					// Now find the maxNumber best values
					// Second the upper neighbor
					//================================================================

					if (numberCandidatesStored >= allocatedROM->maxNumberCandidates)
					{
						// Find the value with the maximum metric first
						idxMaxVal = 0;
						metricMaxVal = allocatedRAM->candidatesNotExcluded[0]->metricValue;

						for (l = 1; l < numberCandidatesStored; l++)
						{
							metricLocal = allocatedRAM->candidatesNotExcluded[l]->metricValue;

							if (metricLocal > metricMaxVal)
							{
								idxMaxVal = l;
								metricMaxVal = metricLocal;
							}
						}

						// Compare the new value with the previously stored index with the highest metric
						if (currentContainerNew->metricValue < metricMaxVal)
						{
							// If the value in the table is higher, remove that value from the table and replace it 
							// by the new one
							allocatedRAM->candidatesExcluded[numberCandidatesNotStored++] =
								allocatedRAM->candidatesNotExcluded[idxMaxVal];
							allocatedRAM->candidatesNotExcluded[idxMaxVal] =
								currentContainerNew;
							positionReplaced_upper = idxMaxVal;
						}
						else
						{
							// If the value in the table is lower, retain that value in the table and mark the new one
							// as the one to cancelled
							allocatedRAM->candidatesExcluded[numberCandidatesNotStored++] = currentContainerNew;
							notSelectedCounter++;
						}
					}
					else
					{
						// There is enough space to store this candidate
						allocatedRAM->candidatesNotExcluded[numberCandidatesStored] =
							currentContainerNew;
						positionReplaced_upper = numberCandidatesStored++;
					}

					//================================================================
					// Now find the maxNumber best values
					// First the lower neighbor
					//================================================================
					// Now find the maxNumber best values
					if (numberCandidatesStored >= allocatedROM->maxNumberCandidates)
					{
						// Find the value with the maximum metric first
						idxMaxVal = 0;
						metricMaxVal = allocatedRAM->candidatesNotExcluded[0]->metricValue;

						for (l = 1; l < numberCandidatesStored; l++)
						{
							metricLocal = allocatedRAM->candidatesNotExcluded[l]->metricValue;

							if (metricLocal > metricMaxVal)
							{
								idxMaxVal = l;
								metricMaxVal = metricLocal;
							}
						}

						// Compare the new value with the previously stored index with the highest metric
						if (currentContainer->metricValue < metricMaxVal)
						{
							// If the value in the table is higher, remove that value from the table and replace it 
							// by the new one
							allocatedRAM->candidatesExcluded[numberCandidatesNotStored++] =
								allocatedRAM->candidatesNotExcluded[idxMaxVal];
							allocatedRAM->candidatesNotExcluded[idxMaxVal] = currentContainer;
							positionReplaced_lower = idxMaxVal;
						}
						else
						{
							// If the value in the table is lower, retain that value in the table and mark the new one
							// as the one to cancelled
							allocatedRAM->candidatesExcluded[numberCandidatesNotStored++] = currentContainer;
							notSelectedCounter++;
						}
					}
					else
					{
						// There is enough space to store this candidate
						allocatedRAM->candidatesNotExcluded[numberCandidatesStored] = currentContainer;
						positionReplaced_lower = numberCandidatesStored++;
					}

					currentContainer = NULL;
					currentContainerNew = NULL;

					// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
					// IMPORTATNT: guarantee the right order at this place, first the upper neighbor, then the
					// lower neighbor
					// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
					if (notSelectedCounter == 0)
					{
						if (positionReplaced_lower < positionReplaced_upper)
						{
							//printf("Have to switch!");
							currentContainer = allocatedRAM->candidatesNotExcluded[positionReplaced_upper];
							allocatedRAM->candidatesNotExcluded[positionReplaced_upper] = allocatedRAM->candidatesNotExcluded[positionReplaced_lower];
							allocatedRAM->candidatesNotExcluded[positionReplaced_lower] = currentContainer;
						}
					}

					currentContainer = NULL;
				}
			} // if..else.. no1Trees 
			/*
			printf("CandidatesStored:%d\n", numberCandidatesStored);
			printf("CandidatesNotStored:%d\n", numberCandidatesNotStored);
			for(l = 0; l < numberCandidatesNotStored; l++)
				printf("Pointer %d\n", allocatedRAM->candidatesExcluded[l]);
				*/
		} // End of "for all candidates"


		// std::cout << "=====================Candidate Exclusion======================" << std::endl;
		//===============================================================================
		// Execute the candidate Exclusion
		//===============================================================================
		// We have a list of valid and a list of invalid candidates.
		// Those which are valid must be processed, all others need not
		// Processing means:
		// - convolution
		// - tree update
		// - radius update (sin)

		// Build up a new database of valid containers
		for (cnt_validContainers = 0; cnt_validContainers < numberCandidatesStored; cnt_validContainers++)
		{
			// Get the next not excluded candidate
			currentContainer = allocatedRAM->candidatesNotExcluded[cnt_validContainers];

			// For the sinus function: There is none specified for one-trees in tables, therefore the branch!!
			// TODO: Catch 1Trees?
			currentContainer->localRadius *=
				GET_SIN_PI_INDEX_INDEX_MAX((jvxUInt16)currentContainer->numAngles, currentContainer->halfAngles, currentContainer->N_angles);

			// Do the tree update
			if (!currentContainer->isOneTree)
			{
				// Update the current position in coding tree for selected candidate

				// Handle symmetric properties also
				useOtherHalfLower = 0;

				// Is lower neighbor on lower half
				if (currentContainer->numAngles >= currentContainer->halfAngles)
				{
					useOtherHalfLower = 1;
					currentContainer->numAngles = currentContainer->N_angles - currentContainer->numAngles - 1;
				}

				// Number of subtrees from here is equal to halfAngles!
				// Skip forward to offset for first subtree
				currentContainer->treePtr++;

				// find the number of nodes in this subtree first (read out from coding tree)
				// Now find the right subtree
				subNodesNew_NT = 0;

				// Forward to right subtree (forward
				for (l = 0; l < currentContainer->numAngles; l++)
				{
					// Read out the sub-nodes for each subtree:
					// currentField plus offset to associated subtree plus offset in field to number subnodes
					ptrTemp = ((currentContainer->treePtr) +
						(jvxInt32)READVALUEFIELD_M(currentContainer->treePtr,
							allocatedROM->externalReferencesTree) + 1);
					subNodesNew_NT += READVALUEFIELD_M(ptrTemp, allocatedROM->externalReferencesTree);
					currentContainer->treePtr++;
				}

				ptrTemp = currentContainer->treePtr;

				// Store field in tree for next angle!
				currentContainer->treePtr += ((jvxUInt32)READVALUEFIELD_M(currentContainer->treePtr, allocatedROM->externalReferencesTree));

				// Arrange lower half value for upper half, we have to go one subtree further!
				if (useOtherHalfLower)
				{
					ptrTemp = ((ptrTemp)+
						READVALUEFIELD_M(ptrTemp, allocatedROM->externalReferencesTree) + 1);
					subNodesNewLower_NT = subNodesNew_NT +
						READVALUEFIELD_M(ptrTemp, allocatedROM->externalReferencesTree);
					subNodesNewLower_NT = currentContainer->numberNodesAll_NT -
						subNodesNewLower_NT;
				}
				else
				{
					subNodesNewLower_NT = subNodesNew_NT;
				}

				// Store offset in indexing
				currentContainer->indexOffset_NT += subNodesNewLower_NT;
			}
			//else
			//{
			// Nothing to do for one-trees
			//}

			// First copy the valid values into the valid index buffers, important to preserve the order in the buffer!!
			allocatedRAM->usedContainers[cnt_validContainers] = currentContainer;
			//printf("PointerStillOut1: %d\n", currentContainer);


		}

		// The excluded candidates go back to the list of available containers
		for (j = 0; j < numberCandidatesNotStored; j++)
		{
			allocatedRAM->availableContainers[cnt_availableContainers++] = allocatedRAM->candidatesExcluded[j];
			//printf("PointerIn1: %d\n", allocatedRAM->candidatesExcluded[j]);

		}

		// Update square of real radius for open loop polar values
		realRadius = realRadius - inputVector[k] * inputVector[k];
		enerCLOL += inputVector[k] * inputVector[k];
	}

	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
	// Special rule for the last angle: we need to reconstruct two coordinates 
	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

	//==================================================================================================
	// =====			FIND INPUT ANGLE														   =====
	//==================================================================================================

	//---> 1: FIND INPUT ANGLE OF TARGET SIGNAL <---
	inVal = inputVector[allocatedROM->dimensionVQ - 2] * inputVector[allocatedROM->dimensionVQ - 2];
	if (inVal < cos(M_PI / 4)*cos(M_PI / 4)*realRadius)
	{
		angleIn = acos(sqrt(inVal / realRadius));
	}
	else
	{
		inVal = realRadius - inVal;
		angleIn = asin(sqrt(inVal / realRadius));
	}

	// Range 0 < x^2/r^2 < 1.0 complete
	if (inputVector[allocatedROM->dimensionVQ - 2] < 0)
	{
		angleIn = M_PI - angleIn;
	}

	// For final angle, process the final TWO values
	//tarPtr++;

	// The final value may require a wrap around
	if (inputVector[allocatedROM->dimensionVQ - 1] < 0)
	{
		angleIn = 2 * M_PI - angleIn;
	}

	//==================================================================================================
	// =====USE VECTOR SEARCH HISTORY TO ENLARGE VECTOR BY ONE VALUE (make 2 out of one)			====
	//==================================================================================================

	// Store number of previously searched vectors
	cnt_validContainersStart = cnt_validContainers;
	

	for (j = 0; j < cnt_validContainersStart; j++)
	{
		// From the list of paths, find the current one. This one will be modified for next layer

		// Index into list of metric/history etc..
		currentContainer = allocatedRAM->usedContainers[j];

		// Always use the same available secondary container
		currentContainerNew = allocatedRAM->availableContainers[--cnt_availableContainers];

		// Metric from history!
		metricLoc = currentContainer->metricValue;

		//==================================================================================================
		// =====	FINAL ANGLE: FIND METRIC FOR UPPER AND LOWER NEIGHBOR AND METRIC WITH THIS			====
		//==================================================================================================

		// Read out from coding tree

		// In this case numberNodes is equivalent to numberAngles, the number of subtrees is 0 anyway!
		currentContainer->treePtr++;
		currentContainer->N_angles =
			CU32_16(CU64_32(READVALUEFIELD_M(currentContainer->treePtr, allocatedROM->externalReferencesTree)));

		if (currentContainer->N_angles == 0)
		{
			printf("Forcing number angles to 1!\n");
			currentContainer->N_angles = 1;
		}

		// Find lower and upper neighbor on the sphere
		angleInQDouble = angleIn * (double)currentContainer->N_angles*C_INV_2_PI - 0.5;

		// Uniform quantization, upper and lower neighbor
		currentContainer->numAngles = (jvxInt16)floor(angleInQDouble); // can be also negative here
		currentContainerNew->numAngles = (jvxInt16)ceil(angleInQDouble);

		// Need to determine halfAngles for sin and cos!
		currentContainer->halfAngles = (currentContainer->N_angles + 1) / 2;

		// Wrap around for angles
		if (currentContainer->numAngles < 0)
		{
			currentContainer->numAngles = currentContainer->N_angles - 1; // Now can not be negative anymore
		}
		if (currentContainerNew->numAngles >= currentContainer->N_angles)
		{
			currentContainerNew->numAngles = 0;
		}

		// Determine signal for lower and upper angle, as we are in last dimensionInAngles we get two values
		// singleValueLower = cos(valueAngleLower)*fldRadiusOnSphere[indexLocal]*fldRadiusAll[indexLocal];
		singleValueLower =
			GET_COS_2PI_INDEX_INDEX_MAX(currentContainer->numAngles, currentContainer->halfAngles, currentContainer->N_angles)
			* currentContainer->localRadius;
		singleValueLowerSecond =
			GET_SIN_2PI_INDEX_INDEX_MAX(currentContainer->numAngles, currentContainer->halfAngles, currentContainer->N_angles)
			*currentContainer->localRadius;

		//singleValueUpper = cos(valueAngleUpper)*fldRadiusOnSphere[indexLocal]*fldRadiusAll[indexLocal];
		singleValueUpper =
			GET_COS_2PI_INDEX_INDEX_MAX(currentContainerNew->numAngles, currentContainer->halfAngles, currentContainer->N_angles)
			*currentContainer->localRadius;
		singleValueUpperSecond =
			GET_SIN_2PI_INDEX_INDEX_MAX(currentContainerNew->numAngles, currentContainer->halfAngles, currentContainer->N_angles)
			*currentContainer->localRadius;

		//--> Find the metric for the last two values!
		// First the value before the last one
		currentContainer->metricValue = (singleValueLower - inputVector[allocatedROM->dimensionVQ - 2]);
		currentContainerNew->metricValue = (singleValueUpper - inputVector[allocatedROM->dimensionVQ - 2]);
		currentContainer->metricValue *= currentContainer->metricValue;
		currentContainerNew->metricValue *= currentContainerNew->metricValue;

		// For the very last value
		tmpSynth1 = (singleValueLowerSecond - inputVector[allocatedROM->dimensionVQ - 1]);
		tmpSynth2 = (singleValueUpperSecond - inputVector[allocatedROM->dimensionVQ - 1]);

		// Complete to determine the metric
		currentContainer->metricValue += tmpSynth1 * tmpSynth1 + metricLoc;
		currentContainerNew->metricValue += tmpSynth2 * tmpSynth2 + metricLoc;

		idxThis = (jvxUInt64)(currentContainer->indexOffset_NT + currentContainer->numAngles);
		idxThat = (jvxUInt64)(currentContainer->indexOffset_NT + currentContainerNew->numAngles);
		metThis = currentContainer->metricValue;
		metThat = currentContainerNew->metricValue;

		if (ipolwghts)
		{
			assert(ipolwghts->nNeighborsInUse < ipolwghts->nNeighborsMax);
			ipolwghts->neighborsI[ipolwghts->nNeighborsInUse] = idxThis;
			ipolwghts->neighborsW[ipolwghts->nNeighborsInUse] = metThis;
			ipolwghts->nNeighborsInUse++;

			assert(ipolwghts->nNeighborsInUse < ipolwghts->nNeighborsMax);
			ipolwghts->neighborsI[ipolwghts->nNeighborsInUse] = idxThat;
			ipolwghts->neighborsW[ipolwghts->nNeighborsInUse] = metThat;
			ipolwghts->nNeighborsInUse ++;
		}

		// Compare metric values with overall metric and find the minimum
		if (metThis < metricMinOverall)
		{
			metricMinOverall = metThis;
			idxMin = idxThis;
		}

		if (metThat < metricMinOverall)
		{
			metricMinOverall = metThat;
			idxMin = idxThat;
		}

		if (ipol)
		{
			jvxData tmp = 0;
			assert(idxThis < ipol->nWeights);
			tmp = 1.0 / (metThis + EPS_MIN_INTERPOL);
			metricI_num += ipol->weights[idxThis] * tmp;
			metricI_den += tmp;

			assert(idxThat < ipol->nWeights);
			tmp = 1.0 / (metThat + EPS_MIN_INTERPOL);
			metricI_num += ipol->weights[idxThat] * tmp;
			metricI_den += tmp;
		}

		// Copy field for indices back to list of available fields for next frame! 
		allocatedRAM->availableContainers[cnt_availableContainers++] = currentContainer;
		allocatedRAM->availableContainers[cnt_availableContainers++] = currentContainerNew;
		//*(fldIndexAvailablePtr++) = *(--fldIndexValidPtr);IDEA FOR FUTURE!
	}

	enerCLOL += inputVector[allocatedROM->dimensionVQ - 2] * inputVector[allocatedROM->dimensionVQ - 2];
	enerCLOL += inputVector[allocatedROM->dimensionVQ - 1] * inputVector[allocatedROM->dimensionVQ - 1];

	allocatedMISC->energyOriginal += enerCLOL;
	allocatedMISC->energyDifference += metricMinOverall;

	if (indexVector_NT)
	{
		*indexVector_NT = idxMin;
	}
	if (metric)
	{
		*metric = metricMinOverall;
	}

	if ( ipol)
	{
		ipol->ipolVal = metricI_num / metricI_den;
	}

#ifdef DEBUG_APVQ
	printf("Metric: %.20f\n", metricMinOverall);
#endif

}

//#endif

int determineNumberNodes(jvxSize dimensionAngles, jvxData radius, jvxData delta, jvxUInt64* returnVal)
{
	int result = 0;
	int result1 = 0;
	jvxUInt16 N_angles = 0;
	jvxUInt16 N_anglesLoc = 0;
	jvxInt16 halfAngles = 0;
	jvxUInt16 indexTreeSub = 0;
	jvxUInt64 N_Subnodes_NT = 0;
	jvxUInt64 N_SubnodesRet_NT = 0;
	jvxUInt16 i = 0;
	jvxData deltaPhi = 0.0;

	assert(returnVal);

	if(dimensionAngles == 1)
	{
		// For all functionality: Determine the number of angles!
		// For obtaining the number of nodes: Find the number of angles for dimension1 ( 0 < phi < 2*pi )!

#ifdef VERBOSE_OUTPUT
		std::cout << "Radius:" << radius << std::endl;
		std::cout << 2.0*M_PI*radius/delta << std::endl;
#endif
		N_angles = CU32_16((jvxUInt32)floor(2.0*M_PI*radius/delta));
		// If field is too small to fit a delta, set at least ONE zentroid!
		if(N_angles == 0)
		{
			printf("ERROR: Number angles set to 0, force it to 1!\n");
			N_angles = 1;
		}

		// Assign the number of nodes to return argument
		*returnVal = (jvxUInt64)N_angles;
#ifdef VERBOSE_OUTPUT
		std::cout << "Nodes:" << returnVal << std::endl;
#endif
		result = 0;
	}
	else
	{
		// For dimension other than one, do processing on this functional level and
		// then go to next level

		N_anglesLoc = CU32_16((jvxUInt32)floor(M_PI * radius/delta));
	
		N_Subnodes_NT = 0;
		N_SubnodesRet_NT = 0;

		// If field is too small to fit a delta, set at least ONE zentroid!
		if(N_anglesLoc == 0)
		{
			printf("ERROR: Number angles set to 0, force it to 1!\n");
			N_anglesLoc = 1;
		}
		
		halfAngles = C32_16(((jvxUInt32)N_anglesLoc+1)/2);
		indexTreeSub = 0;

		// Return the number of nodes from sublayers, for overall nodes add all subtree nodes
		// Browse through all subtrees
		for(i = 0; i < N_anglesLoc; i++)
		{
			// Be sure to create a symmetric tree!
			indexTreeSub = i;
			if(indexTreeSub >= halfAngles)
				indexTreeSub = N_anglesLoc-i-1;
			deltaPhi = (2.0*(jvxData)indexTreeSub+1.0)/2.0 * M_PI/(jvxData)N_anglesLoc;
#ifdef VERBOSE_OUTPUT
			std::cout << deltaPhi << std::endl;
#endif

			result1 = determineNumberNodes((dimensionAngles-1), radius*sin(deltaPhi), delta, &N_SubnodesRet_NT);
			if(result1 != 0)
			{
				break;
			}
			N_Subnodes_NT += N_SubnodesRet_NT;
		}
		*returnVal = N_Subnodes_NT;
		result = result1;
	}
	return(result);
}





jvxUInt64 determineCodingTreeLength(jvxSize dimensionAngles, jvxData radius, jvxData delta, jvxSize depthTree,
	jvxSize* numberExternalValues, jvxSize numberBitsCodebook,
	jvxSize* maxNumberBitsExternalValues, jvxSize* numberStepsPi, jvxSize* numberSteps2Pi,
	jvxCBool isUnder1Tree)
{

	jvxUInt16 i;
	jvxSize numberEntriesTreeSubsphere = 0;
	jvxSize numberEntriesAllSubtrees = 0;
	jvxSize offsetIDTree = 0;

	jvxUInt32 numberThisTree = 3; 
	jvxInt16 halfAngles = 0;

	jvxUInt64 subNodesNumber_NT = 0;
	jvxUInt64 numberNodesMax_NT = 0;
	
	jvxUInt16 N_anglesLoc;

	jvxData deltaPhi = 0;

	assert(numberStepsPi);
	assert(numberSteps2Pi);
	assert(numberExternalValues);
	assert(maxNumberBitsExternalValues);

	// Determine the number of nodes in this tree
	determineNumberNodes((dimensionAngles), radius, delta, &subNodesNumber_NT);

	testWriteValueField(subNodesNumber_NT,  numberExternalValues, numberBitsCodebook, maxNumberBitsExternalValues);
	
	if(dimensionAngles == 1)
	{
		N_anglesLoc = CU32_16((jvxUInt32)floor(2*M_PI * radius/delta));
		if(N_anglesLoc > *numberSteps2Pi)
			*numberSteps2Pi = N_anglesLoc;

		testWriteValueField(offsetIDTree, numberExternalValues, numberBitsCodebook, maxNumberBitsExternalValues);
		numberThisTree = 3;
	}
	else
	{
		N_anglesLoc = CU32_16((jvxUInt32)floor(M_PI * radius/delta));

		/*
		if(isUnder1Tree)
		{
			std::cout << "Number angles:" << N_anglesLoc << ", in dimension " << dimensionAngles << std::endl;
		}
		*/
		if(N_anglesLoc == 1)
			isUnder1Tree = 1;
		if(N_anglesLoc > *numberStepsPi)
			*numberStepsPi = N_anglesLoc;

		if(N_anglesLoc == 0)
		{
			printf("ERROR: Number angles set to 0, force it to 1!\n");;
			N_anglesLoc = 1;
		}
		
		if(depthTree > 1)
		{
			halfAngles = C32_16(((jvxInt32)N_anglesLoc+1)/2);

			if(N_anglesLoc != 1)
			{
				testWriteValueField(halfAngles, numberExternalValues, numberBitsCodebook, maxNumberBitsExternalValues);
				offsetIDTree = (jvxInt32)halfAngles;
				if(halfAngles)
				{
					numberEntriesAllSubtrees = 0;
					for(i = 0; i < halfAngles; i++)
					{
						deltaPhi = (2.0*(jvxData)i+1.0)/2.0 * M_PI/(jvxData)N_anglesLoc;

						testWriteValueField(offsetIDTree, numberExternalValues, numberBitsCodebook, maxNumberBitsExternalValues);
						//if(testWriteValueField(offsetID_NT, numberExternalValues, numberBitsCodebook, maxNumberBitsExternalValues) == false)
						//	std::cout << "OffsetID_NT:" << offsetID_NT << std::endl;
						determineNumberNodes((dimensionAngles-1), radius*sin(deltaPhi), delta, &subNodesNumber_NT);
						numberNodesMax_NT += subNodesNumber_NT;
						numberEntriesTreeSubsphere = determineCodingTreeLength((dimensionAngles-1), radius*sin(deltaPhi), delta, (depthTree-1),
							numberExternalValues, numberBitsCodebook, maxNumberBitsExternalValues, numberStepsPi, numberSteps2Pi,isUnder1Tree);
						numberEntriesAllSubtrees += numberEntriesTreeSubsphere;
						numberThisTree++;// Add offset for navigation
						offsetIDTree += (numberEntriesTreeSubsphere-1);
					}
				}
				else
				{
					assert(0);
				}
			}
			else
			{
				// Write a -1 at the position of the half angles
				testWriteValueFieldNo1Tree(0, numberExternalValues, numberBitsCodebook, maxNumberBitsExternalValues, 1);
				numberThisTree = 3;
			}
		}
		else
		{
			testWriteValueField(halfAngles, numberExternalValues, numberBitsCodebook, maxNumberBitsExternalValues);
			numberThisTree = 3;
		}
	}
	testWriteValueField(numberNodesMax_NT, numberExternalValues, numberBitsCodebook, maxNumberBitsExternalValues);
	return(numberThisTree+numberEntriesAllSubtrees);
}

void testWriteValueField(jvxUInt64 valueToStore_NT, jvxSize* numberValuesExternalBuffer,
	jvxSize numberBitsCodebook, jvxSize* maxNumberBitsExternal)
{
	jvxUInt16 numBits;

	// The coding tree table is of signed format because negative values for external references.
	jvxUInt64 maxVal_NT = ((jvxUInt64)1<<(numberBitsCodebook-1));// 0xffff +1
	if(valueToStore_NT >= maxVal_NT)
	{
		// Test whether the external reference fits into the data format
		if(*numberValuesExternalBuffer-OFFSET_EXTERNAL > maxVal_NT)
			printf("Number of bits (" JVX_PRINTF_CAST_SIZE ") too small to store all external references, use a higher value for this configuration!", numberBitsCodebook); 
		assert(*numberValuesExternalBuffer-OFFSET_EXTERNAL <= maxVal_NT);
		numBits = CU32_16((jvxUInt32)(ceil(log10((jvxData)valueToStore_NT)/log10(2.0))));
		if(numBits > *maxNumberBitsExternal)
			*maxNumberBitsExternal = numBits;
		(*numberValuesExternalBuffer)++;
	}
}

void testWriteValueFieldNo1Tree(jvxUInt64 valueToStore_NT, jvxSize* numberValuesExternalBuffer,
	jvxSize numberBitsCodebook, jvxSize* maxNumberBitsExternal, jvxCBool forwardTree)
{
	jvxUInt16 numBits;
	if(!forwardTree)
	{
		// The coding tree table is of signed formatz because negaitive values for external references.
		jvxUInt64 maxVal_NT = ((jvxUInt64)1<<(numberBitsCodebook-1));// 0xffff +1
		if(valueToStore_NT >= maxVal_NT)
		{
			// Test whether the external reference fits into the data format
			assert(*numberValuesExternalBuffer-OFFSET_EXTERNAL <= maxVal_NT);
			numBits = CU32_16((jvxUInt32)(ceil(log10((jvxData)valueToStore_NT)/log10(2.0))));
			if(numBits > *maxNumberBitsExternal)
				*maxNumberBitsExternal = numBits;
			(*numberValuesExternalBuffer)++;
		}
	}
}

void
writeValueField(jvxTreeListType* ptrFieldWrite,
	jvxUInt64 valueToStore_NT, jvxUInt64* fldExternalVar_NT, jvxSize* numberValuesExternalBuffer,
	jvxSize numberBitsCodebook, jvxSize* maxNumberBitsExternal)
{
	jvxUInt64 maxVal_NT = ((jvxUInt64)1<<(numberBitsCodebook-1));// 0xffff+1
	jvxUInt16 numBits =0;
	if(valueToStore_NT < maxVal_NT)
	{
		*ptrFieldWrite = (jvxUInt16)valueToStore_NT;
	}
	else
	{
		// Test whether the external reference fits into the data format for tree
		assert(*numberValuesExternalBuffer-OFFSET_EXTERNAL <= maxVal_NT);
		numBits = C32_16((jvxInt32)(ceil(log10((jvxData)valueToStore_NT)/log10(2.0))));
		if(numBits > *maxNumberBitsExternal)
			*maxNumberBitsExternal = numBits;
		*ptrFieldWrite = (-((jvxUInt16)(*numberValuesExternalBuffer)-OFFSET_EXTERNAL));
		fldExternalVar_NT[*numberValuesExternalBuffer] = valueToStore_NT;
		(*numberValuesExternalBuffer)++;
	}
}

void
writeValueFieldNo1Tree(jvxTreeListType* ptrFieldWrite,
	jvxUInt64 valueToStore_NT, jvxUInt64* fldExternalVar_NT, jvxSize* numberValuesExternalBuffer,
	jvxSize numberBitsCodebook, jvxSize* maxNumberBitsExternal, jvxCBool forwardTree)
{
	jvxUInt16 numBits = 0;
	if(forwardTree)
	{
		*ptrFieldWrite = ID_FORWARD_TREE;
	}
	else
	{
		jvxUInt64 maxVal_NT = ((jvxUInt64)1<<(numberBitsCodebook-1));// 0xffff+1
		if(valueToStore_NT < maxVal_NT)
		{
			*ptrFieldWrite = (jvxUInt16)valueToStore_NT;
		}
		else
		{
			// Test whether the external reference fits into the data format for tree
			assert(*numberValuesExternalBuffer-OFFSET_EXTERNAL <= maxVal_NT);
			numBits = C32_16((jvxInt32)(ceil(log10((jvxData)valueToStore_NT)/log10(2.0))));
			if(numBits > *maxNumberBitsExternal)
				*maxNumberBitsExternal = numBits;
			*ptrFieldWrite = (-((jvxUInt16)(*numberValuesExternalBuffer)-OFFSET_EXTERNAL));
			fldExternalVar_NT[*numberValuesExternalBuffer] = valueToStore_NT;
			(*numberValuesExternalBuffer)++;
		}
	}
}

jvxUInt32
determineCodingTree(jvxTreeListType* ptrField, jvxUInt64* ptrFieldExtern_NT, jvxSize dimensionAngles,
		jvxData radius, jvxData delta, jvxSize depthTree, jvxSize* numberExternalValues, jvxSize numberBitsCodebook,
	jvxSize* maxNumberBitsExternalValues, jvxCBool* useStepsCosSinPi, jvxSize numStepsCosSinPi, 
	jvxCBool* useStepsCosSin2Pi, jvxSize numStepsCosSin2Pi)
{
	jvxUInt16 i;
	jvxUInt32 numberSubTrees = 0;
	jvxUInt32 numberSubTreesOneBranch = 0;
	jvxUInt32 numberThisTree = 3; //numberNodes, /*radius (in 16Bit), */numberMaxNodes, numSubTrees
	jvxTreeListType* ptrFieldWrite = ptrField+1;
	jvxUInt64 subNodesNumber_NT = 0;
	jvxUInt64 maxNumAccept_NT = 0;
	jvxUInt64 numberNodesMax_NT = 0;
	jvxInt16 halfAngles = 0;
	jvxUInt16 N_anglesLoc = 0;
	jvxUInt64 offsetIDTree = 0;
	jvxTreeListType* ptrFieldWriteSubtreesBegin = NULL;
	jvxData deltaPhi;

	determineNumberNodes((dimensionAngles), radius, delta, &subNodesNumber_NT);
	maxNumAccept_NT = (((jvxUInt64)1)<<(sizeof(jvxUInt64)*8-1));
	if(subNodesNumber_NT > maxNumAccept_NT)
	{
		printf("Fatal Error: Coding tree requires more than 64 bit which is currently not supported!\n");
		assert(0);
	}
	writeValueField(ptrFieldWrite++, subNodesNumber_NT, ptrFieldExtern_NT, numberExternalValues, numberBitsCodebook, 
		maxNumberBitsExternalValues);
	//*(ptrFieldWrite++) = (unsigned short)subNodesNumber;
	
	if(dimensionAngles == 1)
	{
		N_anglesLoc = CU32_16((jvxUInt32)floor(2*M_PI * radius/delta));
		assert(N_anglesLoc - 1 < numStepsCosSin2Pi);
		useStepsCosSin2Pi[N_anglesLoc-1] = c_true;

		writeValueField(ptrFieldWrite++, halfAngles, ptrFieldExtern_NT, numberExternalValues, numberBitsCodebook, maxNumberBitsExternalValues);
		//*ptrFieldWrite++ = halfAngles;
		numberThisTree = 3;
	}
	else
	{
		N_anglesLoc = CU32_16((jvxUInt32)floor(M_PI * radius/delta));

		if(N_anglesLoc == 0)
		{
			printf("ERROR: Number angles set to 0, force it to 1!\n");
			N_anglesLoc = 1;
		}
		
		if(depthTree > 1)
		{
			halfAngles = C32_16(((jvxUInt32)N_anglesLoc+1)/2);

			if(N_anglesLoc != 1)
			//if(halfAngles != 1)// Bug fixed, 17Okt2006
			{
				assert(N_anglesLoc - 1 < numStepsCosSinPi);
				useStepsCosSinPi[N_anglesLoc-1] = 1;
				writeValueField(ptrFieldWrite++, halfAngles, ptrFieldExtern_NT, numberExternalValues, numberBitsCodebook, maxNumberBitsExternalValues);
				//*ptrFieldWrite++ = halfAngles;
				offsetIDTree = (jvxUInt32)halfAngles;
				ptrFieldWriteSubtreesBegin = ptrFieldWrite+halfAngles;
				if(halfAngles)
				{
					for(i = 0; i < halfAngles; i++)
					{
						jvxData deltaPhi = (2.0*(jvxData)i+1.0)/2.0 * M_PI/(jvxData)N_anglesLoc;
						writeValueField(ptrFieldWrite++, offsetIDTree, ptrFieldExtern_NT, numberExternalValues, numberBitsCodebook, maxNumberBitsExternalValues);
						//*ptrFieldWrite++ = offsetID;
						determineNumberNodes((dimensionAngles-1), radius*sin(deltaPhi), delta, &subNodesNumber_NT);
						numberNodesMax_NT += subNodesNumber_NT;
						numberSubTreesOneBranch = determineCodingTree(ptrFieldWriteSubtreesBegin, ptrFieldExtern_NT, 
							(dimensionAngles-1), radius*sin(deltaPhi), delta, (depthTree-1),numberExternalValues, numberBitsCodebook, 
							maxNumberBitsExternalValues, useStepsCosSinPi, numStepsCosSinPi, useStepsCosSin2Pi, numStepsCosSin2Pi);
						numberSubTrees += numberSubTreesOneBranch;
						numberThisTree++;// Add offset for navigation
						offsetIDTree += (numberSubTreesOneBranch-1);
						ptrFieldWriteSubtreesBegin += numberSubTreesOneBranch;
					}
				}
				else
				{
					assert(0);
				}
			}
			else
			{
				// New: Also create a sin entry for a No1Tree
				assert(numStepsCosSinPi > 0);
				useStepsCosSinPi[0] = 1;
				// Write a -1 at the position of the half angles
				writeValueFieldNo1Tree(ptrFieldWrite++, halfAngles, ptrFieldExtern_NT, numberExternalValues, numberBitsCodebook, 
					maxNumberBitsExternalValues, 1);
				deltaPhi = 1.0/2.0 * M_PI/(jvxData)N_anglesLoc;
				determineNumberNodes((dimensionAngles-1), radius*sin(deltaPhi), delta, &subNodesNumber_NT);
				assert(subNodesNumber_NT - 1 < numStepsCosSin2Pi);
				useStepsCosSin2Pi[subNodesNumber_NT-1] = 1;
				numberNodesMax_NT += subNodesNumber_NT;
				numberThisTree = 3;
			}

		}
		else
		{
			writeValueField(ptrFieldWrite++, halfAngles, ptrFieldExtern_NT, numberExternalValues, numberBitsCodebook, maxNumberBitsExternalValues);
			//*ptrFieldWrite++ = halfAngles;
			numberThisTree = 3;
		}
	}
	writeValueField(ptrField, numberNodesMax_NT, ptrFieldExtern_NT, numberExternalValues, numberBitsCodebook, maxNumberBitsExternalValues);
	//*ptrField = (unsigned short)numberNodesMax;
	return(numberThisTree+numberSubTrees);
}

void allocateFieldCosSin(jvxData*** angleField, jvxData*** cosField, jvxData*** sinField, jvxCBool* useStepsCosSin,
	jvxData maxVal, jvxSize numberSteps, jvxUInt64* bitsROM)
{
	jvxUInt16 numVals = 0;
	jvxUInt16 i,j;

	assert(cosField);
	assert(sinField);
	assert(angleField);

	*cosField = (jvxData**)malloc(sizeof(jvxData*)*numberSteps);
	*bitsROM += sizeof(jvxInt16*)*numberSteps;

	*sinField = (jvxData**)malloc(sizeof(jvxData*)*numberSteps);
	*bitsROM += sizeof(jvxInt16*)*numberSteps;

	*angleField = (jvxData**)malloc(sizeof(jvxData*)*numberSteps);
	*bitsROM += sizeof(jvxInt16*)*numberSteps;

//	numberPtrs += numberSteps;
//	numberPtrs += numberSteps;

	
	for(i = 0; i < numberSteps; i++)
	{
		if(useStepsCosSin[i] == 1)
		{
			numVals = ((i+1)+1)/2;
			
			//bitsROM += BITWIDTH_jvxData*numVals;
			(*cosField)[i] = (jvxData*)malloc(sizeof(jvxData)*numVals);
			*bitsROM += sizeof(jvxData)*numVals;

			//bitsROM += BITWIDTH_jvxData*numVals;
			(*sinField)[i] = (jvxData*)malloc(sizeof(jvxData)*numVals);
			*bitsROM += sizeof(jvxData)*numVals;

			(*angleField)[i] = (jvxData*)malloc(sizeof(jvxData)*numVals);
			*bitsROM += sizeof(jvxData)*numVals;

			//numberValues+= numVals;
			//numberValues+= numVals;
			for(j = 0; j < numVals; j++)
			{
				(*angleField)[i][j] = ((2.0*(jvxData)j + 1.0) / 2.0)*(maxVal) / ((jvxData)(i + 1));
				(*cosField)[i][j] = cos(((2.0*(jvxData)j+1.0)/2.0)*(maxVal)/((jvxData)(i+1)));
				(*sinField)[i][j] = sin(((2.0*(jvxData)j+1.0)/2.0)*(maxVal)/((jvxData)(i+1)));

			}
		}
		else
		{
			(*cosField)[i] = NULL;
			(*sinField)[i] = NULL;
			(*angleField)[i] = NULL;
		}
	}
}

void deallocateFieldCosSin(jvxData** angleField, jvxData** cosField, jvxData** sinField, jvxSize numberSteps)
{
	jvxUInt32 i;

	if((cosField)&&(sinField))
	{
		for(i = 0; i < numberSteps; i++)
		{
			if(cosField[i])
				free(cosField[i]);
			if(sinField[i])
				free(sinField[i]);
			if (angleField[i])
				free(angleField[i]);
		}	
		free(cosField);
		free(sinField);
		free(angleField);
	}
}

jvxData readOutRadiusIndex(jvxUInt64 index, jvxSize numberSteps, jvxData minV, jvxData maxV)
{
	jvxData value_quantized = 0.0;
	jvxData range = maxV-minV+2*EPSILON_U_Q;

	if(index >= numberSteps)
	{
		// Index out of range for radius quantization
		printf("Error Radius!");
		assert(0);
	}

	value_quantized = minV+(((jvxData)2*index+1)/(2.0*(jvxData)numberSteps))*range;
	return(value_quantized);
}
	
jvxData convertLog2LinRadius(jvxData radiusLogQ, jvxData r_0, jvxData a_value, jvxData c)
{
	jvxData radiusQ = 0.0;
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

/*
void quantizeVectorOL(const jvxData* vectorToBeQuantized, jvxUInt16 lengthFieldVector, 
					 jvxUInt64* indexVector, jvxUInt16* indexRadius, jvxData* vectorQ,
					 applePeelingROM* allocatedROM, applePeelingRAM* allocatedRAM,
					 applePeelingMISC* allocatedMISC)
{
	jvxUInt16 cnt; 
	double realRadius = 0.0;//EPSILON_RADIUS;
	jvxUInt16 startIndexLower = 0;
	jvxUInt16 startIndexUpper = 0;
	jvxUInt16 useIndex = 0;

	double radiusMinLin;
	double entrySquaredBound = 0.0;

	assert(indexRadius);

	// Determine the real radius first, we do not need the delta anymore because we have no more division!!
	for(cnt = 0; cnt < lengthFieldVector; cnt++)
	{
		realRadius += vectorToBeQuantized[cnt]*vectorToBeQuantized[cnt];
	}

	allocatedMISC->energyRadius += realRadius;

	startIndexLower = 0;
	startIndexUpper = allocatedROM->numberIndicesRadius;

	// Execute the bisection to find the radius quantization index
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
	*indexRadius = startIndexLower;

	// Find the quantized radius (Decoding)
	radiusMinLin = allocatedROM->radiusReconstruct[*indexRadius];

	allocatedMISC->energyDiffRadius += (radiusMinLin-sqrt(realRadius))*(radiusMinLin-sqrt(realRadius));

	// FROM HERE GO TO SPHERE
*/
/*
	if(!sphereQuant.quantizeVectorOLAllInOne_Optimized(fieldValues, radiusMinLin, indexVector, fieldWeights, (realRadius+EPSILON_RADIUS)))
	{
		std::cout << "SPHERICALVQ::quantizeOL: OL Quantization of point on sphere has failed!" << std::endl;
		return(false);
	}*/

/*
	// If quantization finds that zero vector would be best solution, use that
	if(indexVector == -1)
	{
		memset(vectorQ, 0, sizeof(double)*numberValuesIn);
	}
	else
	{
		sphereQuant.decodeVector(numberValuesIn, vectorQ, indexVector, radiusMinLin);
	}* /
}
*/

jvxUInt64 
apVQ_getNumberFrames(void* handleMisc)
{
	applePeelingMISC* allocatedMISC = (applePeelingMISC*) handleMisc;
	return(allocatedMISC->numberFrames);
}

