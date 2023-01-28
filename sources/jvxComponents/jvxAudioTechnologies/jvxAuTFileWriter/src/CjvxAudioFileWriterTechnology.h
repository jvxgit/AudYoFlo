#ifndef __JVXAUTFILEWRITER_TECHNOLOGY_H__
#define __JVXAUTFILEWRITER_TECHNOLOGY_H__

#include "jvx.h"
#include "jvxTechnologies/CjvxTemplateTechnologyVD.h"
#include "CjvxAudioFileWriterDevice.h"
#include "audio_file_writer_common.h"

#include "pcg_exports_technology.h"

class CjvxAudioFileWriterTechnology : 
	public CjvxTemplateTechnologyVD<CjvxAudioFileWriterDevice>,
	public IjvxConfiguration, public IjvxConfigurationDone,
	public genFileWriter_technology
{
private:

	// We keep a secondary map to store the pointers to the "real" pointers
	std::map<IjvxDevice*, CjvxAudioFileWriterDevice*> lstDevType;

	std::string selectedFile;

	wav_params wav_start_params;

	class oneFileDeviceOnConfig
	{
	public:
		std::string fNamePrefix;
		std::string folder;
		std::string devSpec;
		std::string cfg_compact;
	};
	std::list< oneFileDeviceOnConfig> devsOnConfig;

public:

	JVX_CALLINGCONVENTION CjvxAudioFileWriterTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	JVX_CALLINGCONVENTION ~CjvxAudioFileWriterTechnology();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType return_device(IjvxDevice* dev) override;

	// ===============================================================
	// ===============================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan, IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename = "", jvxInt32 lineno = -1) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION done_configuration() override;

#define JVX_INTERFACE_SUPPORT_CONFIGURATION
#define JVX_INTERFACE_SUPPORT_CONFIGURATION_DONE
#define JVX_INTERFACE_SUPPORT_PROPERTIES
#define JVX_INTERFACE_SUPPORT_BASE_CLASS CjvxTemplateTechnologyVD<CjvxAudioFileWriterDevice>
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_BASE_CLASS
#undef JVX_INTERFACE_SUPPORT_CONFIGURATION
#undef JVX_INTERFACE_SUPPORT_CONFIGURATION_DONE
#undef JVX_INTERFACE_SUPPORT_PROPERTIES

	// =====================================================================
	// For all non-object interfaces, return object reference
	// =====================================================================
	#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
	// =====================================================================
	// =====================================================================


	// ===============================================================
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(control_fileio);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(modify_wav_file_params);

	// ===============================================================

	// ===============================================================
	// ===============================================================
	jvxErrorType activateOneFile(
		const std::string& folder_store,
		const std::string& filename_to_be_opened,
		std::string& devSpec,
		jvxSize& idSelectNew,
		const std::string& cfg_compact = "" );
	void update_local_properties(jvxBool trig_host, jvxSize idSelectNew);
	void trigger_close_file_core(std::pair<IjvxDevice*, CjvxAudioFileWriterDevice*> elm);
	void update_wav_params_from_props();
};

#endif
