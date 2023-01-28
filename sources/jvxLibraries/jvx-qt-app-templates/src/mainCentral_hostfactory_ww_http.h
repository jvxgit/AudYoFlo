#ifndef __MAINCENTRAL_HOSTFACTORY_WW_HTTP_H__
#define __MAINCENTRAL_HOSTFACTORY_WW_HTTP_H__

#include "mainCentral_hostfactory_ww.h"

class mainCentral_hostfactory_ww_http : public mainCentral_hostfactory_ww, 
	public IjvxAccessProperties_delayed_report, 
	public jvxWebSocket_properties_report
{

protected:
	
	struct jvxHttpTicket
	{
		jvxSize uId; // MUST be first element in struct
		jvxComponentIdentification cpTp;
	};

	jvxBool webSocketsReady;
	jvxWebSocket_properties_install* wsInstaller;
	//jvxHttp_sequencer_startstop* seqStartStop;
	jvxSize ticksUpdateWebSocket_msec;
	jvxBool output_ws_debug;
	jvxBool autoStartSequencer;
	std::map<jvxHttpTicket*, jvxHttpTicket*> collect_http_ids;

private:

	Q_OBJECT

protected:

	jvxSize* create_slot_http(jvxComponentIdentification tp);
	jvxSize* reference_slot_http(jvxSize uniqueId_http);
	void free_slot_http(jvxSize uniqueId_http);
	void clear_slots_http(jvxComponentIdentification tp);

	jvxErrorType trigger_transfer_simple_property(
		IjvxAccessProperties* propRef, jvxSize uniqueId_app, 
		jvxBool do_set,
		jvxHandle* fld_ptr, jvxSize fld_sz,
		jvxDataFormat fld_form, jvxBool fld_content_only,
		const char* fld_descror, 
		jvxBool binList = false, jvxContext* ctxt = NULL);

	jvxErrorType complete_transfer_simple_property(
			jvxSize uniqueId_http,
			IjvxAccessProperties* propRef, jvxSize appUid, jvxBool do_set,
			jvxHandle* fld_ptr, jvxSize fld_sz,
			jvxDataFormat fld_form, jvxBool fld_content_only,
			const char* fld_descror, 
		jvxBool binList = false, jvxContext* ctxt = NULL);

	jvxErrorType trigger_transfer_property_list(IjvxAccessProperties* propRef, jvxSize appUid, const char* wildcard = "/*");
	jvxErrorType complete_transfer_property_list(jvxSize uniqueId_http, IjvxAccessProperties* propRef, const char* wildcard = "/*");

public:

	mainCentral_hostfactory_ww_http(QWidget* parent);

	jvxErrorType inform_specific(jvxSize idSpec, jvxHandle* fldSpecific, jvxSize fldTypeId)override;
	jvxErrorType report_property_binary_data(jvxComponentIdentification tp, jvxSize uId, jvxByte* payload, 
		jvxPropertyTransferPriority prio) override;
	virtual jvxErrorType report_property_registered(jvxComponentIdentification tp, jvxSize uId) override;
	virtual jvxErrorType report_property_unregistered(jvxComponentIdentification tp, jvxSize uId) override;

	virtual void process_websocket_binary(jvxSize userData, jvxByte* payload, jvxPropertyTransferPriority prio) = 0;
};

#endif
