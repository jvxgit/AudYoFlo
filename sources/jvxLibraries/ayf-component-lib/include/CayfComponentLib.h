#include "common/CjvxObject.h"
#include "common/CjvxConnectionMaster.h"
#include "common/CjvxConnectorMasterFactory.h"
#include "common/CjvxInputOutputConnector.h"
#include "common/CjvxConnectorFactory.h"
#include "common/CjvxNegotiate.h"

#include "common/CjvxProperties.h"

#include "pcg_CayfComponentLib.h"
#include "jvx.h"

#include "ayf-embedding-proxy.h"
#include "ayf-embedding-proxy-entries.h"

class myLocalHost;
	
JVX_INTERFACE IjvxComponentLib
{
public:
	virtual ~IjvxComponentLib() {};

	virtual jvxErrorType deployProcParametersStartProcessor(
		jvxSize numInChans, jvxSize numOutChans,
		jvxSize bSize, jvxSize sRate,
		jvxDataFormat format, jvxDataFormatGroup formGroup) = 0;
	virtual jvxErrorType process_one_buffer_interleaved(
		jvxData* inInterleaved, jvxSize numSamplesIn, jvxSize numChannelsIn,
		jvxData* outInterleaved, jvxSize numSamlesOut, jvxSize numChannelsOut) = 0;
	virtual jvxErrorType stopProcessor() = 0;
};

class CjvxComponentLibContainer
{
private:
	JVX_MUTEX_HANDLE safeAccess;
	jvxSize numInChans = 0;
	jvxSize numOutChans = 0;
	jvxSize bSize = 0;
	jvxSize sRate = 0;
	jvxDataFormat format = JVX_DATAFORMAT_NONE;
	jvxDataFormatGroup formGroup = JVX_DATAFORMAT_GROUP_NONE;

public:

	IjvxComponentLib* proc = nullptr;
	jvxHandle* procClass = nullptr;
	ayfHostBindingReferences* bindRefs = nullptr;
	
	IjvxExternalModuleFactory* fac = nullptr;

	CjvxComponentLibContainer()
	{
		JVX_INITIALIZE_MUTEX(safeAccess);
	};

	~CjvxComponentLibContainer()
	{
		JVX_INITIALIZE_MUTEX(safeAccess);
	};

	void reset()
	{
		proc = nullptr;
		procClass = nullptr;
		numInChans = 0;
		numOutChans = 0;
		bSize = 0;
		sRate = 0;
		format = JVX_DATAFORMAT_NONE;
		formGroup = JVX_DATAFORMAT_GROUP_NONE;
	};

	jvxErrorType setupInit(
		IjvxComponentLib* procArg,
		jvxHandle* procClassArg,
		jvxSize numInChansArg,
		jvxSize numOutChansArg,
		jvxSize bSizeArg,
		jvxSize sRateArg,
		jvxDataFormat formatArg,
		jvxDataFormatGroup formGroupArg,
		ayfHostBindingReferences* bindRefsArg)
	{
		JVX_LOCK_MUTEX(safeAccess);
		proc = procArg;
		procClass = procClassArg;
		numInChans = numInChansArg;
		numOutChans = numOutChansArg;
		bSize = bSizeArg;
		sRate = sRateArg;
		format = formatArg;
		formGroup = formGroupArg;
		JVX_UNLOCK_MUTEX(safeAccess);
		return JVX_NO_ERROR;
	};

	void lock()
	{
		JVX_LOCK_MUTEX(safeAccess);
	};

	void unlock()
	{
		JVX_UNLOCK_MUTEX(safeAccess);
	};

	jvxErrorType setupTerm()
	{
		JVX_LOCK_MUTEX(safeAccess);
		reset();
		JVX_UNLOCK_MUTEX(safeAccess);
	};

	jvxErrorType deployProcParametersStartProcessor()
	{
		jvxErrorType res = JVX_ERROR_NOT_READY;
		JVX_LOCK_MUTEX(safeAccess);
		if (proc)
		{
			res = proc->deployProcParametersStartProcessor(numInChans,
				numOutChans,
				bSize,
				sRate,
				format,
				formGroup);
		}
		JVX_UNLOCK_MUTEX(safeAccess);
		return res;
	};

	jvxErrorType process_one_buffer_interleaved(
		jvxData* inInterleaved, jvxSize numSamplesIn, jvxSize numChannelsIn,
		jvxData* outInterleaved, jvxSize numSamlesOut, jvxSize numChannelsOut)
	{
		jvxErrorType res = JVX_ERROR_NOT_READY;
		JVX_LOCK_MUTEX(safeAccess);
		if (proc)
		{
			res = proc->process_one_buffer_interleaved(
				inInterleaved, numSamplesIn, numChannelsIn,
				outInterleaved, numSamlesOut, numChannelsOut);
		}
		JVX_UNLOCK_MUTEX(safeAccess);
		return res;
	}
	jvxErrorType stopProcessor()
	{
		jvxErrorType res = JVX_ERROR_NOT_READY;
		JVX_LOCK_MUTEX(safeAccess);
		if (proc)
		{
			res = proc->stopProcessor();
		}
		JVX_UNLOCK_MUTEX(safeAccess);
		return res;
	};
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

	public IjvxComponentLib,

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
	ayfHostBindingReferences* binding = nullptr;

private:
	
	enum class ayfTextIoStatus
	{
		AYF_TEXT_IO_STATUS_INIT,
		AYF_TEXT_IO_STATUS_COLLECTING,
		AYF_TEXT_IO_STATUS_RESPONDING,
		AYF_TEXT_IO_STATUS_DONE
	};


	IjvxMinHost* minHostRef = nullptr; 
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

	myLocalHost* locHost = nullptr;
	IjvxHiddenInterface* localAllocatedHost = nullptr;

public:
	CayfComponentLib(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CayfComponentLib();

	// Static functions to initialize embedding system
	static jvxErrorType populateBindingRefs(
		const std::string& myRegisterName, 
		const std::string& rootPath, 
		ayfHostBindingReferences*& bindOnReturn);
	static jvxErrorType unpopulateBindingRefs();

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* hostRef) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxMinHost* hostRef, IjvxConfigProcessor* confProc, const std::string& regName);
	
	virtual jvxErrorType JVX_CALLINGCONVENTION terminate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION terminate(ayfHostBindingReferences*& bindOnReturn);

	/*
	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* theObj) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect() override;
	*/

	virtual jvxErrorType JVX_CALLINGCONVENTION activate() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate() override;

	// ===============================================================
	// Interface <IjvxComponentLibCore>
	// ===============================================================
	jvxErrorType deployProcParametersStartProcessor(
		jvxSize numInChans, jvxSize numOutChans, 
		jvxSize bSize, jvxSize sRate, 
		jvxDataFormat format, jvxDataFormatGroup formGroup) override;
	jvxErrorType process_one_buffer_interleaved(
		jvxData* inInterleaved, jvxSize numSamplesIn, jvxSize numChannelsIn, 
		jvxData* outInterleaved, jvxSize numSamlesOut, jvxSize numChannelsOut) override;
	jvxErrorType stopProcessor() override;
	
	// ===============================================================
	// ===============================================================

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

	// Interface function to involve a node in this chain
	virtual jvxErrorType allocate_main_node() = 0;
	virtual jvxErrorType deallocate_main_node() = 0;
	virtual jvxErrorType on_main_node_selected() { return JVX_NO_ERROR; };
	virtual jvxErrorType before_main_node_unselect() { return JVX_NO_ERROR; };

	jvxErrorType passConfigSection(IjvxSimpleNode* node, const std::string& moduleName);

};
