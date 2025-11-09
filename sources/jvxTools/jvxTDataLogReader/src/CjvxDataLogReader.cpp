#include <sys/stat.h>
#include <iostream>
#include <cassert>

#include "CjvxDataLogReader.h"


/**
 * Constructor: Do initialize all relevant variables.
 *///====================================================
CjvxDataLogReader::CjvxDataLogReader(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_DATALOGREADER);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxDataLogReader*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	fileHandle.fPtr = NULL;

	fileHandle.fSize = 0;
	eventCallbackLocal = NULL;
}

/**
 * Initialize this module, set state to initialized
 *///=========================================================
jvxErrorType
CjvxDataLogReader::initialize(jvxDataRecoveryCallback eventCallback)
{
	jvxErrorType res = CjvxObject::_initialize();
	if(res == JVX_NO_ERROR)
	{
		res = CjvxObject::_select();
		if(res == JVX_NO_ERROR)
		{
			eventCallbackLocal = eventCallback;
		}
	}
	return(res);
}

/**
 * Terminate: If file is still open, close the file and unscan all sessions.
 *///=========================================================================
jvxErrorType
CjvxDataLogReader::terminate()
{
	this->postprocess();
	this->deactivate();
	CjvxObject::_terminate();

	return(JVX_NO_ERROR);
}

/**
 * Open file with name
 *///========================================================================
jvxErrorType
CjvxDataLogReader::activate(const char* filename)
{
	jvxErrorType res = CjvxObject::_activate();
	if(res == JVX_NO_ERROR)
	{
		fileHandle.fName = filename;

#ifdef VC8
		fopen_s(&fileHandle.fPtr, fileHandle.fName.c_str(), "rb");
#else
		fileHandle.fPtr = fopen(fileHandle.fName.c_str(), "rb");
#endif
		if(!fileHandle.fPtr)
		{
			res = JVX_ERROR_OPEN_FILE_FAILED;
		}
		else
		{
			JVX_FSEEK(fileHandle.fPtr, 0, SEEK_END); // seek to end of file
			fileHandle.fSize = JVX_FTELL(fileHandle.fPtr); // get current file pointer
			//int errorCode = errno; // <- what to find here?
			JVX_FSEEK(fileHandle.fPtr, 0, SEEK_SET); // seek back to beginning of file
		}
	}

	if(res != JVX_NO_ERROR)
	{
		this->deactivate();
	}
	return(res);
}

jvxErrorType
CjvxDataLogReader::filename(jvxApiString* fName)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(_common_set_min.theState >= JVX_STATE_ACTIVE)
	{
		if (fName)
		{
			fName->assign(this->fileHandle.fName);
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

/**
 * Close the open file if there is one.
 * If sessions have been scanned they will be erased.
 *///===============================================================
jvxErrorType
CjvxDataLogReader::deactivate()
{
	this->postprocess();
	jvxErrorType res = CjvxObject::_deactivate();
	if(res == JVX_NO_ERROR)
	{
		if(fileHandle.fPtr)
		{
			fclose(fileHandle.fPtr);
		}
		fileHandle.fName = "";
		fileHandle.fSize = 0;
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

/**
 * If a file is open, the file is scanned for available sessions in the file.
 * Therefore the whole file is parsed with a simple statemachine.
 *///==========================================================================
jvxErrorType
CjvxDataLogReader::prepare()
{
	unsigned i,j;
	jvxUInt16 *numTags = NULL;
	jvxBool tagsOK = false;
	jvxErrorType res = CjvxObject::_prepare();
	if(res == JVX_NO_ERROR)
	{
		fileHandle.nextReportProgress = 0.0;

		// Field to compare headers as strings
		char oneWord[JVX_HEADER_F_IO_SIZE_HEADER+1];

		// Stop statemachine if file has ended
		bool fileEnded = false;

		// Counter for completed sessions
		//int cntSessionsComplete = 0;

		// Counter to stop scanning for datasets
		int cntSetsComplete = 0;

		// Session element for push-back to list
		structSession newSessionElm;

		// Store number of available datasets
		jvxInt32 numberSets;

		// Static data for information copy from file to predefined structure
		jvxSessionHeader hdrConst;

		// Static data for information copy from file to predefined structure
		jvxLogFileDataSetHeader scanSet;

		//! Allocate new set for each dataset (unknown static size..)
		jvxLogFileDataSetHeader* newSet = NULL;

		//! Element to store the dataset and the current position of datachunks
		structDataSet newElmSet;

		//! Element to scan for datachunks
		jvxLogFileDataChunkHeader scanChunk;

		// Init field
		memset(oneWord, 0, sizeof(jvxByte)*(JVX_HEADER_F_IO_SIZE_HEADER+1));

		// Init the state machine
		statesScanFile st = BEFORE_START_SESSION;

		int maxIDDataSets = 0;

		long* dataSetIDs = NULL;

		// Loop to scan through the whole file
		while(!fileEnded)
		{
			jvxSize positionBeforeRead = JVX_FTELL(fileHandle.fPtr);
			jvxSize numReadSince = 0;

			switch(st)
			{
			case BEFORE_START_SESSION:

				//===========================================
				//--> State: Init, not in any session yet <--
				//===========================================

				// Scan session header first
				if(fread(&hdrConst, sizeof(hdrConst), 1, fileHandle.fPtr) == 1)
				{
					numReadSince += sizeof(hdrConst);

					// Check that header is OK
					fileHandle.floatingPointId = hdrConst.fldReserved[0];
					memcpy(oneWord, ((jvxByte*)&hdrConst.fldReserved) + 1, sizeof(jvxByte)*(JVX_HEADER_F_IO_SIZE_HEADER-1));
					std::string compareThis = (std::string)oneWord;
					std::string compareWith = (std::string)JVX_HEADER_F_IO_SESSION_BEGIN;
				
					if(compareThis.substr(0, (JVX_HEADER_F_IO_SIZE_HEADER-1) ) == compareWith.substr(1, std::string::npos))
					{
						// Obtain variables from header
						numberSets = hdrConst.numberSets;
						st = NUMBER_SETS_SCANNED;
						newSessionElm.offsetPositionDatasets = JVX_FTELL(fileHandle.fPtr);
						cntSetsComplete = 0;
						//						newSessionElm.dataSets.clear();
					}
				}
				else
				{
					// No error here, this is a planned end
					fileEnded = true;
				}

				if(!fileEnded)
				{
					numTags = reinterpret_cast<jvxUInt16*>(&hdrConst.reserved[0]);
					tagsOK = true;

					// Scan for tags
					for(i = 0; i < *numTags; i++)
					{
						std::string tagName;
						std::string tagValue;
						char readMe[2];
						jvxUInt16 valI16 = 0;

						memset(readMe, 0, 2*sizeof(char));
						if(fread(oneWord, sizeof(char), JVX_HEADER_F_IO_SIZE_HEADER, fileHandle.fPtr) == JVX_HEADER_F_IO_SIZE_HEADER)
						{
							numReadSince += sizeof(char) * JVX_HEADER_F_IO_SIZE_HEADER;
							if(oneWord == std::string(JVX_HEADER_F_IO_ONETAG))
							{
								if(fread(&valI16, sizeof(jvxUInt16), 1, fileHandle.fPtr) == 1)
								{
									for(j = 0; j < valI16; j++)
									{
										if(fread(&readMe, sizeof(char), 1, fileHandle.fPtr) == 1)
										{
											numReadSince += sizeof(char) * 1;
											tagName += readMe;
										}
										else
										{
											tagsOK = false;
											break;
										}
									}
								}
								if(tagsOK)
								{
									if(fread(&valI16, sizeof(jvxUInt16), 1, fileHandle.fPtr) == 1)
									{
										numReadSince += sizeof(jvxUInt16) * 1;
										for(j = 0; j < valI16; j++)
										{
											if(fread(&readMe, sizeof(char), 1, fileHandle.fPtr) == 1)
											{
												numReadSince += sizeof(char) * 1;
												tagValue += readMe;
											}
											else
											{
												tagsOK = false;
												break;
											}
										}
									}
								}
								if(tagsOK)
								{
									oneTagEntry newTag;
									newTag.tagName = tagName;
									newTag.tagValue = tagValue;

									newSessionElm.theTags.push_back(newTag);
								}
								else
								{
									break;
								}
							}
							else
							{
								tagsOK = false;
								break;
							}
						}
						else
						{
							tagsOK = false;
							break;
						}
					}
					if(!tagsOK)
					{
						// Could not scan dataset
						fileEnded = true;
						res = JVX_ERROR_END_OF_FILE;
					}
				}
				break;
			case NUMBER_SETS_SCANNED:

				//===========================================
				//--> State: Scan for datasets <--
				//===========================================

				// Scan complete dataset
				if(fread(&scanSet, sizeof(scanSet), 1, fileHandle.fPtr) == 1)
				{
					numReadSince += sizeof(scanSet) * 1;
					// Copy the scanned datasets, allocate memory that can hold also user specific data
					newSet = (jvxLogFileDataSetHeader*)new jvxByte[(size_t)scanSet.szBytes];
					memcpy(newSet, &scanSet, sizeof(scanSet));

					// Copy remaing bytes form file
					size_t szToGo = scanSet.szBytes-sizeof(scanSet);
					if(szToGo > 0)
					{
						if(fread(((jvxByte*)newSet+sizeof(scanSet)), sizeof(jvxByte), szToGo, fileHandle.fPtr) != szToGo)
						{
							// Could not complete dataset
							delete[](newSet);
							fileEnded = true;
							res = JVX_ERROR_END_OF_FILE;
							break;
						}
						else
						{
							numReadSince += sizeof(jvxByte) * szToGo;
						}
					}

					// Add new element to dataset list
					newElmSet.ptrDataSet = newSet;
					if(newSet->idDataChunk > maxIDDataSets)
						maxIDDataSets = newSet->idDataChunk;

					newSessionElm.dataSets.push_back(newElmSet);

					// scan as many sets as specified in the header
					if((++cntSetsComplete) == numberSets)
					{
						// From index to number..
						maxIDDataSets++;
						if(maxIDDataSets > 0)
						{
							dataSetIDs = new long[maxIDDataSets];
							memset(dataSetIDs, 0, sizeof(long)*maxIDDataSets);
						}

						jvxInt64 position = JVX_FTELL(fileHandle.fPtr);
						for(i = 0; i < newSessionElm.dataSets.size(); i++)
						{
							// Set position to the first datachunk
							newSessionElm.dataSets[i].offsetNextChunkCurrentDataSet = position;
							newSessionElm.dataSets[i].idNextChunk = 0;
						}

						// Set starting position for datachunks in this session and also a current position
						newSessionElm.offsetDataChunksStart = position;
						newSessionElm.offsetDataChunksCurrent = position;

						// Change state
						st = SETS_SCANNED;
						newSessionElm.numDataChunks = 0;
					}
				}
				else
				{
					// Could not scan dataset
					fileEnded = true;
					res = JVX_ERROR_END_OF_FILE;
				}
				break;
			case SETS_SCANNED:

				//===========================================
				//--> State: Skip all datachunks and find end of session <--
				//===========================================

				// Read header
				jvxInt64 posi = JVX_FTELL(fileHandle.fPtr);
				jvxData progress = (jvxData)posi/(jvxData)fileHandle.fSize;
				if(progress >= fileHandle.nextReportProgress)
				{

					if(this->eventCallbackLocal)
					{
						this->eventCallbackLocal(JVX_DATATRECOVERY_EVENT_TYPE_PROGRESS, &progress, sizeof(jvxData));
					}

					fileHandle.nextReportProgress += 0.1;
				}

				jvxInt64 posid = JVX_FTELL(fileHandle.fPtr);
				size_t numRead = fread(oneWord, sizeof(jvxByte), JVX_HEADER_F_IO_SIZE_HEADER, fileHandle.fPtr);
				if(numRead == JVX_HEADER_F_IO_SIZE_HEADER)
				{
					numReadSince += sizeof(jvxByte) * JVX_HEADER_F_IO_SIZE_HEADER;

					// If end of file, complete this session
					if((std::string)oneWord == (std::string)JVX_HEADER_F_IO_SESSION_END)
					{
						// Copy the number of datachunks with associated dataset ID  to control struct
						for(i = 0; i < newSessionElm.dataSets.size(); i++)
						{
							jvxLogFileDataSetHeader* ptr = newSessionElm.dataSets[i].ptrDataSet;
							newSessionElm.dataSets[i].numDatachunksSetID = dataSetIDs[ptr->idDataChunk];
						}

						// Remove dataset ID counters
						if(dataSetIDs)
							delete[](dataSetIDs);
						dataSetIDs = NULL;
						maxIDDataSets = 0;

						st = BEFORE_START_SESSION;
						newSessionElm.idNextChunk = 0;
						this->sessionList.push_back(newSessionElm);
						newSessionElm.dataSets.clear();
					}

					// If datachunk, skip fwd to next header
					else if((std::string)oneWord == (std::string)JVX_HEADER_F_IO_DATACHUNK)
					{
						// Count datachunks
						newSessionElm.numDataChunks++;
						JVX_FSEEK(fileHandle.fPtr, -JVX_HEADER_F_IO_SIZE_HEADER, SEEK_CUR);
						numReadSince -= JVX_HEADER_F_IO_SIZE_HEADER;
						// Scan datachunk to skip forward until end of datachunk
						if(fread(&scanChunk, sizeof(scanChunk), 1, fileHandle.fPtr) == 1)
						{
							numReadSince += sizeof(scanChunk) * 1;
							if(scanChunk.idDataChunk < maxIDDataSets)
							{
								dataSetIDs[scanChunk.idDataChunk]++;
							}
							int numFwdBytes = (scanChunk.szBytes-sizeof(scanChunk));
							if(JVX_FSEEK(fileHandle.fPtr, numFwdBytes, SEEK_CUR))
							{
								numReadSince += numFwdBytes;
								// For a datachunk before scanning the whole field the file ended
								fileEnded = true;
								res = JVX_ERROR_END_OF_FILE;
								// std::cout << "File integrity failed!" << std::endl;
							}
						}
						else
						{
							// It seemed a datachunk has started but it could not be read
							fileEnded = true;
							res = JVX_ERROR_END_OF_FILE;
							// std::cout << "File integrity failed!" << std::endl;
						}
					}
					else
					{
						// Did not scan datachunk nor session end header
						res = JVX_ERROR_END_OF_FILE;
						// std::cout << "File integrity failed!" << std::endl;
						fileEnded = true;
					}
				}
				else
				{
					// Could not scan endheader, this often happens but should not be the reason not to deliver any output!

					// Copy the number of datachunks with associated dataset ID  to control struct
					for(i = 0; i < newSessionElm.dataSets.size(); i++)
					{
						jvxLogFileDataSetHeader* ptr = newSessionElm.dataSets[i].ptrDataSet;
						newSessionElm.dataSets[i].numDatachunksSetID = dataSetIDs[ptr->idDataChunk];
					}

					// Remove dataset ID counters
					if(dataSetIDs)
					{
						delete[](dataSetIDs);
					}
					dataSetIDs = NULL;
					maxIDDataSets = 0;

					st = BEFORE_START_SESSION;
					newSessionElm.idNextChunk = 0;
					this->sessionList.push_back(newSessionElm);
					newSessionElm.dataSets.clear();
					//						result = false;
					//						fileEnded = true;

				}
				break;
			}
		}

		if(res != JVX_NO_ERROR)
		{
			postprocess();
		}

	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

/**
 * Destroy all lists for datasets and sessions when freeing module.
 *///====================================================================
jvxErrorType
CjvxDataLogReader::postprocess()
{
	jvxErrorType res = CjvxObject::_postprocess();
	if(res == JVX_NO_ERROR)
	{
		// Delete each session
		while(sessionList.size())
		{
			// For each session delete all datasets
			while(sessionList.begin()->dataSets.size())
			{
				delete[]((jvxByte*)sessionList.begin()->dataSets.begin()->ptrDataSet);
				sessionList.begin()->dataSets.erase(sessionList.begin()->dataSets.begin());
			}
			sessionList.erase(sessionList.begin());
		}
	}
	return(res);
}

jvxErrorType
CjvxDataLogReader::floating_point_id_file(jvxUInt8* id)
{
	if(_common_set_min.theState == JVX_STATE_PREPARED)
	{
		if(id)
		{
			*id = fileHandle.floatingPointId;
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

/**
 * When having scanned, return number of available sessions
 *///=============================================================
jvxErrorType
CjvxDataLogReader::number_sessions(jvxSize* numSess)
{
	if(_common_set_min.theState == JVX_STATE_PREPARED)
	{
		if(numSess)
		{
			*numSess = sessionList.size();
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

/**
 * For one session return the number of datasets available.
 *///=================================================================
jvxErrorType
CjvxDataLogReader::number_datasets_session(jvxSize idSess, jvxSize* numSets)
{
	if(_common_set_min.theState == JVX_STATE_PREPARED)
	{
		if(idSess < sessionList.size())
		{
			if(numSets)
			{
				*numSets = (unsigned)sessionList[idSess].dataSets.size();
			}
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_ID_OUT_OF_BOUNDS);
	}
	return(JVX_ERROR_WRONG_STATE);
}

/**
 * Return size of a specified dataset in bytes.
 *///===================================================================
jvxErrorType
CjvxDataLogReader::size_dataset_session(jvxSize idSess, jvxSize idDataSet, jvxSize* szSet)
{
	if(_common_set_min.theState == JVX_STATE_PREPARED)
	{
		if(idSess < sessionList.size())
		{
			if(idDataSet < sessionList[idSess].dataSets.size())
			{
				if(szSet)
					*szSet = (size_t)sessionList[idSess].dataSets[idDataSet].ptrDataSet->szBytes;
				return(JVX_NO_ERROR);
			}
			return(JVX_ERROR_ID_OUT_OF_BOUNDS);
		}
		return(JVX_ERROR_ID_OUT_OF_BOUNDS);
	}
	return(JVX_ERROR_WRONG_STATE);
}




/**
 * 
 *///=================================================================
jvxErrorType
CjvxDataLogReader::number_tags_session(jvxSize idSess, jvxSize* numTags)
{
	if(_common_set_min.theState == JVX_STATE_PREPARED)
	{
		if(idSess < sessionList.size())
		{
			if(numTags)
			{
				*numTags = (unsigned)sessionList[idSess].theTags.size();
			}
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_ID_OUT_OF_BOUNDS);
	}
	return(JVX_ERROR_WRONG_STATE);
}

/**
 * Return size of a specified dataset in bytes.
 *///===================================================================
jvxErrorType
CjvxDataLogReader::name_value_tags_session(jvxSize idSess, jvxSize idTag, jvxApiString* name, jvxApiString* value)
{
	if(_common_set_min.theState == JVX_STATE_PREPARED)
	{
		if(idSess < sessionList.size())
		{
			if(idTag < sessionList[idSess].theTags.size())
			{
				if(name)
					name->assign(sessionList[idSess].theTags[idTag].tagName);
				if(value)
					value->assign( sessionList[idSess].theTags[idTag].tagValue);

				return(JVX_NO_ERROR);
			}
			return(JVX_ERROR_ID_OUT_OF_BOUNDS);
		}
		return(JVX_ERROR_ID_OUT_OF_BOUNDS);
	}
	return(JVX_ERROR_WRONG_STATE);
}






/**
 * Copy a dataset to field passed from outside.
 *///===========================================================
jvxErrorType
CjvxDataLogReader::copy_dataset_session(jvxSize idSess, jvxSize idDataSet, jvxLogFileDataSetHeader* ptrHandle, jvxSize szFld)
{
	if(_common_set_min.theState == JVX_STATE_PREPARED)
	{
		if(idSess < sessionList.size())
		{
			if(idDataSet < sessionList[idSess].dataSets.size())
			{
				if(szFld == sessionList[idSess].dataSets[idDataSet].ptrDataSet->szBytes)
				{
					memcpy(ptrHandle, sessionList[idSess].dataSets[idDataSet].ptrDataSet, sizeof(jvxLogFileDataSetHeader));
					return(JVX_NO_ERROR);
				}
				return(JVX_ERROR_SIZE_MISMATCH);
			}
			return(JVX_ERROR_ID_OUT_OF_BOUNDS);
		}
		return(JVX_ERROR_ID_OUT_OF_BOUNDS);
	}
	return(JVX_ERROR_WRONG_STATE);
}

/**
 * Return the number of datachunks in this session.,
 *///==============================================================
jvxErrorType
CjvxDataLogReader::number_datachunks_session(jvxSize idSession, jvxSize* numChunks)
{
	if(_common_set_min.theState == JVX_STATE_PREPARED)
	{
		if(idSession < sessionList.size())
		{
			if(numChunks)
			{
				*numChunks = (unsigned)sessionList[idSession].numDataChunks;
			}
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_ID_OUT_OF_BOUNDS);
	}
	return(JVX_ERROR_WRONG_STATE);
}

/**
 * Return the number of datachunks in this session for this dataset.,
 *///==============================================================
jvxErrorType
CjvxDataLogReader::number_datachunks_dataset_session(jvxSize idSess, jvxSize idDataSet, jvxSize* numChunks)
{
	if(_common_set_min.theState == JVX_STATE_PREPARED)
	{
		if(idSess < sessionList.size())
		{
			if(idDataSet < sessionList[idSess].dataSets.size())
			{
				if(numChunks)
				{
					*numChunks = (unsigned)sessionList[idSess].dataSets[idDataSet].numDatachunksSetID;
				}
				return(JVX_NO_ERROR);
			}
			return(JVX_ERROR_ID_OUT_OF_BOUNDS);
		}
		return(JVX_ERROR_ID_OUT_OF_BOUNDS);
	}
	return(JVX_ERROR_WRONG_STATE);
}

/**
 * Rewind datachunks for all dataset ids
 *///==================================================================
jvxErrorType
CjvxDataLogReader::rewind_dataChunks_session(jvxSize idSession)
{
	if(_common_set_min.theState == JVX_STATE_PREPARED)
	{
		if(idSession < sessionList.size())
		{
			for(unsigned int i = 0; i < sessionList[idSession].dataSets.size(); i++)
			{
				sessionList[idSession].dataSets[i].offsetNextChunkCurrentDataSet = sessionList[idSession].offsetDataChunksStart;
				sessionList[idSession].offsetDataChunksCurrent = sessionList[idSession].offsetDataChunksCurrent;
				sessionList[idSession].idNextChunk = 0;
			}
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_ID_OUT_OF_BOUNDS);
	}
	return(JVX_ERROR_WRONG_STATE);
}

/**
 * Rewind datachunk read pointer for speciofied dataset
 *///=======================================================================
jvxErrorType
CjvxDataLogReader::rewind_dataChunks_dataset_session(jvxSize idSession, jvxSize idDataSet)
{
	if(_common_set_min.theState == JVX_STATE_PREPARED)
	{
		if(idSession < sessionList.size())
		{
			if(idDataSet < sessionList[idSession].dataSets.size())
			{
				sessionList[idSession].dataSets[idDataSet].offsetNextChunkCurrentDataSet = sessionList[idSession].offsetDataChunksStart;
				sessionList[idSession].dataSets[idDataSet].idNextChunk = 0;
				return(JVX_NO_ERROR);
			}
			return(JVX_ERROR_ID_OUT_OF_BOUNDS);
		}
		return(JVX_ERROR_ID_OUT_OF_BOUNDS);
	}
	return(JVX_ERROR_WRONG_STATE);
}

/**
 * For a specified dataset id, scan for next datachunk. If no more is available, pointer is set to end header.
 *///===========================================================================================================
jvxErrorType
CjvxDataLogReader::find_size_next_datachunk_dataset_session(jvxSize idSession, jvxSize idDataSet, jvxSize* szNextChunk)
{
	//! Element to scan for datachunks
	jvxLogFileDataChunkHeader scanChunk;

	// Field to compare heders as strings
	char oneWord[JVX_HEADER_F_IO_SIZE_HEADER+1];
	memset(oneWord, 0, sizeof(jvxByte)*(JVX_HEADER_F_IO_SIZE_HEADER+1));

	if(_common_set_min.theState == JVX_STATE_PREPARED)
	{
		if(idSession < sessionList.size())
		{
			if(idDataSet < sessionList[idSession].dataSets.size())
			{
				// Set to currently active adress
				if(JVX_FSEEK(fileHandle.fPtr, sessionList[idSession].dataSets[idDataSet].offsetNextChunkCurrentDataSet, SEEK_SET) == 0)
				{
					while(1)
					{
						// Read next datachunk
						if(fread(&scanChunk, sizeof(scanChunk), 1, fileHandle.fPtr) == 1)
						{
							// Compare that this is really a datachunk
							memcpy(oneWord, &scanChunk.fldReserved, sizeof(jvxByte)*JVX_HEADER_F_IO_SIZE_HEADER);
							if((std::string)oneWord == (std::string)JVX_HEADER_F_IO_DATACHUNK)
							{
								// If so, check that it has the right id
								if(scanChunk.idDataChunk == sessionList[idSession].dataSets[idDataSet].ptrDataSet->idDataChunk)
								{
									// If so set the filepointer back to beginning
									JVX_FSEEK(fileHandle.fPtr, -(long)sizeof(scanChunk), SEEK_CUR);
									if(szNextChunk)
									{
										// Return datasize
										*szNextChunk = scanChunk.szBytes;
									}

									// Store current pointer, may be updated also may not be
									sessionList[idSession].dataSets[idDataSet].offsetNextChunkCurrentDataSet = JVX_FTELL(fileHandle.fPtr);
									return(JVX_NO_ERROR);
								}

								// If this chunk was of wrong dataset id, step forward to next chunk
								//sessionList[idSession].dataSets[idSet].idNextChunk++;
								JVX_FSEEK(fileHandle.fPtr, (scanChunk.szBytes-sizeof(scanChunk)), SEEK_CUR);
							}
							else
							{
								// We have found END header here, set back the pointer, there is no more chunk available
								JVX_FSEEK(fileHandle.fPtr, -(long)sizeof(scanChunk), SEEK_CUR);
								return(JVX_ERROR_END_OF_FILE);
							}
						}
						else
						{
							// Could not even scan next header, end of file reached.
							return(JVX_ERROR_END_OF_FILE);
						}
					}
				}
				return(JVX_ERROR_UNEXPECTED);
			}
			return(JVX_ERROR_ID_OUT_OF_BOUNDS);
		}
		return(JVX_ERROR_ID_OUT_OF_BOUNDS);
	}
	return(JVX_ERROR_WRONG_STATE);
}

/**
 * Find the next datachunk available (wont get any forward) and return size of field.
 *///=================================================================================
jvxErrorType
CjvxDataLogReader::find_size_next_datachunk_session(jvxSize idSession, jvxSize* szFld)
{
	//! Element to scan for datachunks
	jvxLogFileDataChunkHeader scanChunk;

	// Field to compare heders as strings
	char oneWord[JVX_HEADER_F_IO_SIZE_HEADER+1];
	memset(oneWord, 0, sizeof(jvxByte)*(JVX_HEADER_F_IO_SIZE_HEADER+1));

	if(_common_set_min.theState == JVX_STATE_PREPARED)
	{
		if(idSession < sessionList.size())
		{
			if(JVX_FSEEK(fileHandle.fPtr, sessionList[idSession].offsetDataChunksCurrent, SEEK_SET) == 0)
			{
				// Try to read next datachunk
				if(fread(&scanChunk, sizeof(scanChunk), 1, fileHandle.fPtr) == 1)
				{
					memcpy(oneWord, &scanChunk.fldReserved, sizeof(jvxByte)*JVX_HEADER_F_IO_SIZE_HEADER);
					if((std::string)oneWord == (std::string)JVX_HEADER_F_IO_DATACHUNK)
					{
						// Got a valid datachunk
						JVX_FSEEK(fileHandle.fPtr, -(long)sizeof(scanChunk), SEEK_CUR);
						if(szFld)
						{
							// return the size
							*szFld = scanChunk.szBytes;
						}
						sessionList[idSession].offsetDataChunksCurrent = JVX_FTELL(fileHandle.fPtr);
						return(JVX_NO_ERROR);
					}
					else
					{
						// Scanned end of file header, rewind
						JVX_FSEEK(fileHandle.fPtr, -(long)sizeof(scanChunk), SEEK_CUR);
						return(JVX_ERROR_END_OF_FILE);
					}
				}
				else
				{
					return(JVX_ERROR_END_OF_FILE);
				}
			}
			return(JVX_ERROR_UNEXPECTED);
		}
		return(JVX_ERROR_ID_OUT_OF_BOUNDS);
	}
	return(JVX_ERROR_WRONG_STATE);
}

/**
 * Copy next datachunk available for any dataset id. It depends on previous call which
 * id is copied. The findAndGet.. will set the pointer propperly.
 *///=====================================================
jvxErrorType
CjvxDataLogReader::copy_next_datachunk_dataset_session(jvxSize idSession, jvxSize idDataSet, jvxLogFileDataChunkHeader* newFld, jvxSize szFld)
{
	//! Element to scan for datachunks
	jvxLogFileDataChunkHeader scanChunk;

	// Field to compare heders as strings
	char oneWord[JVX_HEADER_F_IO_SIZE_HEADER+1];
	memset(oneWord, 0, sizeof(jvxByte)*(JVX_HEADER_F_IO_SIZE_HEADER+1));

	if(_common_set_min.theState == JVX_STATE_PREPARED)
	{
		if(idSession < sessionList.size())
		{
			if(idDataSet < sessionList[idSession].dataSets.size())
			{
				// Set to currently active adress
				if(JVX_FSEEK(fileHandle.fPtr, sessionList[idSession].dataSets[idDataSet].offsetNextChunkCurrentDataSet, SEEK_SET) == 0)
				{
					// Read datachunk
					if(fread(&scanChunk, sizeof(scanChunk), 1, fileHandle.fPtr) == 1)
					{
						// Check that this really has the right setid
						if(scanChunk.idDataChunk == sessionList[idSession].dataSets[idDataSet].ptrDataSet->idDataChunk)
						{
							// If the passed field is large enough, copy
							if(szFld >= sizeof(scanChunk))
							{
								memcpy(newFld, &scanChunk, sizeof(scanChunk));

								// Copy the bits remaining still
								int szToGo = scanChunk.szBytes-sizeof(scanChunk);
								if(szToGo)
								{
									if(!fread((jvxByte*)newFld+sizeof(scanChunk), szToGo, 1, fileHandle.fPtr))
									{
										// This should not happen because file integrity should have been reported earlier
										return(JVX_ERROR_UNEXPECTED);
									}
								}

								// The filepointer is set to the following element for this dataset
								sessionList[idSession].dataSets[idDataSet].idNextChunk++;
								sessionList[idSession].dataSets[idDataSet].offsetNextChunkCurrentDataSet = JVX_FTELL(fileHandle.fPtr);
								return(JVX_NO_ERROR);
							}
							return(JVX_ERROR_SIZE_MISMATCH);
						}

						// Reset the pointer to this chunk
						JVX_FSEEK(fileHandle.fPtr, sessionList[idSession].dataSets[idDataSet].offsetNextChunkCurrentDataSet, SEEK_SET);
						return(JVX_ERROR_UNEXPECTED);
					}
					return(JVX_ERROR_END_OF_FILE);
				}
				return(JVX_ERROR_UNEXPECTED);
			}
			return(JVX_ERROR_ID_OUT_OF_BOUNDS);
		}
		return(JVX_ERROR_ID_OUT_OF_BOUNDS);
	}
	return(JVX_ERROR_WRONG_STATE);
}

/**
 * Copy the next available datachunk. No id checkup.
 *///================================================================
jvxErrorType
CjvxDataLogReader::copy_next_datachunk_session(jvxSize idSession, jvxLogFileDataChunkHeader* newFld, jvxSize szFld)
{
	//! Element to scan for datachunks
	jvxLogFileDataChunkHeader scanChunk;

	// Field to compare heders as strings
	char oneWord[JVX_HEADER_F_IO_SIZE_HEADER+1];
	memset(oneWord, 0, sizeof(jvxByte)*(JVX_HEADER_F_IO_SIZE_HEADER+1));

	if(_common_set_min.theState == JVX_STATE_PREPARED)
	{
		if(idSession < sessionList.size())
		{
			// Set to currently active adress
			if(JVX_FSEEK(fileHandle.fPtr, sessionList[idSession].offsetDataChunksCurrent, SEEK_SET) == 0)
			{
				// Read datachunk
				if(fread(&scanChunk, sizeof(scanChunk), 1, fileHandle.fPtr) == 1)
				{
					// If the passed field is large enough, copy
					if(szFld >= scanChunk.szBytes)
					{
						memcpy(newFld, &scanChunk, scanChunk.szBytes);

						// Copy the bits remaining still
						int szToGo = scanChunk.szBytes-sizeof(scanChunk);
						if(szToGo)
						{
							if(!fread((jvxByte*)newFld+sizeof(scanChunk), szToGo, 1, fileHandle.fPtr))
							{
								// This should not happen because file integrity should have been reported earlier
								return(JVX_ERROR_UNEXPECTED);
							}
						}

						// The filepointer is set to the following element for this dataset
						sessionList[idSession].idNextChunk++;
						sessionList[idSession].offsetDataChunksCurrent = JVX_FTELL(fileHandle.fPtr);
						return(JVX_NO_ERROR);
					}

					// Reset the pointer to this chunk
					JVX_FSEEK(fileHandle.fPtr, sessionList[idSession].offsetDataChunksCurrent, SEEK_SET);
					return(JVX_ERROR_SIZE_MISMATCH);
				}
				return(JVX_ERROR_END_OF_FILE);
			}
			return(JVX_ERROR_UNEXPECTED);
		}
		return(JVX_ERROR_ID_OUT_OF_BOUNDS);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
CjvxDataLogReader::position_session(jvxSize idSession, jvxInt64 *idDatachunkOffset)
{
	if(_common_set_min.theState == JVX_STATE_PREPARED)
	{
		if(idSession < sessionList.size())
		{
			if(idDatachunkOffset)
			{
				*idDatachunkOffset = this->sessionList[idSession].idNextChunk;
			}
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_ID_OUT_OF_BOUNDS);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
CjvxDataLogReader::position_dataset_session(jvxSize idSession, jvxSize idDataSet, jvxInt64 *idDatachunkOffset)
{
	if(_common_set_min.theState == JVX_STATE_PREPARED)
	{
		if(idSession < sessionList.size())
		{
			if(idDataSet < sessionList[idSession].dataSets.size())
			{
				if(idDatachunkOffset)
				{
					*idDatachunkOffset = sessionList[idSession].dataSets[idDataSet].idNextChunk;
				}
				return(JVX_NO_ERROR);
			}
			return(JVX_ERROR_ID_OUT_OF_BOUNDS);
		}
		return(JVX_ERROR_ID_OUT_OF_BOUNDS);
	}
	return(JVX_ERROR_WRONG_STATE);
}

CjvxDataLogReader::~CjvxDataLogReader()
{
	this->terminate();
}
