
#include "jvxNodes/CjvxBareNode1ioRearrange.h"
#include "pcg_exports_node.h"

#include "jvxAudioFixedFftN1Filter/CjvxAudioFixedFftN1Filter.h"

class CjvxSpNSpeaker2Binaural : public CjvxBareNode1ioRearrange, public genSpeaker2Binaural_node
{

private:
	jvxSize idSelectChannel = JVX_SIZE_UNSELECTED;
	jvxBool force2OutputChannels = false;
	CjvxAudioFixedFftN1Filter filtLeft;
	CjvxAudioFixedFftN1Filter filtRight;
	jvxBool engageRendererProc = false;

public:

	JVX_CALLINGCONVENTION CjvxSpNSpeaker2Binaural(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxSpNSpeaker2Binaural();

	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* owner) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	// ===============================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;

	// ===============================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;

	// ===============================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	// ===============================================================

	// Test function
	jvxErrorType test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	// ===============================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	// ===============================================================

	jvxErrorType transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	// ===============================================================
		
	virtual void from_input_to_output() override;
	jvxErrorType accept_input_parameters_stop(jvxErrorType resTest) override;
	jvxErrorType transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	void test_set_output_parameters()override;

	// ===============================================================

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_bypass_mode);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(select_hrtf_file);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_speaker_posthook);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_parameter_speaker);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(get_parameter_speaker);

	void update_bypass_mode(jvxBool updateChain = true);
	jvxErrorType read_hrtf_settings(const std::string& token,
		const std::string& txt,
		const std::string& fName);
	// jvxErrorType try_read_hrtf_file(const std::string& fName);
	void update_properties(jvxBool mayActRendering);
}; 