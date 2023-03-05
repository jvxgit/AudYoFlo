#ifndef __CJVXHOSTTYPEHANDLER_H__
#define __CJVXHOSTTYPEHANDLER_H__

#include "jvx.h"
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
		std::vector<oneSelectedTechnology> selTech;
		jvxSize numSlotsMax;
		jvxSize numSubSlotsMax;
		technologiesT()
		{
			numSlotsMax = 0;
			numSubSlotsMax = 0;
		};
	};

	// >>>>>>>>>>>>>>>>>>>>>>>>

	template <class T> class oneObj
	{
	public:
		jvxOneComponentModuleWrap common;
		T* theHandle_single;// In case multiple objects are not allowed, always return this, otherwise, always return a new instance
		oneObj()
		{
			theHandle_single = NULL;
		};
	} ;

	template <class T> class oneSelectedObj
	{
	public:
		T* obj = nullptr;
		jvxSize idSel = JVX_SIZE_UNSELECTED;
		jvxSize uid = JVX_SIZE_UNSELECTED;
		IjvxConnectorFactory* cfac = nullptr;
		IjvxConnectionMasterFactory* mfac = nullptr;
		oneSelectedObj()
		{
			obj = NULL;
			idSel = JVX_SIZE_UNSELECTED;
			cfac = NULL;
			mfac = NULL;
		};
	} ;

	template <class T> class objT
	{
	public:
		std::vector<oneObj<T>> availableEndpoints;
		std::vector<oneSelectedObj<T>> theHandle_shortcut;
		jvxSize numSlotsMax = 0;
		objT()
		{
			numSlotsMax = 0;
		};
	} ;

	// >>>>>>>>>>>>>>>>>>>>>>>>

class CjvxHostTypeHandler
{
protected:
	typedef struct
	{
		technologiesT technologyInstances;
		jvxComponentType selector[2];
		jvxComponentTypeClass classType;
		jvxComponentTypeClass childClassType;
		std::string description;
		std::string tokenInConfigFile;
	} oneTechnologyType;

	template <typename T> class oneObjType
	{
	public:
		objT<T> instances;
		jvxComponentType selector[1];
		jvxComponentTypeClass classType;
		std::string description;
		std::string tokenInConfigFile;
	} ;
	
	struct
	{
		std::vector<oneTechnologyType> registeredTechnologyTypes;
		std::vector<oneObjType<IjvxNode>> registeredNodeTypes;
		std::vector<oneObjType<IjvxSimpleComponent>> registeredSimpleTypes;
	} _common_set_types;

public:

	CjvxHostTypeHandler();

	~CjvxHostTypeHandler();

	jvxErrorType _component_class(jvxComponentType tp, jvxComponentTypeClass& tpCls);
	jvxErrorType _add_type_host(IjvxObject* theObj, jvxComponentType* tp, jvxSize numtp, const char* description, 
		const char* tokenConfig, jvxComponentTypeClass classType, jvxSize numSlotsMax,
		jvxSize numSubSlotsMax, jvxComponentTypeClass childClassType);

	jvxErrorType _number_types_host(jvxSize* num, jvxComponentTypeClass classType);

	jvxErrorType _description_type_host(jvxSize idx, jvxApiString* theDescription, jvxApiString* theTokenInConfig, jvxComponentType* tp, 
		jvxSize numtp, jvxComponentTypeClass classType);

	jvxErrorType _remove_all_host_types();

};

#endif
