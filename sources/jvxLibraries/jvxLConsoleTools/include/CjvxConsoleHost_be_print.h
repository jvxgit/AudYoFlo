#ifndef __CJVXCONSOLE_HOST_BE_H__
#define __CJVXCONSOLE_HOST_BE_H__

#include "jvx.h"
#include "interfaces/console/IjvxEventLoop_backend_ctrl.h"
#include "CjvxJson.h"
#include "typedefs/console/TjvxFrontendBackend.h"

#define JVX_CONSOLE_FILENAME ".jvx_console"
#define JVX_NUMBER_HISTORY_STORE 20
#define JVX_PRINT_DRIVEHOST_TAB "   "


#include <map>

// =====================================================================================================
class CjvxConsoleHost_be_print: public IjvxEventLoop_backend_ctrl
{

protected:

	IjvxEventLoopObject* evLop;
	jvxState theState;
	std::string internaltextbuffer;
	std::vector<std::string> internaltextbuffer_mem;
	int cntMem;
	jvxSize insert_position;
	std::string linestart;
	std::string highlight_left;
	std::string highlight_right;
	jvxOneFrontendAndState linkedPriFrontend;
	std::list<jvxOneFrontendAndState> linkedSecFrontends;

	jvxOneBackendAndState linkedPriBackend;
	std::list<jvxOneBackendAndState> linkedSecBackends;

	typedef struct
	{
		jvxInterfaceType tp;
		std::list<jvxHandle*> theIfs;
	} oneRegisteredInterface;

	std::map<jvxInterfaceType, oneRegisteredInterface> registeredInterfaceTypes;
	std::string user_input;
	jvxBool arrowupdownmode; 

	JVX_THREAD_ID threadIdMainLoop = JVX_THREAD_ID_INVALID;

	jvxBool config_noquit = false;

	jvxErrorType startupErrorCode = JVX_NO_ERROR;
public:

	CjvxConsoleHost_be_print();
	~CjvxConsoleHost_be_print();

	virtual jvxErrorType JVX_CALLINGCONVENTION add_reference_event_loop(IjvxEventLoopObject* eventLoop) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_reference_event_loop(IjvxEventLoopObject* eventLoop) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_pri_reference_frontend(IjvxEventLoop_frontend* theFrontend) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_pri_reference_frontend(IjvxEventLoop_frontend* theFrontend) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION add_sec_reference_frontend(IjvxEventLoop_frontend* theFrontend) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_sec_reference_frontend(IjvxEventLoop_frontend* theFrontend)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_pri_reference_backend(IjvxEventLoop_backend* thebackend) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_pri_reference_backend(IjvxEventLoop_backend* thebackend) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION add_sec_reference_backend(IjvxEventLoop_backend* thebackend) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_sec_reference_backend(IjvxEventLoop_backend* thebackend)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION add_external_interface(jvxInterfaceType tp, jvxHandle* theIf) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_external_interface(jvxInterfaceType tp, jvxHandle* theIf) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION start_receiving() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_receiving() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION wants_restart(jvxBool* wantsRestart)override;

	// ==================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION process_event(
		TjvxEventLoopElement* elm
		/*
		jvxSize message_id, IjvxEventLoop_frontend* origin, jvxHandle* priv_fe,
		jvxHandle* param, jvxSize paramType,
		jvxSize event_type, jvxOneEventClass event_class, 
		jvxOneEventPriority event_priority, jvxCBool autoDeleteOnProcess,
		jvxHandle* privBlock, jvxBool* reschedule*/
	) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_register_be_commandline(IjvxCommandLine* comLine)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_readout_be_commandline(IjvxCommandLine* comLine)override;

	virtual void process_init(IjvxCommandLine* comLine);
	virtual void process_help_command(IjvxEventLoop_frontend* origin);
	virtual void process_full_command(
		const std::string& command,
		jvxSize message_id, jvxHandle* param,
		jvxSize paramType, jvxOneEventClass event_class,
		jvxOneEventPriority event_priority,
		IjvxEventLoop_frontend* origin, 
		jvxHandle* privBlock, jvxCBitField* extFlags);
	virtual jvxErrorType process_shutdown();

private:
	void store_entry_console_list(const std::string& entry);
	void handle_arrow_up();
	void handle_arrow_down();
};

#endif
