#ifndef __CAYHAUNMATLAB_H__
#define __CAYHAUNMATLAB_H__

#include "jvx.h"
#define JVX_NODE_TYPE_SPECIFIER_TYPE JVX_COMPONENT_AUDIO_NODE
#define JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR "audio_node"

/*
 * Decide which behavior is desired: A zercopy processing module has the same
 * dataformats for input as for output. That includes samplerate, buffersize, formats
 * as well as segmentation and number of channels.
 * The rearrange version may deviate on the output side from the input side. In most cases, it
 * is only the number of channels that deviates on the output side.
 * Note that - of course, you may use the rearrange version also for zerocopy operation. This,
 * however, requires some specific implementations.
 */
// #define JVX_ZEROCOPY_BASE_CLASS

#ifdef JVX_ZEROCOPY_BASE_CLASS
#include "jvxNodes/CjvxBareNode1io_zerocopy.h"
#define JVX_LOCAL_BASE_CLASS CjvxBareNode1io_zerocopy
#else
#include "jvxNodes/CjvxBareNode1io_rearrange.h"
#define JVX_LOCAL_BASE_CLASS CjvxBareNode1io_rearrange
#endif
// #include "jvxAudioNodes/CjvxAudioNode.h"


#ifdef JVX_EXTERNAL_CALL_ENABLED
#include "CjvxMexCalls.h"
#endif

#include "pcg_exports_node.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class CayfAuNMatlab : public JVX_LOCAL_BASE_CLASS, public genMatlabStart
#ifdef JVX_EXTERNAL_CALL_ENABLED
	, public CjvxMexCalls
#endif
{
private:
public:

		JVX_CALLINGCONVENTION CayfAuNMatlab(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
		~CayfAuNMatlab();

		virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
		virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

		// ===================================================================================
		virtual jvxErrorType test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
		virtual void test_set_output_parameters() override;
		// ===================================================================================

		virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
		virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
		virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;
		jvxErrorType transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

		virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
			IjvxConfigProcessor* processor,
			jvxHandle* sectionToContainAllSubsectionsForMe,
			const char* filename,
			jvxInt32 lineno)override;

		virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
			IjvxConfigProcessor* processor,
			jvxHandle* sectionWhereToAddAllSubsections)override;

		// =======================================================================================
#ifdef JVX_EXTERNAL_CALL_ENABLED

		virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* theOwner)override;
		virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;
		virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;
		virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;

#include "mcg_export_project_prototypes.h"
#endif
		int ic_myMexPrintfC(const char* txt);
	};




	/*
	class CayfAuNHeadRest: public CjvxAudioNode //, public genLoudness
	#ifdef JVX_EXTERNAL_CALL_ENABLED
		, public CjvxMexCalls
	#endif
	{

	#ifdef JVX_EXTERNAL_CALL_ENABLED

	#endif


	public:

		JVX_CALLINGCONVENTION CayfAuNHeadRest(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

		virtual JVX_CALLINGCONVENTION ~CayfAuNHeadRest();

		virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
		virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

		virtual jvxErrorType JVX_CALLINGCONVENTION process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender)override;


		virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
			jvxHandle* sectionToContainAllSubsectionsForMe,
			const char* filename = "", jvxInt32 lineno = -1) override;
		virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf,
			IjvxConfigProcessor* processor,
			jvxHandle* sectionWhereToAddAllSubsections)override;

	#ifdef JVX_EXTERNAL_CALL_ENABLED

		virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* theOwner)override;
		virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;
		virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;
		virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;
		virtual jvxErrorType JVX_CALLINGCONVENTION prepare_complete_receiver_to_sender(jvxLinkDataDescriptor* theData)override;
		virtual jvxErrorType JVX_CALLINGCONVENTION before_postprocess_receiver_to_sender(jvxLinkDataDescriptor* theData)override;

	#include "mcg_export_project_prototypes.h"
	#endif

		virtual jvxErrorType JVX_CALLINGCONVENTION prepare_sender_to_receiver(jvxLinkDataDescriptor* theData)override;
		virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_sender_to_receiver(jvxLinkDataDescriptor* theData)override;

		virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

		int ic_myMexPrintfC(const char* txt);

	private:
	};
	*/


#ifdef JVX_PROJECT_NAMESPACE
}
#endif


#endif
