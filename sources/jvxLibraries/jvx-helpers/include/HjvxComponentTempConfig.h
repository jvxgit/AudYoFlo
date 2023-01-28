#ifndef __CJVXCOMPONENTTEMPCONFIG_H__
#define __CJVXCOMPONENTTEMPCONFIG_H__

class CjvxComponentTempConfig
	{
	protected:
		IjvxHost* theRealHost;
		IjvxObject* theConfigProc_obj;
		IjvxConfigProcessor* theConfigProc;
		IjvxHiddenInterface* theHostRef;
		IjvxToolsHost* theToolsHost;
	public:
		CjvxComponentTempConfig();

		jvxErrorType init_tc(IjvxHiddenInterface* theHost);

		jvxErrorType put_configuration_tc(IjvxConfiguration* cfgComp, const char* description_comp);

		jvxErrorType get_configuration_tc(IjvxConfiguration* cfgComp, const char* description_comp);

		jvxErrorType terminate_tc();
	};
#endif
