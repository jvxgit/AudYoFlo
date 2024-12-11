#ifndef _CJVXGENERICWRAPPERTECHNOLOGY_H__
#define _CJVXGENERICWRAPPERTECHNOLOGY_H__

#include "jvx.h"
#include "jvxAudioTechnologies/CjvxAudioTechnology.h"
#include "jvxTechnologies/CjvxTemplateTechnology.h"
#include "pcg_exports_technology.h"
#include "CjvxGenericWrapper_common.h"
#include "CjvxGenericWrapperDevice.h"

#include "CjvxGenericWrapperTechnology_hostRelocator.h"

#define JVX_MY_BASE_CLASS_T  CjvxTemplateTechnology<CjvxGenericWrapperDevice>
// CjvxAudioTechnology

enum class jvxWavFileType
{
	JVX_WAV_FILE_TYPE_PCM_16BIT = 0,
	JVX_WAV_FILE_TYPE_PCM_24BIT = 1,
	JVX_WAV_FILE_TYPE_PCM_32BIT = 2,
	JVX_WAV_FILE_TYPE_PCM_32BIT_FLOAT = 3,
	// ============================================
	JVX_WAV_FILE_TYPE_PCM_16BIT_RF64 = 4,
	JVX_WAV_FILE_TYPE_PCM_24BIT_RF64 = 5,
	JVX_WAV_FILE_TYPE_PCM_32BIT_RF64 = 6,
	JVX_WAV_FILE_TYPE_PCM_32BIT_FLOAT_RF64 = 7
};

// class CjvxGenericWrapperDevice;

class CjvxGenericWrapperTechnology: public JVX_MY_BASE_CLASS_T, public IjvxConfiguration, 
	public IjvxExternalAudioChannels, public genGenericWrapper_technology
{
	friend CjvxGenericWrapperTechnology_hostRelocator;
	friend CjvxGenericWrapperDevice;


private:
	CjvxGenericWrapperTechnology_hostRelocator techHostRelocator;

	jvxSize inPropCallId;
	struct oneTechnologyElement
	{
		IjvxObject* theObj = nullptr;
		CjvxAudioTechnology* theTech = nullptr;
		std::string name;
		jvxSize id = JVX_SIZE_UNSELECTED;
	} ;

	struct runtimeT
	{
		IjvxToolsHost* theToolsHost = nullptr;
		std::vector<oneTechnologyElement> theTechnologies;	
		std::string entrySelect = "";
		std::string entrySelectDefault = "";
		IjvxTechnology* theSelectedTech = nullptr;
		IjvxProperties* theSelectedTechProps = nullptr;
	} ;

	runtimeT runtime;
	CjvxGenericWrapperFiles theFiles;
	CjvxGenericWrapperExtReferences theExtReferences;
	CjvxGenericWrapperDummyChannels theDummyChannels;

	jvxBool skipConfigAudioTech = false;

	JVX_MUTEX_HANDLE safeFilesReferences;

public:
	CjvxGenericWrapperTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual ~CjvxGenericWrapperTechnology();

	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* theOwner)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename, jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION system_ready() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION register_one_set(jvxBool is_input, jvxSize num_channels, jvxInt32 sRate, jvxSize* register_id, IjvxExternalAudioChannels_data* ref, const char*)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION unregister_one_set(jvxSize register_id)override;


	virtual jvxErrorType JVX_CALLINGCONVENTION set_property(
		jvxCallManagerProperties& callGate, 
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& detail)override;

	jvxErrorType processIssuedStringCommand(std::string command);

	jvxErrorType addInputFile_nolock(std::string name, jvxInt32& uniqueId, jvxInt16& numChannelsAdd, std::string& descr );
	jvxErrorType removeInputFile_nolock(jvxSize idx, jvxInt32& unqiueId);
	jvxErrorType removeInputFile_nolock(std::string& name);

	jvxErrorType addOutputFile_nolock(std::string name, std::string fname, jvxInt32& uniqueId, jvxInt16& numChannelsAdd, jvxInt32 sRate, std::string& descr );
	jvxErrorType removeOutputFile_nolock(jvxSize idx, jvxInt32& unqiueId);
	jvxErrorType removeOutputFile_nolock(std::string& name);

	jvxErrorType updatePropertyToFileLists_nolock(jvxInt32 sRate/* = SAMPLERATE_OUTFILE_START*/, std::vector<std::string>& warning);

	void reportUpdateAllPropertiesOutput();

	jvxErrorType updateInputAdditionalFilenamesPropertyList_nolock();
	jvxErrorType updateOutputAdditionalFilenamesPropertyList_nolock();

	jvxErrorType addFileNameInputProperty_lock(std::string & fname);
	jvxErrorType addFileNameInputProperty_nolock(std::string & fname);

	jvxErrorType addFileNameOutputProperty_lock(std::string & fname);
	jvxErrorType addFileNameOutputProperty_nolock(std::string & fname);

	jvxErrorType updateAllDevicesOtherThan_lock(CjvxGenericWrapperDevice* notme);
	// Lock files
	void lock_files(){JVX_LOCK_MUTEX(safeFilesReferences);};

	// Unlock files
	void unlock_files(){JVX_UNLOCK_MUTEX(safeFilesReferences);};

	jvxErrorType request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)override;

	jvxErrorType return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)override;

	jvxErrorType request_device(jvxSize idx, IjvxDevice** dev, jvxCBitField requestFlags)override;

	CjvxGenericWrapperDevice* local_allocate_device(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE, jvxSize idx, jvxBool actAsProxy_init, jvxHandle* fwd_arg) override;
	jvxErrorType local_deallocate_device(CjvxGenericWrapperDevice** elmDev, jvxHandle* fwd_arg)override;

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(cb_prop_selected_input_file_set);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(cb_prop_selected_input_file_set_pre);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(cb_prop_selected_input_file_get);

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(cb_prop_selected_output_file_set);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(cb_prop_selected_output_file_set_pre);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(cb_prop_selected_output_file_get);

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(cb_prop_selected_technology_set);

#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
};


#endif
