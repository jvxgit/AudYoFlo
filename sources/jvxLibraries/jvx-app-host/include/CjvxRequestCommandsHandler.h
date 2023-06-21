#ifndef __CJVXREQUESTCOMMANDSHANDLER_H__
#define __CJVXREQUESTCOMMANDSHANDLER_H__

#include "CjvxAppFactoryHostBase.h"

/**
 * This class was introduced to standardize the handing of the command request triggers arriving.
 */ // =======================================================================================================

JVX_INTERFACE CjvxHandleRequestCommands_callbacks
{
public:
	virtual ~CjvxHandleRequestCommands_callbacks() {};

	/**
	 * If the request to run a sequencer step immediately, this callback is triggered.
	 */
	virtual void trigger_immediate_sequencerStep() = 0;

	/** 
	 * For the remaining command requests, the trigger is stored in the request queue and will
	 * be postponed. This way, the request always comes out in a delayed fashion - even if the 
	 * request was triggered from within the main thread. */
	virtual void trigger_threadChange_forward(CjvxReportCommandRequest* ptr_to_copy) = 0;
	
	/**
	 * If a test request was attached to the queue and all tests runs were completed, the succesful test is
	 * reported to all connected listeners.
	 */
	virtual void run_mainthread_triggerTestChainDone() = 0;

	/*
	 * This callback is called if the component list of a technology has changed. The technology is passed as cpId.
	 * Typically, we end up here since the the request is delayed into the request event queue.
	 */
	virtual void run_mainthread_updateComponentList(jvxComponentIdentification cpId) = 0;

	/*
	 * This callback is called if the properties have changed.
	 * Typically, we end up here since the the request is delayed into the request event queue.
	 */
	virtual void run_mainthread_updateProperties(jvxComponentIdentification cpId) = 0;

	/*
	 * This callback is called if the system state has changed.
	 * Typically, we end up here since the the request is delayed into the request event queue.
	 */
	virtual void run_mainthread_updateSystemStatus() = 0;

};

class CjvxRequestCommandsHandler : public IjvxReportSystemForward
{
public:
	// Host handle set and removed in
	// 1) boot_initialize_base
	// 2) shutdown_terminate_base
	IjvxHost* hostRef = nullptr;

	// Host handle set and removed in
	// 1) on_components_before_configure
	// 2) shutdownHostFactory
	IjvxReportSystem* automationReport = nullptr;

	// Pointer to couple the application and its member function to the code from CjvxHandleRequestCommands.
	// Could be assigned in uMainWindow::uMainWindow()
	CjvxHandleRequestCommands_callbacks* reportRef = nullptr;
	
	// ================================================================
	CjvxRequestCommandsHandler();

	virtual void request_command_in_main_thread(CjvxReportCommandRequest* request, jvxBool removeAfterHandle = true) override;

	jvxErrorType _request_command(const CjvxReportCommandRequest& request, jvxBool verbose = true);
	
	void _run_all_tests();
};

#endif