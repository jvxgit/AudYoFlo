#ifndef __CJVXBARENODE1IOMEX_H__
#define __CJVXBARENODE1IOMEX_H__

#include "jvx.h"
#include "jvxNodes/CjvxBareNode1io.h"
#include "CjvxMexCalls.h"

class CjvxBareNode1ioMex: public CjvxBareNode1io, public CjvxMexCalls
{
protected:

	std::string _theExtCallObjectNameBase;
	IjvxExternalCall* _theExtCallHandlerBase;

public:

	JVX_CALLINGCONVENTION CjvxBareNode1ioMex(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxBareNode1ioMex();
	
	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* theOwner)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename = "",jvxInt32 lineno = -1)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;

	// Local callbacks for data processing
	virtual void init_external_call_local(std::string& nmComponent);
	virtual void terminate_external_call_local();
	virtual jvxErrorType process_oplace(jvxLinkDataDescriptor* data_in, jvxLinkDataDescriptor* data_out);
	virtual jvxErrorType process_iplace(jvxLinkDataDescriptor* data_out);

private:

};

#endif
