
#ifndef __CJVXRTAUDIOFILEREADER_H__
#define __CJVXRTAUDIOFILEREADER_H__

#include "jvx.h"
#include "common/CjvxObject.h"
#include "jvxFileReader.h"

class CjvxRtAudioFileReader : public IjvxRtAudioFileReader, public CjvxObject, private jvxFileReader
{
public:
	CjvxRtAudioFileReader(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	~CjvxRtAudioFileReader(void);

	virtual jvxErrorType JVX_CALLINGCONVENTION get_tag(jvxAudioFileTagType tp, jvxApiString* val)override;

	//! API: Initialize the objects
	virtual jvxErrorType JVX_CALLINGCONVENTION  initialize(IjvxHiddenInterface* theHost)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION  activate(const char* fName, jvxEndpointClass descr, jvxFileDescriptionEndpoint_open* fileDescr)override;

	//! Return the number of channels involved
	virtual jvxErrorType JVX_CALLINGCONVENTION  properties_file(jvxSize* numChannels, jvxInt32* samplerate,
		jvxFileFormat* fformat_file, jvxEndpointClass* endpoint_description,
		jvxSize* length_file, jvxSize* numBitsSample, jvxSize* subformat)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION  properties_processing(
		jvxSize* max_number_read_file, jvxSize* bsize_intern,
		jvxDataFormat* format_proc, jvxFileContinueType* continue_type)override;

	//! API: Function to prepare file for incoming data
	virtual jvxErrorType JVX_CALLINGCONVENTION  prepare(jvxInt32 internalBuffersize,
		jvxFileContinueType contTp, jvxBool boostPriority, jvxFileDescriptionEndpoint_prepare* fileDescr, jvxFileOperationType mode)override;

	//! API: Start processing of input/output files
	virtual jvxErrorType JVX_CALLINGCONVENTION  start()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION  restart(jvxInt32 timeout)override;

	//! API: Post process file stream
	virtual jvxErrorType JVX_CALLINGCONVENTION  postprocess()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION  get_audio_buffer(jvxHandle** buffers, jvxSize numberChannels,
		jvxSize numberSamples, jvxBool strictlyNoBlocking, jvxData* progress)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION  progress(jvxSize* myprogress)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION wind_forward(jvxSize timeSteps = 1)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION wind_backward(jvxSize timeSteps = 1)override;

	//! API: Stop processing of input/output files
	virtual jvxErrorType JVX_CALLINGCONVENTION stop()override;

	//! Close the input file
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	//! Performance measuring: Return the number of elements to be passed to the internal buffer
	virtual jvxErrorType JVX_CALLINGCONVENTION performance(jvxSize* num_elms_to_mod, jvxSize* num_elms_to_file, jvxSize* num_elms_failed, jvxSize* num_elms_min)override;

	//! API: Terminate the objects, opposite of initialize
	virtual jvxErrorType JVX_CALLINGCONVENTION terminate()override;

#include "codeFragments/simplify/jvxObjects_simplify.h"
};

#endif
