#ifndef __CJVXBARENODE1IO_ZEROCOPY_H__
#define __CJVXBARENODE1IO_ZEROCOPY_H__

#include "jvxNodes/CjvxBareNode1io.h"
#include "common/CjvxNegotiate.h"

class CjvxBareNode1io_zerocopy : public CjvxBareNode1io
{
protected:
	struct
	{
		jvxSize num_iochans;
		jvxSize bsize_io;
		jvxSize rate_io;
		jvxDataFormat form_io;
		jvxDataFormatGroup subform_io;
		jvxBool forward_complain;

	}  _common_set_1io_zerocopy;

public:

	JVX_CALLINGCONVENTION CjvxBareNode1io_zerocopy(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;

	virtual jvxErrorType accept_negotiate_output(jvxLinkDataTransferType tp, jvxLinkDataDescriptor* preferredByOutput JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
	virtual jvxErrorType accept_input_parameters_stop(jvxErrorType resTest) override;
	virtual void from_input_to_output() override;
};

#endif
