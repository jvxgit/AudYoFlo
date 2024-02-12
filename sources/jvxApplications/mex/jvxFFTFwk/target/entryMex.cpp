#include "jvx.h"

#include "localMexIncludes.h"
#include "CjvxCToMatlabConverter.h"
#include "CjvxMatlabToCConverter.h"

#include "CjvxFFTFwk.h"

jvxSize uniqueId = 1;
std::map<int, CjvxFFTFwk*> instances;

void mexFunction(int nlhs, mxArray* plhs[],
	int nrhs, const mxArray* prhs[])
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	int retValId = -1;
	SZ_MAT_TYPE ndim = 2;
	SZ_MAT_TYPE dims[2] = { 1, 1 };
	const mxArray* in = NULL;
	mxArray* pplhs0 = NULL;
	mxArray* pplhs1 = NULL;

	jvxBool paramsOk = false;
	if ((nlhs >= 1) && (nlhs <= 4))
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
		if (nrhs >= 1)
		{
			if (mxIsChar(prhs[0]))
			{
				std::string cmd = CjvxMatlabToCConverter::jvx_mex_2_cstring(prhs[0]);
				if (cmd == "init")
				{
					if (nrhs == 2)
					{
						// Initialize noise estimator
						paramsOk = true;
						in = prhs[1];
						CjvxFFTFwk* fftInst = nullptr;
						JVX_DSP_SAFE_ALLOCATE_OBJECT(fftInst, CjvxFFTFwk);
						res = fftInst->init_fft_fwk(in);
						if (res == JVX_NO_ERROR)
						{
							retValId = uniqueId++;
							instances[retValId] = fftInst;
						}
						else
						{
							JVX_DSP_SAFE_DELETE_OBJECT(fftInst);
						}
					}
				}
				else if (cmd == "term")
				{
					if (nrhs == 2)
					{
						jvxSize idx = JVX_SIZE_UNSELECTED;
						res = CjvxMatlabToCConverter::mexArgument2Index<jvxSize>(idx, prhs, 1, nrhs);
						if (res == JVX_NO_ERROR)
						{
							auto elm = instances.find(idx);
							if (elm != instances.end())
							{
								// Terminate noise estimator
								paramsOk = true;
								retValId = idx;
								res = elm->second->term_fft_fwk();
								JVX_SAFE_DELETE_OBJECT(elm->second);
								instances.erase(elm);
							}
							else
							{
								std::string txt = "Passed instance id <";
								txt += jvx_size2String(idx);
								txt += "> is not a valid handle reference.";
								mexWarnMsgTxt(txt.c_str());
							}
						}
						else
						{
							std::string txt = "Invalid type to pass instance id for processing instance.";
							mexWarnMsgTxt(txt.c_str());
						}
					}
				}
				else if (cmd == "input")
				{
					if (nrhs == 3)
					{
						jvxSize idx = JVX_SIZE_UNSELECTED;
						res = CjvxMatlabToCConverter::mexArgument2Index<jvxSize>(idx, prhs, 1, nrhs);
						if (res == JVX_NO_ERROR)
						{
							auto elm = instances.find(idx);
							if (elm != instances.end())
							{							
								if (mxIsData(prhs[2]) && !mxIsComplex(prhs[2]))
								{
									int M = mxGetM(prhs[2]);
									int N = mxGetN(prhs[2]);
									if ((M == 1) && (N == elm->second->hopsize))
									{
										paramsOk = true;
										retValId = idx;
										jvxData* inBuf = (jvxData*)mxGetData(prhs[2]);
										jvxSize szIn = N;
										jvxSize szOut = elm->second->M2P1;
										jvxDataCplx* outBufProc = nullptr;
										jvxDataCplx* outBufAna = nullptr;

										// Allocate output buffer
										SZ_MAT_TYPE ndim = 2;
										SZ_MAT_TYPE dims[2] = { 1, 1 };
										dims[1] = (int)szOut;
										if (nlhs > 0)
										{
											pplhs0 = mxCreateNumericArray(ndim, dims, JVX_DATA_MEX_CONVERT, mxCOMPLEX);
											outBufProc = (jvxDataCplx*)mxGetData(pplhs0);
										}
										if (nlhs > 1)
										{
											pplhs1 = mxCreateNumericArray(ndim, dims, JVX_DATA_MEX_CONVERT, mxCOMPLEX);
											outBufAna = (jvxDataCplx*)mxGetData(pplhs1);
										}

										res = elm->second->process_fft_fwk_input(inBuf, outBufProc, outBufAna);
									}
									else
									{
										std::string txt = "Input data is of wrong dimension, it should be <1x";
										txt += jvx_size2String(N);
										txt += ">.";
										mexWarnMsgTxt(txt.c_str());
										res = JVX_ERROR_INVALID_FORMAT;
									}
								}
								else
								{
									std::string txt = "Input data is of wrong format, it should be <";
									txt += jvxDataFormat_txt(JVX_DATAFORMAT_DATA);
									txt += "-real>.";
									mexWarnMsgTxt(txt.c_str());
									res = JVX_ERROR_INVALID_FORMAT;
								}
								
							}
							else
							{
								std::string txt = "Passed instance id <";
								txt += jvx_size2String(idx);
								txt += "> is not a valid handle reference.";
								mexWarnMsgTxt(txt.c_str());
							}
						}
						else
						{
							std::string txt = "Invalid type to pass instance id for processing instance.";
							mexWarnMsgTxt(txt.c_str());
						}
					}
					else
					{
						std::string txt = "Invalid number of input arguments for sub-command <input>.";
						mexWarnMsgTxt(txt.c_str());
					}
				}
				else if (cmd == "output")
				{
					if (nrhs == 4)
					{
						jvxSize idx = JVX_SIZE_UNSELECTED;
						res = CjvxMatlabToCConverter::mexArgument2Index<jvxSize>(idx, prhs, 1, nrhs);
						if (res == JVX_NO_ERROR)
						{
							auto elm = instances.find(idx);
							if (elm != instances.end())
							{
								if (mxIsData(prhs[2]) && mxIsComplex(prhs[2]))
								{
									int M = mxGetM(prhs[2]);
									int N = mxGetN(prhs[2]);
									if ((M == 1) && (N == elm->second->M2P1))
									{
										jvxDataCplx* inBuf = (jvxDataCplx*)mxGetData(prhs[2]);
										jvxSize szIn = N;

										if (
											(nrhs > 3) &&
											(mxGetM(prhs[3]) == M) &&
											(mxGetN(prhs[3]) == N) &&
											mxIsData(prhs[3]) &&
											!mxIsComplex(prhs[3]))
										{
											jvxData* inWeights = (jvxData*)mxGetData(prhs[3]);

											if (nlhs > 0)
											{
												paramsOk = true;
												retValId = idx;
												jvxSize szOut = elm->second->hopsize;
												jvxData* outBuf = nullptr;

												// Allocate output buffer
												SZ_MAT_TYPE ndim = 2;
												SZ_MAT_TYPE dims[2] = { 1, 1 };
												dims[1] = (int)szOut;

												pplhs0 = mxCreateNumericArray(ndim, dims, JVX_DATA_MEX_CONVERT, mxREAL);
												outBuf = (jvxData*)mxGetData(pplhs0);

												res = elm->second->process_fft_fwk_output(inBuf, inWeights, outBuf);
											}
											else
											{
												std::string txt = "No space for output data, there should be 1 output argument.";
												mexWarnMsgTxt(txt.c_str());
												res = JVX_ERROR_INVALID_ARGUMENT;
											}
										}
										else
										{
											std::string txt = "Weight input data is of wrong format, it should be a real vector of dimension <1x";
											txt += jvx_size2String(N);
											txt += "> and type <";
											txt += jvxDataFormat_txt(JVX_DATAFORMAT_DATA);
											txt += ">";
											mexWarnMsgTxt(txt.c_str());
											res = JVX_ERROR_INVALID_FORMAT;
										}
									}
									else
									{
										std::string txt = "Signal data is of wrong dimension, it should be <1x";
										txt += jvx_size2String(N);
										txt += ">.";
										mexWarnMsgTxt(txt.c_str());
										res = JVX_ERROR_INVALID_FORMAT;
									}
								}
								else
								{
									std::string txt = "Signal data is of wrong format, it should be <";
									txt += jvxDataFormat_txt(JVX_DATAFORMAT_DATA);
									txt += "-real>.";
									mexWarnMsgTxt(txt.c_str());
									res = JVX_ERROR_INVALID_FORMAT;
								}
							}
							else
							{
								std::string txt = "Passed instance id <";
								txt += jvx_size2String(idx);
								txt += "> is not a valid handle reference.";
								mexWarnMsgTxt(txt.c_str());
							}
						}
						else
						{
							std::string txt = "Invalid type to pass instance id for processing instance.";
							mexWarnMsgTxt(txt.c_str());
						}
					}
					else
					{
						std::string txt = "Invalid number of input arguments for sub-command <output>.";
						mexWarnMsgTxt(txt.c_str());
					}
				}
				else
				{
					mexWarnMsgTxt("Invalid command token, expecting <init> to initialize and <term> to terminate noise estimator.");
				}
			} // if(mxIsChar)
		}
		if (pplhs0 == nullptr)
		{
			CjvxCToMatlabConverter::mexReturnInt32(pplhs0, retValId);
		}
		plhs[0] = pplhs0;
		if (pplhs1)
		{
			plhs[1] = pplhs1;
		}
	}

	if (!paramsOk)
	{
		mexWarnMsgTxt(
			"Invalid call to mex function <jvxNeCore> to run noise estimation.\n"
			"The module must be initialized first and can then be involved frame by frame.\n"
			"The following call options are available:\n"
			"Option 1 -- init: Input Argument #1: text <init>, followed by a configuration struct. This function returns a reference handle (id).\n"
			"Option 2 -- term: Input Argument #1: text <term>, followed by the previously obtained reference id. Function returns the passed reference handle (id).\n"
			"Option 3 -- input: Input Argument #1: text <input>, input argument #2: reference handle (id), input argument #3: A <1 x HOPSIZE> array holding input signal buffer. Function returns 1 output array to hold complex vector of fft data for processing and 1 output array to hold complex vector of fft data for analysis.\n"
			"Option 4 -- output: Input Argument #1: text <output>, , input argument #2: reference handle (id), input argument #3: A <1 x M2P1> array holding complex fft vector, input argument #4: A <1 x M2P1> array holding real weight vector. Function returns 1 output array to hold real vector of (processed) output signal.\n");
	}

	return;
}





