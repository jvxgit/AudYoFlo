#ifndef __CJVXCONNECTORFACTORY_H__
#define __CJVXCONNECTORFACTORY_H__

#include "jvx.h"
#include <map>

JVX_INTERFACE CjvxConnectorFactory_report
{
public:
	virtual jvxErrorType JVX_CALLINGCONVENTION report_selected_connector(const char* description, jvxBool isInput) = 0;
};

class CjvxConnectorFactory
{
private:

protected:
	struct oneInputConnectorElement
	{
		IjvxInputConnectorSelect* theConnector;
		//jvxSize uId;
		jvxSize refCnt;
		CjvxConnectorFactory_report* report;
		//std::vector<IjvxOutputConnector*> connectsto;
	public:
		oneInputConnectorElement()
		{
			//uId = 0;
			refCnt = 0;
			theConnector = NULL;
			report = NULL;
		};
	} ;
	
	struct oneOutputConnectorElement
	{
		IjvxOutputConnectorSelect* theConnector;
		jvxSize refCnt;
		CjvxConnectorFactory_report* report;
		//std::vector<IjvxOutputConnector*> connectsto;
	public:
		oneOutputConnectorElement()
		{
			refCnt = 0;
			theConnector = NULL;
			report = NULL;
		};
	} ;

	struct
	{
		std::map<IjvxInputConnectorSelect*, oneInputConnectorElement> input_connectors;
		std::map<IjvxOutputConnectorSelect*, oneOutputConnectorElement> output_connectors;
		jvxState theStatus;
	} _common_set_conn_factory;

	CjvxConnectorFactory();
	
	~CjvxConnectorFactory();

	jvxErrorType _activate_factory(IjvxObject* theRefObject);

	jvxErrorType _deactivate_factory();

	jvxErrorType _number_input_connectors(jvxSize* num_in_connectors);

	jvxErrorType _number_output_connectors(jvxSize* num_out_connectors);

	jvxErrorType _reference_input_connector(jvxSize idx, IjvxInputConnectorSelect** ref);

	jvxErrorType _return_reference_input_connector(IjvxInputConnectorSelect* ref) ;

	jvxErrorType  _reference_output_connector(jvxSize idx, IjvxOutputConnectorSelect** ref );

	jvxErrorType _return_reference_output_connector(IjvxOutputConnectorSelect* ref);

	IjvxInputConnectorSelect* connector_input_name(const std::string& token);
	IjvxOutputConnectorSelect* connector_output_name(const std::string& token);
};

#ifdef JVX_GLOBAL_BUFFERING_VERBOSE
#define JVX_ACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS(dataProc, obj, name, master_ptr, dbg_hint) \
	{ \
		oneInputConnectorElement newElmIn; \
		newElmIn.theConnector = static_cast<IjvxInputConnector*>(this); \
		newElmIn.report = NULL; \
		oneOutputConnectorElement newElmOut; \
		newElmOut.theConnector = static_cast<IjvxOutputConnector*>(this); \
		newElmOut.report = NULL; \
		_common_set_conn_factory.input_connectors[newElmIn.theConnector] = newElmIn; \
		_common_set_conn_factory.output_connectors[newElmOut.theConnector] = newElmOut; \
		CjvxInputOutputConnector::lds_activate(dataProc, obj, \
			static_cast<IjvxConnectorFactory*>(this), master_ptr, \
			name, newElmIn.theConnector, newElmOut.theConnector, dbg_hint); \
		CjvxConnectorFactory::_activate_factory(obj); \
	}
#else
#define JVX_ACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS(dataProc, obj, name, master_ptr, dbg_hint) \
	{ \
		oneInputConnectorElement newElmIn; \
		newElmIn.theConnector = static_cast<IjvxInputConnectorSelect*>(this); \
		newElmIn.report = NULL; \
		oneOutputConnectorElement newElmOut; \
		newElmOut.theConnector = static_cast<IjvxOutputConnectorSelect*>(this); \
		newElmOut.report = NULL; \
		_common_set_conn_factory.input_connectors[newElmIn.theConnector] = newElmIn; \
		_common_set_conn_factory.output_connectors[newElmOut.theConnector] = newElmOut; \
		CjvxInputOutputConnector::lds_activate(dataProc, obj, \
			static_cast<IjvxConnectorFactory*>(this), master_ptr, \
			name, static_cast<IjvxInputConnector*>(this), \
			static_cast<IjvxOutputConnector*>(this)); \
		CjvxConnectorFactory::_activate_factory(obj); \
	}
#endif
// 		newElmIn.connectsto.push_back(newElmOut.theConnector); \

#define JVX_DEACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS() \
	{ \
		CjvxConnectorFactory::_deactivate_factory(); \
		_common_set_conn_factory.input_connectors.clear(); \
		_common_set_conn_factory.output_connectors.clear(); \
		_common_set_ldslave.myParent = NULL; \
		CjvxInputOutputConnector::lds_deactivate(); \
}

#endif
