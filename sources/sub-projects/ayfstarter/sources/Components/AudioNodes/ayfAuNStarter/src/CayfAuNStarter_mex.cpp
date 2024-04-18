#include "CayfAuNStarter.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

#include "mcg_exports_project.h"

	void CayfAuNStarter::initExternalCall()
	{
		std::string nameRegComponent;

		// Obtain register name from generated code
		requestExternalCallRegisterName(nameRegComponent);

		// Set the string reference
		_theExtCallObjectName = nameRegComponent;

		// Request the handle
		requestExternalCallHandle(_common_set_min.theHostRef, &_theExtCallHandler);

		// Initialize it
		initExternalCallhandler(_theExtCallHandler, static_cast<IjvxExternalCallTarget*>(this), _theExtCallObjectName);
	}

	void CayfAuNStarter::terminateExternalCall()
	{
		// Unregister all local functions
		terminateExternalCallhandler(_theExtCallHandler);

		// NEVER derister the _theExtCallHandler instance here - it is still required by the base class
	}

	// ============================================================================================================
	jvxErrorType CayfAuNStarter::ayf_starter_lib_process(
		/* Input signal, NxM->*/ jvxData** paramIn0, jvxInt32 dimInY0, jvxInt32 dimInX0,
		/* Output Parameter 0->*/ jvxData** paramOut0, jvxInt32 dimOutY0, jvxInt32 dimOutX0)
	{
		ayf_starter handle;
		memset(&handle, 0, sizeof(handle));

		if (dimInX0 != dimOutX0)
		{
			return JVX_ERROR_INVALID_SETTING;
		}

		ayf_starter_init(&handle, dimInX0);
		
		handle.prmAsync.volume = processing_lib.prmAsync.volume;

		ayf_starter_prepare(&handle, "avx");

		ayf_starter_process(&handle, paramIn0, paramOut0, dimInY0, dimOutY0, dimInX0);

		ayf_starter_postprocess(&handle);

		return JVX_NO_ERROR;
	}

	jvxErrorType CayfAuNStarter::local_allocate_profiling()
	{
		jvxSize i;
		jvxCBitField spec = 0;
		jvx_bitFClear(spec);

		for (i = 0; i < genStarter_node::develop.config.testpoints.value.entries.size(); i++)
		{
			if (jvx_bitTest(genStarter_node::develop.config.testpoints.value.selection(), i))
			{
				jvx_bitSet(spec, i);
			}
		}

		JVX_SAFE_ALLOCATE_OBJECT_CPP_Z(processing_lib_dbg, struct ayf_starter_data_debug);
		processing_lib.develop.dbgHandler = processing_lib_dbg;

		ayf_starter_data_debug_prepare(processing_lib.develop.dbgHandler, &processing_lib, spec,
			CjvxMexCallsProfiler::jvx_register_entry_profiling_data_cb, reinterpret_cast<jvxHandle*>(static_cast<CjvxMexCallsProfiler*>(this)));

		return JVX_NO_ERROR;
	}

	jvxErrorType CayfAuNStarter::local_deallocate_profiling()
	{
		ayf_starter_data_debug_postprocess(processing_lib.develop.dbgHandler,
			CjvxMexCallsProfiler::jvx_unregister_entry_profiling_data_cb, reinterpret_cast<jvxHandle*>(static_cast<CjvxMexCallsProfiler*>(this)));
		JVX_SAFE_DELETE_OBJECT(processing_lib_dbg);

		processing_lib.develop.dbgHandler = nullptr;

		return JVX_NO_ERROR;
	}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

