#include "ayf-hrtf.h"

ayfHrtfDispenser::ayfHrtfDispenser()
{
	JVX_INITIALIZE_MUTEX(safeAccess);
}

ayfHrtfDispenser::~ayfHrtfDispenser()
{
	JVX_TERMINATE_MUTEX(safeAccess);
}

// ==============================================================================
// IjvxPropertyExtender
// ==============================================================================

jvxErrorType ayfHrtfDispenser::supports_prop_extender_type(jvxPropertyExtenderType tp)
{
	if (tp == jvxPropertyExtenderType::JVX_PROPERTY_EXTENDER_HRTF_DISPENSER)
	{
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
ayfHrtfDispenser::prop_extender_specialization(jvxHandle** prop_extender, jvxPropertyExtenderType tp)
{
	if (prop_extender)
	{
		if (tp == jvxPropertyExtenderType::JVX_PROPERTY_EXTENDER_HRTF_DISPENSER)
		{
			*prop_extender = static_cast<IjvxPropertyExtenderHrtfDispenser*>(this);
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_INVALID_SETTING;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

// ==============================================================================
// IayfHrtfDispenser
// ==============================================================================


jvxErrorType 
ayfHrtfDispenser::get_length_hrir(jvxSize& length_hrir, jvxSize* loadIdArg, jvxSize slotId)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (slotId < dataBaseSlots.size())
	{
		if (dataBaseSlots[slotId].activeDatabase)
		{
			length_hrir = dataBaseSlots[slotId].activeDatabase->length_hrir;
			if (loadIdArg)
			{
				*loadIdArg = dataBaseSlots[slotId].activeDatabase->loadId;
			}
		}
		else
		{
			length_hrir = JVX_SIZE_UNSELECTED;
			if (loadIdArg)
			{
				*loadIdArg = JVX_SIZE_UNSELECTED;
			}
			res = JVX_ERROR_NOT_READY;
		}
	}
	else
	{
		res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	return res;
}


jvxErrorType
ayfHrtfDispenser::bind(jvxSize* samplerateArg)
{
	if (statDispenser >= jvxState::JVX_STATE_INIT)
	{
		// Databases are always allocated for only one samplerate.
		// The first to bind sets the rate!!
		// Nevertheless, we also need to bind a slot!
		if (statDispenser == jvxState::JVX_STATE_ACTIVE)
		{
			// No problem, we can accept that.
			if (samplerateArg)
			{
				if (*samplerateArg == this->samplerate)
				{
					return JVX_NO_ERROR;
				}
				*samplerateArg = this->samplerate;
				return JVX_ERROR_INVALID_SETTING;
			}
		}

		if (!samplerateArg) return JVX_ERROR_INVALID_ARGUMENT;

		this->samplerate = *samplerateArg;
		//if (slotId < dataBaseSlots.size())
		for(auto& elm: dataBaseSlots)
		{
			JVX_LOCK_MUTEX(safeAccess);
			load_selected_database_inlock(elm.activeDatabase, elm.idx);
			JVX_UNLOCK_MUTEX(safeAccess);
		}
		/*
		else
		{
			return JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
		*/
		statDispenser = jvxState::JVX_STATE_ACTIVE;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
ayfHrtfDispenser::number_slots(jvxSize& num)
{
	if (statDispenser >= jvxState::JVX_STATE_INIT)
	{
		num = dataBaseSlots.size();
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
ayfHrtfDispenser::slot_descrption(jvxSize idx, jvxApiString& description)
{
	if (statDispenser >= jvxState::JVX_STATE_INIT)
	{
		if (idx < dataBaseSlots.size())
		{
			description = dataBaseSlots[idx].description;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
ayfHrtfDispenser::copy_closest_hrir_pair(jvxData azimuth_deg, jvxData inclination_deg, 
	jvxData* hrir_left, jvxData* hrir_right, jvxSize length_hrir, jvxSize slotId)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (statDispenser != jvxState::JVX_STATE_ACTIVE)
	{
			return JVX_ERROR_WRONG_STATE;
	}
	if (slotId >= dataBaseSlots.size())
	{
		return JVX_ERROR_ID_OUT_OF_BOUNDS;
	}

	JVX_TRY_LOCK_MUTEX_RESULT_TYPE resW = JVX_TRY_LOCK_MUTEX_NO_SUCCESS;
	JVX_TRY_LOCK_MUTEX(resW, safeAccess);
	//JVX_LOCK_MUTEX(safeAccess);

	if (resW == JVX_TRY_LOCK_MUTEX_SUCCESS)
	{
		if (hrir_left && hrir_right)
		{
			assert(length_hrir == dataBaseSlots[slotId].activeDatabase->length_hrir);
			assert(length_hrir == dataBaseSlots[slotId].length_buffer_hrir);
			float hrir_delay_left = 0;          // unit is samples
			float hrir_delay_right = 0;         // unit is samples
			float const elevation_deg = -inclination_deg + 90.0;
			float source_coordinates[3] = { static_cast<float>(azimuth_deg) , static_cast<float>(elevation_deg), (jvxData)2 };
			mysofa_s2c(source_coordinates);
			mysofa_getfilter_float_nointerp(dataBaseSlots[slotId].activeDatabase->mysofa_db, source_coordinates[0],
				source_coordinates[1], source_coordinates[2], dataBaseSlots[slotId].buffer_hrir_left,
				dataBaseSlots[slotId].buffer_hrir_right, &hrir_delay_left, &hrir_delay_right);

			// Databases which return delayed HRIRs are not supported.
			if (hrir_delay_left != 0.0 || hrir_delay_right != 0.0)
			{
				res = JVX_ERROR_UNSUPPORTED;
			}
			else
			{
				for (int idx_sample = 0; idx_sample < dataBaseSlots[slotId].activeDatabase->length_hrir; idx_sample++)
				{
					hrir_left[idx_sample] = (jvxData)dataBaseSlots[slotId].buffer_hrir_left[idx_sample];
					hrir_right[idx_sample] = (jvxData)dataBaseSlots[slotId].buffer_hrir_right[idx_sample];
				}
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
ayfHrtfDispenser::get_closest_direction(jvxData& azimuth_deg, jvxData& inclination_deg, jvxSize slotId)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;

	if (statDispenser != jvxState::JVX_STATE_ACTIVE)
	{
		return JVX_ERROR_WRONG_STATE;
	}
	if (slotId >= dataBaseSlots.size())
	{
		return JVX_ERROR_ID_OUT_OF_BOUNDS;
	}

	JVX_TRY_LOCK_MUTEX_RESULT_TYPE resW = JVX_TRY_LOCK_MUTEX_NO_SUCCESS;
	JVX_TRY_LOCK_MUTEX(resW, safeAccess);
	//JVX_LOCK_MUTEX(safeAccess);
	if (resW == JVX_TRY_LOCK_MUTEX_SUCCESS)
	{
		azimuth_deg = 0.0;
		inclination_deg = 0.0;
		
		res = JVX_NO_ERROR;

		JVX_UNLOCK_MUTEX(safeAccess);
	}
	else
	{
		res = JVX_ERROR_COMPONENT_BUSY;
	}
	return res;
}

// ==============================================================================
// ==============================================================================

jvxErrorType 
ayfHrtfDispenser::start(const std::string& directory, jvxSize numSlots, const char** slotNames)
{
	char bufRet[MAX_PATH] = { 0 };
	if (statDispenser == jvxState::JVX_STATE_NONE)
	{		
		int sofa_major_version = 0;
		int sofa_minor_version = 0;
		int sofpat = 0;

		mysofa_getversion(&sofa_major_version, &sofa_minor_version, &sofpat);

		if (JVX_GETCURRENTDIRECTORY(bufRet, MAX_PATH))
		{
			curWorkingDir = bufRet;
		}
		pathName = directory;
		// Check folder to collect all files
		JVX_HANDLE_DIR searchHandle;
		JVX_DIR_FIND_DATA searchResult = INIT_SEARCH_DIR_DEFAULT;;
		std::string ext = ".sofa";
		searchHandle = JVX_FINDFIRSTFILE_WC(searchHandle, pathName.c_str(), ext.c_str(), searchResult);
		bool result = true;
		bool foundElement = false;

		// If entry exists
		if (searchHandle != INVALID_HANDLE_VALUE)
		{
			// scan for sofa files
			do
			{
				std::string fileName = pathName;
				fileName += JVX_SEPARATOR_DIR;
				fileName += JVX_GETFILENAMEDIR(searchResult);//searchResult.cFileName;
				
				// Backslash is not wanted
				fileName = jvx_replaceStrInStr(fileName, "\\", "/");
				fileName = jvx_absoluteToRelativePath(fileName, true);
				oneSofaDataBase* newEntry = nullptr;
				JVX_DSP_SAFE_ALLOCATE_OBJECT(newEntry, oneSofaDataBase);
				newEntry->fName = fileName;
				sofaDataBases.push_back(newEntry);

			} while (JVX_FINDNEXTFILE(searchHandle, searchResult, ext.c_str()));//FindNextFile( searchHandle, &searchResult )
			JVX_FINDCLOSE(searchHandle);//FindClose( searchHandle );
		}

		jvxSize cnt = 0;
		dataBaseSlots.resize(numSlots);
		for (auto& elm : dataBaseSlots)
		{
			if (slotNames)
			{
				elm.description = slotNames[cnt];
			}
			else
			{
				elm.description = "hrtf-slot #" + jvx_size2String(cnt);
			}
			elm.idx = cnt++;			
			select_database(0, elm.idx);
		}
		statDispenser = jvxState::JVX_STATE_INIT;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
ayfHrtfDispenser::stop()
{
	if (statDispenser > jvxState::JVX_STATE_NONE)
	{
		JVX_LOCK_MUTEX(safeAccess);
		
		for (auto& sofa_database : sofaDataBases)
		{
			if (sofa_database->is_open) {
				sofa_database->close();
			}
		}

		for (auto& elm : dataBaseSlots)
		{
			elm.activeDatabase = nullptr;

			// The allocation first deallocates and does not re-allocate on nullptr
			this->allocate_hrir_buffers_inlock(elm.activeDatabase, elm.idx);
		}
		JVX_UNLOCK_MUTEX(safeAccess);

		for (auto sofa_database : sofaDataBases)
		{
			JVX_DSP_SAFE_DELETE_OBJECT(sofa_database);
		}
		sofaDataBases.clear();
		dataBaseSlots.clear();

		statDispenser = jvxState::JVX_STATE_NONE;
	}
	return JVX_NO_ERROR;
}

jvxSize 
ayfHrtfDispenser::num_sofa_files()
{
	return sofaDataBases.size();
}


jvxErrorType 
ayfHrtfDispenser::name_sofa_file(jvxSize idx, std::string& name)
{
	if (idx < sofaDataBases.size())
	{
		auto sofa_database = sofaDataBases.begin();

		std::advance(sofa_database, idx);

		name = (*sofa_database)->fName;

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}


jvxErrorType
ayfHrtfDispenser::load_selected_database_inlock(oneSofaDataBase* dbase, jvxSize slotId) 
{
	jvxErrorType res = JVX_NO_ERROR;

	if (dbase == nullptr) return res;


	if (!dbase->is_open)
	{
		res = dbase->open(this->samplerate);
		dbase->loadId = loadIdCnt++;
		assert(res == JVX_NO_ERROR);
	}

	this->allocate_hrir_buffers_inlock(dbase, slotId);

	return res;
}

void
ayfHrtfDispenser::allocate_hrir_buffers_inlock(oneSofaDataBase* dbase, jvxSize slotId)
{
	assert(slotId < dataBaseSlots.size());
	JVX_SAFE_DELETE_FIELD(dataBaseSlots[slotId].buffer_hrir_left);
	JVX_SAFE_DELETE_FIELD(dataBaseSlots[slotId].buffer_hrir_right);
	dataBaseSlots[slotId].length_buffer_hrir = 0;

	if (dbase)
	{
		dataBaseSlots[slotId].length_buffer_hrir = dbase->length_hrir;
		JVX_SAFE_ALLOCATE_FIELD_CPP_Z(dataBaseSlots[slotId].buffer_hrir_left, float, dataBaseSlots[slotId].length_buffer_hrir);
		JVX_SAFE_ALLOCATE_FIELD_CPP_Z(dataBaseSlots[slotId].buffer_hrir_right, float, dataBaseSlots[slotId].length_buffer_hrir);
	}
}


jvxErrorType 
ayfHrtfDispenser::select_database(jvxSize idx_database, jvxSize slotId)
{
	assert(slotId < dataBaseSlots.size());
	if (idx_database >= sofaDataBases.size()) return JVX_ERROR_ID_OUT_OF_BOUNDS;

	auto sofa_database = sofaDataBases.begin();
	std::advance(sofa_database, idx_database);
	if (*sofa_database != dataBaseSlots[slotId].activeDatabase)
	{
		JVX_LOCK_MUTEX(safeAccess);

		oneSofaDataBase* selectMe = *sofa_database;

		// set selected database

		if (!selectMe->is_open) 
		{
			if (this->samplerate != JVX_SIZE_UNSELECTED) {
				this->load_selected_database_inlock(selectMe, slotId);
			}
		}

		// Here, the database is updated		
		if (dataBaseSlots[slotId].length_buffer_hrir != selectMe->length_hrir) {
			this->allocate_hrir_buffers_inlock(selectMe, slotId);
		}

		dataBaseSlots[slotId].activeDatabase = selectMe;
		std::cout << __FUNCTION__ << ": Sofa database " << dataBaseSlots[slotId].activeDatabase->fName << " selected." << std::endl;

		// inform BinauralRender modules
		for (auto elm : this->registeredListeners) {
			elm->report_database_changed(slotId);
		}

		JVX_UNLOCK_MUTEX(safeAccess);
	}
	return JVX_NO_ERROR;
}


jvxSize
ayfHrtfDispenser::selected_database(jvxSize slotId)
{
	jvxSize cnt = 0;
	assert(slotId < dataBaseSlots.size());
	for (auto sofa_database : sofaDataBases)
	{
		if (sofa_database == dataBaseSlots[slotId].activeDatabase)
		{
			return cnt;
		}
		cnt++;
	}
	return JVX_SIZE_UNSELECTED;
}


jvxErrorType oneSofaDataBase::close()
{
	if (!this->is_open) return JVX_ERROR_WRONG_STATE;

	if (this->mysofa_db) {
		mysofa_close(this->mysofa_db);
		this->mysofa_db = nullptr;
	}

	this->length_hrir = JVX_SIZE_UNSELECTED;
	this->is_open = false;
	return JVX_NO_ERROR;
}


jvxErrorType oneSofaDataBase::open(jvxSize samplerate)
{	
	if (this->is_open) return JVX_ERROR_WRONG_STATE;
	if (this->mysofa_db) return JVX_ERROR_WRONG_STATE;
	if (this->length_hrir != JVX_SIZE_UNSELECTED) return JVX_ERROR_WRONG_STATE;

	int mysofa_error = 0;
	int return_length_hrir = 0;
	this->mysofa_db = mysofa_open(this->fName.c_str(), jvxData(samplerate), &return_length_hrir, &mysofa_error);
	if (this->mysofa_db == nullptr) {
		std::cout << __FUNCTION__ << ": Sofa database " << this->fName << " could not be opened. Function mysofa_open returned error code " << mysofa_error <<"."  << std::endl;
		return JVX_ERROR_INTERNAL;
	}
	
	this->length_hrir = return_length_hrir;
	this->is_open = true;

	return JVX_NO_ERROR;
}


jvxErrorType 
ayfHrtfDispenser::register_change_listener(IjvxPropertyExtenderHrtfDispenser_report* ref)
{
	for (auto elm : registeredListeners)
	{
		if (elm == ref)
		{
			return JVX_ERROR_UNSUPPORTED;
		}
	}

	registeredListeners.push_back(ref);

	return JVX_NO_ERROR;
}

jvxErrorType
ayfHrtfDispenser::unregister_change_listener(IjvxPropertyExtenderHrtfDispenser_report* ref)
{
	auto elm = registeredListeners.begin();
	for (; elm != registeredListeners.end(); elm++)
	{
		if (*elm == ref)
		{
			break;
		}
	}
	if (elm != registeredListeners.end())
	{
		registeredListeners.erase(elm);
		return JVX_NO_ERROR;
	}
	
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType 
ayfHrtfDispenser::is_ready(jvxBool* isReady, jvxApiString* reason)
{
	jvxBool isReadyLocal = true;
	std::string errDescr;

	if (dataBaseSlots.size() == 0)
	{
		isReadyLocal = false;
		errDescr = "No slots declared for HRTF dispenser.";
	}
	else
	{
		if (dataBaseSlots[0].activeDatabase == nullptr)
		{
			isReadyLocal = false;
			errDescr = "No active HRTF database in folder <" + curWorkingDir + ">.";
		}
	}
	if (isReady)
	{
		*isReady = isReadyLocal;
	}
	if (reason)
	{
		*reason = errDescr;
	}
	return JVX_NO_ERROR;
}