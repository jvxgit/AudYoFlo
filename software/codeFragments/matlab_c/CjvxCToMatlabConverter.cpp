// Need to include jvx.h here since the order of inclusion matters!
#include "jvx.h"

#if _MATLAB_MEXVERSION < 500
#if (_MSC_VER >= 1600)
#include <yvals.h>
#define __STDC_UTF_16__
#endif
#endif
#include "mex.h"

#include <iostream>

#if _MATLAB_MEXVERSION >= 100
#define SZ_MAT_TYPE mwSize
#else
#define SZ_MAT_TYPE int
#endif

#include "codeFragments/matlab_c/CjvxCToMatlabConverter.h"
#include "codeFragments/matlab_c/HjvxMex2CConverter.h"

//======================================================================
//======================================================================
// MEX-RETURNS MEX-RETURNS MEX-RETURNS MEX-RETURNS MEX-RETURNS MEX-RETURNS
//======================================================================
//======================================================================

//==================================================================================================
// MEX MEX MEX MEX MEX MEX MEX MEX MEX MEX MEX MEX MEX MEX MEX MEX MEX MEX MEX MEX MEX MEX MEX MEX MEX
//==================================================================================================

/**
 * Function to return a list of long values in MEX format.
 *///===============================================================================
void 
CjvxCToMatlabConverter::mexReturnFieldInt32(mxArray*& plhs, std::vector<jvxInt32>& lstStr)
{
	unsigned i;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};

	if(lstStr.size() == 0)
	{
		plhs = NULL;
	}
	else
	{
		dims[0] = 1;
		dims[1] = (int)lstStr.size();
		plhs = mxCreateNumericArray(ndim, dims, mxINT32_CLASS, mxREAL);
		jvxInt32* ptrWrite = (jvxInt32*)mxGetData(plhs);
		for(i = 0; i < lstStr.size(); i++)
		{
			ptrWrite[i] = lstStr[i];
		}
	}
}

/** 
 * Return a long (int32) in MEX format.
 *///=======================================================================
void 
CjvxCToMatlabConverter::mexReturnInt32(mxArray*& plhs, const jvxInt32& value)
{
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};

	plhs = mxCreateNumericArray(ndim, dims, mxINT32_CLASS, mxREAL);
	jvxInt32* ptrWrite = (jvxInt32*)mxGetData(plhs);
	*ptrWrite = value;
}

void
CjvxCToMatlabConverter::mexReturnUInt32(mxArray*& plhs, const jvxUInt32& value)
{
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };

	plhs = mxCreateNumericArray(ndim, dims, mxUINT32_CLASS, mxREAL);
	jvxUInt32* ptrWrite = (jvxUInt32*)mxGetData(plhs);
	*ptrWrite = value;
}
/**
 * Return a long (int8) in MEX format.
 *///=======================================================================
void
CjvxCToMatlabConverter::mexReturnInt8(mxArray*& plhs, const jvxInt8& value)
{
    SZ_MAT_TYPE ndim = 2;
    SZ_MAT_TYPE dims[2] = {1, 1};
    
    plhs = mxCreateNumericArray(ndim, dims, mxINT8_CLASS, mxREAL);
    jvxInt8* ptrWrite = (jvxInt8*)mxGetData(plhs);
    *ptrWrite = value;
}

/**
 * Return a long (int32) in MEX format.
 *///=======================================================================
void
CjvxCToMatlabConverter::mexReturnInt8List(mxArray*& plhs, const jvxInt8* valueList, size_t numberVals)
{
    if((valueList == NULL)||(numberVals == 0))
    {
        plhs = NULL;
        
    }
    else
    {
        SZ_MAT_TYPE ndim = 2;
        SZ_MAT_TYPE dims[2] = {1, 1};
        dims[1] = (int)numberVals;
        
        plhs = mxCreateNumericArray(ndim, dims, mxINT8_CLASS, mxREAL);
        jvxInt8* ptrWrite = (jvxInt8*)mxGetData(plhs);
        memcpy(ptrWrite, valueList, sizeof(jvxInt8)*numberVals);
    }
}

/**
 * Return a long (int32) in MEX format.
 *///=======================================================================
void 
CjvxCToMatlabConverter::mexReturnInt32List(mxArray*& plhs, const jvxInt32* valueList, size_t numberVals)
{
	if((valueList == NULL)||(numberVals == 0))
	{
		plhs = NULL;
		
	}
	else
	{
		SZ_MAT_TYPE ndim = 2;
		SZ_MAT_TYPE dims[2] = {1, 1};
		dims[1] = (int)numberVals;

		plhs = mxCreateNumericArray(ndim, dims, mxINT32_CLASS, mxREAL);
		jvxInt32* ptrWrite = (jvxInt32*)mxGetData(plhs);
		memcpy(ptrWrite, valueList, sizeof(jvxInt32)*numberVals);
	}
}

/** 
 * Return a jvxData in MEX format.
 *///=======================================================================
void 
CjvxCToMatlabConverter::mexReturnData(mxArray*& plhs, const jvxData& value)
{
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};

	plhs = mxCreateNumericArray(ndim, dims, JVX_DATA_MEX_CONVERT, mxREAL);
	jvxData* ptrWrite = (jvxData*)mxGetData(plhs);
	*ptrWrite = value;
}

/** 
 * Return a long (int32) in MEX format.
 *///=======================================================================
void 
CjvxCToMatlabConverter::mexReturnDataList(mxArray*& plhs, const jvxData* valueList, size_t numberVals)
{
	if((valueList == NULL)||(numberVals == 0))
	{
		plhs = NULL;

	}
	else
	{
		SZ_MAT_TYPE ndim = 2;
		SZ_MAT_TYPE dims[2] = {1, 1};
		dims[1] = (int)numberVals;

		plhs = mxCreateNumericArray(ndim, dims, JVX_DATA_MEX_CONVERT, mxREAL);
		jvxData* ptrWrite = (jvxData*)mxGetData(plhs);
		memcpy(ptrWrite, valueList, sizeof(jvxData)*numberVals);
	}
}

/**
 * Return a short data in MEX format.
 *///===========================================================================
void 
CjvxCToMatlabConverter::mexReturnInt16(mxArray*& plhs, const jvxInt16& value)
{
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};

	plhs = mxCreateNumericArray(ndim, dims, mxINT16_CLASS, mxREAL);
	jvxInt16* ptrWrite = (jvxInt16*)mxGetData(plhs);
	*ptrWrite = value;
}

void
CjvxCToMatlabConverter::mexReturnUInt16(mxArray*& plhs, const jvxUInt16& value)
{
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };

	plhs = mxCreateNumericArray(ndim, dims, mxUINT16_CLASS, mxREAL);
	jvxUInt16* ptrWrite = (jvxUInt16*)mxGetData(plhs);
	*ptrWrite = value;
}
/**
 * Return a long (int16) in MEX format.
 *///=======================================================================
void
CjvxCToMatlabConverter::mexReturnInt16List(mxArray*& plhs, const jvxInt16* valueList, size_t numberVals)
{
    if((valueList == NULL)||(numberVals == 0))
    {
        plhs = NULL;
        
    }
    else
    {
        SZ_MAT_TYPE ndim = 2;
        SZ_MAT_TYPE dims[2] = {1, 1};
        dims[1] = (int)numberVals;
        
        plhs = mxCreateNumericArray(ndim, dims, mxINT16_CLASS, mxREAL);
        jvxInt16* ptrWrite = (jvxInt16*)mxGetData(plhs);
        memcpy(ptrWrite, valueList, sizeof(jvxInt16)*numberVals);
    }
}

/**
 * Function to return a single name in MEX format.
 *///==============================================================================
void  
CjvxCToMatlabConverter::mexReturnString(mxArray*& plhs, const std::string& str)
{
	plhs = mxCreateString(str.c_str());
}

/**
 * Function to return a list of strings in MEX format
 *///=======================================================================
void 
CjvxCToMatlabConverter::mexReturnStringCell(mxArray*& plhs, const std::vector<std::string>& lstStr)
{
	unsigned i;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};
	mxArray* arr = NULL;
	plhs = NULL;

	if(lstStr.size() == 0)
	{
		dims[0] = 0;
		dims[1] = (int)0;
		plhs = mxCreateCellArray(ndim, dims);
	}
	else
	{
		dims[0] = (int)lstStr.size();
		dims[1] = 1;
		plhs = mxCreateCellArray(ndim, dims);
		for(i = 0; i < lstStr.size(); i++)
		{
			arr = mxCreateString(lstStr[i].c_str());
			mxSetCell(plhs, i, arr);
			arr = NULL;
		}
	}
}

/**
 * Return bool in MEX format
 *///========================================================================
void 
CjvxCToMatlabConverter::mexReturnBool(mxArray*& plhs, bool trueFalse)
{
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};
	plhs = mxCreateLogicalArray(ndim, dims);
	mxLogical* ptrLog = (mxLogical*)mxGetData(plhs);
	*ptrLog = trueFalse;
}

void  
CjvxCToMatlabConverter::mexReturnErrorCode(mxArray*& plhs, jvxErrorType tp)
{
	mexReturnInt32(plhs, (int)tp);
}

/** 
 * Function to return the indication for an error if arised in MATLAB MEX format.
 *///==============================================================================
void 
CjvxCToMatlabConverter::mexReturnAnswerNegativeResult(mxArray*&plhs, const std::string& expr, jvxErrorType errCode)
{
	mxArray* arr = NULL;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};

	plhs = NULL;

	const char** fld;
	std::string fldName1 = FLD_NAME_ERROR_DESCRIPTION_STRING;
	std::string fldName2 = FLD_NAME_ERROR_ID_INT32;

	fld = new const char*[2];
	fld[0] = fldName1.c_str();
	fld[1] = fldName2.c_str();

	plhs = mxCreateStructArray(ndim, dims, 2, fld);
	delete[](fld);

	mexReturnString(arr, expr);
	mxSetFieldByNumber(plhs, 0, 0, arr);
	arr = NULL;

	mexReturnInt32(arr, (int)errCode);
	mxSetFieldByNumber(plhs, 0, 1, arr);
	arr = NULL;
}

/** 
 * Return a long (int64) in MEX format.
 *///=======================================================================
void 
CjvxCToMatlabConverter::mexReturnInt64(mxArray*& plhs, const jvxInt64& value)
{
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};

	plhs = mxCreateNumericArray(ndim, dims, mxINT64_CLASS, mxREAL);
	long long* ptrWrite = (long long*)mxGetData(plhs);
	*ptrWrite = value;

}

void
CjvxCToMatlabConverter::mexReturnUInt64(mxArray*& plhs, const jvxUInt64& value)
{
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };

	plhs = mxCreateNumericArray(ndim, dims, mxUINT64_CLASS, mxREAL);
	jvxUInt64* ptrWrite = (jvxUInt64*)mxGetData(plhs);
	*ptrWrite = value;

}
/**
 * Return a long (int64) in MEX format.
 *///=======================================================================
void
CjvxCToMatlabConverter::mexReturnInt64List(mxArray*& plhs, const jvxInt64* valueList, size_t numberVals)
{
    if((valueList == NULL)||(numberVals == 0))
    {
        plhs = NULL;
        
    }
    else
    {
        SZ_MAT_TYPE ndim = 2;
        SZ_MAT_TYPE dims[2] = {1, 1};
        dims[1] = (int)numberVals;
        
        plhs = mxCreateNumericArray(ndim, dims, mxINT64_CLASS, mxREAL);
        jvxInt64* ptrWrite = (jvxInt64*)mxGetData(plhs);
        memcpy(ptrWrite, valueList, sizeof(jvxInt64)*numberVals);
    }
}

/*
void 
CjvxCToMatlabConverter::mexReturnUInt64(mxArray*& plhs, const jvxUInt64& value)
{
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};

	plhs = mxCreateNumericArray(ndim, dims, mxUINT64_CLASS, mxREAL);
	long long* ptrWrite = (long long*)mxGetData(plhs);
	*ptrWrite = value;

}
*/

/** 
 * Return a bitfield in MEX format (translates to UInt64.
 *///=======================================================================
void 
CjvxCToMatlabConverter::mexReturnBitField(mxArray*& plhs, const jvxBitField& value)
{
	jvxBitField cpFld = value;
	mexReturnString(plhs, jvx_bitField2String(cpFld));
	/*
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};

	plhs = mxCreateNumericArray(ndim, dims, mxUINT64_CLASS, mxREAL);
	jvxUInt64* ptrWrite = (jvxUInt64*)mxGetData(plhs);
	*ptrWrite = (jvxUInt64)value;
	*/
}

void 
CjvxCToMatlabConverter::mexReturnCBitField(mxArray*& plhs, const jvxCBitField& value)
{
	jvxBitField cpFld;
	cpFld.setValueC(value);
	mexReturnString(plhs, jvx_bitField2String(cpFld));
	/*
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};

	plhs = mxCreateNumericArray(ndim, dims, mxUINT64_CLASS, mxREAL);
	jvxUInt64* ptrWrite = (jvxUInt64*)mxGetData(plhs);
	*ptrWrite = (jvxUInt64)value;
	*/
}

#ifdef JVX_DSP_DATA_FORMAT_DOUBLE
/** 
 * Return a jvxData (jvxData) in MEX format.
 *///=======================================================================
void 
CjvxCToMatlabConverter::mexReturnFloatList(mxArray*& plhs, const float* ptrField, jvxSize numElements)
{
	int i;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};

	dims[1] = (SZ_MAT_TYPE)numElements;

	plhs = mxCreateNumericArray(ndim, dims, JVX_DATA_MEX_CONVERT, mxREAL);
	jvxData* ptrWrite = (jvxData*)mxGetData(plhs);
	for(i = 0; i < numElements; i++)
	{
		*ptrWrite = (jvxData) (*ptrField);
		ptrField ++;
	}
}
#else
/** 
 * Return a jvxData (jvxData) in MEX format.
 *///=======================================================================
void 
CjvxCToMatlabConverter::mexReturnDoubleList(mxArray*& plhs, const double* ptrField, jvxSize numElements)
{
	int i;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};

	dims[1] = numElements;

	plhs = mxCreateNumericArray(ndim, dims, JVX_DATA_MEX_CONVERT, mxREAL);
	jvxData* ptrWrite = (jvxData*)mxGetData(plhs);
	for(i = 0; i < numElements; i++)
	{
		*ptrWrite = (jvxData) (*ptrField);
		ptrField ++;
	}
}
#endif

/** 
 * Return all elements as empty variables
 *///=======================================================================
void 
CjvxCToMatlabConverter::mexFillEmpty(mxArray** plhs, jvxSize nlhs, jvxSize offset)
{
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};

	dims[0] = 0;
	dims[1] = 0;

	for(jvxSize l = offset; l < nlhs; l++)
	{
		if (plhs[l] == NULL)
		{
			plhs[l] = mxCreateNumericArray(ndim, dims, JVX_DATA_MEX_CONVERT, mxREAL);
		}
		else
		{
			mexPrintf("WARNING<%s>: Trying to overwrite previously populated array element.", __FUNCTION__);
		}
	}
};

void 
CjvxCToMatlabConverter::mexReturnSelectionListStruct(mxArray*& plhs, const jvxSelectionList& theSelList)
{
	jvxSize i;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};
	mxArray* arr = NULL;
	std::vector<std::string> lstTxt;

	std::string fldName0 = FLD_NAME_PROPERTY_SELLIST_OPTIONS;
	std::string fldName1 = FLD_NAME_PROPERTY_SELLIST_SELECTION_BITFIELD;
	std::string fldName2 = FLD_NAME_PROPERTY_SELLIST_EXCULSIVE_BITFIELD;

	const char* fld[3];
	fld[0] = fldName0.c_str();
	fld[1] = fldName1.c_str();
	fld[2] = fldName2.c_str();

	plhs = mxCreateStructArray(ndim, dims, 3, fld);

	arr = NULL;
	for(i = 0; i < theSelList.strList.ll(); i++)
	{
		lstTxt.push_back(theSelList.strList.std_str_at(i));
	}
	mexReturnStringCell(arr, lstTxt);
	mxSetFieldByNumber(plhs, 0, 0, arr);

	arr = NULL;
	mexReturnBitField(arr, theSelList.bitFieldSelected());
	mxSetFieldByNumber(plhs, 0, 1, arr);

	arr = NULL;
	mexReturnBitField(arr, theSelList.bitFieldExclusive);
	mxSetFieldByNumber(plhs, 0, 2, arr);
}

void 
CjvxCToMatlabConverter::mexReturnValueInRangeStruct(mxArray*& plhs, const jvxValueInRange& valRange)
{
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};
	mxArray* arr = NULL;
	std::vector<std::string> lstTxt;

	std::string fldName0 = FLD_NAME_PROPERTY_VALUE_IN_RANGE_MIN_DBL;
	std::string fldName1 = FLD_NAME_PROPERTY_VALUE_IN_RANGE_MAX_DBL;
	std::string fldName2 = FLD_NAME_PROPERTY_VALUE_IN_RANGE_VALUE_DBL;

	const char* fld[3];
	fld[0] = fldName0.c_str();
	fld[1] = fldName1.c_str();
	fld[2] = fldName2.c_str();

	plhs = mxCreateStructArray(ndim, dims, 3, fld);

	arr = NULL;
	mexReturnData(arr, valRange.minVal);
	mxSetFieldByNumber(plhs, 0, 0, arr);

	arr = NULL;
	mexReturnData(arr, valRange.maxVal);
	mxSetFieldByNumber(plhs, 0, 1, arr);

	mexReturnData(arr, valRange.val());
	mxSetFieldByNumber(plhs, 0, 2, arr);
}

bool 
CjvxCToMatlabConverter::mexReturnStructSection(mxArray*& plhs, jvxConfigData* theSection, IjvxConfigProcessor* proc)
{
	signed i,j,k;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};

	plhs = NULL;
	mxArray* arr = NULL;
	jvxSize numSections = 0;

	std::string txt;
	jvxData dataDbl;
	jvxValue val;

	jvxSize numStrings = 0, numLists = 0;
	std::vector<std::string> vecStrings;

	jvxApiValueList lstVals;
	jvxConfigData* theSubsection = NULL;

	std::vector<jvxData*> lstDblsCopy;
	std::vector<jvxSize> lengths;

	jvxBitField bfield;
	jvxSize numEntriesList;
	jvxSize sz;
	jvxBool returnCell;

	proc->getNumberEntriesCurrentSection(theSection, &numSections);
	if(numSections)
	{
		const char** fld = new const char*[numSections];
		std::string* fldStr = new std::string[numSections];

		for(i = 0; i < numSections; i++)
		{ 
			fldStr[i] = "entry" + jvx_int2String(i);
			fld[i] = fldStr[i].c_str();
		}

		plhs = mxCreateStructArray(ndim, dims, JVX_SIZE_INT(numSections), fld);

		delete[](fld);
		delete[](fldStr);

		fld = NULL;
		fldStr = NULL;

		// Now fill the struct
		for(i = 0; i < numSections; i++)
		{
			jvxApiString  fldStrRTP;
			const char* fld[3];
			std::string descr = "DESCRIPTOR";
			std::string type = "TYPE";
			std::string content = "CONTENT";
			fld[0] = descr.c_str();
			fld[1] = type.c_str();
			fld[2] = content.c_str();

			mxArray* arrL = mxCreateStructArray(ndim, dims, JVX_SIZE_INT(3), fld);

			// ===============================================

			mxArray* arr = NULL;
			proc->getReferenceEntryCurrentSection_id(theSection, &theSubsection, i);

			proc->getNameCurrentEntry(theSubsection, &fldStrRTP);
			txt = fldStrRTP.std_str();
			mexReturnString(arr, txt);
			mxSetFieldByNumber(arrL, 0, 0, arr);
			arr = NULL;

			jvxConfigSectionTypes tpSection = JVX_CONFIG_SECTION_TYPE_SECTION;
			proc->getTypeCurrentEntry(theSubsection, &tpSection);
			mexReturnInt32(arr, tpSection);
			mxSetFieldByNumber(arrL, 0, 1, arr);
			arr = NULL;

			switch(tpSection)
			{
			case JVX_CONFIG_SECTION_TYPE_SECTION:
				mexReturnStructSection(arr, theSubsection, proc);

				break;
			case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTHEXSTRING:
				proc->getAssignmentBitField(theSubsection, &bfield);
				mexReturnBitField(arr, bfield);
				break;
			case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING:
				proc->getAssignmentString(theSubsection, &fldStrRTP);
				txt = "error";
				txt = fldStrRTP.std_str();
				mexReturnString(arr, txt.c_str());
				break;
			case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE:
				dataDbl = 0.0;
				proc->getAssignmentValue(theSubsection, &val);
				val.toContent(&dataDbl);
				mexReturnData(arr, dataDbl);
				break;
			case JVX_CONFIG_SECTION_TYPE_STRINGLIST:
				numStrings = 0;
				vecStrings.clear();
				proc->getNumberStrings(theSubsection, &numStrings);
				for(j = 0; j < numStrings; j++)
				{
					txt = "error";
					proc->getString_id(theSubsection, &fldStrRTP, j);
					txt = fldStrRTP.std_str();
					vecStrings.push_back(txt);
				}
				mexReturnStringCell(arr, vecStrings);
				break;
			case JVX_CONFIG_SECTION_TYPE_VALUELIST:
				numLists = 0;
				proc->getNumberValueLists(theSubsection, &numLists);
				returnCell = true;
				if (numLists > 0)
				{
					returnCell = false;
					proc->getValueListSize_id(theSubsection, &numEntriesList, 0);

					for (j = 0; j < numLists; j++)
					{
						proc->getValueListSize_id(theSubsection, &sz, 0);
						if (sz != numEntriesList)
						{
							returnCell = true;
							break;
						}
					}
				}
				if (returnCell)
				{
					for (j = 0; j < numLists; j++)
					{
						proc->getValueList_id(theSubsection, &lstVals, j);
						if (lstVals.ll())
						{
							jvxData* copyLst = new jvxData[lstVals.ll()];
							for (k = 0; k < lstVals.ll(); k++)
							{
								lstVals.elm_at(k).toContent(&copyLst[k]);
							}
							lstDblsCopy.push_back(copyLst);
							lengths.push_back(lstVals.ll());
						}
						else
						{
							lstDblsCopy.push_back(NULL);
							lengths.push_back(0);
						}
					}

					mexReturnDataFieldCell(arr, lstDblsCopy, lengths);

					for (j = 0; j < lstDblsCopy.size(); j++)
					{
						delete[](lstDblsCopy[j]);
					}
					lstDblsCopy.clear();
					lengths.clear();
				}
				else
				{
					jvxData** fldBuf = NULL;
					JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(fldBuf, jvxData*, numLists);
					for (j = 0; j < numLists; j++)
					{
						JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(fldBuf[j], jvxData, numEntriesList);
						proc->getValueList_id(theSubsection, &lstVals, j);
						assert(lstVals.ll() == numEntriesList);
						for (k = 0; k < numEntriesList; k++)
						{
							lstVals.elm_at(k).toContent(&fldBuf[j][k]);
						}

					}
					mexReturnGenericNumeric(arr, (const jvxHandle**)fldBuf, (jvxInt32)numLists, (jvxInt32)numEntriesList, JVX_DATAFORMAT_DATA);
					for (j = 0; j < numLists; j++)
					{
						JVX_DSP_SAFE_DELETE_FIELD(fldBuf[j]);
					}
					JVX_DSP_SAFE_DELETE_FIELD(fldBuf);
				}
				break;
			}
			if(arr == NULL)
			{
				mexFillEmpty(&arr,1,0);
			}
			mxSetFieldByNumber(arrL, 0, 2, arr);
			arr = NULL;

			mxSetFieldByNumber(plhs, 0, i, arrL);
			arrL = NULL;
		}//for(i = 0; i < numSections; i++)
	}//if(numSections)
	else
	{
		mexFillEmpty(&plhs,1,0);
	}
	return(true);
}

bool 
CjvxCToMatlabConverter::mexReturnDataFieldCell(mxArray*& plhs, std::vector<jvxData*> lstDbls, std::vector<jvxSize> lengths)
{
	unsigned i;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = {1, 1};
	mxArray* arr = NULL;
	plhs = NULL;

	if(lstDbls.size() == 0)
	{
		mexFillEmpty(&plhs,1,0);
		return(true);
	}

	dims[0] = 1;
	dims[1] = (int)lstDbls.size();
	plhs = mxCreateCellArray(ndim, dims);
	for(i = 0; i < lstDbls.size(); i++)
	{
		mexReturnDataList(arr, lstDbls[i], JVX_SIZE_INT(lengths[i]));
		mxSetCell(plhs, i, arr);
		arr = NULL;
	}
	return(true);
}

//! Matlab C conversion functions
bool
CjvxCToMatlabConverter::mexReturnGenericNumeric(mxArray*& retObject, const jvxHandle** fieldInput, jvxInt32 dimY, jvxInt32 dimX, jvxDataFormat processingFormat)
{
	mxArray* ptr = NULL;

	jvxData* ptrDouble = NULL;
	float* ptrFloat = NULL;
	jvxInt32* ptrInt32 = NULL;
	jvxInt64* ptrInt64 = NULL;
	jvxInt16* ptrInt16 = NULL;
	jvxInt8* ptrInt8 = NULL;

	switch (processingFormat)
	{
	case JVX_DATAFORMAT_DATA:
		ptr = mxCreateNumericMatrix(dimY, dimX, JVX_DATA_MEX_CONVERT, mxREAL);
		ptrDouble = (jvxData*)mxGetData(ptr);
		for (int i = 0; i < dimX; i++)
		{
			for (int ii = 0; ii < dimY; ii++)
			{
				ptrDouble[i*dimY + ii] = ((jvxData*)fieldInput[ii])[i];
			}
		}
		ptrDouble = NULL;
		break;
	case JVX_DATAFORMAT_16BIT_LE:
		ptr = mxCreateNumericMatrix(dimY, dimX, mxINT16_CLASS, mxREAL);
		ptrInt16 = (jvxInt16*)mxGetData(ptr);
		for (int i = 0; i < dimX; i++)
		{
			for (int ii = 0; ii < dimY; ii++)
			{
				ptrInt16[i*dimY + ii] = ((jvxInt16*)fieldInput[ii])[i];
			}
		}
		ptrInt16 = NULL;
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		ptr = mxCreateNumericMatrix(dimY, dimX, mxINT32_CLASS, mxREAL);
		ptrInt32 = (jvxInt32*)mxGetData(ptr);
		for (int i = 0; i < dimX; i++)
		{
			for (int ii = 0; ii < dimY; ii++)
			{
				ptrInt32[i*dimY + ii] = ((jvxInt32*)fieldInput[ii])[i];
			}
		}
		ptrInt32 = NULL;
		break;
	case JVX_DATAFORMAT_64BIT_LE:
		ptr = mxCreateNumericMatrix(dimY, dimX, mxINT64_CLASS, mxREAL);
		ptrInt64 = (jvxInt64*)mxGetData(ptr);
		for (int i = 0; i < dimX; i++)
		{
			for (int ii = 0; ii < dimY; ii++)
			{
				ptrInt64[i*dimY + ii] = ((jvxInt64*)fieldInput[ii])[i];
			}
		}
		ptrInt64 = NULL;
		break;
	case JVX_DATAFORMAT_8BIT:
		ptr = mxCreateNumericMatrix(dimY, dimX, mxINT8_CLASS, mxREAL);
		ptrInt8 = (jvxInt8*)mxGetData(ptr);
		for (int i = 0; i < dimX; i++)
		{
			for (int ii = 0; ii < dimY; ii++)
			{
				ptrInt8[i*dimY + ii] = ((jvxInt8*)fieldInput[ii])[i];
			}
		}
		ptrInt8 = NULL;
		break;
	case JVX_DATAFORMAT_SIZE:
		ptr = mxCreateNumericMatrix(dimY, dimX, JVX_DATA_MEX_CONVERT, mxREAL);
		ptrDouble = (jvxData*)mxGetData(ptr);
		for (int i = 0; i < dimX; i++)
		{
			for (int ii = 0; ii < dimY; ii++)
			{
				ptrDouble[i*dimY + ii] = (jvxData)((jvxSize*)fieldInput[ii])[i];
			}
		}
		ptrDouble = NULL;
		break;
	default:
		assert(0);
	}
	retObject = ptr;
	return(true);
}

jvxErrorType
CjvxCToMatlabConverter::convertMexToC(jvxHandle** fieldOutput, jvxInt32 dimY, jvxInt32 dimX,
	jvxDataFormat processingFormat, const jvxExternalDataType* ptrF, const char* nameVar, bool convertFloat, 
	std::string& theErrordescr)
{
	const mxArray* ptr = (const mxArray*)ptrF;

	jvxData* ptrDouble = NULL;
	float* ptrFloat = NULL;
	jvxInt32* ptrInt32 = NULL;
	jvxInt64* ptrInt64 = NULL;
	jvxInt16* ptrInt16 = NULL;
	jvxInt8* ptrInt8 = NULL;
	jvxUInt32* ptrUInt32 = NULL;
	jvxUInt64* ptrUInt64 = NULL;
	jvxUInt16* ptrUInt16 = NULL;
	jvxUInt8* ptrUInt8 = NULL;

	theErrordescr = "";

	if (mxGetM(ptr) != dimY)
	{
		theErrordescr = ((std::string)"Format of matrix " + nameVar +
			" does not fit to expected field in Y-dimension, expected " + jvx_int2String(dimY) + ", found " + jvx_int2String((int)mxGetM(ptr)));
		return(JVX_ERROR_INVALID_ARGUMENT);
	}
	if (mxGetN(ptr) != dimX)
	{
		theErrordescr = ((std::string)"Format of matrix " + nameVar +
			" does not fit to expected field in X-dimension, expected " + jvx_int2String(dimX) + ", found " + jvx_int2String((int)mxGetN(ptr)));
		return(JVX_ERROR_INVALID_ARGUMENT);
	}

	switch (processingFormat)
	{
	case JVX_DATAFORMAT_DATA:
		if (mxIsData(ptr))
		{
			ptrDouble = (jvxData*)mxGetData(ptr);
			for (int i = 0; i < dimX; i++)
			{
				for (int ii = 0; ii < dimY; ii++)
				{
					((jvxData*)fieldOutput[ii])[i] = ptrDouble[i * dimY + ii];
				}
			}
			ptrDouble = NULL;
		}
		else
		{
			if (convertFloat)
			{
#ifdef JVX_DSP_DATA_FORMAT_FLOAT
				if (mxIsDouble(ptr))
				{
					double* ptrD = (double*)mxGetData(ptr);
					for (int i = 0; i < dimX; i++)
					{
						for (int ii = 0; ii < dimY; ii++)
						{
							((jvxData*)fieldOutput[ii])[i] = (jvxData)ptrD[i * dimY + ii];
						}
					}
				}
#else
				if (mxIsSingle(ptr))
				{
					float* ptrF = (float*)mxGetData(ptr);
					for (int i = 0; i < dimX; i++)
					{
						for (int ii = 0; ii < dimY; ii++)
						{
							((jvxData*)fieldOutput[ii])[i] = (jvxData)ptrDouble[i * dimY + ii];
						}
					}
				}
#endif
				else
				{
					theErrordescr = ((std::string)"Type of matrix " +
						nameVar + " does not match the expected float type, expected float type is " + JVX_DATA_DESCRIPTOR_THE_OTHER);
			
					return(JVX_ERROR_INVALID_ARGUMENT);
				}
			}
			else
			{
				theErrordescr = ((std::string)"Type of matrix " +
					nameVar + " does not match the expected type, expected type is " + JVX_DATA_DESCRIPTOR);
				
				return(JVX_ERROR_INVALID_ARGUMENT);
			}
		}
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		if (!mxIsInt32(ptr))
		{
			theErrordescr = ((std::string)"Type of matrix " + nameVar +
				" does not match the expected type, expected type is int32");
			
			return(JVX_ERROR_INVALID_ARGUMENT);
		}
		ptrInt32 = (jvxInt32*)mxGetData(ptr);
		for (int i = 0; i < dimX; i++)
		{
			for (int ii = 0; ii < dimY; ii++)
			{
				((jvxInt32*)fieldOutput[ii])[i] = ptrInt32[i * dimY + ii];
			}
		}
		ptrInt32 = NULL;
		break;
	case JVX_DATAFORMAT_64BIT_LE:
		if (!mxIsInt64(ptr))
		{
			theErrordescr = ((std::string)"Type of matrix " + nameVar +
				" does not match the expected type, expected type is int64");
			
			return(JVX_ERROR_INVALID_ARGUMENT);
		}
		ptrInt64 = (jvxInt64*)mxGetData(ptr);
		for (int i = 0; i < dimX; i++)
		{
			for (int ii = 0; ii < dimY; ii++)
			{
				((jvxInt64*)fieldOutput[ii])[i] = ptrInt64[i * dimY + ii];
			}
		}
		ptrInt64 = NULL;
		break;
	case JVX_DATAFORMAT_16BIT_LE:
		if (!mxIsInt16(ptr))
		{
			theErrordescr = ((std::string)"Type of matrix " + nameVar +
				" does not match the expected type, expected type is int16");
			
			return(JVX_ERROR_INVALID_ARGUMENT);
		}
		ptrInt16 = (jvxInt16*)mxGetData(ptr);
		for (int i = 0; i < dimX; i++)
		{
			for (int ii = 0; ii < dimY; ii++)
			{
				((jvxInt16*)fieldOutput[ii])[i] = ptrInt16[i * dimY + ii];
			}
		}
		ptrInt16 = NULL;
		break;
	case JVX_DATAFORMAT_8BIT:
		if (!mxIsInt8(ptr))
		{
			theErrordescr = ((std::string)"Type of matrix " + nameVar +
				" does not match the expected type, expected type is int8");
			
			return(JVX_ERROR_INVALID_ARGUMENT);
		}
		ptrInt8 = (jvxInt8*)mxGetData(ptr);
		for (int i = 0; i < dimX; i++)
		{
			for (int ii = 0; ii < dimY; ii++)
			{
				((jvxInt8*)fieldOutput[ii])[i] = ptrInt8[i * dimY + ii];
			}
		}
		ptrInt8 = NULL;
		break;



	case JVX_DATAFORMAT_U32BIT_LE:
		if (!mxIsUint32(ptr))
		{
			theErrordescr = ((std::string)"Type of matrix " + nameVar +
				" does not match the expected type, expected type is uint32");
			
			return(JVX_ERROR_INVALID_ARGUMENT);
		}
		ptrUInt32 = (jvxUInt32*)mxGetData(ptr);
		for (int i = 0; i < dimX; i++)
		{
			for (int ii = 0; ii < dimY; ii++)
			{
				((jvxUInt32*)fieldOutput[ii])[i] = ptrUInt32[i * dimY + ii];
			}
		}
		ptrUInt32 = NULL;
		break;
	case JVX_DATAFORMAT_U64BIT_LE:
		if (!mxIsUint64(ptr))
		{
			theErrordescr = ((std::string)"Type of matrix " + nameVar +
				" does not match the expected type, expected type is int64");
			
			return(JVX_ERROR_INVALID_ARGUMENT);
		}
		ptrUInt64 = (jvxUInt64*)mxGetData(ptr);
		for (int i = 0; i < dimX; i++)
		{
			for (int ii = 0; ii < dimY; ii++)
			{
				((jvxUInt64*)fieldOutput[ii])[i] = ptrUInt64[i * dimY + ii];
			}
		}
		ptrUInt64 = NULL;
		break;
	case JVX_DATAFORMAT_U16BIT_LE:
		if (!mxIsUint16(ptr))
		{
			theErrordescr = ((std::string)"Type of matrix " + nameVar +
				" does not match the expected type, expected type is int16");
			return(JVX_ERROR_INVALID_ARGUMENT);
		}
		ptrUInt16 = (jvxUInt16*)mxGetData(ptr);
		for (int i = 0; i < dimX; i++)
		{
			for (int ii = 0; ii < dimY; ii++)
			{
				((jvxUInt16*)fieldOutput[ii])[i] = ptrUInt16[i * dimY + ii];
			}
		}
		ptrUInt16 = NULL;
		break;
	case JVX_DATAFORMAT_U8BIT:
		if (!mxIsUint8(ptr))
		{
			theErrordescr = ((std::string)"Type of matrix " + nameVar +
				" does not match the expected type, expected type is int8");
			
			return(JVX_ERROR_INVALID_ARGUMENT);
		}
		ptrUInt8 = (jvxUInt8*)mxGetData(ptr);
		for (int i = 0; i < dimX; i++)
		{
			for (int ii = 0; ii < dimY; ii++)
			{
				((jvxUInt8*)fieldOutput[ii])[i] = ptrUInt8[i * dimY + ii];
			}
		}
		ptrUInt8 = NULL;
		break;


	case JVX_DATAFORMAT_SIZE:
		if (mxIsDouble(ptr))
		{
			double* ptrD = (double*)mxGetData(ptr);
			for (int i = 0; i < dimX; i++)
			{
				for (int ii = 0; ii < dimY; ii++)
				{
					((jvxSize*)fieldOutput[ii])[i] = JVX_DATA2SIZE(ptrD[i * dimY + ii]);
				}
			}
			ptrD = NULL;
		}
		else if (mxIsSingle(ptr))
		{
			float* ptrF = (float*)mxGetData(ptr);
			for (int i = 0; i < dimX; i++)
			{
				for (int ii = 0; ii < dimY; ii++)
				{
					((jvxSize*)fieldOutput[ii])[i] = JVX_DATA2SIZE(ptrF[i * dimY + ii]);
				}
			}
			ptrF = NULL;
		}
		else if (mxIsInt8(ptr))
		{
			ptrInt8 = (jvxInt8*)mxGetData(ptr);
			for (int i = 0; i < dimX; i++)
			{
				for (int ii = 0; ii < dimY; ii++)
				{
					((jvxSize*)fieldOutput[ii])[i] = ptrInt8[i * dimY + ii];
				}
			}
			ptrInt8 = NULL;
		}
		else if (mxIsInt16(ptr))
		{
			ptrInt16 = (jvxInt16*)mxGetData(ptr);
			for (int i = 0; i < dimX; i++)
			{
				for (int ii = 0; ii < dimY; ii++)
				{
					((jvxSize*)fieldOutput[ii])[i] = ptrInt16[i * dimY + ii];
				}
			}
			ptrInt16 = NULL;
		}
		else if (mxIsInt32(ptr))
		{
			ptrInt32 = (jvxInt32*)mxGetData(ptr);
			for (int i = 0; i < dimX; i++)
			{
				for (int ii = 0; ii < dimY; ii++)
				{
					((jvxSize*)fieldOutput[ii])[i] = ptrInt32[i * dimY + ii];
				}
			}
			ptrInt32 = NULL;
		}
		else if (mxIsInt64(ptr))
		{
			ptrInt64 = (jvxInt64*)mxGetData(ptr);
			for (int i = 0; i < dimX; i++)
			{
				for (int ii = 0; ii < dimY; ii++)
				{
					((jvxSize*)fieldOutput[ii])[i] = ptrInt64[i * dimY + ii];
				}
			}
			ptrInt64 = NULL;
		}
		else
		{
			theErrordescr = ((std::string)"Type of matrix " + nameVar +
				" does not match the expected type, it may be of type int8, int16, int32, int64, single or double.");
			
			return(JVX_ERROR_INVALID_ARGUMENT);
		}
		break;
	default:
		assert(0);
	}
	return(JVX_NO_ERROR);
}