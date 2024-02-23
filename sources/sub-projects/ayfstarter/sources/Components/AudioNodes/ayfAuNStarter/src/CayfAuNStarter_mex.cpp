#include "CayfAuNStarter.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

#include "mcg_exports_project.h"

	void
		CayfAuNStarter::initExternalCall()
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

	void
		CayfAuNStarter::terminateExternalCall()
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

		ayf_starter_init(&handle);
		
		handle.prmAsync.volume = processing_lib.prmAsync.volume;

		ayf_starter_prepare(&handle);

		ayf_starter_process(&handle, paramIn0, paramOut0, dimInY0, dimOutY0, dimInX0);

		ayf_starter_postprocess(&handle);

		return JVX_NO_ERROR;
	}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

