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

jvxErrorType ayfHrtfDispenser::prop_extender_type(jvxPropertyExtenderType* tp)
{
	if (tp)
	{
		*tp = jvxPropertyExtenderType::JVX_PROPERTY_EXTENDER_HRTF_DISPENSER;
	}
	return JVX_NO_ERROR;
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
ayfHrtfDispenser::get_length_hrir(jvxSize& length_hrir, jvxSize* loadIdArg)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (this->activeDatabase) 
	{
		length_hrir = this->activeDatabase->length_hrir;
		if (loadIdArg)
		{
			*loadIdArg = this->activeDatabase->loadId;
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
	return res;
}


jvxErrorType
ayfHrtfDispenser::init(jvxSize* samplerateArg)
{
	if (this->is_initialized)
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
	jvxErrorType res = this->load_selected_database(activeDatabase);
	assert(res == JVX_NO_ERROR);
	
	this->is_initialized = true;
	return res;
}

jvxErrorType
ayfHrtfDispenser::copy_closest_hrir_pair(jvxData azimuth_deg, jvxData inclination_deg, 
	jvxData* hrir_left, jvxData* hrir_right, jvxSize length_hrir, jvxSize dataBaseId)
{
	jvxErrorType res = JVX_NO_ERROR;

	JVX_TRY_LOCK_MUTEX_RESULT_TYPE resW = JVX_TRY_LOCK_MUTEX_NO_SUCCESS;
	JVX_TRY_LOCK_MUTEX(resW, safeAccess);
	//JVX_LOCK_MUTEX(safeAccess);

	if (resW == JVX_TRY_LOCK_MUTEX_SUCCESS)
	{
		if (dataBaseId == this->activeDatabase->loadId)
		{
			if (hrir_left && hrir_right)
			{
				assert(length_hrir == this->activeDatabase->length_hrir);
				assert(length_hrir == this->length_buffer_hrir);
				float hrir_delay_left = 0;          // unit is samples
				float hrir_delay_right = 0;         // unit is samples
				float const elevation_deg = -inclination_deg + 90.0;
				float source_coordinates[3] = { static_cast<float>(azimuth_deg) , static_cast<float>(elevation_deg), (jvxData)2 };
				mysofa_s2c(source_coordinates);
				mysofa_getfilter_float_nointerp(this->activeDatabase->mysofa_db, source_coordinates[0], source_coordinates[1], source_coordinates[2], this->buffer_hrir_left, this->buffer_hrir_right, &hrir_delay_left, &hrir_delay_right);

				// Databases which return delayed HRIRs are not supported.
				if (hrir_delay_left != 0.0 || hrir_delay_right != 0.0) 
				{
					res = JVX_ERROR_UNSUPPORTED;
				}
				else
				{
					for (int idx_sample = 0; idx_sample < this->activeDatabase->length_hrir; idx_sample++)
					{
						hrir_left[idx_sample] = (jvxData)this->buffer_hrir_left[idx_sample];
						hrir_right[idx_sample] = (jvxData)this->buffer_hrir_right[idx_sample];
					}
				}
			}
		}
		else
		{
			res = JVX_ERROR_INVALID_SETTING;
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
ayfHrtfDispenser::get_closest_direction(jvxData& azimuth_deg, jvxData& inclination_deg)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
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
ayfHrtfDispenser::start(const std::string& directory)
{
	char bufRet[MAX_PATH] = { 0 };
	if (started == false)
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

		auto elm = sofaDataBases.begin();
		if (elm != sofaDataBases.end())
		{
			select_database(0);
		}
		started = true;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
ayfHrtfDispenser::stop()
{
	if (started)
	{
		JVX_LOCK_MUTEX(safeAccess);
		
		for (auto sofa_database : sofaDataBases)
		{
			if (activeDatabase->is_open) {
				sofa_database->close();
			}
		}

		activeDatabase = nullptr;
		JVX_UNLOCK_MUTEX(safeAccess);

		for (auto sofa_database : sofaDataBases)
		{
			JVX_DSP_SAFE_DELETE_OBJECT(sofa_database);
		}
		sofaDataBases.clear();
		
		JVX_SAFE_DELETE_FIELD(this->buffer_hrir_left);
		JVX_SAFE_DELETE_FIELD(this->buffer_hrir_right);

		started = false;
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
ayfHrtfDispenser::load_selected_database(oneSofaDataBase* dbase) 
{
	jvxErrorType res = JVX_NO_ERROR;

	if (dbase == nullptr) return res;

	JVX_LOCK_MUTEX(safeAccess);

	if (!dbase->is_open)
	{
		res = dbase->open(this->samplerate);
		dbase->loadId = loadIdCnt++;
		assert(res == JVX_NO_ERROR);
	}

	this->allocate_hrir_buffers(dbase);

	JVX_UNLOCK_MUTEX(safeAccess);

	return res;
}

void
ayfHrtfDispenser::allocate_hrir_buffers(oneSofaDataBase* dbase)
{
	JVX_SAFE_DELETE_FIELD(this->buffer_hrir_left);
	JVX_SAFE_DELETE_FIELD(this->buffer_hrir_right);
	this->length_buffer_hrir = 0;

	this->length_buffer_hrir = dbase->length_hrir;
	JVX_SAFE_ALLOCATE_FIELD_CPP_Z(this->buffer_hrir_left, float, this->length_buffer_hrir);
	JVX_SAFE_ALLOCATE_FIELD_CPP_Z(this->buffer_hrir_right, float, this->length_buffer_hrir);
}


jvxErrorType 
ayfHrtfDispenser::select_database(jvxSize idx_database)
{
	if (idx_database >= sofaDataBases.size()) return JVX_ERROR_ID_OUT_OF_BOUNDS;

	auto sofa_database = sofaDataBases.begin();
	std::advance(sofa_database, idx_database);
	if (*sofa_database != this->activeDatabase)
	{
		JVX_LOCK_MUTEX(safeAccess);

		oneSofaDataBase* selectMe = *sofa_database;

		// set selected database

		if (!selectMe->is_open) 
		{
			if (this->samplerate != JVX_SIZE_UNSELECTED) {
				this->load_selected_database(selectMe);
			}
		}

		// Here, the database is updated		
		if (this->length_buffer_hrir != selectMe->length_hrir) {
			this->allocate_hrir_buffers(selectMe);
		}

		this->activeDatabase = selectMe;
		std::cout << __FUNCTION__ << ": Sofa database " << activeDatabase->fName << " selected." << std::endl;

		// inform BinauralRender modules
		for (auto elm : this->registeredListeners) {
			elm->report_database_changed();
		}

		JVX_UNLOCK_MUTEX(safeAccess);
	}
	return JVX_NO_ERROR;
}


jvxSize
ayfHrtfDispenser::selected_database()
{
	jvxSize cnt = 0;
	for (auto sofa_database : sofaDataBases)
	{
		if (sofa_database == activeDatabase)
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
	if (activeDatabase == nullptr)
	{
		if (isReady)
		{
			*isReady = false;
		}
		if (reason)
		{
			*reason = "No active HRTF database in folder <" + curWorkingDir + ">.";
		}
	}
	return JVX_NO_ERROR;
}