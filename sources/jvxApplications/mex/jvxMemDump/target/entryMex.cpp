#include "jvx.h"

#if _MATLAB_MEXVERSION < 500
#if (_MSC_VER >= 1600)
#include <yvals.h>
#define __STDC_UTF_16__
#endif
#endif
#include <mex.h>

/*
#pragma comment ( lib, "libmx")
#pragma comment ( lib, "libmex")
#pragma comment ( lib, "libeng")
#pragma comment ( lib, "libmat")*/
//#pragma comment ( lib, "libmatlb") 

//#define DLL_EXPORT __declspec(dllexport)

#include <stdio.h>
#include <string>
#include <vector>

#if _MATLAB_MEXVERSION < 500
#if (_MSC_VER >= 1600)
#include <yvals.h>
#define __STDC_UTF_16__
#endif
#endif

#if _MATLAB_MEXVERSION >= 100
#define SZ_MAT_TYPE mwSize
#else
#define SZ_MAT_TYPE int
#endif

static std::string jvx_mex_2_cstring(const mxArray* phs)
{
	jvxSize bufLen = mxGetM(phs)*mxGetN(phs) * sizeof(char) + 1;
	char* buf = new char[bufLen];
	mxGetString(phs, buf, JVX_SIZE_INT(bufLen));
	std::string str = buf;
	delete[](buf);
	return(str);
}

void mexEmptyCellList(mxArray*& plhs, jvxSize numEntries)
{
	//unsigned i;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };
	mxArray* arr = NULL;
	plhs = NULL;

	if (numEntries == 0)
	{
		dims[0] = 0;
		dims[1] = (int)0;
		plhs = mxCreateCellArray(ndim, dims);
	}
	else
	{
		dims[0] = (int)numEntries;
		dims[1] = 1;
		plhs = mxCreateCellArray(ndim, dims);
		/*
		for (i = 0; i < lstStr.size(); i++)
		{
			arr = mxCreateString(lstStr[i].c_str());
			mxSetCell(plhs, i, arr);
			arr = NULL;
		}*/
	}
}

typedef enum
{
	JVX_TOKEN_INT32,
	JVX_TOKEN_UINT32,
	JVX_TOKEN_FLOAT,
	JVX_TOKEN_DOUBLE
} jvxTokenFormat;

typedef struct
{
	std::string name;
	jvxSize szBytes;
	jvxTokenFormat form;

	union 
	{
		jvxInt32 valI32;
		jvxUInt32 valUI32;
		float valf;
		double vald;
	} content;
} oneFieldStruct;

void mexAddStructToCellList(mxArray*& plhs, std::vector<oneFieldStruct>& addme, jvxSize idx)
{
	jvxSize i;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };
	mxArray* arr = NULL, *oneEntry = NULL;
	std::vector<std::string> lstTxt;
	const char** ptrC = NULL;
	jvxInt32* ptrWriteI32 = NULL;
	jvxUInt32* ptrWriteUI32 = NULL;
	float* ptrWriteF = NULL;
	double* ptrWriteD = NULL;

	ptrC = new const char*[addme.size()];

	for (i = 0; i < addme.size(); i++)
	{
		lstTxt.push_back(addme[i].name);
	}

	for (i = 0; i < addme.size(); i++)
	{
		ptrC[i] = lstTxt[i].c_str();
	}


	oneEntry = mxCreateStructArray(ndim, dims, (int)addme.size(), ptrC);

	/*
	arr = mxCreateNumericArray(ndim, dims, mxINT32_CLASS, mxREAL);
	ptrWriteI32 = (jvxInt32*)mxGetData(arr);
	*ptrWriteI32 = 0;
	*/

	for(i = 0; i < addme.size(); i++)
	{
		arr = NULL;
		switch (addme[i].form)
		{
		case JVX_TOKEN_INT32:
			arr = mxCreateNumericArray(ndim, dims, mxINT32_CLASS, mxREAL);
			ptrWriteI32 = (jvxInt32*)mxGetData(arr);
			*ptrWriteI32 = addme[i].content.valI32;
			break;
		case JVX_TOKEN_UINT32:
			arr = mxCreateNumericArray(ndim, dims, mxUINT32_CLASS, mxREAL);
			ptrWriteUI32 = (jvxUInt32*)mxGetData(arr);
			*ptrWriteUI32 = addme[i].content.valUI32;
			break;
		case JVX_TOKEN_FLOAT:
			arr = mxCreateNumericArray(ndim, dims, mxSINGLE_CLASS, mxREAL);
			ptrWriteF = (float*)mxGetData(arr);
			*ptrWriteF = addme[i].content.valf;
			break;
		case JVX_TOKEN_DOUBLE:
			arr = mxCreateNumericArray(ndim, dims, mxDOUBLE_CLASS, mxREAL);
			ptrWriteD = (double*)mxGetData(arr);
			*ptrWriteD = addme[i].content.vald;
			break;
		default:
			assert(0);
		}
		mxSetFieldByNumber(oneEntry, 0, (int)i, arr);
		arr = NULL;
	}
	mxSetCell(plhs, idx, oneEntry);
	oneEntry = NULL;
	delete[](ptrC);
}

/**
 * C-style entry from MATLAB to MEX component.
 * All command arguments are passed in the optional arguments for
 * this mex function.
 *///=============================================================
void mexFunction( int nlhs, mxArray *plhs[],
 				  int nrhs, const mxArray *prhs[] )
{
	jvxSize i,j;
	jvxSize cnt = 0, cntE = 0;
	jvxByte* ptrB = NULL;
	jvxInt32 tokenI32 = 0;
	float tokenF;
	double tokenD;
	std::string oTok;
	std::string memdump;
	std::string format;
	std::vector<std::string> props_list;
	std::vector<oneFieldStruct> fields;
	jvxBool doit = false;
	jvxBool contRun = true;
	jvxSize numBytesOneEntry = 0;
	jvxSize numBytes;
	jvxSize numEntries;
	jvxSize checkBytes;
	//SZ_MAT_TYPE ndim = 2;
	//SZ_MAT_TYPE dims[2] = {1, 1};

	if((nrhs == 2)&&(nlhs == 1))
	{
		if( (mxIsChar(prhs[0])) && (mxIsChar(prhs[1])))
		{
			memdump = jvx_mex_2_cstring(prhs[0]);
			format = jvx_mex_2_cstring(prhs[1]);

			jvx_parsePropertyStringToKeylist(format, props_list);
			for (i = 0; i < props_list.size(); i++)
			{
				std::string one_expression = props_list[i];
				size_t posi = one_expression.find("=");
				std::string varName = "field_" + jvx_size2String(i);
				std::string fldType = one_expression;
				if (posi != std::string::npos)
				{
					varName = one_expression.substr(0, posi);
					fldType = one_expression.substr(posi + 1, std::string::npos);
				}
				jvx_tokenRemoveCharLeftRight(varName);
				jvx_tokenRemoveCharLeftRight(fldType);

				oneFieldStruct s;
				s.name = varName;
				if (fldType == "int32")
				{
					s.form = JVX_TOKEN_INT32;
					s.szBytes = sizeof(jvxInt32);
					fields.push_back(s);
					numBytesOneEntry += s.szBytes;
				}
				else if (fldType == "uint32")
				{
					s.form = JVX_TOKEN_UINT32;
					s.szBytes = sizeof(jvxUInt32);
					fields.push_back(s);
					numBytesOneEntry += s.szBytes;
				}
				else if (fldType == "float")
				{
					s.form = JVX_TOKEN_FLOAT;
					s.szBytes = sizeof(float);
					fields.push_back(s);
					numBytesOneEntry += s.szBytes;
				}
				else if (fldType == "double")
				{
					s.form = JVX_TOKEN_DOUBLE;
					s.szBytes = sizeof(float);
					fields.push_back(s);
					numBytesOneEntry += s.szBytes;
				}
				else
				{
					mexErrMsgTxt(("Invalid datatype in format specification: " + fldType).c_str());
				}
			}

			numBytes = memdump.size() / 2;
			numEntries = numBytes / numBytesOneEntry;
			checkBytes = numEntries * numBytesOneEntry;

			mexEmptyCellList(plhs[0], numEntries);

			cnt = 0;
			cntE = 0;
			while (contRun)
			{
				for (i = 0; i < fields.size(); i++)
				{
					switch (fields[i].form)
					{
					case JVX_TOKEN_INT32:
						tokenI32 = 0;
						ptrB = (jvxByte*) &tokenI32;
						if ( 
							(cnt + (fields[i].szBytes * 2)) <= memdump.size())
						{
							for (j = 0; j < fields[i].szBytes; j++)
							{
								oTok = memdump.substr(cnt, 2);
								*ptrB = (jvxByte)strtol(oTok.c_str(), NULL, 16);
								ptrB++;
								cnt += 2;
							}
							fields[i].content.valI32 = tokenI32;
						}
						else
						{
							contRun = false;
						}
						break;
					case JVX_TOKEN_FLOAT:
						tokenF = 0.0;
						ptrB = (jvxByte*)&tokenF;
						if (((cnt + fields[i].szBytes) * 2) < memdump.size())
						{
							for (j = 0; j < fields[i].szBytes; j++)
							{
								oTok = memdump.substr(cnt, 2);
								*ptrB = (jvxByte)strtol(oTok.c_str(), NULL, 16);
								ptrB++;
								cnt += 2;
							}
							fields[i].content.valf = tokenF;
						}
						else
						{
							contRun = false;
						}
						break;
					case JVX_TOKEN_DOUBLE:
						tokenF = 0.0;
						ptrB = (jvxByte*)&tokenD;
						if (((cnt + fields[i].szBytes) * 2) < memdump.size())
						{
							for (j = 0; j < fields[i].szBytes; j++)
							{
								oTok = memdump.substr(cnt, 2);
								*ptrB = (jvxByte)strtol(oTok.c_str(), NULL, 16);
								ptrB++;
								cnt += 2;
							}
							fields[i].content.valf = tokenF;
						}
						else
						{
							contRun = false;
						}
						break;
					default:
						break;
					}
				}
				if (contRun)
				{
					mexAddStructToCellList(plhs[0], fields, cntE);
					cntE++;
				}
			}
			return;
		}
		mexErrMsgTxt("Input arguments must be two strings!");
	}
	mexErrMsgTxt("2 Input arguments and 1 Output argument expected!");
}

