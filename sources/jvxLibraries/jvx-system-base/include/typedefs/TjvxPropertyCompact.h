#ifndef __TJVXPROPERTYCOMPACT_H__
#define __TJVXPROPERTYCOMPACT_H__

namespace jvx
{
	class propertyCallCompactElement
	{
	public:
		jvxCallManagerProperties& callMan;
		const propertyRawPointerType::IjvxRawPointerType& rawPtr;
		const propertyAddress::IjvxPropertyAddress& ident;
		const propertyDetail::CjvxTranferDetail& detail;
		jvxErrorType resCall = JVX_ERROR_ELEMENT_NOT_FOUND;

		propertyCallCompactElement(jvxCallManagerProperties& callManArg,
			const propertyRawPointerType::IjvxRawPointerType& rawPtrArg,
			const propertyAddress::IjvxPropertyAddress& identArg,
			const propertyDetail::CjvxTranferDetail& detailArg) :
			callMan(callManArg), rawPtr(rawPtrArg), ident(identArg), detail(detailArg) {};
	};

	class propertyCallCompactList
	{
	public:
		std::list<propertyCallCompactElement*> propReqs;
	};
}

#endif