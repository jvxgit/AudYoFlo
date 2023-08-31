#ifndef CJVXCOMCONHOSTTPL_H__
#define CJVXCOMCONHOSTTPL_H__

// ========================================================================================
template <class T>
class CjvxComConHostTpl : public T
{
public:
	JVX_CALLINGCONVENTION CjvxComConHostTpl(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		T(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
	{
		// Attach cross link to initialize CjvxHostInteraction
		CjvxHostInteraction::hostRefPass = static_cast<IjvxHiddenInterface*>(this);
	};

	virtual JVX_CALLINGCONVENTION ~CjvxComConHostTpl()
	{

	};

	jvxErrorType request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)override
	{
		jvxErrorType res = JVX_NO_ERROR;
		switch (tp)
		{
			// The interfaces are here due to the template argument T=CjvxComponentHostTools

		case JVX_INTERFACE_HOSTTYPEHANDLER:
			if (hdl)
			{
				*hdl = static_cast<IjvxHostTypeHandler*>(this);
			}
			break;	
		
		case JVX_INTERFACE_HOST:
			if (hdl)
			{
				*hdl = static_cast<IjvxHost*>(this);
			}
			break;

		default:

			res = CjvxInterfaceHostTplConnections<IjvxHost, CjvxComponentHostTools>::request_hidden_interface(tp, hdl);

		}
		return(res);
	};

	jvxErrorType return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)
	{
		jvxErrorType res = JVX_NO_ERROR;
		res = JVX_NO_ERROR;
		switch (tp)
		{
			// The interfaces are here due to the template argument T=CjvxComponentHostTools

		case JVX_INTERFACE_HOSTTYPEHANDLER:
			if (hdl == static_cast<IjvxHostTypeHandler*>(this))
			{
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
			break;

		case JVX_INTERFACE_HOST:
			if (hdl == static_cast<IjvxHost*>(this))
			{
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
			break;

		default:
			res = CjvxInterfaceHostTplConnections<IjvxHost, CjvxComponentHostTools>::return_hidden_interface(tp, hdl);

		}
		return res;
	};
	
	// Second part is IjvxComponentHost realized by mix-in!
#include "codeFragments/simplify/jvxComponentHost_simplify.h"

	// ====================================================================================
	// Interface IjvxToolsHost
	// ====================================================================================

#include "codeFragments/simplify/jvxComponentHostTools_simplify.h"


#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"

#include "codeFragments/simplify/jvxHostInteraction_simplify.h"
#include "codeFragments/simplify/jvxHostTypeHandler_simplify.h"
#include "codeFragments/simplify/jvxDataConnections_simplify.h"


};

#endif
