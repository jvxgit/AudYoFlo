#ifndef __AYF_HRTF_DISPENSER_H__
#define __AYF_HRTF_DISPENSER_H__

#include "jvx.h"

#define MYSOFA_EXPORT 
extern "C"
{
#include "mysofa.h"
}

// =========================================================================================
class oneSofaDataBase
{

public:
	// Name of database file to open.
	std::string fName = "none";

	// Number of samples in HRIR. Returned during open.
	jvxSize length_hrir = JVX_SIZE_UNSELECTED;
	
	struct MYSOFA_EASY* mysofa_db = NULL;
	
	// load state of database, tracked by open and close
	bool is_open = false;

	/* Load SOFA database file in path fName with specified samplerate.
		@param samplerate: Sampling rate used to open SOFA database.
	 */

	jvxSize loadId = 0;

	jvxErrorType open(jvxSize samplerate);

	// Unload database and reset values.
	jvxErrorType close();
};

struct oneDBaslot
{
	oneSofaDataBase* activeDatabase = nullptr;
	float* buffer_hrir_left = nullptr;
	float* buffer_hrir_right = nullptr;
	jvxSize length_buffer_hrir = JVX_SIZE_UNSELECTED;
	jvxSize idx = JVX_SIZE_UNSELECTED;
	std::string description;
};

class ayfHrtfDispenser : public IjvxPropertyExtender, public IjvxPropertyExtenderHrtfDispenser
{
private:

	jvxSize loadIdCnt = 1;

	jvxState statDispenser = jvxState::JVX_STATE_NONE;

	/*
	jvxBool is_initialized = false;
	jvxBool started = false;
	*/

	std::string curWorkingDir;
	std::string pathName = "./";
	std::list<oneSofaDataBase*> sofaDataBases;
	
	jvxSize samplerate = JVX_SIZE_UNSELECTED;
	JVX_MUTEX_HANDLE safeAccess;
	
	std::list<IjvxPropertyExtenderHrtfDispenser_report*> registeredListeners;
	std::vector<oneDBaslot> dataBaseSlots;

public: 
	ayfHrtfDispenser();
	~ayfHrtfDispenser();

	// ==============================================================================

	jvxErrorType supports_prop_extender_type(jvxPropertyExtenderType tp) override;
	jvxErrorType prop_extender_specialization(jvxHandle** prop_extender, jvxPropertyExtenderType tp) override;
	
	// ==============================================================================

	jvxErrorType number_slots(jvxSize& num) override;
	jvxErrorType slot_descrption(jvxSize idx, jvxApiString& description)override;
	jvxErrorType bind(jvxSize* samplerate) override;

	// ==============================================================================

	jvxErrorType get_length_hrir(jvxSize& length_hrir, jvxSize* loadId, jvxSize slotId) override;
	jvxErrorType copy_closest_hrir_pair(jvxData azimuth_deg, jvxData inclination_deg, 
		jvxData* hrir_left, jvxData* hrir_right, jvxSize length_hrir, jvxSize slotId) override;
	jvxErrorType get_closest_direction(jvxData& azimuth_deg, jvxData& inclination_deg, jvxSize slotId) override;

	// ==============================================================================

	jvxErrorType register_change_listener(IjvxPropertyExtenderHrtfDispenser_report* ref) override;
	jvxErrorType unregister_change_listener(IjvxPropertyExtenderHrtfDispenser_report* ref) override;

	jvxErrorType is_ready(jvxBool* isReady, jvxApiString* reason) override;

	// ==============================================================================

	jvxErrorType start(const std::string& directory, jvxSize numSlots, const char** slotNames);
	jvxErrorType stop();


	jvxSize num_sofa_files();
	jvxErrorType name_sofa_file(jvxSize idx, std::string& name);

	// Select SOFA database without loading it.
	jvxErrorType select_database(jvxSize idx_database, jvxSize slotId);

	// Load the selected SOFA database.
	jvxErrorType load_selected_database_inlock(oneSofaDataBase* dbase, jvxSize slotId);

	// Return list index of selected SOFA database.
	jvxSize selected_database(jvxSize slotId);

	void allocate_hrir_buffers_inlock(oneSofaDataBase* dbase, jvxSize slotId);

};

#endif