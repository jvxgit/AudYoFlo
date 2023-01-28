#ifndef __CJVXNETWORKSLAVETECHNOLOGY_H__
#define __CJVXNETWORKSLAVETECHNOLOGY_H__

#include "jvxAudioTechnologies/CjvxAudioTechnology.h"
#include "CjvxNetworkTechnology.h"

#include "jvx_connect_server_st.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class CjvxNetworkSlaveTechnology: public CjvxNetworkTechnology
{
	friend class CjvxNetworkSlaveDevice;

	jvx_connect_server_family* socketServerFam;

public:

	JVX_CALLINGCONVENTION CjvxNetworkSlaveTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxNetworkSlaveTechnology();
	virtual jvxErrorType initializeUnderlyingConnection() override;
	virtual IjvxDevice* allocateOneDevice(const char* fldTxt, jvxSize idx) override;
};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif