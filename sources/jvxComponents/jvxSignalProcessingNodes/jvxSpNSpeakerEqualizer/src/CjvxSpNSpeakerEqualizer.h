
#include "jvxNodes/CjvxBareNode1io_zerocopy.h"
#include "pcg_exports_node.h"
#include "jvxAudioFftEqualizer/CjvxAudioFftEqualizer.h"

class CjvxSpNSpeakerEqualizer : public CjvxBareNode1io_zerocopy, public CjvxAudioFftEqualizer, public genSpeakerEqualizer_node
{

private:
	jvxSize idSelectChannel;
	jvxBool proc_eq_involved;
public:

	JVX_CALLINGCONVENTION CjvxSpNSpeakerEqualizer(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxSpNSpeakerEqualizer();

	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* owner)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	void JVX_CALLINGCONVENTION test_set_output_parameters() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;

	// ========================================================================
	// Properties
	// ========================================================================
	
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_channel_posthook);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_parameter_channel_posthook);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(get_parameter_channel_prehook);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(select_eq_file_posthook);


	// ========================================================================
	// Configuration
	// ========================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	void update_properties();
	jvxErrorType read_eq_settings(const std::string& token,
		const std::string& txt,
		const std::string& fName);
	jvxErrorType try_read_eq_file(const std::string& fName);

};