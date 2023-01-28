#ifndef __IJVXTEXT2SPEECH_H__
#define __IJVXTEXT2SPEECH_H__

JVX_INTERFACE IjvxText2Speech_report
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxText2Speech_report(){};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION task_complete(jvxByte* fld, jvxSize szOutput, 
		jvxText2SpeechFormat out_format, jvxSize uniqueId) = 0;
		
	virtual jvxErrorType JVX_CALLINGCONVENTION generation_failed(jvxSize uniqueId) = 0;
	
};

JVX_INTERFACE IjvxText2Speech: public IjvxObject
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxText2Speech(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* theHostRef) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION add_task(const char* txtFld, jvxText2SpeechFormat* out_format, IjvxText2Speech_report* backward_ref, jvxSize* uniqueId) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION remove_task(jvxSize uniqueId) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION terminate() = 0;

};

#endif