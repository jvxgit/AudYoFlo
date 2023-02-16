#include "flutter_native_local.h"

int ffi_config_save(void* opaque_hdl)
{
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString astr;
	char* retPtr = nullptr;

	JRE;
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

	res = ll->save_config();
	JSE;

	return res;
}

int ffi_get_compile_flags(void* opaque_hdl, int* dataTpFormat, int* num32BitWordsSelection)
{
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_NO_ERROR;
	jvxDataTypeSpec dtSpec = jvxDataTypeSpec::JVX_DATA_TYPE_SPEC_DOUBLE;
	jvxSize num32BitWordsBitfield = JVX_NUMBER_32BITS_BITFIELD;

	JRE;
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
	res = ll->get_compile_flags(dtSpec, num32BitWordsBitfield);
	if (res == JVX_NO_ERROR)
	{
		if (dataTpFormat)
		{
			*dataTpFormat = dtSpec;

		}

		if (num32BitWordsSelection)
		{
			*num32BitWordsSelection = num32BitWordsBitfield;
		}
	}
	JSE;
	return JVX_NO_ERROR;
}

int ffi_set_operation_flags(void* opaque_hdl, int reportSetArg, int reportSyncDirectArg)
{
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_NO_ERROR;
	bool reportSet = false;
	bool reportSyncDirect = false;

	JRE;
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
	if (reportSetArg != 0)
	{
		reportSet = true;
	}

	if (reportSyncDirectArg != 0)
	{
		reportSyncDirect = true;
	}
	res = ll->set_operation_flags(reportSet, reportSyncDirect);
	JSE;
	return res;
}

/*
int ffi_get_native_config_parameters(void* opaque_hdl, int* auto_start, int* auto_stop, int* skipstate_selected)
{
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_NO_ERROR;
	jvxDataTypeSpec dtSpec = jvxDataTypeSpec::JVX_DATA_TYPE_SPEC_DOUBLE;
	jvxSize num32BitWordsBitfield = JVX_NUMBER_32BITS_BITFIELD;

	JRE;
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
	jvxBool auto_start_bool = false;
	jvxBool  auto_stop_bool = false;
	jvxBool skipstate_selected_bool = true;

	res = ll->transfer_native_config_parameters(auto_start_bool, auto_stop_bool, skipstate_selected_bool, false);
	if (res == JVX_NO_ERROR)
	{
		*auto_start = 0;
		*auto_stop = 0;
		*skipstate_selected = 1;

		if (auto_start_bool)
		{
			*auto_start = 1;

		}

		if (auto_stop_bool)
		{
			*auto_stop = 1;
		}
		if (skipstate_selected_bool)
		{
			*skipstate_selected = 1;
		}
	}
	JSE;
	return res;
}

int ffi_set_native_config_parameters(void* opaque_hdl, int auto_start, int auto_stop, int skipstate_selected)
{
	jvxLibHost* ll = nullptr;
	jvxErrorType res = JVX_NO_ERROR;
	jvxDataTypeSpec dtSpec = jvxDataTypeSpec::JVX_DATA_TYPE_SPEC_DOUBLE;
	jvxSize num32BitWordsBitfield = JVX_NUMBER_32BITS_BITFIELD;

	JRE;
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
	jvxBool auto_start_bool = false;
	jvxBool  auto_stop_bool = false;
	jvxBool skipstate_selected_bool = true;

	auto_start_bool = (auto_start != 0);
	auto_stop_bool = (auto_stop != 0);
	skipstate_selected_bool = (skipstate_selected != 0);

	res = ll->transfer_native_config_parameters(auto_start_bool, auto_stop_bool, skipstate_selected_bool, true);	
	JSE;
	return res;
}
*/
