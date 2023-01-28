#ifndef __CAYHAUNSUBCHAIN_H__
#define __CAYHAUNSUBCHAIN_H__

#include "jvx.h"

#include "jvxNodes/CjvxBareNode1io_rearrange.h"

#include "CayfAuNFlexibleConnection.h"
#include "CayfAuNFixedConnection.h"

#include "pcg_CayfAuNSubChain.h"
#include "pcg_CayfAuNSubChainIf.h"

// Realized as a template to allow pull-in of other base classes in the future
template <class R>
class CayfAuNSubChain : public R, public genAuNSubChainIf, public genAuNSubChainFlex
{
private:
public:

	JVX_CALLINGCONVENTION CayfAuNSubChain(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
		R(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
	{
	};

		~CayfAuNSubChain()
		{
		}

		virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* obj)override
		{
			jvxErrorType res = R::select(obj);
			if (res == JVX_NO_ERROR)
			{
				genAuNSubChainIf::init_all();
				genAuNSubChainIf::allocate_all();
				genAuNSubChainIf::register_all(this);

				genAuNSubChainIf::ext_chain_control.ex_interface.ptr = static_cast<IjvxPropertyExtender*>(this);
			}
			return res;
		};

		virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override
		{
			jvxErrorType res = R::unselect();
			if (res == JVX_NO_ERROR)
			{
				genAuNSubChainIf::ext_chain_control.ex_interface.ptr = nullptr;

				genAuNSubChainIf::unregister_all(this);
				genAuNSubChainIf::deallocate_all();
			}
			return res;
		};

		virtual jvxErrorType JVX_CALLINGCONVENTION activate()override
		{
			jvxErrorType res = R::activate();
			if (res == JVX_NO_ERROR)
			{
				if (isFlexibleConnection)
				{
					genAuNSubChainFlex::init_all();
					genAuNSubChainFlex::allocate_all();
					genAuNSubChainFlex::register_all(this);
					genAuNSubChainFlex::register_callbacks(this, trigger_chain, this, nullptr);					
				}
			}
			return(res);
		};

		virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override
		{
			jvxErrorType res = R::deactivate();
			if (res == JVX_NO_ERROR)
			{
				
				if (isFlexibleConnection)
				{
					genAuNSubChainFlex::unregister_callbacks(this, nullptr);
					genAuNSubChainFlex::unregister_all(this);
					genAuNSubChainFlex::deallocate_all();
				}
			}
			return(res);
		};

		// ===================================================================================
		// ===================================================================================

		JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL_H(CayfAuNSubChain, trigger_chain)
		{
			if (isFlexibleConnection)
			{
				if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genAuNSubChainFlex::flexible_chain.engage_chain))
				{
					if (genAuNSubChainFlex::flexible_chain.engage_chain.value != c_false)
					{
						this->startup_and_test_connection(JVX_CONNECTION_FEEDBACK_CALL_NULL);
						genAuNSubChainFlex::flexible_chain.engage_chain.value = c_false;
					}
				}
				if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genAuNSubChainFlex::flexible_chain.disengage_chain))
				{
					if (genAuNSubChainFlex::flexible_chain.disengage_chain.value != c_false)
					{
						this->shutdown_connection(JVX_CONNECTION_FEEDBACK_CALL_NULL);
						genAuNSubChainFlex::flexible_chain.disengage_chain.value = c_false;
					}
				}
			}
			return JVX_NO_ERROR;
		};
	};



#endif
