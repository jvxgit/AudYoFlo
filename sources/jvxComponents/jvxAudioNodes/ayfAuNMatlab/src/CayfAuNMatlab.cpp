#include "jvx.h"
#include "CayfAuNMatlab.h"

#include "jvx_misc/jvx_printf.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

#ifdef JVX_EXTERNAL_CALL_ENABLED
#include "mcg_export_project.h"
#endif

int
myMexPrintfC(jvxHandle* priv, const char* txt)
{
	CayfAuNMatlab* me = (CayfAuNMatlab*)priv;
	assert(me);
	return me->ic_myMexPrintfC(txt);
}

int
CayfAuNMatlab::ic_myMexPrintfC(const char* txt)
{
	int res = 0;
	this->_report_text_message(txt, JVX_REPORT_PRIORITY_NONE);
	return res;
}

// ====================================================================================================

CayfAuNMatlab::CayfAuNMatlab(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	JVX_LOCAL_BASE_CLASS(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_NODE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	// _common_spec_proc_set.thisisme = static_cast<IjvxObject*>(this);

#ifdef JVX_EXTERNAL_CALL_ENABLED

#endif
	neg_input.preferred.number_channels.min = 2;
	neg_input.preferred.number_channels.max = 2;

#ifndef JVX_ZEROCOPY_BASE_CLASS
	neg_output.preferred.number_channels.min = 2;
	neg_output.preferred.number_channels.max = 2;
#endif
}

CayfAuNMatlab::~CayfAuNMatlab()
{
}

#ifdef JVX_EXTERNAL_CALL_ENABLED
JVX_MEXCALL_SELECT_REGISTER(CayfAuNMatlab, CjvxBareNode1io_rearrange, _theExtCallObjectName)
JVX_MEXCALL_UNSELECT_UNREGISTER(CayfAuNMatlab, CjvxBareNode1io_rearrange, _theExtCallObjectName)
JVX_MEXCALL_PREPARE(CayfAuNMatlab, CjvxBareNode1io_rearrange)
JVX_MEXCALL_POSTPROCESS(CayfAuNMatlab, CjvxBareNode1io_rearrange)
#endif

jvxErrorType
CayfAuNMatlab::activate()
{
	jvxErrorType res = JVX_LOCAL_BASE_CLASS::activate();
	if(res == JVX_NO_ERROR)
	{
#ifdef JVX_EXTERNAL_CALL_ENABLED
		res = CjvxMexCalls::activate();
#endif

		// Starting redirection of jvxprintf
		jvx_set_printf(reinterpret_cast<jvxHandle*>(this), myMexPrintfC);
	}
	return(res);
};

jvxErrorType
CayfAuNMatlab::deactivate()
{
	jvxErrorType res = JVX_LOCAL_BASE_CLASS::deactivate();
	if(res == JVX_NO_ERROR)
	{

#ifdef JVX_EXTERNAL_CALL_ENABLED
		res = CjvxMexCalls::deactivate();
#endif

	}
	return(res);
};

jvxErrorType 
CayfAuNMatlab::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_LOCAL_BASE_CLASS::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		
	}
	return res;
}

void 
CayfAuNMatlab::test_set_output_parameters()
{
	// Set the appropriate output parameters

#ifndef JVX_ZEROCOPY_BASE_CLASS
	// Copy from input side
	update_ldesc_from_output_params_on_test();
#endif
}

jvxErrorType
CayfAuNMatlab::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:

		// To do at this position
		res = JVX_LOCAL_BASE_CLASS::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		break;

	default:
		res = JVX_LOCAL_BASE_CLASS::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
