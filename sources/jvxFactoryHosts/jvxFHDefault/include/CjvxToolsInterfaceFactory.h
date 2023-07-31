#ifndef __CJVXTOOLSINTERFACEFACTORY_H__
#define __CJVXTOOLSINTERFACEFACTORY_H__

#include "jvx.h"
#include "jvxFactoryHosts/CjvxInterfaceFactory.h"

#define JVX_MINIMUM_HOST_FUNCTIONALITY

#include "jvxHosts/CjvxHostInteractionTools.h"
#include "jvxHosts/CjvxUniqueId.h"

#define SECTIONNAME_ALL_SUBSECTIONS "factoryhost_allSubSections"
#define SECTIONNAME_ALL_EXTERNAL_ENTRIES "factoryhost_allExternalEntries"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class CjvxToolsInterfaceFactory : public CjvxInterfaceFactory<IjvxFactoryHost>,
		public CjvxHostInteractionTools<CjvxHostInteraction>,
		public IjvxConfiguration, public IjvxConfigurationExtender
{
private:

	typedef struct
	{
		std::string selector;
		std::string assignmentString;
		jvxValue assignmentValue;
		std::vector<std::string> assignmentStringList;
		std::vector<std::vector<jvxValue> > assignmentValueMatrix;
		jvxConfigSectionTypes tp;
	} oneExternalConfigEntry;

	typedef struct
	{
		std::string selector;
		IjvxConfigurationExtender_report* ref;
	} oneExternalConfigHook;

	std::vector<oneExternalConfigEntry> externalConfigEntries;
	std::vector<oneExternalConfigHook> externalConfigHooks;
	
	jvxSize myUniqueId;

public:

	// ===================================================================================================
	// ===================================================================================================

	JVX_CALLINGCONVENTION CjvxToolsInterfaceFactory(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxToolsInterfaceFactory();

	// ===================================================================================================
	// Interface <IjvxStateMachine>
	// ===================================================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* theOtherHost) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* theOwner) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION activate() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION terminate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION system_ready() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION system_about_to_shutdown() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION owner(IjvxObject** theOwner) override;


	// ===================================================================================================
	// Interface <IjvxHiddenInterface>
	// ===================================================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)override;

	// ===================================================================================================
	// Interface <IjvxToolsHost>
	// ===================================================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION number_tools(const jvxComponentIdentification&, jvxSize* num)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION identification_tool(const jvxComponentIdentification&, jvxSize idx, jvxApiString* description, jvxApiString* descriptor, jvxBool* mulInst)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_tool(const jvxComponentIdentification&, IjvxObject** theObject, jvxSize filter_id, const char* filter_descriptor, 
		jvxBitField filter_stateMask = (jvxBitField)JVX_STATE_DONTCARE, IjvxReferenceSelector* decider = nullptr) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_tool(const jvxComponentIdentification&, IjvxObject* theObject) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION instance_tool(jvxComponentType, IjvxObject** theObject, jvxSize filter_id, const char* filter_descriptor) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_instance_tool(jvxComponentType, IjvxObject* theObject, jvxSize filter_id, const char* filter_descriptor) override;

	// ===================================================================================================
	// Interface <IjvxConfiguration>
	// ===================================================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename = "", jvxInt32 lineno = -1)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)override;

	// ===================================================================================================
	// Interface <IjvxConfigurationExtender>
	// ===================================================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_configuration_entries()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION set_configuration_entry(const char* entryname, jvxHandle* fld, jvxConfigSectionTypes tp, jvxSize id)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration_entry(const char* entryname, jvxHandle* fld, jvxConfigSectionTypes* tp, jvxSize id)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION register_extension(IjvxConfigurationExtender_report* bwd, const char* sectionName)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unregister_extension(const char* sectionName)override;

	// ===================================================================================================
#include "codeFragments/simplify/jvxHostInteraction_simplify.h"
#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"

};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif


#endif

