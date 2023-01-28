#include "CjvxCoreControllerDevice.h"

CjvxCoreControllerDevice::CjvxCoreControllerDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxApplicationControllerDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxDevice*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);

	runtime.coreHdl.object = NULL;
	runtime.coreHdl.hdl = NULL;
	runtime.theToolsHost = NULL;
	runtime.subModuleLoaded = false;
}

CjvxCoreControllerDevice::~CjvxCoreControllerDevice()
{
}

	jvxErrorType 
CjvxCoreControllerDevice::activate()
{
	jvxErrorType res = CjvxApplicationControllerDevice::activate();
	jvxErrorType resL = JVX_NO_ERROR;
	if(res == JVX_NO_ERROR)
	{
		resL = _common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&runtime.theToolsHost));
		if((resL == JVX_NO_ERROR) && runtime.theToolsHost)
		{
			resL = runtime.theToolsHost->reference_tool(JVX_COMPONENT_THREADCONTROLLER, &runtime.coreHdl.object, 0, NULL);
			if((resL == JVX_NO_ERROR) && (runtime.coreHdl.object))
			{
				resL = runtime.coreHdl.object->request_specialization(reinterpret_cast<jvxHandle**>(&runtime.coreHdl.hdl), NULL, NULL);
			}
		}

		runtime.subModuleLoaded = false;
		
		if(runtime.coreHdl.hdl)
		{
			resL = runtime.coreHdl.hdl->initialize(_common_set_min.theHostRef);
			if(resL == JVX_NO_ERROR)
			{
				resL = runtime.coreHdl.hdl->configure(50, 1000);
				if(resL == JVX_NO_ERROR)
				{
					resL = runtime.coreHdl.hdl->activate();
					if(resL == JVX_NO_ERROR)
					{
						runtime.subModuleLoaded = true;
					}
				}
			}
		}

		if(!runtime.subModuleLoaded)
		{
			this->_report_text_message("Warning: Failed to load core controller sub module", JVX_REPORT_PRIORITY_WARNING);
		}
	}
	return(res);
}

jvxErrorType 
CjvxCoreControllerDevice::prepare()
{
	jvxErrorType res = CjvxApplicationControllerDevice::prepare();
	if(res == JVX_NO_ERROR)
	{
		if(runtime.subModuleLoaded)
		{
			jvxErrorType resL = runtime.coreHdl.hdl->prepare();
			if(resL != JVX_NO_ERROR)
			{
					this->_report_text_message("Warning: Failed to prepare core controller sub module", JVX_REPORT_PRIORITY_WARNING);
			}
		}
	}
	return(res);
}

jvxErrorType 
CjvxCoreControllerDevice::postprocess()
{
	jvxErrorType res = CjvxApplicationControllerDevice::postprocess();
	if(res == JVX_NO_ERROR)
	{
		if(runtime.subModuleLoaded)
		{
			jvxErrorType resL = runtime.coreHdl.hdl->post_process();
			if(resL != JVX_NO_ERROR)
			{
				this->_report_text_message("Warning: Failed to postprocess core controller sub module", JVX_REPORT_PRIORITY_WARNING);
			}
		}
	}
	return(res);
}

jvxErrorType 
CjvxCoreControllerDevice::deactivate()
{
	jvxErrorType res = CjvxApplicationControllerDevice::deactivate();
	jvxErrorType resL = JVX_NO_ERROR;
	if(res == JVX_NO_ERROR)
	{
		if(runtime.theToolsHost)
		{
	
			if(runtime.coreHdl.hdl)
			{
				resL = runtime.coreHdl.hdl->deactivate();
				resL = runtime.coreHdl.hdl->terminate();
			}
			resL = runtime.theToolsHost->return_reference_tool(JVX_COMPONENT_THREADCONTROLLER, runtime.coreHdl.object);
			resL = _common_set_min.theHostRef->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle*>(runtime.theToolsHost));
		}
		runtime.subModuleLoaded = false;
	}
	return(res);
}
