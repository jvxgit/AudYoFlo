#ifndef __CjvxAuNAnalyze_H__
#define __CjvxAuNAnalyze_H__

#include "jvx.h"
#include "jvxNodes/CjvxBareNode1io.h"
// #include "jvxAudioNodes/CjvxAudioNode_logfile.h"
#include "IjvxIntegrateWaveGenerators.h"

#include "CjvxMultiChannelCircBufferPlotResampler.h"

#include "jvx_spectral_plot.h"
#include "jvx_waveform_plot.h"

#include "pcg_exports_node.h"

// =========================================================================

class CjvxAuNAnalyze: public CjvxBareNode1io /*CjvxAudioNode_logfile*/, 
	public genAnalyzeAudio_node
{
	jvxSpectralPlot theSpectrumMatHdl;
	jvxWaveformPlot theWaveformMatHdl;
	struct
	{
		jvxSize sub_sample_cnt = 0;
		jvxSize sub_sample_factor = 1;		
	} runtime;

public:

	JVX_CALLINGCONVENTION CjvxAuNAnalyze(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxAuNAnalyze();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(var)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	void update_plot_part2();

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(cb_set_plot_params);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(cb_set_props_plotting_td);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(cb_set_props_plotting_fd);
	// JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(cb_install_plot_matrix);
};

#endif

