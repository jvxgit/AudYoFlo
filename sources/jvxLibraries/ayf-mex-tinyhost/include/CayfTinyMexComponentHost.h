#ifndef __CAYFTINYMEXCOMPONENTHOST_H__
#define __CAYFTINYMEXCOMPONENTHOST_H__
#include "jvx.h"

#include "localMexIncludes.h"
#include "CjvxCToMatlabConverter.h"
#include "CjvxPropertiesToMatlabConverter.h"

class CayfTinyMexComponentHost : 
	public CjvxCToMatlabConverter, public CjvxPropertiesToMatlabConverter,
	public IjvxHiddenInterface, public IjvxToolsHost
{
protected:

	jvxBool isTech = true;
	IjvxObject* theObjectMain = nullptr;
	IjvxTechnology* theTech = nullptr;
	IjvxDevice* theDev = nullptr;
	jvx_propertyReferenceTriple theTriple;

	IjvxObject* theGlobalConfigProcessor = nullptr;

#ifdef JVX_OS_WINDOWS
	jvxBool consoleAttached = false;
#endif

public:
	CayfTinyMexComponentHost();
	~CayfTinyMexComponentHost();
	void shutdownCore();

	void command(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);

	void print_usage();
	jvxErrorType init_component(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[], jvxBool& dedicatedReturn, int offsetlhs = 0, int offsetrhs = 0);
	jvxErrorType term_component(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[], jvxBool& dedicatedReturn, int offsetlhs = 0, int offsetrhs = 0);
	jvxErrorType props_component(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[], jvxBool& dedicatedReturn, int offsetlhs = 0, int offsetrhs = 0);
	jvxErrorType get_props_component(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[], jvxBool& dedicatedReturn, int offsetlhs = 0, int offsetrhs = 0);
	jvxErrorType set_props_component(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[], jvxBool& dedicatedReturn, int offsetlhs = 0, int offsetrhs = 0);

	// Interface == IjvxHiddenInterface ==
	jvxErrorType request_hidden_interface(jvxInterfaceType, jvxHandle**) override;
	jvxErrorType return_hidden_interface(jvxInterfaceType, jvxHandle*) override;
	jvxErrorType object_hidden_interface(IjvxObject** objRef) override;

	// Interface == IjvxToolsHost ==
	jvxErrorType number_tools(const jvxComponentIdentification&, jvxSize* num) override;
	jvxErrorType identification_tool(const jvxComponentIdentification&, jvxSize idx, jvxApiString* description, jvxApiString* descriptor, jvxBool* multipleInstances) override;
	jvxErrorType reference_tool(const jvxComponentIdentification&, IjvxObject** theObject,
		jvxSize filter_id, const char* filter_descriptor, jvxBitField filter_stateMask = (jvxBitField)JVX_STATE_DONTCARE,
		IjvxReferenceSelector* decider = nullptr) override;
	jvxErrorType return_reference_tool(const jvxComponentIdentification&, IjvxObject* theObject) override;
	jvxErrorType instance_tool(jvxComponentType, IjvxObject** theObject, jvxSize filter_id, const char* filter_descriptor) override;
	jvxErrorType return_instance_tool(jvxComponentType tp, IjvxObject* theObject, jvxSize filter_id, const char* filter_descriptor) override;

	jvxErrorType request_reference_object(IjvxObject** obj) override;
	jvxErrorType return_reference_object(IjvxObject* obj) override;

	virtual void init_tech_component() = 0;
	virtual void term_tech_component() = 0;


};

#endif