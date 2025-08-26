#include "jvx-helpers.h"

void
jvx_run_external_command(IjvxExternalCall* hdl, std::string command)
{
	if (hdl)
	{
		jvxErrorType resL = hdl->executeExternalCommand(command.c_str());
		if (resL != JVX_NO_ERROR)
		{
			jvxApiError err;
			hdl->lasterror(&err);
			std::string errText = "Error in command <" + command + ">: " + err.errorDescription.std_str();
			hdl->postMessageExternal(errText.c_str());
		}
	}
}

/*
 * jvx_executeExternalCommand_1data(usefulRtprocFeatures.theMatlabCall,
 *					"global inc_re; global indsp_re; global inc_im; global indsp_im; [myHandle] = jvxAuNnr_motor.add_entry_external_call(myHandle, 'fft_fw_rpm', " + jvx_size2String(dbg.fCnt) + ", " + jvx_size2String(cplxDsp) + ", inc_re, indsp_re, inc_im, indsp_im);",
 *								 4, "inc_re", 513, procControl.ram.spectrumChassis.re, (int)JVX_DATAFORMAT_DATA, "indsp_re", 513, dbg.buf_513_1, (int)JVX_DATAFORMAT_DATA, 
 *								 "inc_im", 513, procControl.ram.spectrumChassis.im, (int)JVX_DATAFORMAT_DATA, "indsp_im", 513, dbg.buf_513_2, (int)JVX_DATAFORMAT_DATA);
 */
void
jvx_run_external_command_1data(IjvxExternalCall* hdl, std::string command, int numEntries, ...)
{
	jvxSize i;
	va_list ap;
	jvxExternalDataType* ext = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	const char* ptr = NULL;
	if (hdl)
	{
		jvxErrorType resL = JVX_NO_ERROR;
		va_start(ap, numEntries);

		for (i = 0; i < numEntries; i++)
		{
			ptr = NULL;
			ptr = va_arg(ap, const char*);
			assert(ptr);
			int ll = -1;
			ll = va_arg(ap, int);
			assert(ll > 0);
			jvxHandle* ptrData = NULL;
			ptrData = va_arg(ap, jvxData*);
			assert(ptrData);
			int form = (int)JVX_DATAFORMAT_NONE;
			form = va_arg(ap, int);
			assert(form != (int)JVX_DATAFORMAT_NONE);

			if (ptrData && ptr)
			{
				ext = NULL;
				res = hdl->convertCToExternal(&ext, ptrData, ll, (jvxDataFormat)form);
				if ((res == JVX_NO_ERROR) && ext)
				{
					res = hdl->putVariableToExternal("global", ptr, ext);
					if (res != JVX_NO_ERROR)
					{
						hdl->postMessageExternal(("Failed to put variable " + (std::string)ptr + " to workspace.").c_str(), false);
					}
				}
				else
				{
					hdl->postMessageExternal(("Failed to prepare variable " + (std::string)ptr + " to copy to workspace.").c_str(), false);
				}
			}
			else
			{
				hdl->postMessageExternal(("Failed to prepare variable " + (std::string)ptr + " to copy to workspace.").c_str(), false);
			}
		}
		resL = hdl->executeExternalCommand(command.c_str());
		if (resL != JVX_NO_ERROR)
		{
			jvxApiError err;
			hdl->lasterror(&err);
			std::string errText = "Error in command <" + command + ">: " + err.errorDescription.std_str();
			hdl->postMessageExternal(errText.c_str());
		}
	}
}
