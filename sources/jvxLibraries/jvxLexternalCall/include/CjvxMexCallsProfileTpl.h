#ifndef __CJVXSUBCLASSMEX__H__
#define __CJVXSUBCLASSMEX__H__

#include "CjvxMexCallsProfileInterfacing.h"
#include "CjvxMexCallsTpl.h"

template <class T>
class CjvxMexCallsProfileTpl : public CjvxMexCallsTpl<T>, public CjvxMexCallsProfileInterfacing
{
protected:
	struct
	{
		jvxBool matlab_debug_enabled = false; // <- this means: allow matlab and c code operated in parallel - required for verification
		jvxBool skipInvolveCCode = false;
		jvxBool matlab_profiling_enabled = false;		
	} config;	

	//! temporary fields for parallel processing C and Matlab
	jvxData** dbgFldCopyInputs = nullptr;

public:
	
	JVX_CALLINGCONVENTION CjvxMexCallsProfileTpl(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) : 
		CjvxMexCallsTpl<T>(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL) {};

	// Some callbacks
	virtual jvxErrorType local_allocate_profiling() = 0;
	virtual jvxErrorType local_deallocate_profiling() = 0;
	
	jvxErrorType prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
		jvxErrorType res = JVX_NO_ERROR;

		CjvxMexCalls::prepare_connect_icon_enter(_common_set_icon.theData_in);

		// Run this code before actually preparing this component to allow pre-run configuration in jvxDebugConfigAec
		if (_theExtCallHandler)
		{
			// We need to run this BEFORE we run any prepare call to obtain the
			// modified parameter settings in the Matlab startup
			if (config.matlab_debug_enabled && config.matlab_profiling_enabled)
			{
				res = CjvxMexCallsProfiler::profile_config_on_prepare();
			}
		}

		res = T::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));

		// =============================================================================================
		if (res == JVX_NO_ERROR)
		{
			// Matlab initialization is complete here
			CjvxMexCalls::prepare_connect_icon_leave(_common_set_icon.theData_in, &_common_set_ocon.theData_out);

			// Here, run the c code initialization!
			res = local_prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));

			if (res == JVX_NO_ERROR)
			{
				JVX_SAFE_ALLOCATE_2DFIELD_CPP_Z(dbgFldCopyInputs,
					jvxData,
					_common_set_icon.theData_in->con_params.number_channels,
					_common_set_icon.theData_in->con_params.buffersize);

				if (config.matlab_debug_enabled && config.matlab_profiling_enabled)
				{
					res = local_allocate_profiling();
				}
			}
		}

		if (res != JVX_NO_ERROR)
		{
			// Cleanup!
			this->postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL_NULL);
		}
		return res;
	};

	jvxErrorType postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
		jvxErrorType res = JVX_NO_ERROR;
		jvxErrorType resL = JVX_NO_ERROR;

		if (config.matlab_debug_enabled && config.matlab_profiling_enabled)
		{
			// Stop the profiler
			CjvxMexCallsProfiler::profile_stop_on_postprocess();
			res = local_deallocate_profiling();
		}

		JVX_SAFE_DELETE_2DFIELD(dbgFldCopyInputs, _common_set_icon.theData_in->con_params.number_channels);

		// Ignore all return values - if these functions fail we have a more severe problem!!
		res = T::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		res = local_postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		res = CjvxMexCalls::postprocess_connect_icon_leave(_common_set_icon.theData_in);
		
		return res;
	};

	jvxErrorType process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
	{
		jvxSize i;
		jvxErrorType res = JVX_NO_ERROR;

		jvxBool engaged = false;
		jvxBool skipCCode = false;

		// Pass control to Matlab processing	

		// Check if checkbox in Matlab processing was checked
		if (_theExtCallHandler)
		{
			CjvxMexCalls::is_matlab_processing_engaged(&engaged);
		}

		if(engaged)
		{
			jvxData** buffers_in = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_icon.theData_in, idx_stage);
			jvxData** buffers_out = jvx_process_icon_extract_output_buffers<jvxData>(_common_set_ocon.theData_out);
		
			if (config.matlab_debug_enabled)
			{
				// ===================================================
				// This case to run Matlab and C code in parallel
				// ===================================================

				if (config.matlab_profiling_enabled)
				{
					// Run this command only once!! Need to put it here since processing information is not available in "prepare"!!
					CjvxMexCallsProfiler::profile_start_in_process();
				}

				// Copy input data for later usage
				for (i = 0; i < _common_set_icon.theData_in->con_params.number_channels; i++)
				{
					jvxData* ptrTo = dbgFldCopyInputs[i];
					jvxData* ptrFrom = (jvxData*)buffers_in[i];
					memcpy(ptrTo, ptrFrom, sizeof(jvxData) * _common_set_icon.theData_in->con_params.buffersize);
				}


				if (config.skipInvolveCCode)
				{
					skipCCode = true;
				}

				// This lets Matlab run one frame of processing
				// ======================================================================================
				res = CjvxMexCalls::process_buffers_icon(_common_set_icon.theData_in, &_common_set_ocon.theData_out);
				// ======================================================================================

				if (!skipCCode)
				{
					for (i = 0; i < _common_set_icon.theData_in->con_params.number_channels; i++)
					{
						jvxData* ptrFrom = dbgFldCopyInputs[i];
						jvxData* ptrTo = (jvxData*)buffers_in[i];
						memcpy(ptrTo, ptrFrom, sizeof(jvxData) * _common_set_icon.theData_in->con_params.buffersize);
					}
				}
			}
			else
			{
				return res;
			}
		}

		if (!skipCCode)
		{
			// Run core algorithm in C
			res = local_process_buffers_icon(mt_mask, idx_stage);

			// Providing data only if c code is operated
			CjvxMexCallsProfiler::provideDataMexCall();
		}

		if (config.matlab_debug_enabled && config.matlab_profiling_enabled)
		{
			// Profiler step not necessary requires c code execution
			CjvxMexCallsProfiler::profile_step_in_process();
		}

		// Forward calling chain
		return fwd_process_buffers_icon(mt_mask, idx_stage);
	};
};

#endif
