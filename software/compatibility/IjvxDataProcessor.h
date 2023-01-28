#ifndef __IJVXDATAPROCESSOR_H__
#define __IJVXDATAPROCESSOR_H__

// Minimal variant required for data processing on small platforms
JVX_INTERFACE IjvxDataProcessor_core
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxDataProcessor_core() {};

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_sender_to_receiver(jvxLinkDataDescriptor* theData) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_complete_receiver_to_sender(jvxLinkDataDescriptor* theData) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_st(jvxLinkDataDescriptor* theData,
		jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_sender_to_receiver(jvxLinkDataDescriptor* theData) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION before_postprocess_receiver_to_sender(jvxLinkDataDescriptor* theData) = 0;
};

// Extension if desired to run on larger platforms
JVX_INTERFACE IjvxDataProcessor: public IjvxDataProcessor_core
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxDataProcessor(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION reference_object(IjvxObject** refObject) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION supports_multithreading(jvxBool* supports) = 0;

	// Reconfigure: is it really necessary? I do not know, only portaudio uses it actually
	virtual jvxErrorType JVX_CALLINGCONVENTION reconfigured_receiver_to_sender(jvxLinkDataDescriptor* theData) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_mt(jvxLinkDataDescriptor* theData, 
		jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender, jvxSize* channelSelect, 
jvxSize numEntriesChannels, jvxInt32 offset_input, jvxInt32 offset_output, jvxInt32 numEntries) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION last_error_process(char* fld_text, jvxSize fldSize, jvxErrorType* err, jvxInt32* id_error, jvxLinkDataDescriptor* theData) = 0; 

	/* 
	Name modifications:

	request_data_link_sender_to_receiver_allocate -> prepare_sender_to_receiver
	report_data_link_receiver_to_sender_allocated -> prepare_complete_receiver_to_sender
	report_data_link_receiver_to_sender_changed -> reconfigured_receiver_to_sender
	request_data_link_sender_to_receiver_deallocate -> postprocess_sender_to_receiver
	report_data_link_receiver_to_sender_about_to_deallocate -> before_postprocess_receiver_to_sender
	*/


};

#endif