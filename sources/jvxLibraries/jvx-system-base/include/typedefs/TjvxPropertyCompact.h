#ifndef __TJVXPROPERTYCOMPACT_H__
#define __TJVXPROPERTYCOMPACT_H__

namespace jvx
{
	class propertyCallCompactElementRef
	{
	public:
		jvxCallManagerProperties& callMan;
		const propertyRawPointerType::IjvxRawPointerType& rawPtr;
		const propertyAddress::IjvxPropertyAddress& ident;
		const propertyDetail::CjvxTranferDetail& detail;
		jvxErrorType& resCall;

		propertyCallCompactElementRef(jvxCallManagerProperties& callManArg,
			const propertyRawPointerType::IjvxRawPointerType& rawPtrArg,
			const propertyAddress::IjvxPropertyAddress& identArg,
			const propertyDetail::CjvxTranferDetail& detailArg,
			jvxErrorType& resCallArg) :
			callMan(callManArg), rawPtr(rawPtrArg), ident(identArg), detail(detailArg), resCall(resCallArg) {};
	};

	template <class T1, class T2, class T3> class propertyCallCompactElement
	{
	public:
		jvxCallManagerProperties callMan;
		T1 rawPtrt1;
		T2 identt1;
		T3 detailt1;
		jvxErrorType resCall = JVX_ERROR_ELEMENT_NOT_FOUND;

		propertyCallCompactElement(const T1& rawPtr, const T2& ident, const T3& detail) :
			rawPtrt1(rawPtr), identt1(ident), detailt1(detail) 
		{
			callMan.access_protocol = jvxAccessProtocol::JVX_ACCESS_PROTOCOL_NO_CALL;
		};
		propertyCallCompactElementRef toRefElement() 
		{
			return propertyCallCompactElementRef(callMan, rawPtrt1, identt1, detailt1, resCall);
		}
	};

	class propertyCallCompactRefList
	{
	public:
		std::list<propertyCallCompactElementRef> propReqs;
	};
}

#endif
