#ifndef __CjvxLinkDataSlave_H__
#define __CjvxLinkDataSlave_H__
#if 0
class CjvxLinkDataSlave
{
private:
	typedef enum
	{
		JVX_LINK_DATA_SLAVE_NOT_STARTED,
		JVX_LINK_DATA_SLAVE_LOCAL_INIT_COMPLETE,
		JVX_LINK_DATA_SLAVE_NEXT_INIT_COMPLETE,
		JVX_LINK_DATA_SLAVE_LOCAL_TERM_COMPETE
	} jvxStatusLinkDataSlave;

protected:
	struct
	{
		IjvxDataProcessor* data_processor;
		IjvxObject* object;
		IjvxLinkDataSlave* meSlave;
		jvxStatusLinkDataSlave myState;
	} _common_set_ldslave;

	CjvxLinkDataSlave()
	{
		_common_set_ldslave.data_processor = NULL;
		_common_set_ldslave.object = NULL;
		_common_set_ldslave.meSlave = NULL;
		_common_set_ldslave.myState = JVX_LINK_DATA_SLAVE_NOT_STARTED;
 	};
	
	~CjvxLinkDataSlave()
	{
	};
	
	jvxErrorType lds_activate(IjvxDataProcessor* theDataProc, IjvxObject* theObjRef, IjvxLinkDataSlave* meSlave)
	{
		jvxErrorType res = JVX_NO_ERROR;
		_common_set_ldslave.data_processor = theDataProc;
		_common_set_ldslave.object = theObjRef;
		_common_set_ldslave.meSlave = meSlave;
		return res;
	};

	jvxErrorType lds_deactivate()
	{
		jvxErrorType res = JVX_NO_ERROR;
		_common_set_ldslave.data_processor = NULL;
		_common_set_ldslave.object = NULL;
		_common_set_ldslave.meSlave = NULL;
		return res;
	};

	jvxErrorType _prepare_connect(jvxLinkDataDescriptor* theData) 
	{
		jvxErrorType res = JVX_NO_ERROR;

		// Call function which fills in buffer and initialize the processing
		if (_common_set_ldslave.myState == JVX_LINK_DATA_SLAVE_NOT_STARTED)
		{
			res = _common_set_ldslave.data_processor->prepare_sender_to_receiver(theData);
			_common_set_ldslave.myState = JVX_LINK_DATA_SLAVE_LOCAL_INIT_COMPLETE;
		}

		if (res == JVX_NO_ERROR)
		{
			if (_common_set_ldslave.myState == JVX_LINK_DATA_SLAVE_LOCAL_INIT_COMPLETE)
			{
				if (theData->link.next)
				{
					// Create new dataLinkDescriptor for next stage
					theData->link.descriptor_out_linked = new jvxLinkDataDescriptor;

					// Intialize descriptor
					jvx_initDataLinkDescriptor(theData->link.descriptor_out_linked);

					// Copy descriptor values from prevous object. We might do something else here!
					jvx_copyDataLinkDescriptor(theData->link.descriptor_out_linked, theData);

					// Copy pointer to user hints
					theData->link.descriptor_out_linked->con_data.user_hints = theData->con_data.user_hints;

					// Set cros references for double linked list
					theData->link.descriptor_out_linked->link.previous = _common_set_ldslave.meSlave;
					theData->link.descriptor_out_linked->con_link.master = theData->con_link.master;
					theData->link.descriptor_out_linked->con_link.tp_master = theData->con_link.tp_master;

					// TODO: Where do we get the propper next pointer? At the moment, we trust what was filled in
					res = theData->link.next->prepare_connect(theData->link.descriptor_out_linked);

					if (res == JVX_NO_ERROR)
					{

						// This is for convenience: support old type of processing principle
						theData->con_compat.buffersize =
							theData->link.descriptor_out_linked->con_params.buffersize;
						theData->con_compat.format =
							theData->link.descriptor_out_linked->con_params.format;
						theData->con_compat.from_receiver_buffer_allocated_by_sender =
							theData->link.descriptor_out_linked->con_data.buffers;
						theData->con_compat.number_buffers =
							theData->link.descriptor_out_linked->con_data.number_buffers;
						theData->con_compat.number_channels =
							theData->link.descriptor_out_linked->con_params.number_channels;
						theData->con_compat.rate =
							theData->link.descriptor_out_linked->con_params.rate;
						theData->con_compat.ext =
							theData->link.descriptor_out_linked->con_data.ext;

						// Here would be the right placce to enque a backward user hint

						// Now, create forwarding of backward oriented user hints
						theData->con_compat.user_hints = theData->link.descriptor_out_linked->con_compat.user_hints;

					}

					_common_set_ldslave.myState = JVX_LINK_DATA_SLAVE_NEXT_INIT_COMPLETE;
				}
			}
		}

		if (res == JVX_NO_ERROR)
		{
			if (_common_set_ldslave.myState == JVX_LINK_DATA_SLAVE_NEXT_INIT_COMPLETE)
			{
				// We might do something here - in particular we might switch buffers to e.g. directly use the output buffers
				res = _common_set_ldslave.data_processor->prepare_complete_receiver_to_sender(theData);
				_common_set_ldslave.myState = JVX_LINK_DATA_SLAVE_LOCAL_TERM_COMPETE;
			}
		}
		return res;
	};
	
	jvxErrorType _prepare_connect_no_check(jvxLinkDataDescriptor* theData)
	{
		jvxErrorType res = JVX_NO_ERROR;

		// Call function which fills in buffer and initialize the processing
		res = _common_set_ldslave.data_processor->prepare_sender_to_receiver(theData);
		if (res == JVX_NO_ERROR)
		{
			if (theData->link.next)
			{
				// Create new dataLinkDescriptor for next stage
				theData->link.descriptor_out_linked = new jvxLinkDataDescriptor;

				// Intialize descriptor
				jvx_initDataLinkDescriptor(theData->link.descriptor_out_linked);

				// Copy descriptor values from prevous object. We might do something else here!
				jvx_copyDataLinkDescriptor(theData->link.descriptor_out_linked, theData);

				// Copy pointer to user hints
				theData->link.descriptor_out_linked->con_data.user_hints = theData->con_data.user_hints;

				// Set cros references for double linked list
				theData->link.descriptor_out_linked->link.previous = _common_set_ldslave.meSlave;
				theData->link.descriptor_out_linked->con_link.master = theData->con_link.master;
				theData->link.descriptor_out_linked->con_link.tp_master = theData->con_link.tp_master;

				// TODO: Where do we get the propper next pointer? At the moment, we trust what was filled in
				res = theData->link.next->prepare_connect(theData->link.descriptor_out_linked);

				if (res == JVX_NO_ERROR)
				{

					// This is for convenience: support old type of processing principle
					theData->con_compat.buffersize =
						theData->link.descriptor_out_linked->con_params.buffersize;
					theData->con_compat.format =
						theData->link.descriptor_out_linked->con_params.format;
					theData->con_compat.from_receiver_buffer_allocated_by_sender =
						theData->link.descriptor_out_linked->con_data.buffers;
					theData->con_compat.number_buffers =
						theData->link.descriptor_out_linked->con_data.number_buffers;
					theData->con_compat.number_channels =
						theData->link.descriptor_out_linked->con_params.number_channels;
					theData->con_compat.rate =
						theData->link.descriptor_out_linked->con_params.rate;
					theData->con_compat.ext =
						theData->link.descriptor_out_linked->con_data.ext;

					// Here would be the right placce to enque a backward user hint

					// Now, create forwarding of backward oriented user hints
					theData->con_compat.user_hints = theData->link.descriptor_out_linked->con_compat.user_hints;

				}
			}
		}

		if (res == JVX_NO_ERROR)
		{
			// We might do something here - in particular we might switch buffers to e.g. directly use the output buffers
			res = _common_set_ldslave.data_processor->prepare_complete_receiver_to_sender(theData);
		}
		return res;
	};

	jvxErrorType _postprocess_connect(jvxLinkDataDescriptor* theData)
	{
		jvxErrorType res = JVX_NO_ERROR;

		if (_common_set_ldslave.myState >= JVX_LINK_DATA_SLAVE_LOCAL_TERM_COMPETE)
		{
			res = _common_set_ldslave.data_processor->before_postprocess_receiver_to_sender(theData);
			// assert(res == JVX_NO_ERROR); // If this might fail, we would have a severe problem
			_common_set_ldslave.myState = JVX_LINK_DATA_SLAVE_NEXT_INIT_COMPLETE;
		}

		if (_common_set_ldslave.myState >= JVX_LINK_DATA_SLAVE_NEXT_INIT_COMPLETE)
		{
			theData->con_compat.buffersize = 0;
			theData->con_compat.format = JVX_DATAFORMAT_NONE;
			theData->con_compat.from_receiver_buffer_allocated_by_sender = NULL;
			theData->con_compat.number_buffers = 0;
			theData->con_compat.number_channels = 0;
			theData->con_compat.rate = 0;
			theData->con_compat.ext.hints = 0;
			theData->con_compat.ext.segmentation_x = 0;
			theData->con_compat.ext.segmentation_y = 0;
			theData->con_compat.ext.subformat = JVX_DATAFORMAT_GROUP_NONE;
			theData->con_compat.user_hints = NULL;

			if (theData->link.next)
			{
				res = theData->link.next->postprocess_connect(theData->link.descriptor_out_linked);
				//assert(res == JVX_NO_ERROR);

				// Reset pointer references
				theData->link.descriptor_out_linked->link.next = NULL;
				theData->link.descriptor_out_linked->con_link.master = NULL;

				delete theData->link.descriptor_out_linked;
				theData->link.descriptor_out_linked = NULL;
			}
			_common_set_ldslave.myState = JVX_LINK_DATA_SLAVE_LOCAL_INIT_COMPLETE;

		}

		if (_common_set_ldslave.myState >= JVX_LINK_DATA_SLAVE_LOCAL_INIT_COMPLETE)
		{
			res = _common_set_ldslave.data_processor->postprocess_sender_to_receiver(theData);
			//assert(res == JVX_NO_ERROR); // This should never fail
			_common_set_ldslave.myState = JVX_LINK_DATA_SLAVE_NOT_STARTED;
		}
		return res;
	};

	jvxErrorType _postprocess_connect_no_check(jvxLinkDataDescriptor* theData)
	{
		jvxErrorType res = JVX_NO_ERROR;

		res = _common_set_ldslave.data_processor->before_postprocess_receiver_to_sender(theData);
		// assert(res == JVX_NO_ERROR); // If this might fail, we would have a severe problem

		theData->con_compat.buffersize = 0;
		theData->con_compat.format = JVX_DATAFORMAT_NONE;
		theData->con_compat.from_receiver_buffer_allocated_by_sender = NULL;
		theData->con_compat.number_buffers = 0;
		theData->con_compat.number_channels = 0;
		theData->con_compat.rate = 0;
		theData->con_compat.ext.hints = 0;
		theData->con_compat.ext.segmentation_x = 0;
		theData->con_compat.ext.segmentation_y = 0;
		theData->con_compat.ext.subformat = JVX_DATAFORMAT_GROUP_NONE;
		theData->con_compat.user_hints = NULL;

		if (theData->link.next)
		{
			res = theData->link.next->postprocess_connect(theData->link.descriptor_out_linked);
			//assert(res == JVX_NO_ERROR);

			// Reset pointer references
			theData->link.descriptor_out_linked->link.next = NULL;
			theData->link.descriptor_out_linked->con_link.master = NULL;

			delete theData->link.descriptor_out_linked;
			theData->link.descriptor_out_linked = NULL;
		}

		res = _common_set_ldslave.data_processor->postprocess_sender_to_receiver(theData);
		//assert(res == JVX_NO_ERROR); // This should never fail
		
		return res;
	};
	
	jvxErrorType _process_start(jvxLinkDataDescriptor* theData)
	{
		jvxErrorType res = JVX_NO_ERROR;

		// Propagate to next object
		if (theData->link.next)
		{
			res = theData->link.next->process_start(theData->link.descriptor_out_linked);
		}

		theData->con_compat.idx_receiver_to_sender = theData->link.descriptor_out_linked->pipeline.idx_stage;

		return res;
	};

	jvxErrorType _process_buffers(jvxLinkDataDescriptor* theData)
	{
		jvxErrorType res = JVX_NO_ERROR;
		res = _common_set_ldslave.data_processor->process_st(theData, theData->pipeline.idx_stage, theData->con_compat.idx_receiver_to_sender);

		if (res == JVX_NO_ERROR)
		{
			if (theData->link.next)
			{
				// Copy the admin data
				theData->link.descriptor_out_linked->admin = theData->admin;
				res = theData->link.next->process_buffers(theData->link.descriptor_out_linked);
			}
		}
		return res;
	};

	jvxErrorType _process_stop(jvxLinkDataDescriptor* theData)
	{
		jvxErrorType res = JVX_NO_ERROR;

		if (theData->link.next)
		{
			res = theData->link.next->process_stop(theData);
		}

		return res;
	};

	jvxErrorType _start_connect(jvxLinkDataDescriptor* theData)
	{
		jvxErrorType res = JVX_NO_ERROR;

		// Do whatever you like
		if (theData->link.next)
		{
			res = theData->link.next->start_connect(theData);
		}
		return res;
	};
	
	jvxErrorType _stop_connect(jvxLinkDataDescriptor* theData)
	{
		jvxErrorType res = JVX_NO_ERROR;
		if (theData->link.next)
		{
			res = theData->link.next->stop_connect(theData);
		}

		// Do whatever you like
		return res;
	};

	jvxErrorType _link_request_reference_object(IjvxObject** obj)
	{
		jvxErrorType res = JVX_NO_ERROR;
		if (obj)
		{
			*obj = _common_set_ldslave.object;
		}
		return res;
	};

	jvxErrorType _link_return_reference_object(IjvxObject* obj)
	{
		jvxErrorType res = JVX_NO_ERROR;
		return res;
	};
};
#endif
#endif
