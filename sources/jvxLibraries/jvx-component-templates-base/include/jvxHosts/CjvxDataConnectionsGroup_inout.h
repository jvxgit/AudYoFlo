#ifndef __CJVXDATACONNECTIONSGROUP_INOUT_H__
#define __CJVXDATACONNECTIONSGROUP_INOUT_H__

#include "common/CjvxConnectionMaster.h"
#include "common/CjvxInputOutputConnector.h"
#include "common/CjvxConnectorFactory.h"
#include "common/CjvxObject.h"

#include "jvxHosts/CjvxDataConnectionsGroup_types.h"

// ================================================================================================
template<class T> class oneIOConnector  
{
public:
	T* iocon;
	jvxSize refCnt;
	oneIOConnector()
	{
		iocon = NULL;
		refCnt = 0;
	};
};

class CjvxDataConnectionsGroup_external : public IjvxConnectorFactory
{
	friend class CjvxDataConnectionsGroup;

public:

private:
	
	IjvxObject* refObject;
	std::map<jvxHandle*, oneIOConnector<IjvxInputConnector> > input_connectors;
	std::map<jvxHandle*, oneIOConnector<IjvxOutputConnector> > output_connectors;

public:
	CjvxDataConnectionsGroup_external();
	~CjvxDataConnectionsGroup_external();

	jvxErrorType set_object_ref(IjvxObject* objRefPass);

	jvxErrorType add_outside_input_connector(IjvxInputConnector* icon);
	jvxErrorType add_outside_output_connector(IjvxOutputConnector* icon);

	jvxErrorType rem_outside_input_connector(IjvxInputConnector* icon);
	jvxErrorType rem_outside_output_connector(IjvxOutputConnector* ocon);

	virtual jvxErrorType JVX_CALLINGCONVENTION request_reference_object(IjvxObject** obj) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_object(IjvxObject* obj) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION number_input_connectors(jvxSize* num_in_connectors) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_input_connector(jvxSize idx, IjvxInputConnector** ref)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_input_connector(IjvxInputConnector* ref) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION number_output_connectors(jvxSize* num_out_connectors) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_output_connector(jvxSize idx, IjvxOutputConnector** ref) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_output_connector(IjvxOutputConnector* ref) override;
};

#endif
