#ifndef JVX_INPUT_OUTPUT_CONNECTOR_SUPPRESS_AUTOSTART
protected:
	jvxCommonSetNodeAutoStart _common_set_autostart;
#endif

public: 

#ifndef JVX_INPUTOUTPUT_CONNECTOR_OBJECT_REFERENCE
#define JVX_INPUTOUTPUT_CONNECTOR_OBJECT_REFERENCE static_cast<IjvxObject*>(this)
#define JVX_INPUTOUTPUT_CONNECTOR_UNDEFINE_OBJECT_REFERENCE
#endif

#include "jvxInputConnector_simplify.h"
#include "jvxOutputConnector_simplify.h"
#include "jvxConnectorCommon_simplify.h"

#ifndef JVX_INPUT_OUTPUT_SUPPRESS_PREPARE_POSTPROCESS_TO
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override
	{
		jvxErrorType res = JVX_NO_ERROR;
		jvxBool forward = true;

#ifdef JVX_INPUT_OUTPUT_CONNECTOR_MASTER
		forward = false;
#endif

#ifdef JVX_INPUT_OUTPUT_CONNECTOR_BACKWARD_API
		JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb_loc);
		jvxErrorType resF = JVX_NO_ERROR;
#endif

#ifndef JVX_INPUT_OUTPUT_CONNECTOR_SUPPRESS_AUTOSTART

		_common_set_autostart.impPrepareOnChainPrepare = false;
		if (_common_set_autostart.prepareOnChainPrepare)
		{
			if (_common_set_min.theState == JVX_STATE_ACTIVE)
			{
				// Call of implicit prepare
				this->prepare();
				_common_set_autostart.impPrepareOnChainPrepare = true;
			}
		}
#endif
		if (_common_set_min.theState < JVX_STATE_PREPARED)
		{
			return JVX_ERROR_WRONG_STATE;
		}

		_common_set_icon.theData_in->con_pipeline.num_additional_pipleline_stages = JVX_MAX(
			_common_set_icon.theData_in->con_pipeline.num_additional_pipleline_stages,
			_common_set_ldslave.num_additional_pipleline_stages);
		_common_set_icon.theData_in->con_data.number_buffers = JVX_MAX(
			_common_set_icon.theData_in->con_data.number_buffers,
			_common_set_icon.theData_in->con_pipeline.num_additional_pipleline_stages + 1);
		_common_set_icon.theData_in->con_data.number_buffers = JVX_MAX(
			_common_set_icon.theData_in->con_data.number_buffers,
			_common_set_ldslave.num_min_buffers_in);

		// ################################################################
#ifdef JVX_INPUT_OUTPUT_CONNECTOR_BACKWARD_API

#ifdef JVX_INPUTOUTPUTCONNECTOR_VERBOSE
		std::cout << __FUNCTION__ << "::" << __FILE__ << ": " << __LINE__ << ": Input Number buffers = " << _common_set_icon.theData_in->con_data.number_buffers << std::endl;
#endif

		res = _common_set_ldslave.data_processor->prepare_sender_to_receiver(_common_set_icon.theData_in);
#else

		res = jvx_allocate_pipeline_and_buffers_prepare_to(_common_set_icon.theData_in
#ifdef JVX_GLOBAL_BUFFERING_VERBOSE
			, _common_set_ldslave.descriptor.c_str()
#endif
		);

#endif
		// ################################################################

		if (res == JVX_NO_ERROR)
		{

			// The only deviation may be the number of output channels - which is taken from the node parameter set
			_common_set_ocon.theData_out.con_data.number_buffers = _common_set_ldslave.num_min_buffers_out;
			_common_set_ocon.theData_out.con_pipeline.num_additional_pipleline_stages = 0;
			_common_set_ocon.theData_out.con_sync.type_timestamp = _common_set_icon.theData_in->con_sync.type_timestamp;

			// Forward the "forward allocation flags" - JVX_LINKDATA_ALLOCATION_MASK_FORWARD_ELEMENT_TO_ELEMENT is declared in TjvxDataLinkDescriptor.h
			// We can input additional flags by setting _common_set_ocon.theData_out.con_data.alloc_flags in advance
			_common_set_ocon.theData_out.con_data.alloc_flags |= (_common_set_icon.theData_in->con_data.alloc_flags & _common_set_ldslave.fwd_alloc_flags);

			res = _prepare_connect_icon(forward JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

			// ################################################################
#ifdef JVX_INPUT_OUTPUT_CONNECTOR_BACKWARD_API
			if (res == JVX_NO_ERROR)
			{
#ifdef JVX_INPUTOUTPUTCONNECTOR_VERBOSE
				std::cout << __FUNCTION__ << "::" << __FILE__ << ": " << __LINE__ << ": Output Number buffers = " << _common_set_ocon.theData_out.con_data.number_buffers << std::endl;
#endif

				// This is for convenience: support old type of processing principle
				_common_set_icon.theData_in->con_compat.buffersize =
					_common_set_ocon.theData_out.con_params.buffersize;
				_common_set_icon.theData_in->con_compat.format =
					_common_set_ocon.theData_out.con_params.format;
				_common_set_icon.theData_in->con_compat.from_receiver_buffer_allocated_by_sender =
					_common_set_ocon.theData_out.con_data.buffers;
				_common_set_icon.theData_in->con_compat.number_buffers =
					_common_set_ocon.theData_out.con_data.number_buffers;
				_common_set_icon.theData_in->con_compat.number_channels =
					_common_set_ocon.theData_out.con_params.number_channels;
				_common_set_icon.theData_in->con_compat.rate =
					_common_set_ocon.theData_out.con_params.rate;

				_common_set_icon.theData_in->con_compat.ext.segmentation_x =
					_common_set_ocon.theData_out.con_params.segmentation.x;
				_common_set_icon.theData_in->con_compat.ext.segmentation_y =
					_common_set_ocon.theData_out.con_params.segmentation.y;
				_common_set_icon.theData_in->con_compat.ext.subformat =
					_common_set_ocon.theData_out.con_params.format_group;
				_common_set_icon.theData_in->con_compat.ext.hints =
					_common_set_ocon.theData_out.con_params.hints;

				// Here would be the right placce to enque a backward user hint

				// Now, create forwarding of backward oriented user hints
				_common_set_icon.theData_in->con_compat.user_hints = _common_set_ocon.theData_out.con_compat.user_hints;

				res = _common_set_ldslave.data_processor->prepare_complete_receiver_to_sender(_common_set_icon.theData_in);
				if (res != JVX_NO_ERROR)
				{
					goto exit_error_1;
				}
			}
			else
			{
				goto exit_error_0;
			}
#endif
			// ################################################################

		}

		return res;

		// ################################################################
#ifdef JVX_INPUT_OUTPUT_CONNECTOR_BACKWARD_API

	// Error cases
		exit_error_1 :

		resF = _postprocess_connect_icon(forward JVX_CONNECTION_FEEDBACK_CALL_A(fdb_loc));
		jvx_neutralDataLinkDescriptor(_common_set_icon.theData_in, false);

	exit_error_0:

		resF = _common_set_ldslave.data_processor->postprocess_sender_to_receiver(_common_set_icon.theData_in);
		jvx_neutralDataLinkDescriptor(&_common_set_ocon.theData_out, true);

#endif
		// ################################################################

		return res;
	};
#endif

#ifndef JVX_INPUT_OUTPUT_SUPPRESS_PREPARE_POSTPROCESS_TO
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override
	{
		jvxErrorType res = JVX_NO_ERROR;
		jvxBool forward = true;
#ifdef JVX_INPUT_OUTPUT_CONNECTOR_MASTER
		forward = false;
#endif

		assert(_common_set_min.theState >= JVX_STATE_PREPARED);

#ifdef JVX_INPUT_OUTPUT_CONNECTOR_BACKWARD_API
		res = _common_set_ldslave.data_processor->before_postprocess_receiver_to_sender(_common_set_icon.theData_in);
		assert(res == JVX_NO_ERROR);

		_common_set_icon.theData_in->con_compat.user_hints = NULL;

		_common_set_icon.theData_in->con_compat.buffersize = 0;
		_common_set_icon.theData_in->con_compat.format = JVX_DATAFORMAT_NONE;
		_common_set_icon.theData_in->con_compat.from_receiver_buffer_allocated_by_sender = NULL;
		_common_set_icon.theData_in->con_compat.number_buffers = 0;
		_common_set_icon.theData_in->con_compat.number_channels = 0;
		_common_set_icon.theData_in->con_compat.rate = 0;
		_common_set_icon.theData_in->con_compat.ext.hints = 0;
		_common_set_icon.theData_in->con_compat.ext.segmentation_x = 0;
		_common_set_icon.theData_in->con_compat.ext.segmentation_y = 0;
		_common_set_icon.theData_in->con_compat.ext.subformat = JVX_DATAFORMAT_GROUP_NONE;

#endif

		res = _postprocess_connect_icon(forward JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		assert(res == JVX_NO_ERROR);

#ifdef JVX_INPUT_OUTPUT_CONNECTOR_BACKWARD_API

		// Deallocate buffers in theData_in
		res = _common_set_ldslave.data_processor->postprocess_sender_to_receiver(_common_set_icon.theData_in);

#else

		res = jvx_deallocate_pipeline_and_buffers_postprocess_to(_common_set_icon.theData_in);
#endif
		assert(res == JVX_NO_ERROR);

#ifndef JVX_INPUT_OUTPUT_CONNECTOR_SUPPRESS_AUTOSTART
		if (_common_set_autostart.impPrepareOnChainPrepare)
		{
			if (_common_set_min.theState == JVX_STATE_PREPARED)
			{
				// Call of implicit prepare
				this->postprocess();
			}
		}
		_common_set_autostart.impPrepareOnChainPrepare = false;
#endif

		return res;
	};
#endif

	virtual jvxErrorType JVX_CALLINGCONVENTION number_next(jvxSize* num) override
	{
#ifdef JVX_INPUT_OUTPUT_CONNECTOR_MASTER
		if (num)
			*num = 0;
		return JVX_NO_ERROR;
#else
		return _number_next(num);
#endif
	}

	virtual jvxErrorType JVX_CALLINGCONVENTION reference_next(jvxSize idx, IjvxConnectionIterator** next) override
	{
#ifdef JVX_INPUT_OUTPUT_CONNECTOR_MASTER
		if (next)
		{
			*next = nullptr;
		}
		return JVX_ERROR_ID_OUT_OF_BOUNDS;
#else
		return _reference_next(idx, next);
#endif

	}

	virtual jvxErrorType JVX_CALLINGCONVENTION reference_component(
		jvxComponentIdentification* cpTp,
		jvxApiString* modName,
		jvxApiString* lContext) override
	{
		return _reference_component(cpTp, modName, lContext);
	}

#ifdef JVX_INPUTOUTPUT_CONNECTOR_UNDEFINE_OBJECT_REFERENCE
#undef JVX_INPUTOUTPUT_CONNECTOR_OBJECT_REFERENCE
#endif

	