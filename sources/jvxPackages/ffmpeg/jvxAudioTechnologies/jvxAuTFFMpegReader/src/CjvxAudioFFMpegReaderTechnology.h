#ifndef __JVXAUTFILEREADER_TECHNOLOGY_H__
#define __JVXAUTFILEREADER_TECHNOLOGY_H__

#include "jvx.h"
#include "jvxTechnologies/CjvxTemplateTechnologyVD.h"
#include "CjvxAudioFFMpegReaderDevice.h"
#include "pcg_exports_technology.h"

class CjvxAudioFFMpegReaderTechnology : 
	public CjvxTemplateTechnologyVD<CjvxAudioFFMpegReaderDevice>,
	public IjvxConfiguration, public IjvxConfigurationDone,
	public genFileReader_technology
{
private:

	// We keep a secondary map to store the pointers to the "real" pointers
	std::map<IjvxDevice*, CjvxAudioFFMpegReaderDevice*> lstDevType;

	std::list<std::string> fNamesOnConfig;
	std::string selectedFile;
public:

	JVX_CALLINGCONVENTION CjvxAudioFFMpegReaderTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	JVX_CALLINGCONVENTION ~CjvxAudioFFMpegReaderTechnology();

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
#define JVX_INTERFACE_SUPPORT_BASE_CLASS CjvxTemplateTechnologyVD<CjvxAudioFFMpegReaderDevice>
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
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(modify_fileio);
	// ===============================================================

	// ===============================================================
	// ===============================================================
	jvxErrorType activateOneFile(
		const std::string& filename_to_be_opened,
		std::string& devSpec,
		jvxSize& idSelectNew);
	void update_local_properties(jvxBool trig_host, jvxSize idSelectNew);
	void trigger_close_file_core(std::pair<IjvxDevice*, CjvxAudioFFMpegReaderDevice*> elm);
};

#endif
