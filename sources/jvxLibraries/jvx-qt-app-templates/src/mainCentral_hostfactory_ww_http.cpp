#include "mainCentral_hostfactory_ww_http.h"

#include <QScrollArea>
#include <QMessageBox>

#include "jvx.h"

#include <iostream>

Q_DECLARE_METATYPE(jvxWebServerHandlerPurpose);

mainCentral_hostfactory_ww_http::mainCentral_hostfactory_ww_http(QWidget* parent): 
	mainCentral_hostfactory_ww(parent)
{
	qRegisterMetaType<jvxWebServerHandlerPurpose>("jvxWebServerHandlerPurpose");
	ticksUpdateWebSocket_msec = 1000;
	output_ws_debug = false;
	webSocketsReady = false;
	wsInstaller = NULL;
	//seqStartStop = NULL;
	autoStartSequencer = false;
}

// =======================================================================

jvxErrorType
mainCentral_hostfactory_ww_http::inform_specific(jvxSize idSpec, jvxHandle* fldSpecific, jvxSize fldTypeId)
{
	jvxConfigWeb* setParams = NULL;;
	switch (idSpec)
	{
	case JVX_MAINWINDOW_UI_EXTENSION_SPECIFIC_HTTP_CONNECTED:
		setParams = (jvxConfigWeb*)fldSpecific;

		setParams->flow.autoStartSequencer = autoStartSequencer;

		setParams->ws.ticksUpdateWebSocket_msec = this->ticksUpdateWebSocket_msec;
		setParams->ws.numTicksUpdateWebSocket = 10;
		setParams->ws.reportData = static_cast<jvxWebSocket_properties_report*>(this);
		setParams->ws.verbose_out = output_ws_debug;
		wsInstaller = setParams->ws.useSocket;
		break;
	case JVX_MAINWINDOW_UI_EXTENSION_SPECIFIC_HTTP_DISCONNECTED:
		wsInstaller = NULL;
		break;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
mainCentral_hostfactory_ww_http::report_property_binary_data(jvxComponentIdentification tp, jvxSize uId, jvxByte* payload, jvxPropertyTransferPriority prio)
{
	process_websocket_binary(uId, payload, prio);
	return JVX_NO_ERROR;
}

jvxErrorType
mainCentral_hostfactory_ww_http::report_property_registered(jvxComponentIdentification tp, jvxSize uId)
{
	return JVX_NO_ERROR;
}

jvxErrorType
mainCentral_hostfactory_ww_http::report_property_unregistered(jvxComponentIdentification tp, jvxSize uId)
{
	return JVX_NO_ERROR;
}

jvxSize*
mainCentral_hostfactory_ww_http::create_slot_http(jvxComponentIdentification tp)
{
	jvxHttpTicket* uIdPtr = new jvxHttpTicket;
	uIdPtr->uId = 0;
	uIdPtr->cpTp = tp;

	// std::cout << "Allocate id <" << (INT_PTR) uIdPtr << ">" << std::endl;

	collect_http_ids[uIdPtr] = uIdPtr;
	return reinterpret_cast<jvxSize*>(uIdPtr);
}

/*
void
mainCentral_hostfactory_ww_http::remove_slot_http(jvxSize* ext_uid)
{
	auto elm = collect_http_ids.find(ext_uid);
	if (elm != collect_http_ids.end())
	{
		delete elm->second;
		collect_http_ids.erase(elm);
	}
	else
	{
		assert(0);
	}
}
*/

void
mainCentral_hostfactory_ww_http::free_slot_http(jvxSize ext_uid)
{
	auto elm = collect_http_ids.begin();
	for (; elm != collect_http_ids.end(); elm++)
	{
		if (elm->second)
		{
			if (elm->second->uId == ext_uid)
			{
				// std::cout << "Delete id <" << (INT_PTR)elm->first << ">" << std::endl;
				
				delete elm->second;
				collect_http_ids.erase(elm);

				// std::cout << "Number of active slots: " << collect_http_ids.size() << std::endl;
				
				return;
			}
		}
	}
	assert(0); // Failed to find the processid
}

jvxSize*
mainCentral_hostfactory_ww_http::reference_slot_http(jvxSize ext_uid)
{
	auto elm = collect_http_ids.begin();
	for (; elm != collect_http_ids.end(); elm++)
	{
		if (elm->second)
		{
			if (elm->second->uId == ext_uid)
			{
				return reinterpret_cast<jvxSize*>(elm->second);
			}
		}
	}
	return NULL;
}

void 
mainCentral_hostfactory_ww_http::clear_slots_http(jvxComponentIdentification tp)
{
	jvxBool doRun = true;
	while (doRun)
	{		
		doRun = false;
		auto elm = collect_http_ids.begin();
		for(; elm != collect_http_ids.end(); elm++)
		{
			assert(elm->first);
			if(elm->first->cpTp == tp)
			{
				collect_http_ids.erase(elm);
				doRun = true;
				break;
			}
		}
	}
}

// ========================================================================0

jvxErrorType
mainCentral_hostfactory_ww_http::trigger_transfer_simple_property(
	IjvxAccessProperties* propRef, jvxSize appUid, jvxBool do_set,
	jvxHandle* fld_ptr, jvxSize fld_sz, 
	jvxDataFormat fld_form, jvxBool fld_content_only, 
	const char* fld_descror, jvxBool binList, jvxContext* ctxt)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	jvxCallManagerProperties callGate;
	if (propRef)
	{
		callGate.context = ctxt;

		jvxCallManagerProperties callGate;
		jvxComponentIdentification cpId;
		propRef->get_reference_component_description(nullptr, nullptr, nullptr, &cpId);
		jvxSize* mySlot = create_slot_http(cpId);

		// Trigger get property on custom device. Values will be available later
		res = propRef->property_start_delayed_group(callGate, static_cast<IjvxAccessProperties_delayed_report*>(this),
			(jvxHandle*)intptr_t(appUid), jPSCH(mySlot));
		if (res == JVX_ERROR_COMPONENT_BUSY)
		{
			std::cout << "Live process not triggered since the latest request is still ongoing." << std::endl;
			return res;
		}

		if (do_set)
		{
			ident.reset(fld_descror);
			trans.reset(fld_content_only, 0, JVX_PROPERTY_DECODER_NONE, binList);

			res = propRef->set_property(callGate,
				jPRG(fld_ptr, fld_sz, fld_form),
				ident, trans, jPSCH(mySlot));
			assert(res == JVX_ERROR_POSTPONE);
		}
		else
		{
			ident.reset(fld_descror);
			trans.reset(fld_content_only, 0, JVX_PROPERTY_DECODER_NONE);
			res = propRef->get_property(callGate, jPRG(fld_ptr, fld_sz, fld_form), ident, trans);
			assert(res == JVX_ERROR_POSTPONE);
		}
		res = propRef->property_stop_delayed_group(callGate, nullptr, jPSCH(mySlot));
	}
	return res;
}

jvxErrorType
mainCentral_hostfactory_ww_http::trigger_transfer_property_list(IjvxAccessProperties* propRef, jvxSize appUid, const char* wildcard)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	if (propRef)
	{
		jvxCallManagerProperties callGate;
		jvxComponentIdentification cpId;
		propRef->get_reference_component_description(nullptr, nullptr, nullptr, &cpId);
		jvxSize* mySlot = create_slot_http(cpId);

		res = propRef->property_start_delayed_group(callGate, this, (jvxHandle*)intptr_t(appUid), jPSCH(mySlot));
		if (res == JVX_ERROR_COMPONENT_BUSY)
		{
			std::cout << "Live process not triggered since the latest request is still ongoing." << std::endl;
			return res;
		}

		res = propRef->create_refresh_property_cache(callGate,wildcard, jPSCH(mySlot));
		assert(res == JVX_ERROR_POSTPONE);

		res = propRef->property_stop_delayed_group(callGate, nullptr, jPSCH(mySlot));
	}
	return res;
}

jvxErrorType
mainCentral_hostfactory_ww_http::complete_transfer_simple_property(
	jvxSize uniqueId_http,
	IjvxAccessProperties* propRef, jvxSize appUid, jvxBool do_set,
	jvxHandle* fld_ptr, jvxSize fld_sz,
	jvxDataFormat fld_form, jvxBool fld_content_only,
	const char* fld_descror, jvxBool binList, jvxContext* ctxt)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	jvxSize* mySlot = reference_slot_http(uniqueId_http);
	assert(mySlot);

	callGate.context = ctxt;
	if (do_set)
	{
		ident.reset(fld_descror);
		trans.reset(fld_content_only, 0, JVX_PROPERTY_DECODER_NONE, binList);
		res = propRef->set_property(callGate,
			jPRG(fld_ptr,fld_sz, fld_form),
			ident, trans, jPSCH(mySlot));
	}
	else
	{
		ident.reset(fld_descror);
		trans.reset(fld_content_only, 0, JVX_PROPERTY_DECODER_NONE);
		res = propRef->get_property(callGate, jPRG(fld_ptr, fld_sz, fld_form), ident, trans, jPSCH(mySlot));

	}
	return res;
}

jvxErrorType
mainCentral_hostfactory_ww_http::complete_transfer_property_list(
	jvxSize uniqueId_http,
	IjvxAccessProperties* propRef, 
	const char* wildcard)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	jvxSize* mySlot = reference_slot_http(uniqueId_http);
	assert(mySlot);

	res = propRef->create_refresh_property_cache(callGate, wildcard, jPSCH(mySlot));
	return res;
}
