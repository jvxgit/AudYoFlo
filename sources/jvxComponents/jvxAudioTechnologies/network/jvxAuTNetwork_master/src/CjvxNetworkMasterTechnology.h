#ifndef __CJVXNETWORKMASTERTECHNOLOGY_H__
#define __CJVXNETWORKMASTERTECHNOLOGY_H__

#include "jvxAudioTechnologies/CjvxAudioTechnology.h"
#include "CjvxNetworkTechnology.h"

#include "jvx_connect_client_st.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class CjvxNetworkMasterTechnology: public CjvxNetworkTechnology
{
	friend class CjvxNetworkMasterDevice;
	jvx_connect_client_family* socketClientFam;

public:

	JVX_CALLINGCONVENTION CjvxNetworkMasterTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxNetworkMasterTechnology();
	virtual jvxErrorType initializeUnderlyingConnection() override;
	virtual IjvxDevice* allocateOneDevice(const char* fldTxt, jvxSize idx) override;
};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif