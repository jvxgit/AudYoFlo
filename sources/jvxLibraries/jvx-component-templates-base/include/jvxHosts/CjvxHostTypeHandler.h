#ifndef __CJVXHOSTTYPEHANDLER_H__
#define __CJVXHOSTTYPEHANDLER_H__

#include "jvx.h"
#include "CjvxHostInteraction.h"

	// >>>>>>>>>>>>>>>>>>>>>>>>

	struct oneTechnology
	{
		jvxOneComponentModuleWrap common;
		IjvxTechnology* theHandle_single; // In case multiple objects are not allowed, always return this, otherwise, always return a new instance
		oneTechnology()
		{
			theHandle_single = NULL;
		}
	} ;

	struct oneExternalTechnology
	{
		IjvxTechnology* theHandle_single; // In case multiple objects are not allowed, always return this, otherwise, always return a new instance
		oneExternalTechnology()
		{
			theHandle_single = NULL;
		}
	};

	struct oneSelectedDevice
	{
		IjvxDevice* dev;
		IjvxConnectorFactory* cfac;
		IjvxConnectionMasterFactory* mfac;
		jvxSize uid = JVX_SIZE_UNSELECTED;
		oneSelectedDevice()
		{
			dev = NULL;
			cfac = NULL;
			mfac = NULL;
		}
	} ;

	struct oneSelectedTechnology
	{
		IjvxTechnology* theHandle_shortcut_tech;
		jvxSize idSel;
		std::string nmExternal;
		jvxSize uid = JVX_SIZE_UNSELECTED;
		std::vector<oneSelectedDevice> theHandle_shortcut_dev;
		jvxSize numSubSlotsMax;
		oneSelectedTechnology()
		{
			theHandle_shortcut_tech = NULL;
			idSel = JVX_SIZE_UNSELECTED;
			numSubSlotsMax = 0;
		}
	};

	struct technologiesT
	{
		std::vector<oneTechnology> availableTechnologies;
		std::vector<oneExternalTechnology> externalTechnologies;
		std::vector<oneSelectedTechnology> selTech;
		jvxSize numSlotsMax;
		jvxSize numSubSlotsMaxDefault;

		// For slotId a number of subslots!
		// This field will be cleared when the hosttype handler is reallocated
		std::map<jvxSize, jvxSize> numSubSlotsMax_spec;
		technologiesT()
		{
			numSlotsMax = 0;
			numSubSlotsMaxDefault = 0;
		};
	};

	// >>>>>>>>>>>>>>>>>>>>>>>>
	// >>>>>>>>>>>>>>>>>>>>>>>>

class CjvxHostTypeHandler: public CjvxHostInteraction
{
protected:
	struct oneDynExtModule
	{
		std::string moduleName;
		std::list<IjvxObject*> associatedExternalComponents;
	};

	struct oneDynExtCfg
	{
		std::string moduleName;
		std::string cfgString;
		std::string fName;
		int lineNo = 0;
		jvxSize refCnt = 0;
	};

	typedef struct
	{
		technologiesT technologyInstances;
		jvxComponentType selector[2];
		jvxComponentTypeClass classType;
		jvxComponentTypeClass childClassType;
		std::string description;
		std::string tokenInConfigFile;
	} oneTechnologyType;

	struct
	{
		std::vector<oneTechnologyType> registeredTechnologyTypes;
		std::vector<oneObjType<IjvxNode>> registeredNodeTypes;
		std::vector<oneObjType<IjvxSimpleComponent>> registeredSimpleTypes;
	} _common_set_types;

	std::map<std::string, oneDynExtModule> extModuleDefinitions;
	std::map<std::string, oneDynExtCfg> extModulesConfigs;

public:

	CjvxHostTypeHandler();

	~CjvxHostTypeHandler();

	// =================================================================================

	jvxErrorType fwd_add_external_component(CjvxObject * meObj,
		IjvxObject * theObj, IjvxGlobalInstance * theGlob, const char* locationDescription, jvxBool allowMultipleInstance,
		jvxInitObject_tp funcInit, jvxTerminateObject_tp funcTerm, jvxComponentType tp) override;

	void fwd_remove_external_component(CjvxObject* meObj, IjvxObject* theObj) override;

	jvxErrorType fwd_load_all_components(jvxComponentType tp,
		jvxHandle* specCompAdd, jvxBool allowMultipleObjectsAdd,
		IjvxObject* newObjAdd, IjvxGlobalInstance* newGlobInstAdd,
		IjvxPackage* thePack, jvxSize thePackIdx,
		const std::string& fileName,
		jvxInitObject_tp funcInitAdd, jvxTerminateObject_tp funcTermAdd,
		JVX_HMODULE dllHandle, jvxBitField dllProps, jvxSize& numAdded) override;

	void fwd_pre_unload_all_components() override;

	void fwd_unload_all_components() override;

	// =================================================================================
	jvxErrorType _component_class(jvxComponentType tp, jvxComponentTypeClass& tpCls);
	jvxErrorType _add_type_host(IjvxObject* theObj, jvxComponentType* tp, jvxSize numtp, const char* description, 
		const char* tokenConfig, jvxComponentTypeClass classType, jvxSize numSlotsMax,
		jvxSize numSubSlotsMax, jvxComponentTypeClass childClassType);

	jvxErrorType _number_types_host(jvxSize* num, jvxComponentTypeClass classType);

	jvxErrorType _description_type_host(jvxSize idx, jvxApiString* theDescription, jvxApiString* theTokenInConfig, jvxComponentType* tp, 
		jvxSize numtp, jvxComponentTypeClass classType);

	jvxErrorType _remove_all_host_types();

	jvxErrorType _add_numsubslots_type_host(const jvxComponentIdentification& tp, jvxSize numSubSlotsMax);

};

#endif
