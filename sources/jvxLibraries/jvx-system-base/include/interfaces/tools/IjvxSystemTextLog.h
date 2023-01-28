#ifndef _IJVXSYSTEMTEXTLOG_H__
#define _IJVXSYSTEMTEXTLOG_H__

JVX_INTERFACE IjvxTextLog: public IjvxObject
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxTextLog(){};
  
	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* hostRef, const char* strFileName,
		jvxSize loclBuffer, jvxSize writeAtOnce, jvxSize circBufferSize, jvxSize dbgLevel) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION addTextLogExpression(const char* oneExppression) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION debug_config(jvxSize* level, const char* moduleName, jvxBool* moduleTextLog) = 0;


	virtual jvxErrorType JVX_CALLINGCONVENTION terminate() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION start() = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop()  = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION addEntry_direct(const char* txt, const char* moduleName = NULL, jvxCBitField outEnum = (jvxCBitField)-1) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION addEntry_buffered(const char* txt, const char* moduleName = NULL, jvxCBitField outEnum = (jvxCBitField)-1) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION addEntry_buffered_nb(const char* txt, const char* moduleName = NULL, jvxCBitField outEnum = (jvxCBitField)-1) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION produceStackframe_direct(const char* description) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION current_size_cbuffer_return(jvxSize* sz, jvxTextLogOutputPositionCircBuffer* pos, jvxSize* numLostBytes) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION update_cbuffer_return(jvxByte* buf, jvxSize* sz, jvxTextLogOutputPositionCircBuffer* pos, jvxSize* numLostBytes) = 0;
};

#endif