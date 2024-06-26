
#define JVX_NODE_TYPE_SPECIFIER_TYPE JVX_COMPONENT_AUDIO_NODE
#define JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR "audio_node"

#include "jvxNodes/CjvxBareNode1ioRearrange.h"
#include "pcg_exports_node.h"

#include "HjvxMicroConnection.h"
#include "CayfAuNFixedConnection.h"

#define JVX_LOCAL_BASE_CLASS AyfConnection::CayfAuNFixedConnection<CjvxBareNode1ioRearrange>

class CjvxAuNBitstreamEncoder: 
	public JVX_LOCAL_BASE_CLASS,
	public IayfConnectionStateSwitchNode	
{
private:
	
	std::map<jvxSize, refComp<IjvxAudioCodec> > lstCodecInstances;
	jvxSize id_selected = JVX_SIZE_UNSELECTED;

	std::string config_token;
	IjvxAudioCodec* ptrCodec = nullptr;
	IjvxAudioEncoder* theEncoder = nullptr;
	IjvxProperties* theEncoderProps = nullptr;

	jvxSize uIdl = JVX_SIZE_UNSELECTED;
	jvxSize refCountHost = 1;
	jvxLinkDataDescriptor decoderDescrIn;
	jvxLinkDataDescriptor decoderDescrOut;

public:

	JVX_CALLINGCONVENTION CjvxAuNBitstreamEncoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxAuNBitstreamEncoder();

#if 0
	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* owner)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;
#endif

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	// ===================================================================================
	jvxErrorType disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	// ===================================================================================
	virtual jvxErrorType test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	// ===================================================================================	

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(get_processing_monitor);

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;
	
	jvxErrorType activate_encoder();
	jvxErrorType deactivate_encoder();

	virtual jvxErrorType runStateSwitch(jvxStateSwitch ss, IjvxSimpleNode* node, const char* moduleName, IjvxObject* theOwner = nullptr) override;
	virtual jvxErrorType componentsAboutToConnect() override;
	virtual jvxErrorType runTestChainComplete(jvxErrorType lastResult, IjvxSimpleNode* node, const char* moduleName, jvxSize uniqueId) override;

};
