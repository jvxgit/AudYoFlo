#include "flutter_native_local.h"

int ffi_sequencer_status(void* opaque_hdl)
{
	jvxSequencerStatus stat = JVX_SEQUENCER_STATUS_NONE;
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_NO_ERROR;

#ifdef JVX_SECURE_OPAQUE_PTR
if (opaque_hdl)
{
	res = JVX_ERROR_ELEMENT_NOT_FOUND;
	auto elm = lst_active_referenes.find(opaque_hdl);
	if (elm != lst_active_referenes.end())
	{
	}
}
#else
	ll = reinterpret_cast<jvxLibHost*>(opaque_hdl);
#endif

	res = ll->sequencer_status(&stat);
	return (int)stat;
}

int ffi_sequencer_start(void* opaque_hdl, int granularity_state_report)
{
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSequencerStatus stat = JVX_SEQUENCER_STATUS_NONE;
	jvxApiString astr;

#ifdef JVX_SECURE_OPAQUE_PTR
	if (opaque_hdl)
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
		auto elm = lst_active_referenes.find(opaque_hdl);
		if (elm != lst_active_referenes.end())
		{
		}
	}
#else
	ll = reinterpret_cast<jvxLibHost*>(opaque_hdl);
#endif
	res = ll->process_system_ready(&astr);
	if (res == JVX_NO_ERROR)
	{

		ll->sequencer_status(&stat);
		if (stat == JVX_SEQUENCER_STATUS_NONE)
		{
			res = ll->sequencer_start(granularity_state_report);
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
	}
	return res;
}

int ffi_sequencer_stop(void* opaque_hdl)
{
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSequencerStatus stat = JVX_SEQUENCER_STATUS_NONE;

#ifdef JVX_SECURE_OPAQUE_PTR
	if (opaque_hdl)
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
		auto elm = lst_active_referenes.find(opaque_hdl);
		if (elm != lst_active_referenes.end())
		{
		}
	}
#else
	ll = reinterpret_cast<jvxLibHost*>(opaque_hdl);
#endif

		ll->sequencer_status(&stat);
		if (stat != JVX_SEQUENCER_STATUS_NONE)
		{
			res = ll->sequencer_stop();
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
	return res;
}

int ffi_sequencer_ack_stop(void* opaque_hdl)
{
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSequencerStatus stat = JVX_SEQUENCER_STATUS_NONE;

#ifdef JVX_SECURE_OPAQUE_PTR
	if (opaque_hdl)
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
		auto elm = lst_active_referenes.find(opaque_hdl);
		if (elm != lst_active_referenes.end())
		{
		}
	}
#else
	ll = reinterpret_cast<jvxLibHost*>(opaque_hdl);
#endif

	ll->sequencer_status(&stat);
	if (stat != JVX_SEQUENCER_STATUS_NONE)
	{
		res = ll->sequencer_ack_stop();
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}

	return res;
}

int ffi_sequencer_trigger(void* opaque_hdl)
{
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSequencerStatus stat = JVX_SEQUENCER_STATUS_NONE;

#ifdef JVX_SECURE_OPAQUE_PTR
	if (opaque_hdl)
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
		auto elm = lst_active_referenes.find(opaque_hdl);
		if (elm != lst_active_referenes.end())
		{
		}
	}
#else
	ll = reinterpret_cast<jvxLibHost*>(opaque_hdl);
#endif

	res = ll->sequencer_trigger();
	return res;
}