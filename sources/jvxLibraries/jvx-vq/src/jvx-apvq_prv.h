#include "jvx.h"

#define READVALUEFIELD_M(tree, fldExternalVar) ((jvxUInt64)((*tree <= OFFSET_EXTERNAL)?fldExternalVar[-(*tree)+OFFSET_EXTERNAL]:*tree))//readValueField(tree,fldExternalVar)
#define GET_COS_PI_INDEX_INDEX_MAX(index, indexHalfAngles, indexMax) (index>=indexHalfAngles? (-allocatedROM->cosFieldPi[indexMax-1][(indexMax-index-1)]):(allocatedROM->cosFieldPi[indexMax-1][index]))
#define GET_SIN_PI_INDEX_INDEX_MAX(index, indexHalfAngles, indexMax) (index>=indexHalfAngles? (allocatedROM->sinFieldPi[indexMax-1][(indexMax-index-1)]):(allocatedROM->sinFieldPi[indexMax-1][index]))
#define GET_ANGLE_PI_INDEX_INDEX_MAX(index, indexHalfAngles, indexMax) (index>=indexHalfAngles? (M_PI - allocatedROM->angleFieldPi[indexMax-1][(indexMax-index-1)]):(allocatedROM->angleFieldPi[indexMax-1][index]))

#define GET_COS_2PI_INDEX_INDEX_MAX(index, indexHalfAngles, indexMax) (index>=indexHalfAngles? (allocatedROM->cosField2Pi[indexMax-1][(indexMax-index-1)]):(allocatedROM->cosField2Pi[indexMax-1][index]))
#define GET_SIN_2PI_INDEX_INDEX_MAX(index, indexHalfAngles, indexMax) (index>=indexHalfAngles? (-allocatedROM->sinField2Pi[indexMax-1][(indexMax-index-1)]):(allocatedROM->sinField2Pi[indexMax-1][index]))
#define GET_ANGLE_2PI_INDEX_INDEX_MAX(index, indexHalfAngles, indexMax) (index>=indexHalfAngles? (2*M_PI - allocatedROM->angleField2Pi[indexMax-1][(indexMax-index-1)]):(allocatedROM->angleField2Pi[indexMax-1][index]))

#define NOT_EXTRA_ZERO

// =============================================================================================

#define MAX_AMPLITUDE 1.0
#define AP_VERSION "25112020"

#define OFFSET_EXTERNAL (-2)
#define ID_FORWARD_TREE (-1)

#define EPSILON_U_Q 0.00000001
#define C_INV_2_PI 1.0/(2.0*M_PI)
#define C_INV_PI 1.0/(M_PI)

#define EPSILON_RADIUS 0.00000000001

#define EPS_MIN_INTERPOL 1e-7

typedef jvxInt16 jvxTreeListType;
#define JVX_NUMBER_MAX_BITS_STORE_TREE (sizeof(jvxTreeListType)*8 - 1)

/// =============================================================================================
// Forward declaration
struct jvxApvq_interpol;
struct jvxApvq_ipolweights;

typedef struct 
{
	jvxCBool quantize_sphere_gain;
	jvxSize elementSize;
	jvxSize dimensionVQ;
	jvxSize overallBitrate;
	jvxUInt64 numberNodesMax_NT;
	jvxUInt64 numberNodesValid_NT;
	jvxSize numBitsSpherePart;
	jvxSize numBitsRadiusPart;
	jvxUInt64 numberIndicesRadius;
	jvxTreeListType* linearListTree;
	jvxUInt64* externalReferencesTree;
	jvxData** cosFieldPi;
	jvxData** sinFieldPi;
	jvxData** angleFieldPi;
	jvxData** cosField2Pi;
	jvxData** sinField2Pi;
	jvxData** angleField2Pi;
	jvxData* radiusFldBoundsSquared;
	jvxData* radiusReconstruct;
	jvxSize maxNumberCandidates;
} applePeelingROM;
// END ================== TYPEDEFS FOR ROM =======================
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// START ================== TYPEDEFS FOR RAM =======================
typedef struct
{
	//jvxUInt16 indexFieldCurrentValue;
	jvxData localRadius;
	jvxData metricValue;
	jvxData* fieldSource;
	jvxData* fieldDest; // allocated
	jvxTreeListType* treePtr;
	jvxUInt64 indexOffset_NT;
	jvxCBool isOneTree;
	jvxCBool isSecondaryField;
	jvxData singleValue;
	jvxInt16 numAngles; // can also be nagative
	jvxInt16 halfAngles; // can also be negative!
	jvxUInt16 N_angles;
	jvxUInt64 numberNodesAll_NT;
	jvxUInt64 numberNodesHalf_NT;
} candidateSearchStruct;

typedef struct
{
	jvxSize elementSize;
	jvxData* fieldConvOutputUnQ;
	candidateSearchStruct* candidates;
	candidateSearchStruct** usedContainers;
	candidateSearchStruct** availableContainers;
	candidateSearchStruct** candidatesExcluded;
	candidateSearchStruct** candidatesNotExcluded;
} applePeelingRAM;
// END ================== TYPEDEFS FOR ROM =======================
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// START ================== MISC TYPEDEFS =======================
typedef struct
{
	jvxSize elementSize;
	jvxData avalue;
	jvxData maxAmplitude;
	jvxData maxRadius;
	jvxSize numberElementsLinearList;
	jvxSize numberElementsExternalReferences;
	jvxSize numberBitsToStoreExternalElements;
	jvxSize numberStepsPi;
	jvxSize numberSteps2Pi;

	jvxData energyOriginal;
	jvxData energyDifference;
	jvxData energyRadius;
	jvxData energyDiffRadius;
	jvxData energyDiffSphere;

	jvxSize numRadiusLog;
	jvxSize numRadiusLin;
	jvxSize overload;
	jvxSize numberFrames;

	jvxData effectiveBitrate;
} applePeelingMISC;

// =============================================================================================

jvxInt32 factorial(jvxInt32 input);
jvxInt16 C32_16(jvxInt32 in);
jvxUInt16 CU32_16(jvxUInt32 in);
jvxInt32 C64_32(jvxInt64 in);
jvxUInt32 CU64_32(jvxUInt64 in);

// =============================================================================================

int determineNumberNodes(jvxSize dimensionAngles, jvxData radius, jvxData delta, jvxUInt64* returnVal);
void testWriteValueFieldNo1Tree(jvxUInt64 valueToStore_NT, jvxSize* numberValuesExternalBuffer,
	jvxSize numberBitsCodebook, jvxSize* maxNumberBitsExternal, jvxCBool forwardTree);
void testWriteValueField(jvxUInt64 valueToStore_NT, jvxSize* numberValuesExternalBuffer,
	jvxSize numberBitsCodebook, jvxSize* maxNumberBitsExternal);
jvxUInt64 determineCodingTreeLength(jvxSize dimensionAngles, jvxData radius, jvxData delta, jvxSize depthTree,
	jvxSize* numberExternalValues, jvxSize numberBitsCodebook,
	jvxSize* maxNumberBitsExternalValues, jvxSize* numberStepsPi, jvxSize* numberSteps2Pi,
	jvxCBool isUnder1Tree);
jvxUInt32 determineCodingTree(jvxTreeListType* ptrField, jvxUInt64* ptrFieldExtern_NT, jvxSize dimensionAngles,
	jvxData radius, jvxData delta, jvxSize depthTree, jvxSize* numberExternalValues, jvxSize numberBitsCodebook,
	jvxSize* maxNumberBitsExternalValues, jvxCBool* useStepsCosSinPi, jvxSize numStepsCosSinPi,
	jvxCBool* useStepsCosSin2Pi, jvxSize numStepsCosSin2Pi);
void writeValueField(jvxTreeListType* ptrFieldWrite,
	jvxUInt64 valueToStore_NT, jvxUInt64* fldExternalVar_NT, jvxSize* numberValuesExternalBuffer,
	jvxSize numberBitsCodebook, jvxSize* maxNumberBitsExternal);
void writeValueFieldNo1Tree(jvxTreeListType* ptrFieldWrite,
	jvxUInt64 valueToStore_NT, jvxUInt64* fldExternalVar_NT, jvxUInt32* numberValuesExternalBuffer,
	jvxUInt16 numberBitsCodebook, jvxUInt16* maxNumberBitsExternal, jvxCBool forwardTree);
void allocateFieldCosSin(jvxData*** angleField, jvxData*** cosField, jvxData*** sinField, jvxCBool* useStepsCosSin,
	jvxData maxVal, jvxSize numberSteps, jvxUInt64* bitsROM);
void deallocateFieldCosSin(jvxData** angleField, jvxData** cosField, jvxData** sinField, jvxSize numberSteps);
//static void staticAllocateRAMSearch(jvxUInt16 numSearchMax, jvxUInt16 dimVQ, jvxUInt64* bitsRAM, applePeelingRAM* allocatedRAM);
jvxData readOutRadiusIndex(jvxUInt64 index, jvxSize numberSteps, jvxData minV, jvxData maxV);
jvxData convertLog2LinRadius(jvxData radiusLogQ, jvxData r_0, jvxData a_value, jvxData c);
jvxUInt64 getIndexRadius(jvxData realRadius, applePeelingROM* allocatedROM);
void quantizeVectorSphereCL(jvxData* inputVector, jvxData radiusQ, jvxData* impulseResp, jvxUInt64* indexVector_NT,
	jvxData realRadius, applePeelingROM* allocatedROM, applePeelingRAM* allocatedRAM,
	applePeelingMISC* allocatedMISC);
void quantizeVectorSphereOL(jvxData* inputVector, jvxData radiusQ,
	jvxData realRadius, applePeelingROM* allocatedROM, applePeelingRAM* allocatedRAM,
	applePeelingMISC* allocatedMISC,
	jvxUInt64* indexVector_NT, jvxData* metric = NULL,
	jvxApvq_interpol* ipol = NULL,
	jvxApvq_ipolweights* ipolwghts = NULL );

//#endif
void decodeVectorSphere(jvxData* vectorOutput, jvxUInt64 index_NT, applePeelingROM* allocatedROM, jvxBool outCart);