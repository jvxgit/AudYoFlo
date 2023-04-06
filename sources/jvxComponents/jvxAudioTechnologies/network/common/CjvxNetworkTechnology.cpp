#include "CjvxNetworkTechnology.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

CjvxNetworkTechnology::CjvxNetworkTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxAudioTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	tp = JVX_CONNECT_SOCKET_TYPE_NONE;
	numReqDevices = 0;
	theRealHost = NULL;
}

CjvxNetworkTechnology::~CjvxNetworkTechnology()
{
	terminate();
}

jvxErrorType
CjvxNetworkTechnology::select(IjvxObject* theOwner)
{
	jvxErrorType resL = JVX_NO_ERROR;
	jvxErrorType res = CjvxAudioTechnology::select(theOwner);

	if(res == JVX_NO_ERROR)
	{
		genNetworkMaster_technology::init_all();
		genNetworkMaster_technology::allocate_all();
		genNetworkMaster_technology::register_all(static_cast<CjvxProperties*>(this));

		// Different connction types
		genNetworkMaster_technology::integrateiplink.properties_sockets.socketTypeSelection.value.entries.push_back("TCP");
		genNetworkMaster_technology::integrateiplink.properties_sockets.socketTypeSelection.value.entries.push_back("UDP");
#if defined(JVX_WITH_PCAP)
		genNetworkMaster_technology::integrateiplink.properties_sockets.socketTypeSelection.value.entries.push_back("PCAP");
#endif
#if defined(JVX_OS_WINDOWS) && defined(JVX_WITH_BTH)
		genNetworkMaster_technology::integrateiplink.properties_sockets.socketTypeSelection.value.entries.push_back("BLUETOOTH");
#endif
	
#if defined(JVX_WITH_PCAP)
		
		genNetworkMaster_technology::integrateiplink.properties_sockets.socketTypeSelection.value.selection() = 0x4;
#else
		// Default selection: UDP
		genNetworkMaster_technology::integrateiplink.properties_sockets.socketTypeSelection.value.selection() = 0x2;
#endif
		
	}
	return(res);
}

jvxErrorType
CjvxNetworkTechnology::unselect()
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if(_common_set_min.theState == JVX_STATE_SELECTED)
	{
		genNetworkMaster_technology::unregister_all(static_cast<CjvxProperties*>(this));
		genNetworkMaster_technology::deallocate_all();
		res = CjvxAudioTechnology::unselect();
	}
	return(res);
}

jvxErrorType
CjvxNetworkTechnology::activate()
{
	int i;
	
	jvxErrorType resL = JVX_NO_ERROR;
	jvxErrorType res = CjvxAudioTechnology::activate();
	
	int cnt = 0;
	if(res == JVX_NO_ERROR)
	{
		init_tc(_common_set_min.theHostRef);
		put_configuration_tc(static_cast<IjvxConfiguration*>(this), _common_set_min.theDescription.c_str());

		if(jvx_bitTest(genNetworkMaster_technology::integrateiplink.properties_sockets.socketTypeSelection.value.selection(), cnt))
		{
			tp = JVX_CONNECT_SOCKET_TYPE_TCP;
		}
		cnt++;
		if(jvx_bitTest(genNetworkMaster_technology::integrateiplink.properties_sockets.socketTypeSelection.value.selection(), cnt))
		{
			tp = JVX_CONNECT_SOCKET_TYPE_UDP;
		}
		cnt++;
#if defined(JVX_WITH_PCAP)
		if(jvx_bitTest(genNetworkMaster_technology::integrateiplink.properties_sockets.socketTypeSelection.value.selection(), cnt))
		{
			tp = JVX_CONNECT_SOCKET_TYPE_PCAP;
		}
		cnt++;
#if defined(JVX_OS_WINDOWS) && defined(JVX_WITH_BTH)
		if(jvx_bitTest(genNetworkMaster_technology::integrateiplink.properties_sockets.socketTypeSelection.value.selection(), cnt)
		{
			tp = JVX_CONNECT_SOCKET_TYPE_BTH;
		}
		cnt++;
#endif
#else
#if defined(JVX_OS_WINDOWS) && defined(JVX_WITH_BTH)
		if(jvx_bitTest(genNetworkMaster_technology::integrateiplink.properties_sockets.socketTypeSelection.value.selection(), cnt))
		{
			tp = JVX_CONNECT_SOCKET_TYPE_BTH;
		}
		cnt++;
#endif
#endif

		initializeUnderlyingConnection();

	}
	return(res);
}

jvxErrorType
	CjvxNetworkTechnology::deactivate()
{
	jvxSize i;
	jvxErrorType res = _pre_check_deactivate();
	if(res == JVX_NO_ERROR)
	{
		get_configuration_tc(static_cast<IjvxConfiguration*>(this), _common_set_min.theDescription.c_str());

		terminate_tc();

		auto elm = _common_tech_set.lstDevices.begin();
		for(; elm != _common_tech_set.lstDevices.end(); elm++)
		{
			delete(elm->hdlDev);
		}
		_common_tech_set.lstDevices.clear();
		_deactivate();
	}
	return(res);
}

void 
CjvxNetworkTechnology::informDeviceSelected()
{
	if (numReqDevices == 0)
	{
		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY,
			genNetworkMaster_technology::integrateiplink.properties_sockets.socketTypeSelection);
	}
	numReqDevices++;
}

void
CjvxNetworkTechnology::informDeviceUnselected()
{
	assert(numReqDevices > 0);
	numReqDevices--;
	if (numReqDevices == 0)
	{
		CjvxProperties::_undo_update_property_access_type(
			genNetworkMaster_technology::integrateiplink.properties_sockets.socketTypeSelection);
	}
}

jvxErrorType 
CjvxNetworkTechnology::set_property(jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	jvxErrorType res = CjvxAudioTechnology::set_property(
		callGate,
		rawPtr, ident, trans);
	
	JVX_TRANSLATE_PROP_ADDRESS_IDX_CAT(ident, propId, category);

	if (
		(propId == genNetworkMaster_technology::integrateiplink.properties_sockets.specifyCommand.globalIdx) &&
		(category == genNetworkMaster_technology::integrateiplink.properties_sockets.specifyCommand.category))
	{
		if (numReqDevices == 0)
		{
			if (genNetworkMaster_technology::integrateiplink.properties_sockets.specifyCommand.value == "reactivate")
			{
				jvxCBitField req;
				jvx_bitFClear(req);
				jvx_bitSet(req, JVX_REPORT_REQUEST_REACTIVATE_SHIFT);
				_report_command_request(req, &_common_set.theComponentType, sizeof(_common_set.theComponentType));
			}
		}
	}

	if (
		(propId == genNetworkMaster_technology::integrateiplink.properties_sockets.socketTypeSelection.globalIdx) &&
		(category == genNetworkMaster_technology::integrateiplink.properties_sockets.socketTypeSelection.category))
	{
		if (numReqDevices == 0)
		{
			jvxCBitField req;
			jvx_bitFClear(req);
			jvx_bitSet(req, JVX_REPORT_REQUEST_REACTIVATE_SHIFT);
			_report_command_request(req, &_common_set.theComponentType, sizeof(_common_set.theComponentType));
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxNetworkTechnology::put_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename, jvxInt32 lineno)
{
	if ((_common_set_min.theState == JVX_STATE_SELECTED) || (callConf->call_purpose == JVX_CONFIGURATION_PURPOSE_CONFIG_ON_COMPONENT_STARTSTOP))
	{
		std::vector<std::string> warns;
		genNetworkMaster_technology::put_configuration_all(callConf, processor, sectionToContainAllSubsectionsForMe, &warns);

	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxNetworkTechnology::get_configuration(jvxCallManagerConfiguration* callConf, 
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	genNetworkMaster_technology::get_configuration_all(callConf, processor, sectionWhereToAddAllSubsections);
	return JVX_NO_ERROR;
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
