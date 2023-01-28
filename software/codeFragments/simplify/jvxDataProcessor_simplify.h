
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_object(IjvxObject** refObject) override
	{
		return(_reference_object(refObject));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_sender_to_receiver(jvxLinkDataDescriptor* theData) override
	{
		jvxErrorType res = JVX_ERROR_WRONG_STATE;
		if(_common_set_min.theState == JVX_STATE_PREPARED)
		{
			res = jvx_allocateDataLinkDescriptor(theData, true);
		}
		return(res);
	};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_complete_receiver_to_sender(jvxLinkDataDescriptor* theData) override
	{
		return(JVX_NO_ERROR);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION supports_multithreading(jvxBool* supports)override
	{
		return(_supports_multithreading(supports));
	};

	/* TODO: Add member functions for multithreading negotiations */

	virtual jvxErrorType JVX_CALLINGCONVENTION process_mt(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender, 
	jvxSize* channelSelect , jvxSize numEntriesChannels, jvxInt32 offset_input, jvxInt32 offset_output, jvxInt32 numEntries) override
	{
		return(_process_mt(theData, idx_sender_to_receiver, idx_receiver_to_sender, channelSelect, numEntriesChannels, offset_input, offset_output, numEntries));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION reconfigured_receiver_to_sender(jvxLinkDataDescriptor* theData)override
	{
		return(JVX_NO_ERROR);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender)override
	{
		return(_process_st(theData, idx_sender_to_receiver, idx_receiver_to_sender));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION last_error_process(char* fld_text, jvxSize fldSize, jvxErrorType* err, jvxInt32* id_error, jvxLinkDataDescriptor* theData)override
	{
		return(_last_error_process(fld_text, fldSize, err, id_error, theData));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_sender_to_receiver(jvxLinkDataDescriptor* theData)override
	{
		jvxErrorType res = JVX_ERROR_WRONG_STATE;
		if(_common_set_min.theState == JVX_STATE_PREPARED)
		{
			res = jvx_deallocateDataLinkDescriptor(theData, true);
		}
		return(res);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION before_postprocess_receiver_to_sender(jvxLinkDataDescriptor* theData)override
	{
		return(JVX_NO_ERROR);
	};

