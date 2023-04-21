#ifndef __CAYFAUTOMATIONMODULES_H__
#define __CAYFAUTOMATIONMODULES_H__

#include "jvx.h"

JVX_INTERFACE ayfAutoConnect_callbacks
{
	virtual ~ayfAutoConnect_callbacks() {};
	virtual jvxErrorType adapt_single_property_on_connect(jvxSize purposeId, const std::string& modName, IjvxProperties* props) = 0;
	virtual jvxErrorType allow_master_connect(jvxSize purposeId, jvxComponentIdentification tpId) = 0;

};

class ayfConnectConfigCpEntry
{
public:

	ayfConnectConfigCpEntry(
		jvxComponentType cpTpArg,
		const std::string& modNameArg = "",
		const std::string& oconNmArg = "",
		const std::string& iconNmArg = "",
		const std::string& manSuffixArg = "") :
		cpTp(cpTpArg), modName(modNameArg), oconNm(oconNmArg), iconNm(iconNmArg), manSuffix(manSuffixArg)
	{
	};

	jvxComponentType cpTp = JVX_COMPONENT_UNKNOWN;
	std::string modName;
	std::string oconNm;
	std::string iconNm;
	std::string manSuffix;
};

// 
class ayfConnectConfig
{
public:
	std::list<ayfConnectConfigCpEntry> connectedNodes;
	std::string oconNmMaster;
	std::string iconNmMaster;
	std::string chainNamePrefix = "myChainCfgName";
	jvxSize connectionCategory = JVX_SIZE_UNSELECTED;

	ayfConnectConfig() {};
	ayfConnectConfig(const std::string& chainName, const std::list<ayfConnectConfigCpEntry>& connectedNodesArg,
		const std::string& oconNmMasterArg = "default", const std::string& iconNmMasterArg = "",
		jvxSize connectionCategoryArg = JVX_SIZE_UNSELECTED) :
		chainNamePrefix(chainName), connectedNodes(connectedNodesArg) ,
		oconNmMaster(oconNmMasterArg), iconNmMaster(iconNmMasterArg), connectionCategory(connectionCategoryArg)
	{};
};

class ayfConnectConfigCpEntryRuntime : public ayfConnectConfigCpEntry
{
public:
	jvxComponentIdentification cpId = JVX_COMPONENT_UNKNOWN;
	std::string oconNm;
	std::string iconNm;
	ayfConnectConfigCpEntryRuntime(const ayfConnectConfigCpEntry& cp, 
		const std::string& iconNmArg = "default", 
		const std::string& oconNmArg = "default") :
		ayfConnectConfigCpEntry(cp), cpId(JVX_COMPONENT_UNKNOWN), oconNm(oconNmArg), iconNm(iconNmArg) {};
};

class CayfAutomationModules
{
protected:

	// For each master, store the connected modules
	std::map<jvxComponentIdentification, std::list<ayfConnectConfigCpEntryRuntime> > ids_sub_components_file_in;
	IjvxReport* reportRefPtr = nullptr;
	IjvxHost* refHostRefPtr = nullptr;
	ayfAutoConnect_callbacks* cbPtr = nullptr;
	ayfConnectConfig config;
	jvxSize purposeId = JVX_SIZE_UNSELECTED;
public:

	CayfAutomationModules()
	{
	};

	jvxErrorType activate(IjvxReport* report, 
		IjvxHost* host, 
		ayfAutoConnect_callbacks* cb, 
		jvxSize purpId,
		const ayfConnectConfig& cfg)
	{
		reportRefPtr = report;
		refHostRefPtr = host;
		cbPtr = cb;
		purposeId = purpId;
		config = cfg;
		return JVX_NO_ERROR;
	}

	jvxErrorType deactivate()
	{
		assert(ids_sub_components_file_in.size() == 0);
		// If we end the program, we may reach here without explicitely deactivatin a single chain.
		// The reason is that we disconnect automation before deactivating non-default components
		/*
		for (auto& elm : ids_sub_components_file_in)
		{
			deactivate_all_submodule_audio_input(elm.first);
		}
		*/

		reportRefPtr = nullptr;
		refHostRefPtr = nullptr;
		cbPtr = nullptr;
		purposeId = JVX_SIZE_UNSELECTED;
		config.chainNamePrefix.clear();
		config.connectedNodes.clear();
		return JVX_NO_ERROR;
	}

	void try_connect(
		jvxComponentIdentification master,
		const std::string& prefixChain,
		const std::string& oconNmMaster,
		const std::string& iconNmMaster,
		jvxSize connectionCategory);
	jvxErrorType activate_all_submodules(const jvxComponentIdentification& tp_activated);
	void deactivate_all_submodules(const jvxComponentIdentification& tp_deactivated);
	void adapt_all_submodules(const std::string& modName, jvxComponentIdentification tpCp);
};

#endif
