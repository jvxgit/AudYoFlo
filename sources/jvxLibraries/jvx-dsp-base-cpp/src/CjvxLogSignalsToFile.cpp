#include "CjvxLogSignalsToFile.h"

CjvxLogSignalsToFile::CjvxLogSignalsToFile()
{
	this->fldWrite_frames_channels = NULL;
	this->idxWrite_frames_channels = NULL;
	this->nmFile = "not_configured";
	status_module = JVX_STATE_NONE;
	status_file = JVX_STATE_NONE;
	theToolsHost = NULL;
	theDataLogger_hdl = NULL;
	theDataLogger_obj = NULL;
	theHost = NULL;
	JVX_INITIALIZE_MUTEX(this->safeAccess);
}

CjvxLogSignalsToFile::~CjvxLogSignalsToFile()
{
	JVX_TERMINATE_MUTEX(this->safeAccess);
}
	
jvxErrorType 
CjvxLogSignalsToFile::init_logging(IjvxHiddenInterface* theHostIn, jvxSize numFramesMemoryLoc, jvxSize stopTimeout_msecLoc)
{
	jvxErrorType resL = JVX_NO_ERROR;
	
	if(status_module == JVX_STATE_NONE)
	{
		nmFile = "not_configured";

		theToolsHost = NULL;
		theDataLogger_hdl = NULL;
		theDataLogger_obj = NULL;

		 numFramesMemory = numFramesMemoryLoc;
		 stopTimeout_msec = stopTimeout_msecLoc;

		theHost = theHostIn;

		if(theHost)
		{
			resL = theHost->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, 
								reinterpret_cast<jvxHandle**>(&theToolsHost));
			if((resL == JVX_NO_ERROR) && theToolsHost)
			{
				resL = theToolsHost->instance_tool(JVX_COMPONENT_DATALOGGER, &theDataLogger_obj, 0, NULL);
				if((resL == JVX_NO_ERROR) && theDataLogger_obj)
				{
					resL = theDataLogger_obj->request_specialization(reinterpret_cast<jvxHandle**>(&theDataLogger_hdl), NULL, NULL);
				}
			}
		}

		if(theDataLogger_hdl)
		{
			status_module = JVX_STATE_ACTIVE;
			status_file = JVX_STATE_INIT;
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_NOT_READY);
	}
	return(JVX_ERROR_WRONG_STATE);
}
	
jvxErrorType 
CjvxLogSignalsToFile::add_one_lane(jvxSize numChans, jvxSize bSize, jvxDataFormat form, const std::string& channelName, jvxInt32 dataDescriptor, jvxSize& uniqueId)
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_LOCK_MUTEX(safeAccess);
	if(status_module == JVX_STATE_ACTIVE)
	{
		oneLogFileLane newElm;
		newElm.bsize = bSize;
		newElm.form = form;
		newElm.numChannels = numChans;
		newElm.offsetChannels = 0;
		newElm.channelName = channelName;
		newElm.dataDescriptor = dataDescriptor;
		uniqueId = this->theStoreLanes.size();
		this->theStoreLanes.push_back(newElm);
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	JVX_UNLOCK_MUTEX(safeAccess);
	return res;
}

jvxErrorType 
CjvxLogSignalsToFile::remove_all_lanes()
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_LOCK_MUTEX(safeAccess);
	if(status_module == JVX_STATE_ACTIVE)
	{
		this->theStoreLanes.clear();
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	JVX_UNLOCK_MUTEX(safeAccess);
	return res;
}

jvxErrorType 
CjvxLogSignalsToFile::activate_logging(std::string folderLoc, std::string fName_prefixLoc)
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_LOCK_MUTEX(safeAccess);
	if(status_module >= JVX_STATE_ACTIVE)
	{
		folder = folderLoc;
		fName_prefix = fName_prefixLoc;

		res = this->logfile_select_nolock(folder, fName_prefix);
		if(res == JVX_NO_ERROR)
		{
			if(status_module == JVX_STATE_PROCESSING)
			{
				res = this->logfile_start_nolock();
			}
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	JVX_UNLOCK_MUTEX(safeAccess);
	return res;
}

jvxErrorType 
CjvxLogSignalsToFile::deactivate_logging()
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_LOCK_MUTEX(safeAccess);
	if(status_module >= JVX_STATE_ACTIVE)
	{
		if(status_module == JVX_STATE_PROCESSING)
		{
			res = this->logfile_stop_nolock();			
		}
		if(res == JVX_NO_ERROR)
		{
			res = this->logfile_unselect_nolock();
		}	
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	JVX_UNLOCK_MUTEX(safeAccess);
	return res;
}
	
jvxErrorType 
CjvxLogSignalsToFile::reactivate_logging()
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_LOCK_MUTEX(safeAccess);
	if(status_module >= JVX_STATE_ACTIVE)
	{
		if(status_module == JVX_STATE_PROCESSING)
		{
			res = this->logfile_stop_nolock();
		}
		if(res == JVX_NO_ERROR)
		{
			if(status_file == JVX_STATE_ACTIVE)
			{
				res = this->logfile_unselect_nolock();
				res = this->logfile_select_nolock(folder, fName_prefix);
			}
		}		
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	JVX_UNLOCK_MUTEX(safeAccess);
	return res;
}

jvxErrorType 
CjvxLogSignalsToFile::start_processing()
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_LOCK_MUTEX(safeAccess);
	if(status_module == JVX_STATE_ACTIVE)
	{
		res = logfile_start_nolock();
		if(res == JVX_NO_ERROR)
		{
			status_module = JVX_STATE_PROCESSING;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	JVX_UNLOCK_MUTEX(safeAccess);
	return(res);
}

jvxErrorType 
CjvxLogSignalsToFile::stop_processing()
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_LOCK_MUTEX(safeAccess);
	if(status_module == JVX_STATE_PROCESSING)
	{
		res = logfile_stop_nolock();
		assert(res == JVX_NO_ERROR);
		status_module = JVX_STATE_ACTIVE;
	}
	JVX_UNLOCK_MUTEX(safeAccess);
	return res;
}
	
jvxErrorType 
CjvxLogSignalsToFile::lock_logging()
{
	JVX_LOCK_MUTEX(safeAccess);
	return(JVX_NO_ERROR);
}

jvxErrorType 
CjvxLogSignalsToFile::try_lock_logging()
{
	JVX_TRY_LOCK_MUTEX_RESULT_TYPE resM;
	JVX_TRY_LOCK_MUTEX(resM, safeAccess);
	if(resM == JVX_TRY_LOCK_MUTEX_SUCCESS)
	{
		return JVX_NO_ERROR;
	}
	return(JVX_ERROR_COMPONENT_BUSY);
}

jvxErrorType 
CjvxLogSignalsToFile::unlock_logging()
{
	JVX_UNLOCK_MUTEX(safeAccess);
	return(JVX_NO_ERROR);
}

jvxErrorType 
CjvxLogSignalsToFile::process_logging_try(jvxSize uniqueId, jvxHandle** bufs, jvxSize numChans, jvxDataFormat form, jvxSize bSize, jvxData* fHeightPercent)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::string err;
	JVX_TRY_LOCK_MUTEX_RESULT_TYPE resM;
	JVX_TRY_LOCK_MUTEX(resM, safeAccess);
	if(resM == JVX_TRY_LOCK_MUTEX_SUCCESS)
	{
		if((status_module == JVX_STATE_PROCESSING) && (status_file == JVX_STATE_PROCESSING))
		{
			if(uniqueId < theStoreLanes.size())
			{
				oneLogFileLane oneElm = theStoreLanes[uniqueId];

				if(numChans != oneElm.numChannels)
				{
					res = JVX_ERROR_INVALID_SETTING;
				}
				if(form != oneElm.form)
				{
					res = JVX_ERROR_INVALID_SETTING;
				}
				if(bSize != oneElm.bsize)
				{
					res = JVX_ERROR_INVALID_SETTING;
				}
				
				if(res == JVX_NO_ERROR)
				{
					// write all buffers
					for(i = 0; i < oneElm.numChannels; i++)
					{
						memcpy(fldWrite_frames_channels[oneElm.offsetChannels+i], bufs[i], jvxDataFormat_size[oneElm.form] * oneElm.bsize);
						this->_add_data_reference(idxWrite_frames_channels[oneElm.offsetChannels+i], JVX_SIZE_INT32(uniqueId), JVX_SIZE_INT32(i), err);
					}
					if(fHeightPercent)
					{
						this->_fill_height(*fHeightPercent);
						*fHeightPercent *= 100;
					}
				}
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
		JVX_UNLOCK_MUTEX(safeAccess);
	}
	else
	{
		res = JVX_ERROR_COMPONENT_BUSY;
	}
	return res;
}

jvxErrorType
CjvxLogSignalsToFile::process_logging_lock(jvxSize uniqueId, jvxHandle** bufs, jvxSize numChans, jvxDataFormat form, jvxSize bSize, jvxData* fHeightPercent)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::string err;
	JVX_LOCK_MUTEX(safeAccess);
	if((status_module == JVX_STATE_PROCESSING) && (status_file == JVX_STATE_PROCESSING))
	{
		if(uniqueId < theStoreLanes.size())
		{
			oneLogFileLane oneElm = theStoreLanes[uniqueId];

			if(numChans != oneElm.numChannels)
			{
				res = JVX_ERROR_INVALID_SETTING;
			}
			if(form != oneElm.form)
			{
				res = JVX_ERROR_INVALID_SETTING;
			}
			if(bSize != oneElm.bsize)
			{
				res = JVX_ERROR_INVALID_SETTING;
			}

			if(res == JVX_NO_ERROR)
			{
				// write all buffers
				for(i = 0; i < oneElm.numChannels; i++)
				{
					memcpy(fldWrite_frames_channels[oneElm.offsetChannels+i], bufs[i], jvxDataFormat_size[oneElm.form] * oneElm.bsize);
					this->_add_data_reference(idxWrite_frames_channels[oneElm.offsetChannels+i], JVX_SIZE_INT32(uniqueId), JVX_SIZE_INT32(i), err);
				}
				if(fHeightPercent)
				{
					this->_fill_height(*fHeightPercent);
					*fHeightPercent *= 100;
				}
			}
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	JVX_UNLOCK_MUTEX(safeAccess);
	return res;
}

jvxErrorType
CjvxLogSignalsToFile::process_logging_nolock(jvxSize uniqueId, jvxHandle** bufs, jvxSize numChans, jvxDataFormat form, jvxSize bSize, jvxData* fHeightPercent)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::string err;
	if((status_module == JVX_STATE_PROCESSING) && (status_file == JVX_STATE_PROCESSING))
	{
		if(uniqueId < theStoreLanes.size())
		{
			oneLogFileLane oneElm = theStoreLanes[uniqueId];

			if(numChans != oneElm.numChannels)
			{
				res = JVX_ERROR_INVALID_SETTING;
			}
			if(form != oneElm.form)
			{
				res = JVX_ERROR_INVALID_SETTING;
			}
			if(bSize != oneElm.bsize)
			{
				res = JVX_ERROR_INVALID_SETTING;
			}

			if(res == JVX_NO_ERROR)
			{
				// write all buffers
				for(i = 0; i < oneElm.numChannels; i++)
				{
					memcpy(fldWrite_frames_channels[oneElm.offsetChannels+i], bufs[i], jvxDataFormat_size[oneElm.form] * oneElm.bsize);
					this->_add_data_reference(idxWrite_frames_channels[oneElm.offsetChannels+i], JVX_SIZE_INT32(uniqueId), JVX_SIZE_INT32(i), err);
				}
				if(fHeightPercent)
				{
					this->_fill_height(*fHeightPercent);
					*fHeightPercent *= 100;
				}
			}
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	return res;}

jvxErrorType
CjvxLogSignalsToFile::terminate_logging()
{
	jvxErrorType res = JVX_NO_ERROR;
	stop_processing();
	deactivate_logging();
	remove_all_lanes();
	status_module = JVX_STATE_NONE;
	return(JVX_NO_ERROR);
}

// ========================================================================================

jvxErrorType
CjvxLogSignalsToFile::logfile_select_nolock(std::string fname_path, std::string fname_prefix)
{
	int cnt = 0;
	std::string nm;
	std::vector<std::string> errMessages;
	
	switch(status_file)
	{
	case JVX_STATE_INIT:
		// Open next file
		while(1)
		{
			nm = fname_prefix + "_" + jvx_int2String(cnt) + ".lvx";
			if(!jvxFileExists(fname_path.c_str(), nm.c_str()))
			{
				break;
			}
			else
			{
				cnt++;
			}
		}
	
		nmFile = fname_path + JVX_SEPARATOR_DIR + nm;

		HjvxDataLogger::_activate(theDataLogger_hdl, numFramesMemory, false, JVX_SIZE_INT32(stopTimeout_msec));
		reportText("Activating data log file " + nmFile + " for saving audio data." );

		status_file = JVX_STATE_ACTIVE;
		break;
	default:
		break;
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxLogSignalsToFile::logfile_start_nolock()
{
	jvxSize i,j;
	jvxSize cnt = 0;
	std::vector<std::string> errMessages;
	switch(status_file)
	{
	case JVX_STATE_INIT:
		break;
	case JVX_STATE_ACTIVE:
		fldWrite_frames_channels = NULL;
		idxWrite_frames_channels = NULL;
		numChannelsAll = 0;
		for(i = 0; i < theStoreLanes.size(); i++)
		{
			theStoreLanes[i].offsetChannels = numChannelsAll;
			numChannelsAll += theStoreLanes[i].numChannels;
		}		

		JVX_SAFE_NEW_FLD(fldWrite_frames_channels, jvxHandle*, numChannelsAll);
		memset(fldWrite_frames_channels, 0, sizeof(jvxData*)* numChannelsAll);
		JVX_SAFE_NEW_FLD(idxWrite_frames_channels, jvxSize, numChannelsAll);
		memset(idxWrite_frames_channels, 0, sizeof(jvxSize)* numChannelsAll);

		for(i = 0; i < theStoreLanes.size(); i++)
		{
			assert(cnt == theStoreLanes[i].offsetChannels);
			for(j = 0; j < theStoreLanes[i].numChannels; j++)
			{
				HjvxDataLogger::_add_line(theStoreLanes[i].channelName + "[" + jvx_size2String(j) + "]", theStoreLanes[i].bsize,
				theStoreLanes[i].form, theStoreLanes[i].dataDescriptor, idxWrite_frames_channels[cnt]);
				cnt++;
			}
		}

		HjvxDataLogger::_start(nmFile, errMessages, theHost);

		for(i = 0; i < numChannelsAll; i++)
		{
			HjvxDataLogger::_field_reference(idxWrite_frames_channels[i], reinterpret_cast<jvxByte**>(&fldWrite_frames_channels[i]));
		}
		reportText("Starting data log file " + nmFile + " for saving data for " + jvx_size2String(theStoreLanes.size()) + " lanes." );
		status_file = JVX_STATE_PROCESSING;
		break;
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxLogSignalsToFile::logfile_stop_nolock()
{
	jvxSize numWrittenMod = 0;
	jvxSize numWrittenFile = 0;
	std::vector<std::string> errMessages;

	switch(status_file)
	{
	case JVX_STATE_PROCESSING:
		HjvxDataLogger::_stop(&numWrittenMod, &numWrittenFile, errMessages);
		HjvxDataLogger::_clean_all();

		JVX_SAFE_DELETE_FLD(fldWrite_frames_channels, jvxData*);
		JVX_SAFE_DELETE_FLD(idxWrite_frames_channels, jvxSize);
		
		reportText("Stopping data log file " + nmFile);

		if(numWrittenMod > numWrittenFile)
		{
			reportText("Lost buffers while writing log files.");
		}
		status_file = JVX_STATE_ACTIVE;
		break;
	default:
		break;
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxLogSignalsToFile::logfile_unselect_nolock()
{
	std::vector<std::string> errMessages;

	switch(status_file)
	{
	case JVX_STATE_ACTIVE:

		this->remove_all_lanes();
		HjvxDataLogger::_deactivate();
		reportText("Deactivation data log file " + nmFile );
	
		status_file = JVX_STATE_INIT;
		break;
	default:
		break;
	}
	return(JVX_NO_ERROR);
}

void
CjvxLogSignalsToFile::reportText(std::string txt)
{
	jvxErrorType res;
	if(theHost)
	{
		IjvxReport* theReport = NULL;
		res = theHost->request_hidden_interface(JVX_INTERFACE_REPORT, reinterpret_cast<jvxHandle**>(&theReport));
		if((res == JVX_NO_ERROR) && theReport)
		{
			theReport->report_simple_text_message(txt.c_str(),JVX_REPORT_PRIORITY_SUCCESS );
			theHost->return_hidden_interface(JVX_INTERFACE_REPORT, reinterpret_cast<jvxHandle*>(theReport));
		}
	}
}

jvxErrorType 
CjvxLogSignalsToFile::add_tag(const char* tagName, const char* tagValue)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if(theDataLogger_hdl)
	{
		res = theDataLogger_hdl->addSessionTag(tagName, tagValue);
	}
	return res;
}

jvxErrorType 
CjvxLogSignalsToFile::remove_all_tags()
{

	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if(theDataLogger_hdl)
	{
		res = theDataLogger_hdl->removeAllSessionTags();
	}
	return res;
}
