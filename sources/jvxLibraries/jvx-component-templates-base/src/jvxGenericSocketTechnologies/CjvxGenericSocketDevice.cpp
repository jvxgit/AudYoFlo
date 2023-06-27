
#include "jvxGenericSocketTechnologies/CjvxGenericSocketDevice.h"


// ==================================================================================================
// ==================================================================================================

CjvxGenericSocketDevice::CjvxGenericSocketDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxGenericConnectionDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
	// , jvxrtst_local(&jvxrtst_bkp_local.jvxos)
{
}

CjvxGenericSocketDevice::~CjvxGenericSocketDevice()
{
}


jvxErrorType
CjvxGenericSocketDevice::put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,  const char* filename, jvxInt32 lineno)
{
	jvxSize i;
	std::vector<std::string> warns;

	if (_common_set_min.theState >= JVX_STATE_SELECTED)
	{
		CjvxGenericSocketDevice_pcg::put_configuration_all(callConf, processor, sectionToContainAllSubsectionsForMe, &warns);
		for (i = 0; i < warns.size(); i++)
		{
			std::cout << "::" << __FUNCTION__ << ": Warning when loading confiugration from file " << filename << ": " << warns[i] << std::endl;
		}
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxGenericSocketDevice::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	if (_common_set_min.theState >= JVX_STATE_SELECTED)
	{
		CjvxGenericSocketDevice_pcg::get_configuration_all(callConf, processor, sectionWhereToAddAllSubsections);
	}
	return(JVX_NO_ERROR);
}

void 
CjvxGenericSocketDevice::select_connection_specific()
{
	jvxSize i;
	CjvxGenericSocketDevice_pcg::init_all();
	CjvxGenericSocketDevice_pcg::allocate_all();
	CjvxGenericSocketDevice_pcg::register_all(static_cast<CjvxProperties*>(this));

	_lock_properties_local();
	// To do
	_unlock_properties_local();
}

void
CjvxGenericSocketDevice::unselect_connection_specific()
{
	// SPecific here
	_lock_properties_local();
	// Todo
	_unlock_properties_local();

	CjvxGenericSocketDevice_pcg::unregister_all(static_cast<CjvxProperties*>(this));
	CjvxGenericSocketDevice_pcg::deallocate_all();
}

jvxErrorType
CjvxGenericSocketDevice::activate_connection_port()
{
	

	if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
	{
		jvx_lock_text_log(jvxrtst_bkp);
		jvxrtst << "::" << __FUNCTION__ << ": " << "Starting input buffering:" << std::endl;
		jvxrtst << "-> Size of input buffer: " << runtime.sz_mem_incoming << std::endl;
		jvx_unlock_text_log(jvxrtst_bkp);
	}

	// Copy socket configuration
	std::string cfgToken = "65000";

	return this->theConnectionTool->start_port(idDevice, (jvxHandle*)cfgToken.c_str(), JVX_CONNECT_PRIVATE_ARG_TYPE_INPUT_STRING, static_cast<IjvxConnection_report*>(this));
}

jvxErrorType
CjvxGenericSocketDevice::deactivate_connection_port()
{
	// Last chance to transmit a "goodbye message"
	this->goodbye_connection();

	this->theConnectionTool->stop_port(idDevice);

	if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
	{
		jvx_lock_text_log(jvxrtst_bkp);
		jvxrtst << "::" << __FUNCTION__ << ": " << "stopping connection on COM port <" << idDevice << ">," << std::endl;
		jvx_unlock_text_log(jvxrtst_bkp);
	}


#if 0
	jvx_message_queue_stop(theMQueue);
	jvx_message_queue_terminate(theMQueue);
#endif
	CjvxGenericSocketDevice_pcg::unregister_all(static_cast<CjvxProperties*>(this));
	CjvxGenericSocketDevice_pcg::deallocate_all();
	return JVX_NO_ERROR;
}