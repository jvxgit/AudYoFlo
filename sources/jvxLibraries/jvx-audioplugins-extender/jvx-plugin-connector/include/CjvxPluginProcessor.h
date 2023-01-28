#ifndef __CJVXPLUGINCONNECTOR_H__
#define __CJVXPLUGINCONNECTOR_H__

#include "jvx.h"
// #include "jvxHCon.h"
#include "HjvxMicroConnection.h"
#include "common/CjvxNegotiate.h"
#include "jvx-helpers-cpp.h"
#include "allHostsStatic_common.h"

#include "TjvxPluginCommon.h"
#include "IjvxPluginCommon.h"

class CjvxPluginProcessor: public HjvxMicroConnection_hooks_simple, public IjvxAudioPluginRuntime
{
protected:
	class oneComponentDescriptor
	{
	public:
		std::string name;
		jvxInitObject_tp funcInit = nullptr;
		jvxTerminateObject_tp funcTerm = nullptr;
		IjvxObject* objNode = nullptr; 
		IjvxNode* theNode = nullptr;
	};

	jvxSize refCountHost = 0;
	IjvxDataConnections* intfCons = nullptr;
	HjvxMicroConnection* theMicroConnection = nullptr;
	CjvxNegotiate_input neg_input;
	CjvxNegotiate_output neg_output;
	jvxLinkDataDescriptor descr_conn_in;
	jvxLinkDataDescriptor descr_conn_out;
	std::list< oneComponentDescriptor> registeredComponents;

	IjvxPropertyExtenderStreamAudioPlugin* theExtender = nullptr;

	IjvxHiddenInterface** lstNodes = nullptr;
	jvxSize numNodes = 0;
	jvx_constCharStringLst icons;
	jvx_constCharStringLst ocons;

	struct
	{
		struct string_set
		{
			std::string val;
			jvxBool set = false;
		};

		string_set systemPath; // "--systemdir"
		string_set userPath; // "--userdir"

		std::string textLog_filename = ".tlog"; // "--textlogfilename"
		std::vector<std::string> textLog_expressions; // "--textlogexpr"
		jvxBool textLog_activate = false; // "--textlog"
		jvxSize textLog_sizeInternBufferFile = 8192; // "--textlogszfile"
		jvxSize textLog_sizeInternBufferRW = 1024; // "--textlogszrw"
		jvxSize textLog_dbglevel = 10; // "--textloglev"
		jvxBool out_cout = false; // "--n_out_cout"
		jvxBool verbose_dll = false; // "--verbose_dll"
		std::vector<std::string> dllsDirectories; // "--jvxdir"
	} host_parameters;

	jvxAudioPluginBinaryStateChange currentState = JVX_AUDIOPLUGIN_STATE_STOPPED;
	
	JVX_MUTEX_HANDLE safeAccessProcessing;
	jvxBool connection_started = false;

public:

	virtual jvxErrorType hook_test_negotiate(jvxLinkDataDescriptor* proposed JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	
	virtual jvxErrorType hook_test_accept(jvxLinkDataDescriptor* dataIn  
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	virtual jvxErrorType hook_forward(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	virtual jvxErrorType hook_test_update(jvxLinkDataDescriptor* dataIn  JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	
	virtual jvxErrorType hook_check_is_ready(jvxBool* is_ready, jvxApiString* astr) override;

	// =====================================================================

	CjvxPluginProcessor();
	~CjvxPluginProcessor();

	// =====================================================================

	jvxErrorType initialize(IjvxPropertyExtenderStreamAudioPlugin* extender);
	jvxErrorType terminate();

	jvxErrorType add_config_text(const std::string& config, const std::string& text);
	jvxErrorType add_config_flag(const std::string& config, jvxBool flag);
	jvxErrorType add_config_value(const std::string& config, jvxSize value);

	jvxErrorType add_component(jvxInitObject_tp init, jvxTerminateObject_tp term, const std::string& nm);
	jvxErrorType clear_components();

	jvxErrorType initialize_connection(jvxSize numPluginInChannels, jvxSize numPluginOutChannels);
	jvxErrorType terminate_connection();

	jvxErrorType update_plugin_parameters(jvxSize bsize, jvxSize srate);

	jvxErrorType start_connection();
	jvxErrorType stop_connection();

	jvxErrorType request_property_reference(IjvxProperties** theProp, const std::string& name);
	jvxErrorType return_property_reference(IjvxProperties* theProp, const std::string& name);

	jvxErrorType configure_host();
	void parameter_reset();
	
	jvxErrorType getConfiguration(std::string& txt);
	jvxErrorType putConfiguration(const std::string& txtOut);

	// ======================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION disengage_processing() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION engage_processing() override;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION try_lock_processing() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION lock_processing() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unlock_processing() override;

	// ======================================================================
	jvxErrorType handle_message_from_controller(
		IjvxPluginMessageControl* respond, 
		jvxAudioPluginBinaryHeader* ptr);

	// ======================================================================
};

#endif