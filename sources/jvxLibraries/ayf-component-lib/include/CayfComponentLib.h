#ifndef __CAYFCOMPONENTLIB_H_
#define __CAYFCOMPONENTLIB_H_

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

//#include "IayfComponentLib.h"

class myLocalHost;
class CayfComponentLibContainer;

class CjvySimpleConnectorFactory : public IjvxConnectorFactory, public CjvxConnectorFactory
{
private:
	IjvxObject* objRef = nullptr;
public:
	CjvySimpleConnectorFactory(IjvxObject* objArg) : objRef(objArg) {};

	void activate(IjvxInputConnectorSelect* icon, IjvxOutputConnectorSelect* ocon,
		IjvxConnectionMaster* master, const std::string& dbgHint)
	{
		oneInputConnectorElement newElmIn;
		newElmIn.theConnector = icon;
		oneOutputConnectorElement newElmOut;
		newElmOut.theConnector = ocon;

		_common_set_conn_factory.input_connectors[newElmIn.theConnector] = newElmIn;
		_common_set_conn_factory.output_connectors[newElmOut.theConnector] = newElmOut;
		CjvxConnectorFactory::_activate_factory(objRef); 
	};

	void deactivate()
	{
		CjvxConnectorFactory::_deactivate_factory();
		_common_set_conn_factory.input_connectors.clear();
		_common_set_conn_factory.output_connectors.clear();
	};

#include "codeFragments/simplify/jvxConnectorFactory_simplify.h"

	virtual jvxErrorType JVX_CALLINGCONVENTION request_reference_object(IjvxObject** obj)override
	{
		if (obj)
		{
			*obj = objRef;
		}
		return JVX_NO_ERROR;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_object(IjvxObject* obj) override
	{
		if (obj == objRef)
		{
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_INVALID_ARGUMENT;
	};
};


class CjvxSimpleConnectionMasterFactory: public IjvxConnectionMasterFactory, public CjvxConnectionMasterFactory
{
private:
	IjvxObject * objRef = nullptr;
public:
	CjvxSimpleConnectionMasterFactory(IjvxObject* objArg) : objRef(objArg) {};

	void activate(const std::string& name, IjvxConnectionMaster* mas)
	{
		oneMasterElement newElm;
		newElm.descror = "default";
		newElm.theMaster = mas;
		_common_set_conn_master_factory.masters[newElm.theMaster] = newElm;
		_activate_master_factory(objRef);
	};

	void deactivate()
	{
		_deactivate_master_factory();
		_common_set_conn_master_factory.masters.clear();		
	};

#include "codeFragments/simplify/jvxConnectorMasterFactory_simplify.h"

	virtual jvxErrorType JVX_CALLINGCONVENTION request_reference_object(IjvxObject** obj)override
	{
		if (obj)
		{
			*obj = objRef;
		}
		return JVX_NO_ERROR;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_object(IjvxObject* obj) override
	{
		if (obj == objRef)
		{
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_INVALID_ARGUMENT;
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

	//public IjvxComponentLib,

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

	CayfComponentLibContainer* parent = nullptr;
	IjvxConfigProcessor* confProcHdl = nullptr;
	jvxConfigData* cfgDataInit = nullptr;

	jvxBool exposeFactories = true;
	CjvySimpleConnectorFactory* optConFactory = nullptr;
	CjvxSimpleConnectionMasterFactory* optMasFactory = nullptr;
	
protected:
	IjvxNode* mainNode = nullptr;
	ayfHostBindingReferences* binding = nullptr;
	ayfHostBindingReferencesMinHost* bindingMinHost = nullptr;
	ayfHostBindingReferencesEmbHost* bindingEmbHost = nullptr;

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

	struct
	{
		jvxComponentIdentification tp = JVX_COMPONENT_CONFIG_PROCESSOR;
		refComp<IjvxConfigProcessor> retCfgProc;
		IjvxHost* hostRef = nullptr;
	} embHost;

public:
	CayfComponentLib(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE, CayfComponentLibContainer* parent);
	~CayfComponentLib();

	// Static functions to initialize embedding system
	static jvxErrorType populateBindingRefs(
		const std::string& myRegisterName, 
		const std::string& rootPath, 
		ayfHostBindingReferences*& bindOnReturn);
	static jvxErrorType unpopulateBindingRefs();

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* hostRef) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxMinHost* hostRef, IjvxConfigProcessor* confProc, const std::string& realRegName);
	
	virtual jvxErrorType JVX_CALLINGCONVENTION terminate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION terminate(ayfHostBindingReferencesMinHost*& bindOnReturn);

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
		jvxDataFormat format, jvxDataFormatGroup formGroup);
	jvxErrorType process_one_buffer_interleaved(
		jvxData* inInterleaved, jvxSize numSamplesIn, jvxSize numChannelsIn, 
		jvxData* outInterleaved, jvxSize numSamlesOut, jvxSize numChannelsOut);
	jvxErrorType stopProcessor();
	
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

	virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)override
	{
		jvxErrorType res = JVX_NO_ERROR;

		switch (tp)
		{
		case JVX_INTERFACE_PROPERTIES:
			*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxProperties*>(this));
			break;

		case JVX_INTERFACE_CONNECTOR_FACTORY:
			*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxConnectorFactory*>(optConFactory));
			break;

		case JVX_INTERFACE_CONNECTOR_MASTER_FACTORY:
			*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxConnectionMasterFactory*>(optMasFactory));
			break;

		default:
			res = _request_hidden_interface(tp, hdl);
		}
		return(res);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)override
	{
		jvxErrorType res = JVX_NO_ERROR;

		switch (tp)
		{
		case JVX_INTERFACE_PROPERTIES:
			if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxProperties*>(this)))
			{
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
			break;

		case JVX_INTERFACE_CONNECTOR_FACTORY:
			if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxConnectorFactory*>(optConFactory)))
			{
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
			break;
		case JVX_INTERFACE_CONNECTOR_MASTER_FACTORY:
			if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxConnectionMasterFactory*>(optMasFactory)))
			{
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
			break;

		default:
			res = _return_hidden_interface(tp, hdl);
		}
		return(res);
	};

	jvxErrorType
		object_hidden_interface(IjvxObject** objRef) override
	{
		if (objRef)
		{
			*objRef = static_cast<IjvxObject*>(this);
		}
		return JVX_NO_ERROR;
	};






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

#endif
