#ifndef __CJVXEXAMPLEDEVICE_H__
#define __CJVXEXAMPLEDEVICE_H__

#include "jvxCustomTechnologies/CjvxCustomDevice.h"
#include "pcg_exports_example.h"

#include "jvx_threads.h"

#ifdef JVX_OS_LINUX
#include "unistd.h"
#include <fcntl.h>
#endif

#include "jvx_example_field1.h"
#include "jvx_example_field2.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class CjvxExampleDevice: public CjvxCustomDevice, public genExample
{
private:
	
	struct
	{
		jvx_thread_handler fptr;
		jvxHandle* hdl;
	} thread_ascan;

	struct
	{
	    jvx_example_field1 inspField1;
	    jvx_example_field2 inspField2;
	} runtime;

	JVX_THREADS_FORWARD_C_CALLBACK_DECLARE_ALL

public:
	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
	// Component class interface member functions
	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
	JVX_CALLINGCONVENTION CjvxExampleDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxExampleDevice() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION activate() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION start() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop() override;

	// ======================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, 
		const char* filename, jvxInt32 lineno) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_property(
		jvxCallManagerProperties& callGate, 
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans)override;

	//JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(cb_example_pramirq0_read_trigger_pre);

	//JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(cb_example_pramirq0_read_trigger_post);

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(cb_example_field1_read_memory_get_pre);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(cb_example_field2_read_memory_get_pre);

};


#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
