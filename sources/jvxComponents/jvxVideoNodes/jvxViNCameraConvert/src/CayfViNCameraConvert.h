#include "jvxNodes/CjvxBareNode1ioRearrange.h"
#include "jvxVideoNodes/CjvxVideoNodeTpl.h"

class CayfViNCameraConvert: public CjvxVideoNodeTpl<CjvxBareNode1ioRearrange>
{
public:
	JVX_CALLINGCONVENTION CayfViNCameraConvert(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CayfViNCameraConvert();

	jvxErrorType test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(var))override; 
	jvxErrorType transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override; 
};