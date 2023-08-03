#ifndef __CJVXAPPHOSTJVX_H__
#define __CJVXAPPHOSTJVX_H__

#include "jvxHosts/CjvxFullHost.h"
#include "common/CjvxConfigurationLine.h"
#include "common/CjvxPropertyPool.h"

class CjvxAppHost :
	public IjvxConfigurationDone, public IjvxConfigurationLine,
	public IjvxConfigurationAttach,
	public IjvxPropertyPool, public CjvxPropertyPool,
	public CjvxFullHost, public CjvxConfigurationLine
{
protected:
	
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
	
	class CjvxConfigurationSubModule
	{
		public:
		std::string prefix;
		IjvxConfiguration* cfgRef = nullptr;
	};

	std::list<CjvxConfigurationSubModule> registeredConfigSubmodules;
public:

	CjvxAppHost(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	// ====================================================================================
	// Interface IjvxStateMachine
	// ====================================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION activate() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate() override;

    // ====================================================================================
	// Interface IjvxConfiguration
	// ====================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename = "", jvxInt32 lineno = -1) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections) override;
	
	// ====================================================================================
	// Interface IjvxConfigurationDone
	// ====================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION done_configuration() override;

	// ====================================================================================
	// Interface IjvxHiddenInterfacet
	// ====================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION object_hidden_interface(IjvxObject** objRef) override;

	// ====================================================================================
	// Interface: IjvxConfigurationLine
	// ====================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION number_lines(jvxSize* num_lines)  override;
	virtual jvxErrorType JVX_CALLINGCONVENTION master_line(jvxSize idxLine, jvxComponentIdentification* tp_master) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION add_line(const jvxComponentIdentification& tp_master, const char* description) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION add_slave_line(const jvxComponentIdentification& tp_master, 
		const jvxComponentIdentification& tp_slave) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_slave_line(const jvxComponentIdentification& tp_master, 
		const jvxComponentIdentification& tp_slave) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION number_slaves_line(const jvxComponentIdentification& tp_master, 
		jvxSize* num_lines) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION slave_line_master(const jvxComponentIdentification& tp_master, jvxSize idxEntry, 
		jvxComponentIdentification* tp_slave) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION add_configuration_property_line(const jvxComponentIdentification& tp_master, 
		jvxSize system_property_id) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_configuration_property_line(const jvxComponentIdentification& tp_master, 
		jvxSize system_property_id) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION number_property_ids_line(const jvxComponentIdentification& tp_master, jvxSize* num_lines) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION property_id_line_master(const jvxComponentIdentification& tp_master, 
		jvxSize idxEntry, jvxSize* the_prop_id) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_property_was_set(const jvxComponentIdentification& tpFrom, jvxPropertyCategoryType cat, 
		jvxSize propIdx,
		jvxBool onlyContent, const jvxComponentIdentification& tpTo) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION refresh_all() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_line(const jvxComponentIdentification& tp_master) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_all_lines()  override;
	  
	// ===============================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION number_system_properties(jvxSize* num_properties)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION system_property_id(jvxSize id, jvxSize* id_prop, jvxApiString* astr) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION number_entries_pool_property(jvxSize id_prop, jvxSize* num_pool_associations) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION entry_pool_property(jvxSize id_prop, jvxSize id_prop_entry,
		jvxComponentIdentification* tp, jvxBool* isMaster) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION add_system_property(jvxSize id_prop, jvxDataFormat format, const char* descr) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION add_entry_system_property(jvxSize id_prop, jvxComponentIdentification tp, jvxBool isMaster) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_entries_system_property(jvxSize id_prop) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_system_property(jvxSize id_prop) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_property_changed(
		jvxSize propId, jvxPropertyCategoryType category, jvxBool contentOnly,
		jvxSize offsetStart, jvxSize numElements,
		jvxComponentIdentification tpOrigin)override;
    
	// ===========================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION attach_configuration_submodule(const char* prefix, IjvxConfiguration* props) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION detach_configuration_submodule(IjvxConfiguration* props) override;

	// ===========================================================================================

	// Interface IjvxHost

    virtual jvxErrorType JVX_CALLINGCONVENTION activate_selected_component(const jvxComponentIdentification&) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION clear_configuration_entries() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_configuration_entry(const char* entryname, jvxHandle* fld, jvxConfigSectionTypes tp, jvxSize id) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration_entry(const char* entryname, jvxHandle* fld, jvxConfigSectionTypes* tp, jvxSize id) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION register_extension(IjvxConfigurationExtender_report* bwd, const char* sectionName) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION unregister_extension(const char* sectionName) override;

#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
};

#endif