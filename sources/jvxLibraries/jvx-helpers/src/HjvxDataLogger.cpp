#include "jvx-helpers.h"

HjvxDataLogger::HjvxDataLogger()
{
	this->_loggerData = NULL;
	this->_runtime.headerSetPtrsLines = NULL;
	this->_runtime.headerChunkPtrsLines = NULL;
	this->_nonBlockingWrite = false;
	this->_numFramesMemory = 100;
	this->_theState = JVX_STATE_INIT;
};

HjvxDataLogger::~HjvxDataLogger()
{
	_deactivate();
};

jvxErrorType
HjvxDataLogger::_state(jvxState& stateReturn)
{
	stateReturn = _theState;
	return(JVX_NO_ERROR);
}

// Start in state JVX_STATE_INIT
jvxErrorType
HjvxDataLogger::_activate(IjvxDataLogger* logFileRef, jvxSize numFramesMemory, jvxBool nonBlockingWrite, jvxInt32 timeoutstop_ms)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_theState == JVX_STATE_INIT)
	{
		_theState = JVX_STATE_ACTIVE;
		_loggerData = logFileRef;
		_nonBlockingWrite = nonBlockingWrite;
		_numFramesMemory = numFramesMemory;
		_timeoutStop_ms = timeoutstop_ms;
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
};

jvxErrorType
HjvxDataLogger::_add_line(const std::string description, jvxSize numElements, jvxDataFormat formatElements, jvxInt32 dataDescriptorUser, jvxSize& theId)
{
	jvxErrorType res = JVX_NO_ERROR;
	oneLine newElement;
	if (_theState == JVX_STATE_ACTIVE)
	{
		newElement.format = formatElements;
		assert(numElements <= 0x7FFFFFFF); // <- 32 bit limitation
		newElement.numElements = (jvxInt32)numElements;
		newElement.descriptionLine = description;
		newElement.numEntriesAdded = 0;
		newElement.numEntriesFailed = 0;
		newElement.descriptorUser = dataDescriptorUser;
		theId = _activeLines.size();
		_activeLines.push_back(newElement);
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
};

jvxErrorType
HjvxDataLogger::_number_lines(jvxSize& numOnReturn)
{
	numOnReturn = _activeLines.size();
	return(JVX_NO_ERROR);
};

jvxErrorType
HjvxDataLogger::_description_line(std::string& descOnReturn, jvxSize lineNo)
{
	if (lineNo < _activeLines.size())
	{
		descOnReturn = _activeLines[lineNo].descriptionLine;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_ID_OUT_OF_BOUNDS);
};

jvxErrorType
HjvxDataLogger::_number_elements_line(jvxInt32& numElements, jvxSize lineNo)
{
	if (lineNo < _activeLines.size())
	{
		numElements = _activeLines[lineNo].numElements;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_ID_OUT_OF_BOUNDS);
};

jvxErrorType
HjvxDataLogger::_format_elements_line(jvxDataFormat& format, jvxSize lineNo)
{
	if (lineNo < _activeLines.size())
	{
		format = _activeLines[lineNo].format;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_ID_OUT_OF_BOUNDS);
};

jvxErrorType
HjvxDataLogger::_number_entries_line(jvxInt64& numAddedDataChunks, jvxSize lineNo)
{
	if (lineNo < _activeLines.size())
	{
		numAddedDataChunks = _activeLines[lineNo].numEntriesAdded;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_ID_OUT_OF_BOUNDS);
};

jvxErrorType
HjvxDataLogger::_number_entries_failed_line(jvxInt64& numDatachunksFailed, jvxSize lineNo)
{
	if (lineNo < _activeLines.size())
	{
		numDatachunksFailed = _activeLines[lineNo].numEntriesFailed;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_ID_OUT_OF_BOUNDS);
};

jvxErrorType
HjvxDataLogger::_start(const std::string& filename, std::vector<std::string>& errMessages, IjvxHiddenInterface* hostRef)
{
	int i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType errCodeLocal = JVX_NO_ERROR;
	int numBytesOneSet = 0;

	errMessages.clear();
	if (_theState == JVX_STATE_ACTIVE)
	{
		_filename = filename;
		_runtime.headerSetPtrsLines = NULL;
		_runtime.headerChunkPtrsLines = NULL;

		if (_loggerData)
		{
			// Allocate memory
			JVX_SAFE_NEW_FLD(_runtime.headerSetPtrsLines, jvxLogFileDataSetHeader*, _activeLines.size());
			JVX_SAFE_NEW_FLD(_runtime.headerChunkPtrsLines, jvxLogFileDataChunkHeader*, _activeLines.size());

			for (i = 0; i < (int)_activeLines.size(); i++)
			{

				// Allocate the required number of datachunk templates including the load part of that field
				int numBytesFrameDatachunk = sizeof(jvxLogFileDataChunkHeader);
				numBytesFrameDatachunk += (int)jvxDataFormat_size[_activeLines[i].format] * _activeLines[i].numElements;
				numBytesOneSet += numBytesFrameDatachunk;

				JVX_SAFE_NEW_OBJ(_runtime.headerSetPtrsLines[i], jvxLogFileDataSetHeader);
				memset(_runtime.headerSetPtrsLines[i], 0, sizeof(jvxLogFileDataSetHeader));
				_runtime.headerSetPtrsLines[i]->szBytes = sizeof(jvxLogFileDataSetHeader);
				_runtime.headerSetPtrsLines[i]->dataTypeGlobal = _activeLines[i].format;
				_runtime.headerSetPtrsLines[i]->dataDescriptorUser = _activeLines[i].descriptorUser;
				_runtime.headerSetPtrsLines[i]->idDataChunk = -1; // To be filled �ater
				_runtime.headerSetPtrsLines[i]->numberElementsIfConstant = _activeLines[i].numElements;
				jvx_fillCharStr(_runtime.headerSetPtrsLines[i]->noteOnType, JVX_FILE_IO_SIZE_NOTE, _activeLines[i].descriptionLine);

				JVX_SAFE_NEW_FLD_CVRT(_runtime.headerChunkPtrsLines[i], jvxByte, numBytesFrameDatachunk, jvxLogFileDataChunkHeader*);// = (jvxLogFileDataChunkHeader*)new byte[numBytesFrameDatachunk];
				memset(_runtime.headerChunkPtrsLines[i], 0, numBytesFrameDatachunk);
				_runtime.headerChunkPtrsLines[i]->szBytes = numBytesFrameDatachunk;
				_runtime.headerChunkPtrsLines[i]->idUser = i;
				_runtime.headerChunkPtrsLines[i]->dataTypeGlobal = _activeLines[i].format;
				_runtime.headerChunkPtrsLines[i]->subIdUser = 0;
				_runtime.headerChunkPtrsLines[i]->ticktime_us = 0;
				_runtime.headerChunkPtrsLines[i]->numberElements = _activeLines[i].numElements;
			}

			res = _loggerData->initialize(numBytesOneSet * _numFramesMemory, numBytesOneSet, hostRef);
			if (res != JVX_NO_ERROR)
			{
				std::string errDescr = "Function call <initialize> failed: error code ";
				errDescr += jvxErrorType_descr(res);
				errMessages.push_back(errDescr);
			}

			if (res == JVX_NO_ERROR)
			{
				for (i = 0; i < (int)_activeLines.size(); i++)
				{
					res = _loggerData->associate_datahandle(_runtime.headerSetPtrsLines[i]);
					if (res != JVX_NO_ERROR)
					{
						std::string errDescr = "Function call <associate_datahandle> failed: error code ";
						errDescr += jvxErrorType_descr(res);
						errMessages.push_back(errDescr);
						break;
					}
					else
					{
						_runtime.headerChunkPtrsLines[i]->idDataChunk = _runtime.headerSetPtrsLines[i]->idDataChunk;
					}
				}
			}

			if (res == JVX_NO_ERROR)
			{
				res = _loggerData->activate(this->_filename.c_str(), false);
				if (res != JVX_NO_ERROR)
				{
					std::string errDescr = "Function call <activate> failed: error code ";
					errDescr += jvxErrorType_descr(res);
					errMessages.push_back(errDescr);
				}
			}

			if (res == JVX_NO_ERROR)
			{
				res = _loggerData->start(false);
				if (res != JVX_NO_ERROR)
				{
					std::string errDescr = "Function call <start> failed: error code ";
					errDescr += jvxErrorType_descr(res);
					errMessages.push_back(errDescr);
				}
			}

			if (res != JVX_NO_ERROR)
			{
				_deactivate();
			}
			else
			{
				_theState = JVX_STATE_PROCESSING;
			}
		}
		else
		{
			// No file logger reference passed
			res = JVX_ERROR_NOT_READY;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
};


// All available in state JVX_STATE_PROCESSING
jvxErrorType
HjvxDataLogger::_field_reference(jvxSize idxLine, jvxByte** field, jvxSize* numElms)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_theState == JVX_STATE_PROCESSING)
	{
		if (idxLine < _activeLines.size())
		{
			if (field)
			{
				if (_runtime.headerChunkPtrsLines)
				{
					*field = ((jvxByte*)_runtime.headerChunkPtrsLines[idxLine]) + sizeof(jvxLogFileDataChunkHeader);
					if (numElms)
					{
						*numElms = _runtime.headerChunkPtrsLines[idxLine]->numberElements;
					}
				}
				else
				{
					res = JVX_ERROR_UNEXPECTED;
				}
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
};

jvxErrorType
HjvxDataLogger::_add_data_reference(jvxSize idxLine, jvxInt32 idUser, jvxInt32 subIdUser, std::string& errDescr, bool noBlock)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_theState == JVX_STATE_PROCESSING)
	{
		if (idxLine < _activeLines.size())
		{
			assert(_loggerData);

			jvxInt64 tStamp = 0;
			res = _loggerData->timestamp_us(&tStamp);
			if (res == JVX_NO_ERROR)
			{
				_runtime.headerChunkPtrsLines[idxLine]->ticktime_us = tStamp;
			}
			else
			{
				errDescr = "Function call <_add_data_reference> failed: error code ";
				errDescr += jvxErrorType_descr(res);
			}

			_runtime.headerChunkPtrsLines[idxLine]->idUser = idUser;
			_runtime.headerChunkPtrsLines[idxLine]->subIdUser = subIdUser;

			if (res == JVX_NO_ERROR)
			{
				res = _loggerData->pass_new(_runtime.headerChunkPtrsLines[idxLine], noBlock);
				if (res != JVX_NO_ERROR)
				{
					errDescr = "Function call <passNewData> failed: error code ";
					errDescr += jvxErrorType_descr(res);
				}
			}
		}//if(idxLine < _activeLines.size())
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
};

jvxErrorType
HjvxDataLogger::_stop(jvxSize* numBytesWrittenToBuffer, jvxSize* numBytesWrittenToFile, std::vector<std::string>& errorDescrs)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	if (_theState == JVX_STATE_PROCESSING)
	{
		_theState = JVX_STATE_ACTIVE;
		assert(_loggerData);

		res = _loggerData->stop(numBytesWrittenToBuffer, numBytesWrittenToFile, _timeoutStop_ms);
		if (res != JVX_NO_ERROR)
		{
			errorDescrs.push_back(std::string("Function call <stop> failed: error code ") + jvxErrorType_descr(res));
		}

		res = _loggerData->deactivate();
		if (res != JVX_NO_ERROR)
		{
			errorDescrs.push_back(std::string("Function call <deactivate> failed: error code ") + jvxErrorType_descr(res));
		}

		for (i = 0; i < (int)_activeLines.size(); i++)
		{
			res = _loggerData->deassociate_datahandle(_runtime.headerSetPtrsLines[i]);
			if (res != JVX_NO_ERROR)
			{
				errorDescrs.push_back(std::string("Function call <deassociate_datahandle> failed: error code ") + jvxErrorType_descr(res));
			}

			JVX_SAFE_DELETE_OBJ(_runtime.headerSetPtrsLines[i]);
			JVX_SAFE_DELETE_FLD(_runtime.headerChunkPtrsLines[i], jvxByte);
		}//for(i = 0; i < _activeLines.size(); i++)

		JVX_SAFE_DELETE_FLD(_runtime.headerSetPtrsLines, jvxLogFileDataSetHeader*);
		JVX_SAFE_DELETE_FLD(_runtime.headerChunkPtrsLines, jvxLogFileDataChunkHeader*);

		res = _loggerData->terminate();
		if (res != JVX_NO_ERROR)
		{
			errorDescrs.push_back(std::string("Function call <deassociateDataHandle> failed: error code ") + jvxErrorType_descr(res));
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
};


jvxErrorType
HjvxDataLogger::_clean_all()
{
	if (_theState == JVX_STATE_ACTIVE)
	{
		_activeLines.clear();
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
};

jvxErrorType
HjvxDataLogger::_deactivate()
{
	std::vector<std::string> errD;
	if (_theState == JVX_STATE_PROCESSING)
	{

		_stop(NULL, NULL, errD);
	}
	if (_theState == JVX_STATE_ACTIVE)
	{
		_clean_all();
		_theState = JVX_STATE_INIT;
	}
	return(JVX_NO_ERROR);
};

jvxErrorType
HjvxDataLogger::_handle_logfile(IjvxDataLogger** hdlOnReturn)
{
	if (hdlOnReturn)
	{
		*hdlOnReturn = this->_loggerData;
	}
	return(JVX_NO_ERROR);
};

jvxErrorType
HjvxDataLogger::_fill_height(jvxData& fHeight)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_theState == JVX_STATE_PROCESSING)
	{
		res = this->_loggerData->buffer_fillheight(&fHeight);
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
};

jvxErrorType
HjvxDataLogger::_add_session_tag(std::string tagName, std::string tagValue)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_theState == JVX_STATE_ACTIVE)
	{
		res = this->_loggerData->addSessionTag(tagName.c_str(), tagValue.c_str());
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
};

jvxErrorType
HjvxDataLogger::_remove_session_tags()
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_theState == JVX_STATE_ACTIVE)
	{
		res = this->_loggerData->removeAllSessionTags();
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
};

