#ifndef __CJVXRS232_H__
#define __CJVXRS232_H__

#include "jvx.h"
#include "common/CjvxObject.h"

class CjvxRs232;

#define POLL_COM_PORTS_FOR_DETECT
#define POLL_COM_PORTS_MAX_ID 255

// #define JVX_RS232_DEBUG
// #define JVX_RS232_DEBUG_TXT
#define JVX_RS232_VERBOSE_PORT_CONFIG_STARTUP

typedef struct
{
    CjvxRs232* this_pointer;
    jvxSize portId;
}jvxRs232PrivateThread;

typedef struct
{
    /*
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
		
      JVX_RS232_HANDLE hdl;
      IjvxConnection_report* theReport;

		

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
    */
    std::string fileNamePath;
    std::string friendlyName;
    int fd;
    IjvxConnection_report* theReport;
    jvxState theState;
    jvxRs232Config cfg;
    int line_status;
    JVX_MUTEX_HANDLE safeAccessConnection;
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

    struct
    {
	JVX_THREAD_HANDLE hdl;
	JVX_THREAD_ID id;
        JVX_THREAD_HANDLE hdls;
        JVX_THREAD_ID ids;
        jvxBool inLoopActive;
	jvxRs232PrivateThread* enter;
        int eventfd;

	JVX_NOTIFY_HANDLE threadStart;
    } threads;

    struct
    {
        jvxTimeStampData tRef;
        std::string fn;
        JVX_MUTEX_HANDLE safeAccess;
    } dbg;

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

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* hostRef, jvxHandle* priv, jvxConnectionPrivateTypeEnum whatsthis);

	virtual jvxErrorType JVX_CALLINGCONVENTION availablePorts(jvxApiStringList* allPorts);

	virtual jvxErrorType JVX_CALLINGCONVENTION threading_model(jvxConnectionThreadingModel* thread_model)
	{
		if (thread_model)
			*thread_model = JVX_CONNECT_THREAD_MULTI_THREAD;
		return JVX_NO_ERROR;
	};

	// =============================================================
	// Port specific commands
	// =============================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION state_port(jvxSize idPort, jvxState* theState);

	virtual jvxErrorType JVX_CALLINGCONVENTION start_port(jvxSize idPort, jvxHandle* cfg, jvxConnectionPrivateTypeEnum whatsthis, IjvxConnection_report* theReport);

	virtual jvxErrorType JVX_CALLINGCONVENTION get_constraints_buffer(jvxSize idPort, jvxSize* bytesPrepend, jvxSize* fldMinSize, jvxSize* fldMaxSize);

	virtual jvxErrorType JVX_CALLINGCONVENTION sendMessage_port(jvxSize idPort, jvxByte* fld, jvxSize* szFld, jvxHandle* priv, jvxConnectionPrivateTypeEnum whatsthis);

	virtual jvxErrorType JVX_CALLINGCONVENTION readMessage_port(jvxSize idPort, jvxByte* fld, jvxSize* szFld, jvxHandle* priv, jvxConnectionPrivateTypeEnum whatsthis)
	{
		return JVX_ERROR_UNSUPPORTED;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION scanMessage_port(jvxSize idPort, jvxSize* requiredSize)
	{
		return JVX_ERROR_UNSUPPORTED;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION stop_port(jvxSize idPort);

	virtual jvxErrorType JVX_CALLINGCONVENTION control_port(jvxSize idPort, jvxSize operation_id, jvxHandle* priv, jvxConnectionPrivateTypeEnum whatsthis);

	// =============================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION terminate();

#include "codeFragments/simplify/jvxObjects_simplify.h"


	// ========================================================
	void receiveLoop(int portId);
    void signalLoop(int portId);

#ifdef JVX_RS232_DEBUG
	void startprintout(jvxSize portId);
	void stoprintout(jvxSize portId);
	void printoutBuffer(jvxSize portId, jvxByte* fld, jvxSize sz, jvxByte startByte, jvxByte stopByte, jvxByte escByte, std::string token);
	void printoutText(jvxSize idPort, std::string token);
#endif
};

#endif
