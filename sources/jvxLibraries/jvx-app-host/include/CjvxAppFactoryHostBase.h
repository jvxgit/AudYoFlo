#ifndef __CJVXAPPHOSTFACTORYBASE_H__
#define __CJVXAPPHOSTFACTORYBASE_H__

#include "jvx.h"
#include "commandline/CjvxCommandLine.h"
#include "allHostsStatic_common.h"
#include "interfaces/all-hosts/configHostFeatures_common.h"

#include "CjvxRequestCommandsHandler.h"

#ifdef JVX_HOST_USE_ONLY_STATIC_OBJECTS
#define JVX_APP_FACTORY_HOST_CLASSNAME CjvxAppFactoryHostBase_nd
#else
#define JVX_APP_FACTORY_HOST_CLASSNAME CjvxAppFactoryHostBase
#endif

class JVX_APP_FACTORY_HOST_CLASSNAME
{
protected:

	struct
	{
		std::string configFilename;
		std::string configFilename_ovlay;
		std::string changeWorkDir;
		std::string textLog_filename;
		std::vector<std::string> textLog_expressions;
		jvxBool textLog_activate;
		jvxSize textLog_sizeInternBufferFile;
		jvxSize textLog_sizeInternBufferRW;
		jvxSize textLog_dbglevel;
		jvxCBool textLog_outCout;
		jvxBool out_cout;
		jvxBool verbose_dll;
		jvxBool verbose_slots;
		std::vector<std::string> dllsDirectories;
		std::list<std::string> configDirectories;
		std::string systemPath;
		std::string userPath;
	} _command_line_parameters;

	struct
	{
		struct
		{
			IjvxObject* hobject;
			IjvxFactoryHost* hFHost;
		} theHost;

		struct
		{
			IjvxToolsHost* hTools;
		} theTools;

		std::vector<oneAddedStaticComponent> addedStaticObjects;
	} involvedComponents;

	IjvxReport* report;
	IjvxReportOnConfig* report_on_config;
	IjvxReportStateSwitch* report_on_state_switch;
	jvxState bootState;
	jvxBool verbose;
	IjvxCommandLine* command_line;
	IjvxBootSteps* boot_steps;

	IjvxSectionOriginList* lstSectionsFile = nullptr;

	std::string cfg_filename_in_use;
	std::string cfg_filename_olay_in_use;

	CjvxRequestCommandsHandler reqHandle;

public:
	JVX_APP_FACTORY_HOST_CLASSNAME();
	~JVX_APP_FACTORY_HOST_CLASSNAME();

	virtual jvxErrorType boot_configure(jvxApiString* errorMessage, IjvxCommandLine* comLine, IjvxReport* report_ptr, 
		IjvxReportOnConfig* report_on_config_ptr, IjvxReportStateSwitch* report_on_state_switch, jvxHandle* context);
	virtual jvxErrorType shutdown_unconfigure(jvxApiString* errorMessage, jvxHandle* context);
	//virtual jvxErrorType shutdown_dropconfig(jvxApiString* errorMessage, jvxHandle* context);

	virtual jvxErrorType boot_initialize(jvxApiString* errorMessage, jvxHandle* context);
	virtual jvxErrorType shutdown_terminate(jvxApiString* errorMessage, jvxHandle* context);

	jvxErrorType boot_activate(jvxApiString* errorMessage, jvxHandle* context);
	jvxErrorType shutdown_deactivate(jvxApiString* errorMessage, jvxHandle* context);

	jvxErrorType boot_prepare(jvxApiString* errorMessage, jvxHandle* context);
	jvxErrorType shutdown_postprocess(jvxApiString* errorMessage, jvxHandle* context);

	jvxErrorType boot_start(jvxApiString* errorMessage, jvxHandle* context);
	jvxErrorType shutdown_stop(jvxApiString* errorMessage, jvxHandle* context);

	virtual jvxErrorType openHostFactory(jvxApiString* errorMessage, jvxHandle* context);
	virtual jvxErrorType closeHostFactory(jvxApiString* errorMessage, jvxHandle* context);
	virtual jvxErrorType shutdownHostFactory(jvxApiString* errorMessage, jvxHandle* context);

	// Entries for higher level functions

	virtual void command_line_specify_specific(IjvxCommandLine* cmdLine);	
	virtual void command_line_read_specific(IjvxCommandLine* cmdLine);

	virtual jvxErrorType boot_initialize_specific(jvxApiString* errloc) = 0;
	virtual jvxErrorType boot_specify_slots_specific() = 0;
	virtual jvxErrorType boot_prepare_specific(jvxApiString* errloc) = 0;
	virtual jvxErrorType boot_start_specific(jvxApiString* errloc) = 0;
	virtual jvxErrorType boot_activate_specific(jvxApiString* errloc) = 0;

	virtual jvxErrorType shutdown_terminate_specific(jvxApiString* errloc) = 0;
	virtual jvxErrorType shutdown_deactivate_specific(jvxApiString* errloc) = 0;
	virtual jvxErrorType shutdown_postprocess_specific(jvxApiString* errloc) = 0;
	virtual jvxErrorType shutdown_stop_specific(jvxApiString* errloc) = 0;

	virtual void boot_prepare_host_start();
	virtual void boot_prepare_host_stop();
	virtual void shutdown_postprocess_host();

	virtual void report_text_output(const char* txt, jvxReportPriority prio, jvxHandle* context) = 0;
	virtual void inform_config_read_start(const char* nmfile, jvxHandle* context) = 0;
	virtual void inform_config_read_stop(jvxErrorType resRead, const char* nmfile, jvxHandle* context) = 0;

	virtual void get_configuration_specific_ext(jvxCallManagerConfiguration* callConf,
		IjvxConfigurationExtender* ext) = 0;
	virtual void get_configuration_specific(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* cfg, jvxConfigData* data) = 0;
	virtual void put_configuration_specific(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* cfg, jvxConfigData* data, const char* fName) = 0;
	virtual void put_configuration_specific_ext(jvxCallManagerConfiguration* callConf,
		IjvxConfigurationExtender* ext) = 0;

	virtual void report_config_file_read_successful(jvxCallManagerConfiguration* callConf, const char* fName) = 0;

	virtual void init_command_line_parameters(IjvxCommandLine* commLine);
	virtual void register_command_line_parameters(IjvxCommandLine* commLine);
	virtual void read_command_line_parameters(IjvxCommandLine* commLine);

	virtual void static_load_loop();
	virtual void static_unload_loop();

protected:

	jvxErrorType configureFromFile(jvxCallManagerConfiguration* callConf, 
		const std::string& fName, 
		std::list<std::string>& filepaths,
		jvxHandle* context);
	jvxErrorType configureComplete();

	jvxErrorType configureToFile(jvxCallManagerConfiguration* callConf, 
		const std::string& fName, jvxBool forceFlat = false, const std::string& fName_old = "");
	virtual void resetReferences();
};

#if 0

#include "jvx.h"
#include "codeFragments/commandline/CjvxCommandline.h"
#include "allHostsStatic_common.h"

#ifdef JVX_HOST_USE_ONLY_STATIC_OBJECTS
#define JVX_APPHOST_FACTORY_CLASSNAME CjvxAppHostFactoryBase_nd
#else
#define JVX_APPHOST_FACTORY_CLASSNAME CjvxAppHostFactoryBase
#endif

class JVX_APPHOST_FACTORY_CLASSNAME : public CjvxCommandline
{
protected:

	struct
	{
		struct
		{
			IjvxObject* hobject;
			IjvxHost* hHost;
		} theHost;

		struct
		{
			IjvxToolsHost* hTools;
		} theTools;

		std::vector<oneAddedStaticComponent> addedStaticObjects;
	} involvedComponents;

	IjvxReport* report;
	IjvxReportOnConfig* report_on_config;
	jvxState bootState;
	jvxBool verbose;

public:
	JVX_APPHOST_CLASSNAME();
	~JVX_APPHOST_CLASSNAME();
	
	jvxErrorType boot_configure(jvxApiString* errorMessage, char* clStr[], int clNum, IjvxReport* report_ptr, IjvxReportOnConfig* report_on_config_ptr, jvxHandle* context);
	jvxErrorType boot_initialize(jvxApiString* errorMessage, jvxHandle* context);
	jvxErrorType boot_activate(jvxApiString* errorMessage, jvxHandle* context);
	jvxErrorType boot_prepare(jvxApiString* errorMessage, jvxHandle* context);
	jvxErrorType boot_start(jvxApiString* errorMessage, jvxHandle* context);
	
	jvxErrorType shutdown_stop(jvxApiString* errorMessage, jvxHandle* context);
	jvxErrorType shutdown_postprocess(jvxApiString* errorMessage, jvxHandle* context);
	jvxErrorType shutdown_deactivate(jvxApiString* errorMessage, jvxHandle* context);
	jvxErrorType shutdown_terminate(jvxApiString* errorMessage, jvxHandle* context);
	jvxErrorType shutdown_dropconfig(jvxApiString* errorMessage, jvxHandle* context);

	// Entries for higher level functions
	virtual jvxErrorType boot_initialize_specific(jvxApiString* errloc) = 0;
	virtual jvxErrorType boot_prepare_specific(jvxApiString* errloc) = 0;
	virtual jvxErrorType boot_start_specific(jvxApiString* errloc) = 0;
	virtual jvxErrorType boot_activate_specific(jvxApiString* errloc) = 0;

	virtual jvxErrorType shutdown_terminate_specific(jvxApiString* errloc) = 0;
	virtual jvxErrorType shutdown_deactivate_specific(jvxApiString* errloc) = 0;
	virtual jvxErrorType shutdown_postprocess_specific(jvxApiString* errloc) = 0;
	virtual jvxErrorType shutdown_stop_specific(jvxApiString* errloc) = 0;


	virtual void report_text_output(const char* txt, jvxReportPriority prio, jvxHandle* context) = 0;
	virtual void inform_config_read_start(const char* nmfile, jvxHandle* context) = 0;
	virtual void inform_config_read_stop(jvxErrorType resRead, const char* nmfile, jvxHandle* context) = 0;

	virtual void get_configuration_specific_ext(IjvxConfigurationExtender* ext, jvxFlagTag flagtag) = 0;
	virtual void get_configuration_specific(IjvxConfigProcessor* cfg, jvxConfigData* data, jvxFlagTag flagtag) = 0;
	virtual void put_configuration_specific(IjvxConfigProcessor* cfg, jvxConfigData* data, jvxFlagTag flagtag, const char* fName) = 0;
	virtual void put_configuration_specific_ext(IjvxConfigurationExtender* ext, jvxFlagTag flagtag) = 0;

	virtual void report_config_file_read_successful(const char* fName, jvxFlagTag flagtag) = 0;

protected:
	jvxErrorType configureFromFile(const std::string& fName, jvxFlagTag flagtag = (jvxFlagTag)0);
	jvxErrorType configureToFile(const std::string& fName, jvxFlagTag flagtag = (jvxFlagTag)0);

private:
	void resetReferences();
};
#endif
#endif
