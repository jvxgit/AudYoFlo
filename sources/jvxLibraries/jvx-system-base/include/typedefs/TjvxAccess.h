#ifndef __TJVXACCESS_H__
#define __TJVXACCESS_H__

// Entry functions in DLl objects
typedef jvxErrorType JVX_CALLINGCONVENTION (*jvxInitObject_tp)(IjvxObject**, IjvxGlobalInstance**, IjvxObject* objTemplate);
typedef jvxErrorType JVX_CALLINGCONVENTION (*jvxTerminateObject_tp)(IjvxObject*);
typedef jvxErrorType JVX_CALLINGCONVENTION (*jvxDllProperties_tp)(jvxBitField*);

#define JVX_OPEN_OBJECT_DLL "jvxInitObject"
#define JVX_CLOSE_OBJECT_DLL "jvxTerminateObject"
#define JVX_PROPERTIES_DLL "jvxDllProperties"

struct jvxOneComponentModuleWrap
{
	jvxSize id;
	bool isExternalComponent;

	jvxComponentType tp;
	jvxBool allowsMultiObjects;
	IjvxObject* hObject;
	IjvxGlobalInstance* hGlobInst;
	jvxBitField dllProps;
	//jvxBool block_state_change;

	struct
	{
		std::string description;
	} externalLink;

	struct
	{
		JVX_HMODULE sharedObjectEntry;
		std::string dllPath;
		jvxInitObject_tp funcInit;
		jvxTerminateObject_tp funcTerm;
	} linkage;
	jvxOneComponentModuleWrap()
	{
		isExternalComponent = false;
		id = JVX_SIZE_UNSELECTED;
		// dllPath
		linkage.sharedObjectEntry = NULL;
		linkage.funcInit = NULL;
		linkage.funcTerm = NULL;
		tp = JVX_COMPONENT_UNKNOWN;
		allowsMultiObjects = false;
		// externalLink.description
		dllProps.bit_fclear();
		hObject = NULL;
		hGlobInst = NULL;
		//block_state_change = false;
	};
};

struct jvxOneComponentModule
{
	jvxOneComponentModuleWrap common;
	IjvxObject* theObj_single;
	IjvxGlobalInstance* theGlob_single;
	int refCount;
	std::vector<IjvxObject*> leased;
	jvxOneComponentModule()
	{
		theObj_single = NULL;
		theGlob_single = NULL;
		refCount = 0;
		// leased
	}
};
	
#endif
