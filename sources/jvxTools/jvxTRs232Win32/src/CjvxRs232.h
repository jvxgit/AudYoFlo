#ifndef __CJVXRS232_H__
#define __CJVXRS232_H__

#include "jvx.h"
#include "common/CjvxObject.h"

class CjvxRs232;

#define POLL_COM_PORTS_FOR_DETECT
#define POLL_COM_PORTS_MAX_ID 255
//#define JVX_RS232_DEBUG

typedef struct
{
	CjvxRs232* this_pointer;
	jvxSize portId;
} threadEntryStruct;

typedef struct
{
	struct
	{
		std::string friendlyName;
		std::string description;
		std::string shortcut;
		std::string location;
		jvxUInt32 id;
	} init;

	struct
	{
		jvxRs232Config cfg;
		JVX_RS232_HANDLE hdl;
		IjvxConnection_report* theReport;

		struct
		{
			struct
			{

				jvxSize szfld;
				jvxByte* fld;

			} transmit;

			struct
			{

				jvxSize szfld;
				jvxByte* fld;
				jvxSize offset_read;
				jvxSize offset_write;
			} receive;
		} sc;

	} runtime;

	struct
	{
		JVX_THREAD_HANDLE hdl;
		JVX_THREAD_ID id;
		jvxBool inLoopActive;
		threadEntryStruct enter;
	} threads;

	jvxState theState;
	JVX_MUTEX_HANDLE safeAccessConnection;
	
#ifdef JVX_RS232_DEBUG
	struct
	{
		std::string fn;
		JVX_MUTEX_HANDLE safeAccess;
		jvxTimeStampData tRef;
	}dbg;
#endif
} oneRs232Port;

class CjvxRs232: public IjvxConnection, public CjvxObject
{
private:
	struct
	{
		std::vector<oneRs232Port> thePorts;
		jvxSize refCount;
		JVX_MUTEX_HANDLE safeAccess;
	} _system;

public:
	CjvxRs232(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	~CjvxRs232();

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* hostRef, 
		jvxHandle* priv, jvxConnectionPrivateTypeEnum whatsthis) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION availablePorts(jvxApiStringList* allPorts) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION threading_model(jvxConnectionThreadingModel* thread_model) override
	{
		if (thread_model)
			*thread_model = JVX_CONNECT_THREAD_MULTI_THREAD;
		return JVX_NO_ERROR;
	};

	// =============================================================
	// Port specific commands
	// =============================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION state_port(jvxSize idPort, jvxState* theState) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION start_port(jvxSize idPort, jvxHandle* cfg, jvxConnectionPrivateTypeEnum whatsthis, 
		IjvxConnection_report* theReport)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_constraints_buffer(jvxSize idPort, jvxSize* bytesPrepend, 
		jvxSize* fldMinSize, jvxSize* fldMaxSize)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION sendMessage_port(jvxSize idPort, jvxByte* fld, jvxSize* szFld, 
		jvxHandle* priv, jvxConnectionPrivateTypeEnum whatsthis)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION readMessage_port(jvxSize idPort, jvxByte* fld, jvxSize* szFld, jvxHandle* priv,
		jvxConnectionPrivateTypeEnum whatsthis)override
	{
		return JVX_ERROR_UNSUPPORTED;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION scanMessage_port(jvxSize idPort, jvxSize* requiredSize)override
	{
		return JVX_ERROR_UNSUPPORTED;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION stop_port(jvxSize idPort) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION control_port(jvxSize idPort, 
		jvxSize operation_id, jvxHandle* priv, jvxConnectionPrivateTypeEnum whatsthis) override;

	// =============================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION terminate() override;

#include "codeFragments/simplify/jvxObjects_simplify.h"


	// ========================================================
	void receiveLoop(int portId);
	jvxSize numberBytesWrapSend(jvxByte* fld, jvxSize szFld, jvxByte byteStart, jvxByte byteStop, jvxByte byteEsc);
	void insertByteWrapReplace(jvxByte* copyForm, jvxSize szFldFrom, jvxByte* copyTo, jvxSize szFldTo, jvxByte byteStart, 
		jvxByte byteStop, jvxByte byteEsc, jvxSize& written);
	
	jvxSize numberBytesUnwrapReceive(jvxByte* fld, jvxSize offset_read, jvxSize offset_write, jvxByte byteStart, jvxByte byteStop, jvxByte byteEsc, jvxByte& specialChar);
	void removeByteWrapReplace(jvxByte* copyForm, jvxSize offset_f_read, jvxSize offset_f_write, jvxByte* copyTo, jvxSize szFldTo, jvxByte byteStart, jvxByte byteStop, jvxByte byteEsc,
		jvxSize & read, jvxSize & write, jvxByte& theStopChar);

#ifdef JVX_RS232_DEBUG
	void startprintout(jvxSize portId);
	void stoprintout(jvxSize portId);
	void printoutBuffer(jvxSize portId, jvxByte* fld, jvxSize sz, jvxByte startByte, jvxByte stopByte, jvxByte escByte, std::string token);
	void printoutText(jvxSize idPort, std::string token);
#endif

};

#endif
