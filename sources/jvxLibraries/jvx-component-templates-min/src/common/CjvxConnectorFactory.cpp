#include "common/CjvxConnectorFactory.h"

CjvxConnectorFactory::CjvxConnectorFactory()
	{
		_common_set_conn_factory.theStatus = JVX_STATE_INIT;
 	}
	
CjvxConnectorFactory::~CjvxConnectorFactory()
	{
	}

	jvxErrorType 
		CjvxConnectorFactory::_activate_factory(IjvxObject* theRefObject)
	{
		if (_common_set_conn_factory.theStatus == JVX_STATE_INIT)
		{			
			_common_set_conn_factory.theStatus = JVX_STATE_ACTIVE;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	}

	jvxErrorType
		CjvxConnectorFactory::_deactivate_factory()
	{
		if (_common_set_conn_factory.theStatus == JVX_STATE_ACTIVE)
		{
			_common_set_conn_factory.theStatus = JVX_STATE_INIT;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	}

	jvxErrorType
		CjvxConnectorFactory::_number_input_connectors(jvxSize* num_in_connectors)
	{
		if (num_in_connectors)
		{
			*num_in_connectors = _common_set_conn_factory.input_connectors.size();
		}
		return JVX_NO_ERROR;
	}

	jvxErrorType
		CjvxConnectorFactory::_number_output_connectors(jvxSize* num_out_connectors)
	{
		if (num_out_connectors)
		{
			*num_out_connectors = _common_set_conn_factory.output_connectors.size();
		}
		return JVX_NO_ERROR;
	}

	jvxErrorType
		CjvxConnectorFactory::_reference_input_connector(jvxSize idx, IjvxInputConnector** ref)
	{
		std::map<IjvxInputConnector*, oneInputConnectorElement>::iterator elm = _common_set_conn_factory.input_connectors.begin();
		std::advance(elm, idx);
		if (elm != _common_set_conn_factory.input_connectors.end())
		{
			if (ref)
			{
				*ref = elm->second.theConnector;
				elm->second.refCnt++;
			}
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ID_OUT_OF_BOUNDS;
	}

	jvxErrorType
		CjvxConnectorFactory::_return_reference_input_connector(IjvxInputConnector* ref)
	{
		std::map<IjvxInputConnector*, oneInputConnectorElement>::iterator elm = _common_set_conn_factory.input_connectors.find(ref);
		if (elm != _common_set_conn_factory.input_connectors.end())
		{
			assert(elm->second.refCnt > 0);
			elm->second.refCnt--;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}

	jvxErrorType
		CjvxConnectorFactory::_reference_output_connector(jvxSize idx, IjvxOutputConnector** ref )
	{
		std::map<IjvxOutputConnector*, oneOutputConnectorElement>::iterator elm = _common_set_conn_factory.output_connectors.begin();
		std::advance(elm, idx);
		if (elm != _common_set_conn_factory.output_connectors.end())
		{
			if (ref)
			{
				*ref = elm->second.theConnector;
				elm->second.refCnt++;
			}
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ID_OUT_OF_BOUNDS;
	}

	jvxErrorType
		CjvxConnectorFactory::_return_reference_output_connector(IjvxOutputConnector* ref)
	{
		std::map<IjvxOutputConnector*, oneOutputConnectorElement>::iterator elm = _common_set_conn_factory.output_connectors.find(ref);
		if (elm != _common_set_conn_factory.output_connectors.end())
		{
			assert(elm->second.refCnt > 0);
			elm->second.refCnt--;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}

	IjvxInputConnector* 
		CjvxConnectorFactory::connector_input_name(const std::string& token)
	{
		jvxApiString aStr;
		std::map<IjvxInputConnector*, oneInputConnectorElement>::iterator elm_in = _common_set_conn_factory.input_connectors.begin();
		for (; elm_in != _common_set_conn_factory.input_connectors.end(); elm_in++)
		{
			elm_in->second.theConnector->descriptor_connector(&aStr);
			if (aStr.std_str() == token)
			{
				return elm_in->second.theConnector;
			}
		}
		return NULL;
	}

	IjvxOutputConnector* 
		CjvxConnectorFactory::connector_output_name(const std::string& token)
	{
		jvxApiString aStr;
		std::map<IjvxOutputConnector*, oneOutputConnectorElement>::iterator elm_out = _common_set_conn_factory.output_connectors.begin();
		for (; elm_out != _common_set_conn_factory.output_connectors.end(); elm_out++)
		{
			elm_out->second.theConnector->descriptor_connector(&aStr);
			if (aStr.std_str() == token)
			{
				return elm_out->second.theConnector;
			}
		}
		return NULL;
	}
