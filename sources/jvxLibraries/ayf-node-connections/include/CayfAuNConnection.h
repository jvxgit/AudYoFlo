#ifndef __CAYFAUNCONNECTION_H__
#define __CAYFAUNCONNECTION_H__

#include "CayfConnection.h"

namespace AyfConnection
{
	template <class T>
	class CayfAuNConnection : public T, public IjvxPropertyExtender, public IjvxPropertyExtenderChainControl
	{
	protected:
		AyfConnection::CayfConnection theConnection;
		std::list<AyfConnection::CayfConnectionComponent> lstMods;
		jvxSize uid = 1;
		IayfConnectionStateSwitchNode* stateSwitchHandler = nullptr;
		jvxBool isFlexibleConnection = false;
		std::string nmProcess = "simple-connection-chain";
		std::string descrProcess = "Simple Connection";
		std::string descrorProcess = "simpleConnection";
		jvxSize numBuffers = 1;

	public:
		CayfAuNConnection(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
			T(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
		{
		  T::_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_NODE);
		  T::_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
		  T::_common_set.thisisme = static_cast<IjvxObject*>(this);
		};

		virtual jvxErrorType shutdown_connection(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
		{
			theConnection.disconnect_terminate();
			return JVX_NO_ERROR;
		};

		virtual jvxErrorType JVX_CALLINGCONVENTION prop_extender_type(jvxPropertyExtenderType* tp) override
		{
			if (tp)
			{
				*tp = jvxPropertyExtenderType::JVX_PROPERTY_EXTENDER_CHAIN_CONTROL;
			}
			return JVX_NO_ERROR;
		};

		virtual jvxErrorType JVX_CALLINGCONVENTION prop_extender_specialization(jvxHandle** prop_extender, jvxPropertyExtenderType tp) override			
		{
			jvxErrorType res = JVX_ERROR_UNSUPPORTED;
			switch (tp)
			{
			case jvxPropertyExtenderType::JVX_PROPERTY_EXTENDER_CHAIN_CONTROL:
				if (prop_extender)
				{
					*prop_extender = static_cast<IjvxPropertyExtenderChainControl*>(this);
				}
				res = JVX_NO_ERROR;
				break;
			default:
				break;
			}
			return res;
		};

		virtual jvxErrorType JVX_CALLINGCONVENTION set_stateswitch_node_handler(IayfConnectionStateSwitchNode* ssCb) override
		{
			stateSwitchHandler = ssCb;
			return JVX_NO_ERROR;
		};

		virtual jvxErrorType JVX_CALLINGCONVENTION attach_component_chain(
			const char* str, jvxComponentType tp, jvxBool activateBeforeAttach,
			jvxSize* uniqueId = nullptr, IjvxReferenceSelector* deciderArg = nullptr) override
		{
			jvxSize uIdLoc = uid++;
			lstMods.push_back(AyfConnection::CayfConnectionComponent(str, tp, false, uIdLoc, deciderArg));
			if (uniqueId)
				*uniqueId = uIdLoc;
			return JVX_NO_ERROR;
		};

		virtual jvxErrorType JVX_CALLINGCONVENTION attach_component_chain(
			const char* str, IjvxObject* theObj, jvxSize* uniqueId = nullptr) override
		{
			jvxSize uIdLoc = uid++;
			lstMods.push_back(AyfConnection::CayfConnectionComponent(str, theObj, false, uIdLoc));
			if (uniqueId)
				*uniqueId = uIdLoc;
			return JVX_NO_ERROR;
		};

		virtual jvxErrorType JVX_CALLINGCONVENTION detach_component_chain(const char* modName, jvxSize uniqueId = JVX_SIZE_UNSELECTED) override
		{
			jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
			for (auto elm = lstMods.begin(); elm != lstMods.end(); elm++)
			{
				if (JVX_CHECK_SIZE_SELECTED(uniqueId))
				{
					if (uniqueId == elm->uId)
					{
						lstMods.erase(elm);
						res = JVX_NO_ERROR;
						break;
					}
				}
				else if (modName == elm->modName)
				{
					lstMods.erase(elm);
					res = JVX_NO_ERROR;
					break;
				}
			}
			return res;
		};

		virtual jvxErrorType startup_and_test_connection(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) = 0;

	};
};

#endif
