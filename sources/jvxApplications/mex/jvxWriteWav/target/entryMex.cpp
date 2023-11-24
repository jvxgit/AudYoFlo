#include "jvx.h"
#include "jvxFileWriter.h"

#include "localMexIncludes.h"
#include "CjvxMatlabToCConverter.h"
#include "CjvxCToMatlabConverter.h"

#define CLIP_VALUE 0.99
#define SMOOTH_LEVELS 0.995

/**
 * C-style entry from MATLAB to MEX component.
 * All command arguments are passed in the optional arguments for
 * this mex function.
 * Input arguments
 * 1) input signal,
 * 2) numerator coeffs
 * 3) denominator coeffs, can be even left out (=1)
 * 4) Filter states, can even be left out to start with 0-states
 * Output arguments
 * 1) Output signal
 * 2) Filterstates, can be left out if not interesting
 *///=============================================================
void mexFunction( int nlhs, mxArray *plhs[],
 				  int nrhs, const mxArray *prhs[] )
{
	jvxSize i,ix,iy;	
	jvxBool paramsOk = true;

	if (!
		((nrhs >= 2) && (nrhs <= 3)))
	{
		paramsOk = false;
	}
	if (paramsOk)
	{
		if (!
			((nlhs >= 1) && (nlhs <= 1)))
		{
			paramsOk = false;
		}
	}

	if(paramsOk)
	{
		std::string fName;
		jvxSize rate = 48000;

		if (nrhs >= 2)
		{
			jvxErrorType res = CjvxMatlabToCConverter::mexArgument2String(fName, prhs, 0, nrhs);
			jvxSize numChans = mxGetM(prhs[1]);
			jvxSize numSamples = mxGetN(prhs[1]);
			jvxData* ptrRead = reinterpret_cast<jvxData*>(mxGetData(prhs[1]));
			
			if (nrhs >= 3)
			{
				res = CjvxMatlabToCConverter::mexArgument2Index<jvxSize>(rate, prhs, 2, nrhs);
			}
			if(res == JVX_NO_ERROR)
			{
				jvxFileWriter fWriter;
				jvxEndpointClass epCls = JVX_FILETYPE_WAV;
				jvxFileDescriptionEndpoint_open fileDescrO = jvxFileDescriptionEndpoint_open::createWrite();
				fileDescrO.fFormatFile = JVX_FILEFORMAT_PCM_FIXED;
				fileDescrO.numberBitsSample = 16;
				fileDescrO.samplerate = rate;
				fileDescrO.littleEndian = true;				
				fileDescrO.numberChannels = numChans;
				fileDescrO.lengthFile = numSamples;
				fileDescrO.subtype = JVX_WAV_32;

				res = fWriter.activate(fName, epCls, &fileDescrO);
				if (res == JVX_NO_ERROR)
				{
					jvxFileDescriptionEndpoint_prepare fileDescrP;
					fileDescrP.dFormatAppl = JVX_DATAFORMAT_DATA;
					fileDescrP.numSamplesFrameMax_moveBgrd = 8192;
					res = fWriter.prepare(8192, false, &fileDescrP, jvxFileOperationType::JVX_FILEOPERATION_BATCH);
					if (res == JVX_NO_ERROR)
					{
						fWriter.start();
						
						jvxData** bufsChannels = nullptr;
						jvxSize copybufSize = 128;
						jvxSize offsX = 0;
						jvxSize toCopy = 0;
						JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(bufsChannels, jvxData*, numChans);
						for (i = 0; i < numChans; i++)
						{
							JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(bufsChannels[i], jvxData, copybufSize);
						}
						jvxSize numFrames = floor((jvxData)fileDescrO.lengthFile / (jvxData)copybufSize);
						for (i = 0; i < numFrames; i++)
						{
							toCopy = copybufSize;
							for (ix = 0; ix < toCopy; ix++)
							{
								for (iy = 0; iy < fileDescrO.numberChannels; iy++)
								{
									jvxSize ixx = ix + offsX;
									((jvxData*)bufsChannels[iy])[ix] = ptrRead[ixx * fileDescrO.numberChannels + iy];
								}
							}
							fWriter.add_buffer(reinterpret_cast<jvxHandle**>(bufsChannels), fileDescrO.numberChannels, toCopy, true, nullptr);							
							offsX += toCopy;
						}

						toCopy = fileDescrO.lengthFile - numFrames * copybufSize;
						if (toCopy)
						{
							/* Copy in correct way*/
							for (ix = 0; ix < toCopy; ix++)
							{
								for (iy = 0; iy < fileDescrO.numberChannels; iy++)
								{
									jvxSize ixx = ix + offsX;
									((jvxData*)bufsChannels[iy])[ix] = ptrRead[ixx * fileDescrO.numberChannels + iy];
								}
							}
							fWriter.add_buffer(reinterpret_cast<jvxHandle**>(bufsChannels), fileDescrO.numberChannels, toCopy, true, nullptr);																			
						}
						
						CjvxCToMatlabConverter::mexReturnInt32(plhs[0], JVX_NO_ERROR);
						
						for (i = 0; i < numChans; i++)
						{
							JVX_DSP_SAFE_DELETE_FIELD(bufsChannels[i]);
						}
						JVX_DSP_SAFE_DELETE_FIELD(bufsChannels);

						fWriter.stop();
						fWriter.postprocess();
					}
					fWriter.deactivate();
				}
			}
		}
		return;
	}
	mexWarnMsgTxt("This function requires 1-3 input and 1 output arguments!");
}

