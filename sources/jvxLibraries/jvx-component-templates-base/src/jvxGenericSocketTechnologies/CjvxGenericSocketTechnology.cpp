#include "jvxGenericSocketTechnologies/CjvxGenericSocketTechnology.h"

CjvxGenericSocketTechnology::CjvxGenericSocketTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxGenericConnectionTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL) 
{
	connectionIdenitificationToken = "jvxTNewSocket";
}

CjvxGenericSocketTechnology::~CjvxGenericSocketTechnology()
{
}

jvxErrorType 
CjvxGenericSocketTechnology::select(IjvxObject* owner)
{
	jvxErrorType res = CjvxGenericConnectionTechnology::select(owner);
	if (res == JVX_NO_ERROR)
	{

		CjvxGenericSocketTechnology_pcg::init_all();
		CjvxGenericSocketTechnology_pcg::allocate_all();
		CjvxGenericSocketTechnology_pcg::register_all(this);
	}
	return res;
}

jvxErrorType 
CjvxGenericSocketTechnology::unselect()
{
	jvxErrorType res = CjvxGenericConnectionTechnology::_pre_check_unselect();
	if (res == JVX_NO_ERROR)
	{
		CjvxGenericSocketTechnology_pcg::unregister_all(this);
		CjvxGenericSocketTechnology_pcg::deallocate_all();		
		CjvxGenericConnectionTechnology::unselect();
	}
	return res;
}

// ==============================================================================

jvxErrorType 
CjvxGenericSocketTechnology::activate()
{
	jvxErrorType res = CjvxGenericConnectionTechnology::activate();
	if (res == JVX_NO_ERROR)
	{
		// The properties only have an impact on "activate", therefore, any changes in the
		// socket arguments wont have any impact on the current section but WILL have one
		// in the next session!
		// CjvxProperties::_update_properties_on_start();
	}
	return res;
}

jvxErrorType 
CjvxGenericSocketTechnology::deactivate()
{
	jvxErrorType res = CjvxGenericConnectionTechnology::deactivate();
	if (res == JVX_NO_ERROR)
	{
		// The properties only have an impact on "activate", therefore, any changes in the
		// socket arguments wont have any impact on the current section but WILL have one
		// in the next session!
		// CjvxProperties::_update_properties_on_stop();
	}
	return res;
}

// ==============================================================================

jvxErrorType
CjvxGenericSocketTechnology::initializeConnectionCoreModule(IjvxConnection* conn)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string txtConfigure;
	jvxSocketsConnectionType tp = CjvxGenericSocketTechnology_pcg::translate__socket_settings__connection_type_from();
	txtConfigure = jvxSocketsConnectionType_txt(tp);
	res = conn->initialize(_common_set_min.theHostRef,
		(jvxHandle*)txtConfigure.c_str(),
		JVX_CONNECT_PRIVATE_ARG_TYPE_INPUT_STRING);	
	return res;
}

jvxErrorType
CjvxGenericSocketTechnology::request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)
{
	jvxErrorType res = JVX_NO_ERROR;

	switch (tp)
	{
	case JVX_INTERFACE_CONFIGURATION:
		*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxConfiguration*>(this));
		break;

	default:
		res = CjvxGenericConnectionTechnology::request_hidden_interface(tp, hdl);
	}
	return(res);
};

jvxErrorType
CjvxGenericSocketTechnology::return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)
{
	jvxErrorType res = JVX_NO_ERROR;

	switch (tp)
	{
	case JVX_INTERFACE_CONFIGURATION:
		if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxConfiguration*>(this)))
		{
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	default:
		res = CjvxGenericConnectionTechnology::return_hidden_interface(tp, hdl);
	}
	return(res);
};

jvxErrorType 
CjvxGenericSocketTechnology::put_configuration(jvxCallManagerConfiguration* callMan, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename , jvxInt32 lineno )
{
	if (_common_set_min.theState == JVX_STATE_SELECTED)
	{
		CjvxGenericSocketTechnology_pcg::put_configuration__socket_settings(callMan,
			processor, sectionToContainAllSubsectionsForMe);
	}
	return JVX_NO_ERROR;
}


jvxErrorType 
CjvxGenericSocketTechnology::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{	
	if (_common_set_min.theState >= JVX_STATE_SELECTED)
	{
		CjvxGenericSocketTechnology_pcg::get_configuration__socket_settings(callMan,
			processor, sectionWhereToAddAllSubsections);
	}
	return JVX_NO_ERROR;
}
