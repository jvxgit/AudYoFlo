#ifndef _CJVXRTAUDIOFILEWRITER_H__
#define _CJVXRTAUDIOFILEWRITER_H__

#include "jvx.h"
#include "common/CjvxObject.h"
#include "jvxFileWriter.h"

class CjvxRtAudioFileWriter :	public IjvxRtAudioFileWriter, public CjvxObject, private jvxFileWriter
{
public:
	CjvxRtAudioFileWriter(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	~CjvxRtAudioFileWriter();

	virtual jvxErrorType JVX_CALLINGCONVENTION  activate(const char* fName, jvxEndpointClass descr, 
		jvxFileDescriptionEndpoint_open* fileDescr, jvxBool allowOverwriteExisting)override;

	virtual jvxErrorType add_tag(jvxAudioFileTagType tp, const char* val)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION  deactivate()override;

	//! API: Function to prepare recording
	virtual jvxErrorType JVX_CALLINGCONVENTION  prepare(jvxSize internalBuffersize, 
		jvxBool boostPriority, jvxFileDescriptionEndpoint_prepare* fileDescr, jvxFileOperationType mode)override;

	//! API: Add a buffer to the internal buffer. Write to file may be delayed.
	virtual jvxErrorType JVX_CALLINGCONVENTION  add_audio_buffer(void** buffers, jvxSize numberChannels, 
		jvxSize numberSamples, jvxBool strictlyNoBlocking, jvxData* bFillHeight_scale) override;

	//! API: Post process file stream
	virtual jvxErrorType JVX_CALLINGCONVENTION  postprocess()override;

	//! API: Initialize the objects
	virtual jvxErrorType JVX_CALLINGCONVENTION  initialize(IjvxHiddenInterface* theHost)override;

	//! API: Terminate the objects, opposite of initialize
	virtual jvxErrorType JVX_CALLINGCONVENTION  terminate()override;

	//! API: Start processing of input/output files
	virtual jvxErrorType JVX_CALLINGCONVENTION  start()override;

	//! API: Stop processing of input/output files
	virtual jvxErrorType JVX_CALLINGCONVENTION  stop()override;

	//! Return the number of channels involved
virtual jvxErrorType JVX_CALLINGCONVENTION  set_properties_file(jvxSize* numChannels, jvxInt32* samplerate,
	jvxFileFormat* fformat_file, jvxEndpointClass* endpoint_description, jvxSize* numBitsSample, jvxSize* subtformat)override;

		//! Return the number of channels involved
	virtual jvxErrorType JVX_CALLINGCONVENTION  get_properties_file(jvxSize* numChannels, jvxInt32* samplerate,
		jvxFileFormat* fformat_file, jvxEndpointClass* endpoint_description, jvxSize* numBitsSample, jvxSize* subtformat)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION  set_properties_processing(
		jvxSize* max_number_read_file, jvxSize* bsize_intern,
		jvxDataFormat* format_proc)override;

		virtual jvxErrorType JVX_CALLINGCONVENTION  get_properties_processing(
			jvxSize* max_number_read_file, jvxSize* bsize_intern,
			jvxDataFormat* format_proc)override;

	//! Performance measuring: Return the number of elements to be passed to the internal buffer
	virtual jvxErrorType JVX_CALLINGCONVENTION performance(jvxSize* num_elms_to_mod, jvxSize* num_elms_to_file, 
		jvxSize* num_elms_failed, jvxSize* num_elms_min)override;

#include "codeFragments/simplify/jvxObjects_simplify.h"
};


#endif
