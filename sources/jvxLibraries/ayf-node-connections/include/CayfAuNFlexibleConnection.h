#ifndef __CAYFAUNFLEXIBLECONNECTION_H__
#define __CAYFAUNFLEXIBLECONNECTION_H__

#include "CayfConnection.h"
#include "CayfAuNConnection.h"

namespace AyfConnection
{
	// ===============================================================================
	enum class ayfFlexibleConnectionUnconnectedMode
	{
		AYF_FLEXIBLE_CONNECTION_UNCONNECT_TALKTHROUGH,
		AYF_FLEXIBLE_CONNECTION_UNCONNECT_SILENT
	};

	template <class S>
	class CayfAuNFlexibleConnection : public CayfAuNConnection<S>
	{
		jvxLinkDataDescriptor descr_in;
		jvxLinkDataDescriptor descr_out;
		ayfFlexibleConnectionUnconnectedMode unconnectMode = ayfFlexibleConnectionUnconnectedMode::AYF_FLEXIBLE_CONNECTION_UNCONNECT_SILENT;

		// Status of the connection, use the base object state lock
		JVX_MUTEX_HANDLE safeAccessConnectionState;
		jvxState statFlexProc = JVX_STATE_ACTIVE;
		jvxBool lastTestOk = false;		

	public:
	
		CayfAuNFlexibleConnection(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
			CayfAuNConnection<S>(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
		{
			CayfAuNConnection<S>::isFlexibleConnection = true;
			JVX_INITIALIZE_MUTEX(safeAccessConnectionState);
		};

		~CayfAuNFlexibleConnection()
		{
			JVX_TERMINATE_MUTEX(safeAccessConnectionState);
		}

		jvxErrorType test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(var)) override
		{
			jvxErrorType res = CayfAuNConnection<S>::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(var));
			if (res == JVX_NO_ERROR)
			{
				// If there is a valid connection, run the test function
				jvxState stat = JVX_STATE_NONE;
				CayfAuNConnection<S>::theConnection.state(&stat);
				if(stat == JVX_STATE_ACTIVE) 
				{	
					// On prepared the test function should never be run!!
					assert(statFlexProc == JVX_STATE_ACTIVE);

					lastTestOk = false;
					jvxErrorType resL = CayfAuNConnection<S>::theConnection.test_connection(JVX_CONNECTION_FEEDBACK_CALL(var));
					if (resL == JVX_NO_ERROR)
					{
						lastTestOk = true;
					}					
				}
			}
			return res;
		};

		virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override
		{
			jvxErrorType res = JVX_NO_ERROR;
			res = S::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			if (res == JVX_NO_ERROR)
			{
				assert(statFlexProc == JVX_STATE_ACTIVE);
				statFlexProc = JVX_STATE_PREPARED;
				try_engage_connection(JVX_CONNECTION_FEEDBACK_CALL_NULL);
			}
			return res;
		}		

		virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override
		{
			jvxErrorType res = JVX_NO_ERROR;
			res = S::start_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			if (res == JVX_NO_ERROR)
			{
				assert(statFlexProc == JVX_STATE_PREPARED);
				statFlexProc = JVX_STATE_PROCESSING;
				try_engage_connection(JVX_CONNECTION_FEEDBACK_CALL_NULL);
			}
			return res;
		}

		
		virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override
		{
			jvxErrorType res = JVX_NO_ERROR;
			res = S::stop_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			if (res == JVX_NO_ERROR)
			{
				assert(statFlexProc == JVX_STATE_PROCESSING);
				
				// The state of the component is now PREPARED and the connection must follow accordingly
				statFlexProc = JVX_STATE_PREPARED;
				try_disengage_connection(statFlexProc JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
				
			}
			return res;
		}

		virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override
		{
			jvxErrorType res = JVX_NO_ERROR;
			res = S::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			if (res == JVX_NO_ERROR)
			{
				assert(statFlexProc == JVX_STATE_PREPARED);

				// The state of the component is now ACTIVE and the connection must follow accordingly
				statFlexProc = JVX_STATE_ACTIVE;

				try_disengage_connection(statFlexProc JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
			}
			return res;
		}

		virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override
		{
			jvxHandle** bufsOutput = nullptr;
			jvxSize i;
			jvxSize nChans;
			jvxSize szBytes;
			jvxErrorType res = process_buffer_connection(mt_mask, idx_stage);
			if (res != JVX_NO_ERROR)
			{
				switch (unconnectMode)
				{
				case ayfFlexibleConnectionUnconnectedMode::AYF_FLEXIBLE_CONNECTION_UNCONNECT_SILENT:
					bufsOutput = jvx_process_icon_extract_output_buffers<jvxHandle>(
						CayfAuNConnection<S>::_common_set_ocon.theData_out);
					nChans = CayfAuNConnection<S>::_common_set_ocon.theData_out.con_params.number_channels;
					szBytes = jvxDataFormat_getsize(CayfAuNConnection<S>::_common_set_ocon.theData_out.con_params.format) *
						CayfAuNConnection<S>::_common_set_ocon.theData_out.con_params.buffersize;
					for (i = 0; i < nChans; i++)
					{
						memset(bufsOutput[i], 0, szBytes);
					}
					break;
				case ayfFlexibleConnectionUnconnectedMode::AYF_FLEXIBLE_CONNECTION_UNCONNECT_TALKTHROUGH:
					// Run talkthrough
					return S::process_buffers_icon(mt_mask, idx_stage);
					break;
				default:
					assert(0);
				}
			}
			return CayfAuNConnection<S>::fwd_process_buffers_icon(mt_mask, idx_stage);
		}		

		virtual jvxErrorType startup_and_test_connection(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override
		{
			jvxErrorType res = JVX_NO_ERROR;
			// Let us directly forward the call to the connection

			// The flexible connection can not be connected if there is no active connection yet!!
			if (CayfAuNConnection<S>::_common_set_icon.theData_in == nullptr)
			{
				return JVX_ERROR_NOT_READY;
			}

			jvxState stat = JVX_STATE_NONE;
			CayfAuNConnection<S>::theConnection.state(&stat);
			if (stat == JVX_STATE_NONE)
			{
				// We need to postpone the automatic connection until after full boot to prevent deactivation of components on config
				jvxBool allowConnect = true;
				IjvxHost* theHost = reqInterface<IjvxHost>(CayfAuNConnection<S>::_common_set_min.theHostRef);
				if (theHost)
				{
					theHost->boot_complete(&allowConnect);
				}

				if (allowConnect)
				{
					jvx_initDataLinkDescriptor(&descr_in);
					jvx_initDataLinkDescriptor(&descr_out);

					res = CayfAuNConnection<S>::theConnection.init_connect(AyfConnection::CayfConnectionConfig(
						CayfAuNConnection<S>::_common_set_min.theHostRef, this, CayfAuNConnection<S>::lstMods,
						&descr_in,
						&descr_out,
						ayfConnectionOperationMode::AYF_CONNECTION_FLEXIBLE,
						CayfAuNConnection<S>::nmProcess,
						CayfAuNConnection<S>::descrProcess, CayfAuNConnection<S>::descrorProcess,
						nullptr, nullptr, false, 
						CayfAuNConnection<S>::numBuffers), 
						CayfAuNConnection<S>::stateSwitchHandler);
					if (res != JVX_NO_ERROR)
					{
						std::cout << "Failed to start connection, try it next time!" << std::endl;
					}
					else
					{
						std::cout << "Sub-chain connected!" << std::endl;
						lastTestOk = false;
					}
				}
			}

			res = JVX_ERROR_NOT_READY;
			stat = JVX_STATE_NONE;
			CayfAuNConnection<S>::theConnection.state(&stat);
			if (stat == JVX_STATE_ACTIVE)
			{
				res = JVX_NO_ERROR;
				descr_in.con_params = CayfAuNConnection<S>::_common_set_icon.theData_in->con_params;
				descr_out.con_params = CayfAuNConnection<S>::_common_set_ocon.theData_out.con_params;

				lastTestOk = false;

				// Now, the connection should be available
				jvxErrorType resL = CayfAuNConnection<S>::theConnection.test_connection(JVX_CONNECTION_FEEDBACK_CALL(fdb));
				if (resL == JVX_NO_ERROR)
				{
					lastTestOk = true;
				}
			}
			else
			{
				// If this failed, we can run a talkthrough - we should not allow this!!
				// T::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			}

			try_engage_connection(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			return res;
		}

		virtual jvxErrorType shutdown_connection(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override
		{
			try_disengage_connection(JVX_STATE_ACTIVE JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			CayfAuNConnection<S>::shutdown_connection(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			lastTestOk = false;
			return JVX_NO_ERROR;
		}

		// Engage a connection to be prepared and / or processing targetStateFlexProc defines where to go
		virtual jvxErrorType try_engage_connection(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
		{
			jvxErrorType res = JVX_NO_ERROR;
			jvxState stat = JVX_STATE_NONE;	
			CayfAuNConnection<S>::theConnection.state(&stat);
			if ((stat == JVX_STATE_ACTIVE) && 
				(statFlexProc >= JVX_STATE_PREPARED)) // Target state of the COMPONENT: if the component is in PREPARED or PROCESSING STATE
			{
				if (!lastTestOk)
				{
					res = CayfAuNConnection<S>::theConnection.test_connection(JVX_CONNECTION_FEEDBACK_CALL(fdb));
					if (res == JVX_NO_ERROR)
					{
						lastTestOk = true;
					}
				}
				res = prepare_connection(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			}

			if (res == JVX_NO_ERROR)
			{
				CayfAuNConnection<S>::theConnection.state(&stat);
				if ((stat == JVX_STATE_PREPARED) && 
					(statFlexProc == JVX_STATE_PROCESSING)) // Target state of the COMPONENT : if the component is in PROCESSING STATE
				{
					JVX_LOCK_MUTEX(safeAccessConnectionState);
					res = start_connection(JVX_CONNECTION_FEEDBACK_CALL(fdb));
					JVX_UNLOCK_MUTEX(safeAccessConnectionState);
				}
			}
			return res;
		}

		// Engage a connection to be prepared and / or processing sourceStateFlexProc defines from where to go
		virtual jvxErrorType try_disengage_connection(jvxState targetState JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
		{
			jvxErrorType res = JVX_NO_ERROR;
			jvxState stat = JVX_STATE_NONE;
			CayfAuNConnection<S>::theConnection.state(&stat);
			if ((stat == JVX_STATE_PROCESSING) && 
				(targetState < stat)) // Target state of the COMPONENT : If component state is lower than processing
			{
				JVX_LOCK_MUTEX(safeAccessConnectionState);
				res = stop_connection(JVX_CONNECTION_FEEDBACK_CALL(fdb));
				JVX_UNLOCK_MUTEX(safeAccessConnectionState);
			}

			CayfAuNConnection<S>::theConnection.state(&stat);
			if ((stat == JVX_STATE_PREPARED) && 
				(targetState < stat)) // Target state of the COMPONENT : If component state is lower than prepared
			{
				res = postprocess_connection(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			}
			return res;
		}

	protected:

		virtual jvxErrorType process_buffer_connection(jvxSize mt_mask, jvxSize idx_stage)
		{
			jvxErrorType res = JVX_ERROR_NOT_READY;
			JVX_LOCK_MUTEX(safeAccessConnectionState);
			jvxState stat = JVX_STATE_NONE;
			CayfAuNConnection<S>::theConnection.state(&stat);
			if (stat == JVX_STATE_PROCESSING)
			{
				res = CayfAuNConnection<S>::theConnection.process_connection(
					CayfAuNConnection<S>::_common_set_icon.theData_in, mt_mask, idx_stage,
					&CayfAuNConnection<S>::_common_set_ocon.theData_out);
			}
			JVX_UNLOCK_MUTEX(safeAccessConnectionState);
			return res;
		}

		virtual jvxErrorType prepare_connection(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
		{
			jvxErrorType res = JVX_NO_ERROR;
			res = CayfAuNConnection<S>::theConnection.prepare_connection();
			return res;
		}

		virtual jvxErrorType postprocess_connection(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
		{
			jvxErrorType res = JVX_NO_ERROR;
			res = CayfAuNConnection<S>::theConnection.postprocess_connection();
			return res;
		}

		virtual jvxErrorType start_connection(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
		{
			jvxErrorType res = JVX_NO_ERROR;
			res = CayfAuNConnection<S>::theConnection.start_connection();
			return res;
		}

		virtual jvxErrorType stop_connection(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
		{
			jvxErrorType res = JVX_NO_ERROR;
			res = CayfAuNConnection<S>::theConnection.stop_connection();
			return res;
		}
	};
};

#endif
