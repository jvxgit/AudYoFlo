#ifndef __CAYFAUNFIXEDCONNECTION_H__
#define __CAYFAUNFIXEDCONNECTION_H__

#include "jvx.h"
#include "CayfAuNFlexibleConnection.h"

namespace AyfConnection
{	
	template <class S>
		class CayfAuNFixedConnection :
		public CayfAuNConnection<S>,
		public HjvxMicroConnection_hooks_simple,
		public HjvxMicroConnection_hooks_fwd
	{
	public:

		JVX_CALLINGCONVENTION CayfAuNFixedConnection(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
			CayfAuNConnection<S>(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
		{
		};

		~CayfAuNFixedConnection()
		{
		}

		virtual jvxErrorType JVX_CALLINGCONVENTION system_ready() override
		{
			// Do not connect any chains in this module before we have fully booted	
			// Invoke a "test" run on system startup
			if (CayfAuNConnection<S>::_common_set_icon.theData_in)
			{
				this->_request_test_chain_master(CayfAuNConnection<S>::_common_set_icon.theData_in->con_link.uIdConn);
			}
			return JVX_NO_ERROR;
		};

		// ===================================================================================
		virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override
		{
			JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, static_cast<IjvxObject*>(CayfAuNConnection<S>::_common_set_io_common.object),
				CayfAuNConnection<S>::_common_set_io_common.descriptor.c_str(), "Entering input connector <CayfAuNFixedConnection>");

			JVX_CONNECTION_FEEDBACK_ON_ENTER_LINKDATA_TEXT_I(fdb, (CayfAuNConnection<S>::_common_set_icon.theData_in));

			jvxErrorType res = startup_and_test_connection(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			return res;
		};

		virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override
		{
			jvxState stat = JVX_STATE_NONE;
			CayfAuNConnection<S>::theConnection.state(&stat);
			if (stat == JVX_STATE_ACTIVE)
			{
				CayfAuNConnection<S>::shutdown_connection(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			}
			return CayfAuNConnection<S>::disconnect_connect_icon(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		};

		virtual void test_set_output_parameters() override
		{
			// This part has been dealt with before in this class
		}

		jvxErrorType transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override
		{
			jvxErrorType res = CayfAuNConnection<S>::theConnection.transfer_forward_connection(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			if (res == JVX_NO_ERROR)
			{
				res = CayfAuNConnection<S>::_transfer_forward_icon(true, tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}
			return res;
		};

		// ===================================================================================

		virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override
		{
			jvxErrorType res = JVX_NO_ERROR;

			// This includes forwarding towards the next elements
			CayfAuNConnection<S>::prepare_autostart();
			res = CayfAuNConnection<S>::theConnection.prepare_connection();
			return res;

		};

		virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override
		{
			jvxErrorType res = JVX_NO_ERROR;

			res = CayfAuNConnection<S>::theConnection.postprocess_connection();
			CayfAuNConnection<S>::postprocess_autostart();

			return res;
		};

		virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override
		{
			CayfAuNConnection<S>::start_autostart();
			return CayfAuNConnection<S>::theConnection.start_connection();
		};

		virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override
		{
			jvxErrorType res = CayfAuNConnection<S>::theConnection.stop_connection();
			CayfAuNConnection<S>::stop_autostart();
			return res;
		};

		// ===========================================================================

		virtual jvxErrorType JVX_CALLINGCONVENTION process_start_icon(
			jvxSize pipeline_offset,
			jvxSize* idx_stage,
			jvxSize tobeAccessedByStage,
			callback_process_start_in_lock clbk,
			jvxHandle* priv_ptr) override
		{
			return CayfAuNConnection<S>::theConnection.prepare_process_connection(nullptr,
				pipeline_offset,
				idx_stage,
				tobeAccessedByStage,
				clbk,
				priv_ptr);
		};

		virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_icon(
				jvxSize idx_stage,
				jvxBool operate_first_call,
				jvxSize tobeAccessedByStage,
				callback_process_stop_in_lock cb,
				jvxHandle* priv_ptr) override
		{
			return CayfAuNConnection<S>::theConnection.postprocess_process_connection(
				idx_stage,
				operate_first_call,
				tobeAccessedByStage,
				cb,
				priv_ptr);
		}


		// ===========================================================================

		virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage) override
		{
			jvxErrorType res = JVX_NO_ERROR;

			// Run the decode microconnection
			return CayfAuNConnection<S>::theConnection.process_connection(nullptr);
		};

		jvxErrorType transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override
		{
			jvxErrorType res = JVX_ERROR_UNSUPPORTED;
			switch (tp)
			{
			case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:

				// To do at this position
				res = CayfAuNConnection<S>::theConnection.negotiate_connection(tp, data,
											       CayfAuNConnection<S>::_common_set_ocon.theData_out.con_params JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				break;

			default:
			  res = CayfAuNConnection<S>::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}
			return res;
		};


		// =======================================================================================


		// Interface HjvxMicroConnection_hooks_simple
		virtual jvxErrorType hook_test_negotiate(jvxLinkDataDescriptor* proposed JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override
		{
			// If the micro chain start settings do not match, we need to forward then towards the previous linked object
			jvxErrorType res = JVX_ERROR_INVALID_SETTING;
			if (CayfAuNConnection<S>::_common_set_icon.theData_in)
			{
				res = CayfAuNConnection<S>::_common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(
					JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS,
					proposed JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				if (res == JVX_NO_ERROR)
				{
					refresh_test_connection(JVX_CONNECTION_FEEDBACK_CALL(fdb));
				}
			}
			return res;
		};

		/* This is the endpoint of the micro connection. In dataIn, the call gets the output constraints of the
		 * final element in the chain.
		 */
		virtual jvxErrorType hook_test_accept(jvxLinkDataDescriptor* dataIn  JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override
		{
			jvxErrorType res = hook_test_update(dataIn  JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			return res;
		};

		virtual jvxErrorType hook_test_update(jvxLinkDataDescriptor* dataIn  JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override
		{
			// Copy output settings in case the negoatiate from behind the next element has made modifications
			CayfAuNConnection<S>::_common_set_ocon.theData_out.con_params = dataIn->con_params;

			return JVX_NO_ERROR;
		};

		virtual jvxErrorType hook_check_is_ready(jvxBool* is_ready, jvxApiString* astr) override
		{
			// In this callback we may specify a condition that this component is NOT ready
			return JVX_NO_ERROR;
		};

		virtual jvxErrorType hook_forward(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override
		{
			// In this callback we may accept forwarded messages
			// return CayfAuNConnection<S>::_trans_icon(true JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			return JVX_NO_ERROR;
		};

		// virtual jvxErrorType hook_text_chain() override;
		// ========================================================================
		// ========================================================================

		virtual jvxErrorType hook_prepare(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override
		{
			// Durectly forward the call
			return CayfAuNConnection<S>::_prepare_connect_icon(true JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}

		;
		virtual jvxErrorType hook_postprocess(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override
		{
			return CayfAuNConnection<S>::_postprocess_connect_icon(true JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}

		;
		virtual jvxErrorType hook_start(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override
		{
			jvxSize idRuntime = JVX_SIZE_UNSELECTED;
			CayfAuNConnection<S>::_common_set.theUniqueId->obtain_unique_id(&idRuntime, CayfAuNConnection<S>::_common_set.theDescriptor.c_str());
			return CayfAuNConnection<S>::_start_connect_icon(true, idRuntime JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}

		;
		virtual jvxErrorType hook_stop(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override
		{
			return CayfAuNConnection<S>::_stop_connect_icon(true, nullptr JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}

		virtual jvxErrorType hook_process_start(
				jvxSize pipeline_offset,
				jvxSize* idx_stage,
				jvxSize tobeAccessedByStage,
				callback_process_start_in_lock clbk,
				jvxHandle* priv_ptr) override
		{
			jvxErrorType res = JVX_NO_ERROR;
			if (CayfAuNConnection<S>::_common_set_ocon.ocon)
			{
				// Input / output connector implementations are never zerocopy
				// We start a regular output connector here
				res = CayfAuNConnection<S>::_common_set_ocon.ocon->process_start_ocon(
					pipeline_offset,
					idx_stage,
					tobeAccessedByStage,
					clbk,
					priv_ptr);
			}
			return res;
		}

		virtual jvxErrorType hook_process(jvxSize mt_mask, jvxSize idx_stage) override
		{
			jvxErrorType res = JVX_NO_ERROR;
			if (CayfAuNConnection<S>::_common_set_ocon.ocon)
			{
				return CayfAuNConnection<S>::_common_set_ocon.ocon->process_buffers_ocon(mt_mask, idx_stage);
			}
			return res;
		}

		virtual jvxErrorType hook_process_stop(
				jvxSize idx_stage,
				jvxBool shift_fwd,
				jvxSize tobeAccessedByStage,
				callback_process_stop_in_lock clbk,
				jvxHandle* priv_ptr) override
		{
			jvxErrorType res = JVX_NO_ERROR;
			if (CayfAuNConnection<S>::_common_set_ocon.ocon)
			{
				// Input / output connector implementations are never zerocopy
				// We start a regular output connector here
				res = CayfAuNConnection<S>::_common_set_ocon.ocon->process_stop_ocon(
					idx_stage,
					shift_fwd,
					tobeAccessedByStage,
					clbk,
					priv_ptr);
			}
			return res;
		}

		virtual jvxErrorType startup_and_test_connection(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override			
		{
			jvxErrorType res = JVX_NO_ERROR;
			// Let us directly forward the call to the connection
			jvxState stat = JVX_STATE_NONE;
			CayfAuNConnection<S>::theConnection.state(&stat);
			if (stat == JVX_STATE_NONE)
			{
				// If we have a booting host, we need to wait until system has been bootet since 
				// components may be deactivated during boot process
				jvxBool allowConnect = true;
				IjvxHost* theHost = reqInterface<IjvxHost>(CayfAuNConnection<S>::_common_set_min.theHostRef);
				if (theHost)
				{
					theHost->boot_complete(&allowConnect);
				}

				if (allowConnect)
				{
					jvxSize idProcDepends = JVX_SIZE_UNSELECTED;
					if (CayfAuNConnection<S>::_common_set_icon.theData_in)
					{
						idProcDepends = CayfAuNConnection<S>::_common_set_icon.theData_in->con_link.uIdConn;
					}
					res = CayfAuNConnection<S>::theConnection.init_connect(AyfConnection::CayfConnectionConfig(
						CayfAuNConnection<S>::_common_set_min.theHostRef, this,
						CayfAuNConnection<S>::lstMods,
						CayfAuNConnection<S>::_common_set_icon.theData_in,
						&CayfAuNConnection<S>::_common_set_ocon.theData_out,
						ayfConnectionOperationMode::AYF_CONNECTION_EFFICIENT,
						CayfAuNConnection<S>::nmProcess, CayfAuNConnection<S>::descrProcess,
						CayfAuNConnection<S>::descrorProcess, this, this, false, CayfAuNConnection<S>::numBuffers, true), 
						CayfAuNConnection<S>::stateSwitchHandler,
						idProcDepends, true);
					if (res != JVX_NO_ERROR)
					{
						std::cout << "Failed to start connection, try it next time!" << std::endl;
					}
					else
					{
						std::cout << "Sub-chain connected!" << std::endl;
					}
				}
			}

			// ================================================================================

			res = JVX_ERROR_NOT_READY;
			CayfAuNConnection<S>::theConnection.state(&stat);
			if (stat == JVX_STATE_ACTIVE)
			{
				// Now, the connection should be available.
				// This call propagates through the micro connection and stops at the input from this
				// component as the micro connection endpoint. For elements following the micro connection
				// we need to forward this test call towards the next element outside the micro chain!!
				res = CayfAuNConnection<S>::theConnection.test_connection(JVX_CONNECTION_FEEDBACK_CALL(fdb));
				if (res == JVX_NO_ERROR)
				{
					// Found in class CjvxBareNode1io
					update_simple_params_from_ldesc();

					// From here we approach everything following the micro chain!!
					res = CayfAuNConnection<S>::_test_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));					
				}
			}
			else
			{
				// If this failed, we can run a talkthrough - we should not allow this!!
				// T::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			}
			return res;
		}
	
		virtual void refresh_test_connection(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) //override
		{
			// Found in class CjvxBareNode1io
			update_simple_params_from_ldesc();
		}
	};

}


#endif
