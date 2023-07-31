#ifndef __CJVXCONNECTIONHOST_H__
#define __CJVXCONNECTIONHOST_H__

#include "jvx.h"
#include "common/CjvxObject.h"
#include "jvxHosts/CjvxDataConnections.h"
#include "jvxFactoryHosts/CjvxDefaultInterfaceFactory.h"
#include "jvxHosts/CjvxMinHost.h"
#include "jvxHosts/CjvxUniqueId.h"

class CjvxConnectionMinHost: 
	public CjvxDefaultInterfaceFactory<IjvxMinHost>,
	public IjvxDataConnections, public CjvxDataConnections,
	public IjvxReportSystem
{
public:

	JVX_CALLINGCONVENTION CjvxConnectionMinHost(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxConnectionMinHost();

	// ================================================================================================
	// Interface IjvxCoreStateMachine
	// ================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* theOtherHost) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* theOwner) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION activate() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION terminate() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION owner(IjvxObject** dependsOn) override;

	// ====================================================================================
	// Interface IjvxHiddenInterface
	// ====================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION object_hidden_interface(IjvxObject** objRef) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl) override;

	// ====================================================================================
	// Interface IjvxReportSystem
	// ====================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION request_command(const CjvxReportCommandRequest& request) override;

#include "codeFragments/simplify/jvxObjects_simplify.h"
#include "codeFragments/simplify/jvxDataConnections_simplify.h"
#include "codeFragments/simplify/jvxMinHost_simplify.h"
// #include "codeFragments/simplify/jvxUniqueId_simplify.h"


};

#endif
