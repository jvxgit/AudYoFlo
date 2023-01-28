#ifndef _HJVXJSONTOOLS_H__
#define _HJVXJSONTOOLS_H__

#include "jvx-helpers.h"

namespace jvx {
	namespace helper {
		namespace json {
			jvxErrorType commandRequest2Json(const CjvxReportCommandRequest& request, std::string& transToken);
		}
	}
}

#endif
