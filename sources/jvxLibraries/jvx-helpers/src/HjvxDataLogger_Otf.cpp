#include "jvx-helpers.h"

// ================================================================

HjvxDataLogger_Otf::HjvxDataLogger_Otf()
{
	status = JVX_STATE_NONE;
	JVX_INITIALIZE_MUTEX(safeAccess_Otf);
	bwd = NULL;
}

jvxErrorType
HjvxDataLogger_Otf::initialize(IjvxDataLogger_Otf_report* bwdp)
{
	if (status == JVX_STATE_NONE)
	{
		bwd = bwdp;
		status = JVX_STATE_INIT;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
};

jvxErrorType HjvxDataLogger_Otf::terminate()
{
	stop();
	deactivate();
	bwd = NULL;
	status = JVX_STATE_NONE;
	return(JVX_NO_ERROR);
};

jvxErrorType HjvxDataLogger_Otf::activate(const std::string& fName, const std::string& postfix)
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_LOCK_MUTEX(safeAccess_Otf);

	if ((status == JVX_STATE_INIT) ||
		(status == JVX_STATE_ACTIVE))
	{
		int cnt = 0;
		nmFile = "";
		while (1)
		{
			std::string fNameLoc = fName + "_" + jvx_int2String(cnt) + "." + postfix;
			if (!jvxFileExists(".", (fNameLoc).c_str()))
			{
				// Use a filename which does not interact with any of the existing files
				nmFile = fNameLoc;
				break;
			}
			else
			{
				cnt++;
			}
		}

		if (bwd)
		{
			bwd->report_prepare();
		}

		/*
			std::vector<std::string> errMessages;
			CjvxDataLogger::_activate(inProcessing.rtTools.dataLogging.theDataLogger_hdl, LOGFILE_NUMBER_FRAMES_MEMORY, false, LOGFILE_STOP_TIMEOUT_MS );
			this->_report_text_message(("Opened file <"+ nmFile + "> to store log data.").c_str(), JVX_REPORT_PRIORITY_INFO);
		*/

		if (status == JVX_STATE_ACTIVE)
		{
			if (bwd)
			{
				bwd->report_start(nmFile);
			}

			/*
			 startLogfile();
			*/
			status = JVX_STATE_PROCESSING;
		}
		else
		{
			status = JVX_STATE_SELECTED;
		}
	}
	JVX_UNLOCK_MUTEX(safeAccess_Otf);
	return res;
};

jvxErrorType HjvxDataLogger_Otf::deactivate()
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_LOCK_MUTEX(safeAccess_Otf);
	if (status != JVX_STATE_INIT)
	{
		if (status == JVX_STATE_PROCESSING)
		{
			if (bwd)
			{
				bwd->report_stop(nmFile);
			}

			//stopLogfile();
			status = JVX_STATE_ACTIVE;
		}
		else
		{
			status = JVX_STATE_INIT;
		}
	}
	JVX_UNLOCK_MUTEX(safeAccess_Otf);
	return res;
};

jvxErrorType HjvxDataLogger_Otf::start()
{
	jvxErrorType res = JVX_NO_ERROR;

	JVX_LOCK_MUTEX(safeAccess_Otf);
	if (status == JVX_STATE_SELECTED)
	{
		if (bwd)
		{
			bwd->report_start(nmFile);
		}
		//assert(inProcessing.rtTools.dataLogging.theDataLogger_hdl);
		//this->startLogfile();
		status = JVX_STATE_PROCESSING;
	}
	else if (status == JVX_STATE_INIT)
	{
		status = JVX_STATE_ACTIVE;
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	JVX_UNLOCK_MUTEX(safeAccess_Otf);
	return res;
};

jvxErrorType HjvxDataLogger_Otf::stop()
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_LOCK_MUTEX(safeAccess_Otf);
	if (status == JVX_STATE_PROCESSING)
	{
		if (bwd)
		{
			bwd->report_stop(nmFile);
		}
		//this->stopLogfile();
		status = JVX_STATE_SELECTED;
	}
	else if (status == JVX_STATE_ACTIVE)
	{
		status = JVX_STATE_INIT;
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	JVX_UNLOCK_MUTEX(safeAccess_Otf);
	return res;
};

HjvxDataLogger_Otf::~HjvxDataLogger_Otf()
{
	JVX_TERMINATE_MUTEX(safeAccess_Otf);
}
