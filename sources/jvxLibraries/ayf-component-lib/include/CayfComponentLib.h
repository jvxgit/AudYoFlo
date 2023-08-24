#include "common/CjvxObject.h"
#include "common/CjvxConnectionMaster.h"
#include "common/CjvxConnectorMasterFactory.h"
#include "common/CjvxInputOutputConnector.h"
#include "common/CjvxConnectorFactory.h"
#include "common/CjvxNegotiate.h"

#include "common/CjvxProperties.h"

#include "pcg_CayfComponentLib.h"
#include "jvx.h"

enum class ayfHostBindingType
{
	// There is no host at all!!
	AYF_HOST_BINDING_NONE,

	// There is a very minor host binding
	AYF_HOST_BINDING_MIN,

	// There is a full host binding but embedded somewhere
	AYF_HOST_BINDING_EMBEDDED_HOST,

	// There is a full host binding
	AYF_HOST_BINDING_FULL_HOST,
};

struct ayfHostBindingReferences
{
	ayfHostBindingType bindType = ayfHostBindingType::AYF_HOST_BINDING_NONE;

	// An external module is added to host if allowed
	jvxErrorType(*ayf_register_module_host_call)(const char* nm, jvxApiString& nmAsRegistered, IjvxObject* regMe,
		IjvxMinHost** hostOnReturn, IjvxConfigProcessor** cfgOnReturn) = nullptr;

	// Unregister external module
	jvxErrorType(*ayf_unregister_module_host_call)(IjvxObject* regMe) = nullptr;

	// Load config content from host - in most cases due to a postponed load of the module/component
	jvxErrorType(*ayf_load_config_content_call)(IjvxObject* priObj, jvxConfigData** datOnReturn, const char* fName) = nullptr;

	// Release the config content
	jvxErrorType(*ayf_release_config_content_call)(IjvxObject* priObj, jvxConfigData* datOnReturn) = nullptr;

	// Function to attach another component - the secondary component is associated to the first object
	jvxErrorType(*ayf_attach_component_module_call)(const char* nm, IjvxObject* priObj, IjvxObject* attachMe) = nullptr;

	// Detach the previously attached component
	jvxErrorType(*ayf_detach_component_module_call)(const char* nm, IjvxObject* priObj) = nullptr;

	// Forward a text token to the host from where the appropriate action is taken
	jvxErrorType(*ayf_forward_text_command_call)(const char* command, IjvxObject* priObj, jvxApiString& astr) = nullptr;
};

class CayfComponentLib :
	public IjvxNode,
	public CjvxObject,

	// Input connector, output connector
	public IjvxInputConnector, public IjvxOutputConnector, 
	public CjvxInputOutputConnector,

	// master
	public IjvxConnectionMaster, public CjvxConnectionMaster,

	public IjvxProperties, public CjvxProperties,

	public IayfConnectionStateSwitchNode,
	public genComponentLib
{
public:
	struct _oneSubModule
	{
		IjvxObject* obj = nullptr;
		IjvxNode* node = nullptr;
	};

protected:
	std::string regName = "CayfComponentLib";
	std::string chainName = "CayfComponentLibChain";
	std::string mainNodeName = "CayfComponentLibMainNode";
	IjvxObject* mainObj = nullptr;

	IjvxConfigProcessor* confProcHdl = nullptr;
	jvxConfigData* cfgDataInit = nullptr;

protected:
	IjvxNode* mainNode = nullptr;
	ayfHostBindingReferences binding;
private:
	
	enum class ayfTextIoStatus
	{
		AYF_TEXT_IO_STATUS_INIT,
		AYF_TEXT_IO_STATUS_COLLECTING,
		AYF_TEXT_IO_STATUS_RESPONDING,
		AYF_TEXT_IO_STATUS_DONE
	};


	IjvxMinHost* host = nullptr;
	IjvxHiddenInterface* hostRef = nullptr;

	jvxSize uId_process = JVX_SIZE_UNSELECTED;

	struct _procParams
	{
		jvxSize numInChans = 0;
		jvxSize numOutChans = 0;
		jvxSize bSize = 0;
		jvxSize sRate = 0;
		jvxDataFormat format = JVX_DATAFORMAT_NONE;
		jvxDataFormatGroup formGroup = JVX_DATAFORMAT_GROUP_NONE;
		void reset()
		{
			numInChans = 0;
			numOutChans = 0;
			bSize = 0;
			sRate = 0;
			format = JVX_DATAFORMAT_NONE;
			formGroup = JVX_DATAFORMAT_GROUP_NONE;
		}
	};
	_procParams procParams;
	CjvxNegotiate_input neg_input;

	std::string txtMessageCollect;
	std::string txtMessageResponse;
	ayfTextIoStatus txtMessageStatus = ayfTextIoStatus::AYF_TEXT_IO_STATUS_INIT;

public:
	CayfComponentLib(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CayfComponentLib();

	jvxErrorType populateBindingRefs();

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* hostRef)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION terminate()override;

	/*
	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* theObj) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect() override;
	*/

	virtual jvxErrorType JVX_CALLINGCONVENTION activate() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate() override;

	jvxErrorType deployProcParametersStartProcessor(
		jvxSize numInChans, jvxSize numOutChans, 
		jvxSize bSize, jvxSize sRate, 
		jvxDataFormat format, jvxDataFormatGroup formGroup);
	jvxErrorType process_one_buffer_interleaved(
		jvxData* inInterleaved, jvxSize numSamplesIn, jvxSize numChannelsIn, 
		jvxData* outInterleaved, jvxSize numSamlesOut, jvxSize numChannelsOut);
	jvxErrorType stopProcessor();
	
	jvxErrorType add_text_message_token(const std::string& txtIn);
	jvxErrorType new_text_message_status(int value, char* fldRespond, jvxSize szRespond, int* newStatOnReturn);

#define JVX_INPUT_OUTPUT_CONNECTOR_MASTER
#define JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_ICON
#include "codeFragments/simplify/jvxInputOutputConnector_simplify.h"
#undef JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_ICON
#undef JVX_INPUT_OUTPUT_CONNECTOR_MASTER

	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	// =====================================================================
	// Link to default master factory and master implementations
	// =====================================================================
// #include "codeFragments/simplify/jvxConnectorMasterFactory_simplify.h"
	// =====================================================================
	// =====================================================================

	// Callback to report that the system is ready now
	virtual jvxErrorType JVX_CALLINGCONVENTION system_ready() override;

	// Callback to report that the system is about to shutdown
	virtual jvxErrorType JVX_CALLINGCONVENTION system_about_to_shutdown() override;

	// =====================================================================
	// Master interface: Default implementation requires to find current settings
	// =====================================================================
//#define JVX_CONNECTION_MASTER_SKIP_TEST
#include "codeFragments/simplify/jvxConnectionMaster_simplify.h"
//#undef JVX_CONNECTION_MASTER_SKIP_TEST

	// =====================================================================
	// =====================================================================

		// =====================================================================
	// Activate the relevant hidden interfaces
	// =====================================================================
// #define JVX_INTERFACE_SUPPORT_CONNECTOR_FACTORY
// #define JVX_INTERFACE_SUPPORT_CONNECTOR_MASTER_FACTORY
#define JVX_INTERFACE_SUPPORT_PROPERTIES
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_PROPERTIES
// #undef JVX_INTERFACE_SUPPORT_CONNECTOR_MASTER_FACTORY
// #undef JVX_INTERFACE_SUPPORT_CONNECTOR_FACTORY

#define JVX_STATEMACHINE_FULL_SKIP_INIT_TERM
#define JVX_STATE_MACHINE_DEFINE_PREPAREPOSTPROCESS
#define JVX_STATE_MACHINE_DEFINE_STARTSTOP
#define JVX_STATE_MACHINE_DEFINE_READY
#include "codeFragments/simplify/jvxStateMachine_simplify.h"
#undef JVX_STATE_MACHINE_DEFINE_READY
#undef JVX_STATE_MACHINE_DEFINE_PREPAREPOSTPROCESS
#undef JVX_STATE_MACHINE_DEFINE_STARTSTOP
#undef JVX_STATEMACHINE_FULL_SKIP_INIT_TERM

#include "codeFragments/simplify/jvxObjects_simplify.h"

#include "codeFragments/simplify/jvxProperties_simplify.h"
#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"

	virtual jvxErrorType attachAllSubModules(IjvxPropertyExtenderChainControl* propExtSpec) = 0;
	virtual jvxErrorType detachAllSubModules(IjvxPropertyExtenderChainControl* propExtSpec) = 0;
	virtual jvxErrorType allocateMainNode() = 0;
	virtual jvxErrorType deallocateMainNode() = 0;

	jvxErrorType attach_component_module(const std::string& nm, IjvxObject* priObj, IjvxObject* attachMe);
	jvxErrorType detach_component_module(const std::string& nm, IjvxObject* priObj);

	virtual jvxErrorType runStateSwitch(jvxStateSwitch ss, IjvxSimpleNode* node, const char* moduleName, IjvxObject* theOwner) override;
	virtual jvxErrorType componentsAboutToConnect() override;

	jvxErrorType passConfigSection(IjvxSimpleNode* node, const std::string& moduleName);

};
