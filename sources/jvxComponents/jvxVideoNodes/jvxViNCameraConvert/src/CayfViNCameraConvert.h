#include "jvxNodes/CjvxBareNode1ioRearrange.h"
#include "jvxVideoNodes/CjvxVideoNodeTpl.h"

#include "pcg_exports_node.h"

class CayfViNCameraConvert: public CjvxVideoNodeTpl<CjvxBareNode1ioRearrange>, public genCameraConvert_node
{
	struct
	{
		
	} convert;

	struct
	{
		jvxBool convertFormat = false;
		jvxBool convertSize = false;

		struct
		{
			jvxDataFormatGroup from = jvxDataFormatGroup::JVX_DATAFORMAT_GROUP_NONE;
			jvxDataFormatGroup to = jvxDataFormatGroup::JVX_DATAFORMAT_GROUP_NONE;
			jvxByte* fldTmp = nullptr;
			jvxSize szTmp = 0;
			jvxSize width = 0;
			jvxSize height = 0;
			jvxSize szFldIn = 0;
			jvxSize szFldOut = 0;
			struct
			{
				jvxSize plane1_Sz = 0;
				jvxSize plane2_Sz = 0;
			} NV12;
		} convert;

	} runtime;

public:

	JVX_CALLINGCONVENTION CayfViNCameraConvert(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CayfViNCameraConvert();

	/*
	jvxErrorType connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	jvxErrorType disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	*/

	jvxErrorType activate()override;
	jvxErrorType deactivate()override;

	jvxErrorType test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(var))override; 
	jvxErrorType transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override; 
	jvxErrorType prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	jvxErrorType process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage) override;
	
	void from_input_to_output() override;
	jvxErrorType accept_negotiate_output(jvxLinkDataTransferType tp, jvxLinkDataDescriptor* preferredByOutput JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_config);
};