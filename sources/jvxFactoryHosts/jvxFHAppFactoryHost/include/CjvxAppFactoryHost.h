#ifndef __CJVXAPPFACTORYHOST_H__
#define __CJVXAPPFACTORYHOST_H__

#include "jvx.h"
#include "jvxFactoryHosts/CjvxInterfaceFactory.h"

#define JVX_MINIMUM_HOST_FUNCTIONALITY

#include "jvxHosts/CjvxHostInteractionTools.h"
#include "jvxHosts/CjvxUniqueId.h"

#define SECTIONNAME_ALL_SUBSECTIONS "factoryhost_allSubSections"
#define SECTIONNAME_ALL_EXTERNAL_ENTRIES "factoryhost_allExternalEntries"

#include "jvxHosts/CjvxInterfaceHostTpl.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class CjvxFactoryHost : public CjvxInterfaceHostTpl < IjvxFactoryHost, CjvxHostInteractionTools<CjvxHostInteraction> >
{
public:
	JVX_CALLINGCONVENTION CjvxFactoryHost(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		CjvxInterfaceHostTpl <IjvxFactoryHost, CjvxHostInteractionTools<CjvxHostInteraction> >(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
	{
	};

	virtual JVX_CALLINGCONVENTION ~CjvxFactoryHost()
	{
	};
};


class CjvxAppFactoryHost : public CjvxFactoryHost
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

	JVX_CALLINGCONVENTION CjvxAppFactoryHost(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxAppFactoryHost();

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
	virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)override
	{
		jvxErrorType res = JVX_NO_ERROR;
		res = CjvxHostInteraction::request_hidden_interface(tp, hdl);
		if (res != JVX_NO_ERROR)
		{
			res = JVX_NO_ERROR;
			switch (tp)
			{
			case JVX_INTERFACE_PROPERTIES:
				if (hdl)
				{
					*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxProperties*>(this));
				}
				break;
			case JVX_INTERFACE_TOOLS_HOST:
				if (hdl)
				{
					*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxToolsHost*>(this));
				}
				break;
			case JVX_INTERFACE_UNIQUE_ID:
				if (hdl)
				{
					*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxUniqueId*>(this));
				}
				break;
			case JVX_INTERFACE_CONFIGURATION:
				if (hdl)
				{
					*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxConfiguration*>(this));
				}
				break;
			case JVX_INTERFACE_CONFIGURATION_EXTENDER:
				if (hdl)
				{
					*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxConfigurationExtender*>(this));
				}
				break;
			default:

				res = _request_hidden_interface(tp, hdl);
			}
		}
		return(res);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)override
	{
		jvxErrorType res = JVX_NO_ERROR;
		res = CjvxHostInteraction::return_hidden_interface(tp, hdl);
		if (res != JVX_NO_ERROR)
		{
			res = JVX_NO_ERROR;
			switch (tp)
			{
			case JVX_INTERFACE_PROPERTIES:
				if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxProperties*>(this)))
				{
					res = JVX_NO_ERROR;
				}
				else
				{
					res = JVX_ERROR_INVALID_ARGUMENT;
				}
				break;

			case JVX_INTERFACE_TOOLS_HOST:
				if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxToolsHost*>(this)))
				{
					res = JVX_NO_ERROR;
				}
				else
				{
					res = JVX_ERROR_INVALID_ARGUMENT;
				}
				break;

			case JVX_INTERFACE_UNIQUE_ID:
				if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxUniqueId*>(this)))
				{
					res = JVX_NO_ERROR;
				}
				else
				{
					res = JVX_ERROR_INVALID_ARGUMENT;
				}
				break;

			case JVX_INTERFACE_CONFIGURATION:
				if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxConfiguration*>(this)))
				{
					res = JVX_NO_ERROR;
				}
				else
				{
					res = JVX_ERROR_INVALID_ARGUMENT;
				}
				break;

			case JVX_INTERFACE_CONFIGURATION_EXTENDER:
				if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxConfigurationExtender*>(this)))
				{
					res = JVX_NO_ERROR;
				}
				else
				{
					res = JVX_ERROR_INVALID_ARGUMENT;
				}
				break;
			default:
				res = _return_hidden_interface(tp, hdl);
			}
		}
		return res;
	};

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

