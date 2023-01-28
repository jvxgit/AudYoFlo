#include "jvxNodes/CjvxBareNode1io_zerocopy.h"
#include "pcg_exports_node.h"

class CjvxSpNSimpleZeroCopy : public CjvxBareNode1io_zerocopy, public genSimple_node
{

private:
public:

	JVX_CALLINGCONVENTION CjvxSpNSimpleZeroCopy(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxSpNSimpleZeroCopy();

};