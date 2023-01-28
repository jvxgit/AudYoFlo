#ifndef __IJVXRTAUDIOFILEWRITER_H__
#define __IJVXRTAUDIOFILEWRITER_H__

JVX_INTERFACE IjvxRtAudioFileWriter: public IjvxObject
{
public:

	virtual JVX_CALLINGCONVENTION ~IjvxRtAudioFileWriter(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION  activate(const char* fName, jvxEndpointClass descr, 
		jvxFileDescriptionEndpoint_open* fileDescr, jvxBool allowOverwriteExisting = true) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION add_tag(jvxAudioFileTagType tp, const char* val) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION  deactivate() = 0;

	//! API: Function to prepare recording
	virtual jvxErrorType JVX_CALLINGCONVENTION  prepare(jvxSize internalBuffersize, jvxBool boostPriority, jvxFileDescriptionEndpoint_prepare* fileDescr, jvxFileOperationType mode) = 0;

	//! API: Add a buffer to the internal buffer. Write to file may be delayed.
	virtual jvxErrorType JVX_CALLINGCONVENTION  add_audio_buffer(void** buffers, jvxSize numberChannels, 
		jvxSize numberSamples, jvxBool strictlyNoBlocking, jvxData* fHeight = NULL) = 0;

	//! API: Post process file stream
	virtual jvxErrorType JVX_CALLINGCONVENTION  postprocess() = 0;

	//! API: Initialize the objects
	virtual jvxErrorType JVX_CALLINGCONVENTION  initialize(IjvxHiddenInterface * hostRef) = 0;

	//! API: Terminate the objects, opposite of initialize
	virtual jvxErrorType JVX_CALLINGCONVENTION  terminate() = 0;

	//! API: Start processing of input/output files
	virtual jvxErrorType JVX_CALLINGCONVENTION  start() = 0;

	//! API: Stop processing of input/output files
	virtual jvxErrorType JVX_CALLINGCONVENTION  stop() = 0;

		//! Return the number of channels involved
	virtual jvxErrorType JVX_CALLINGCONVENTION  set_properties_file(jvxSize* numChannels, jvxInt32* samplerate,
		 jvxFileFormat* fformat_file, jvxEndpointClass* endpoint_description, jvxSize* numBitsSample, jvxSize* subtype) = 0;

		virtual jvxErrorType JVX_CALLINGCONVENTION  get_properties_file(jvxSize* numChannels, jvxInt32* samplerate,
			jvxFileFormat* fformat_file, jvxEndpointClass* endpoint_description, jvxSize* numBitsSample, jvxSize* subt) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION  set_properties_processing(
		jvxSize* max_number_read_file, jvxSize* bsize_intern,
		jvxDataFormat* format_proc) = 0;

		virtual jvxErrorType JVX_CALLINGCONVENTION  get_properties_processing(
			jvxSize* max_number_read_file, jvxSize* bsize_intern,
			jvxDataFormat* format_proc) = 0;

	//! Performance measuring: Return the number of elements to be passed to the internal buffer
	virtual jvxErrorType JVX_CALLINGCONVENTION performance(jvxSize* num_elms_to_mod, jvxSize* num_elms_to_file, jvxSize* num_elms_failed, jvxSize* num_elms_min) = 0;

};

#endif
