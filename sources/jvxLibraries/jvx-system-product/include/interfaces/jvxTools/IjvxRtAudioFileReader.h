#ifndef __IJVXRTAUDIOFILEREADER_H__
#define __IJVXRTAUDIOFILEREADER_H__

JVX_INTERFACE IjvxRtAudioFileReader: public IjvxObject
{
public:	
	
	virtual JVX_CALLINGCONVENTION ~IjvxRtAudioFileReader(){};
	
	//! API: Initialize the objects
	virtual jvxErrorType JVX_CALLINGCONVENTION  initialize(IjvxHiddenInterface* hostRef) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION activate(const char* fName, jvxEndpointClass descr, jvxFileDescriptionEndpoint_open* fileDescr) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_tag(jvxAudioFileTagType tp, jvxApiString* val) = 0;

	//! Return the number of channels involved
		virtual jvxErrorType JVX_CALLINGCONVENTION  properties_file(jvxSize* numChannels, jvxInt32* samplerate, 
		jvxFileFormat* fformat_file, jvxEndpointClass* endpoint_description, 
		jvxSize* length_file, jvxSize* numBitsSample, jvxSize* subtype) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION  properties_processing(
		jvxSize* max_number_read_file, jvxSize* bsize_intern,
		jvxDataFormat* format_proc, jvxFileContinueType* continue_type) = 0;


	//! API: Function to prepare file for incoming data
	virtual jvxErrorType JVX_CALLINGCONVENTION  prepare(jvxInt32 internalBuffersize, 
		jvxFileContinueType contTp, jvxBool boostPriority, jvxFileDescriptionEndpoint_prepare* fileDescr, jvxFileOperationType mode) = 0;

	//! API: Start processing of input/output files
	virtual jvxErrorType JVX_CALLINGCONVENTION  start() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION  restart(jvxInt32 timeout) = 0;

	//! API: Post process file stream
	virtual jvxErrorType JVX_CALLINGCONVENTION  postprocess() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION  get_audio_buffer(jvxHandle** buffers, jvxSize numberChannels, 
		jvxSize numberSamples, jvxBool strictlyNoBlocking, jvxData* progress = NULL) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION  progress(jvxSize* myprogress) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION wind_forward(jvxSize timeSteps = 1) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION wind_backward(jvxSize timeSteps = 1) = 0;

	//! API: Stop processing of input/output files
	virtual jvxErrorType JVX_CALLINGCONVENTION stop() = 0;

	//! Close the input file
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate() = 0;

	//! Performance measuring: Return the number of elements to be passed to the internal buffer
	virtual jvxErrorType JVX_CALLINGCONVENTION performance(jvxSize* num_elms_to_mod, jvxSize* num_elms_to_file, jvxSize* num_elms_failed, jvxSize* num_elms_min) = 0;

	//! API: Terminate the objects, opposite of initialize
	virtual jvxErrorType JVX_CALLINGCONVENTION terminate() = 0;

};

#endif
