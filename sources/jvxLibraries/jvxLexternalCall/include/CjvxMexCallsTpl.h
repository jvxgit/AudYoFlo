#ifndef __CJVXMEXCALLSTPL__H__
#define __CJVXMEXCALLSTPL__H__

#include "CjvxMexCalls.h"
template <class T>
class CjvxMexCallsTpl : public CjvxMexCalls, public T
{
public:
	CjvxMexCallsTpl(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) : T(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
	{
	};

	jvxErrorType select(IjvxObject* theOwner) override
	{
		jvxErrorType res = JVX_NO_ERROR;
		res = T::select(theOwner);
		if (res == JVX_NO_ERROR)
		{
			// Run the class specific initialization
			initExternalCall();

			// Run mex call base classes function to init matlab call handles
			res = CjvxMexCalls::select(_common_set_min.theHostRef, static_cast<CjvxProperties*>(this), "",
				_theExtCallObjectName.c_str());
		}
		return(res);
	};

	jvxErrorType unselect() override
	{
		jvxErrorType res = JVX_NO_ERROR;
		res = T::unselect();
		if (res == JVX_NO_ERROR)
		{
			// Run the class specific implementation
			terminateExternalCall();
			res = CjvxMexCalls::unselect();

			/*
			* Should have been removed already in <CjvxMexCalls::unselect>
			returnExternalCallHandle(_common_set_min.theHostRef, parent._theExtCallHandler);
			parent._theExtCallHandler = NULL;
			parent._theExtCallObjectName = "";
			*/

			_theExtCallObjectName = "";
		}
		return(res);
	};

	jvxErrorType activate() override
	{
		jvxErrorType res = T::activate();
		if (res == JVX_NO_ERROR)
		{
			res = CjvxMexCalls::activate();
		}
		return res;
	};

	jvxErrorType deactivate() override
	{
		jvxErrorType res = T::deactivate();
		if (res == JVX_NO_ERROR)
		{
			res = CjvxMexCalls::deactivate();
		}
		return(res);
	};
	
	jvxErrorType prepare() override
	{
		jvxErrorType res = T::prepare();
		if (res == JVX_NO_ERROR) 
		{
			res = CjvxMexCalls::prepare();
		}
		return(res);
	};

	jvxErrorType postprocess() override
	{
		jvxErrorType res = T::postprocess();
		if (res == JVX_NO_ERROR)
		{
			res = CjvxMexCalls::postprocess();
		}
		return(res);
	};

	jvxErrorType prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override
	{
		jvxErrorType res = JVX_NO_ERROR;
		// onPrepareConnectMexCalls();

		CjvxMexCalls::prepare_sender_to_receiver(_common_set_icon.theData_in);
		res = T::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (res == JVX_NO_ERROR)
		{
			CjvxMexCalls::prepare_complete_receiver_to_sender(_common_set_icon.theData_in, &_common_set_ocon.theData_out);
		}
		else
		{
			CjvxMexCalls::postprocess_sender_to_receiver(_common_set_icon.theData_in);
		}
		return res;
	};

	jvxErrorType postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override
	{
		jvxErrorType res = JVX_NO_ERROR;
		CjvxMexCalls::before_postprocess_receiver_to_sender(_common_set_icon.theData_in);
		res = T::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		CjvxMexCalls::postprocess_sender_to_receiver(_common_set_icon.theData_in);
		return res;
	};

	jvxErrorType process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage) override
	{
		jvxErrorType res = JVX_NO_ERROR;
		jvxBool isPresent = false;
		jvxBool engaged = false;

		// Pass control to Matlab processing
		CjvxMexCalls::is_extcall_reference_present(&isPresent);
		CjvxMexCalls::is_matlab_processing_engaged(&engaged);
		if (isPresent && engaged)
		{

			// This lets Matlab run one frame of processing
			// ======================================================================================
			res = CjvxMexCalls::process_buffers_icon(_common_set_icon.theData_in, &_common_set_ocon.theData_out);
			// ======================================================================================

			return T::fwd_process_buffers_icon(mt_mask, idx_stage);
		}
		return JVX_ERROR_NOT_READY;
	};

	jvxErrorType put_configuration(jvxCallManagerConfiguration* callConf,
			IjvxConfigProcessor* processor,
			jvxHandle* sectionToContainAllSubsectionsForMe,
			const char* filename,
			jvxInt32 lineno)
	{
		jvxSize i;
		jvxErrorType res = T::put_configuration(callConf,
			processor,
			sectionToContainAllSubsectionsForMe,
			filename, lineno);

		if (res == JVX_NO_ERROR)
		{
			res = CjvxMexCalls::put_configuration(callConf, processor, this->_common_set_min.theState, sectionToContainAllSubsectionsForMe, filename, lineno);
		}
		return res;
	};

	jvxErrorType get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)
	{
		jvxErrorType res = T::get_configuration(callConf, processor,
			sectionWhereToAddAllSubsections);

		if (res == JVX_NO_ERROR)
		{
			res = CjvxMexCalls::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);
		}
		return res;
	};

	// ==========================================================================================
	virtual void initExternalCall() {};
	virtual void terminateExternalCall() {};
	// virtual void onPrepareConnectMexCalls() = 0;
};

#endif