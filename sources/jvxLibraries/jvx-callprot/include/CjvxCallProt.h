#ifndef _CJVXCALLPROT_H__
#define _CJVXCALLPROT_H__

#include <list>

JVX_INTERFACE IjvxCallProt
{
public:
	typedef enum
	{
		JVX_CALL_PROT_ERROR,
		JVX_CALL_PROT_WARNING,
		JVX_CALL_PROT_INFO
	} jvxCallProtClassification;

	virtual JVX_CALLINGCONVENTION ~IjvxCallProt() {};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION setErrorCode(jvxErrorType res, const char* message, const char* origin) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION setIdentification(jvxComponentIdentification tp, const char* descriptor, 
		const char* conn, const char* origin, const char* context = NULL, const char* comment = NULL) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION addEntry(const char* selector,
		const char* message,
		jvxErrorType res,
		jvxCallProtClassification theClassification) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION addBranch(const char* selector, IjvxCallProt* next) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION getErrorShort(jvxBool* errorHere, jvxApiString* onRetMess, jvxApiString* onRetLoc) = 0;
	virtual IjvxCallProt*  JVX_CALLINGCONVENTION newEntry(const char* selector) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION duplicateFrom(const IjvxCallProt* copyme) = 0;

	// ===================================================================================
	virtual jvxErrorType getIdentification(jvxComponentIdentification* cpId, jvxApiString* descriptor,
		jvxApiString* conn, jvxApiString* origin, jvxApiString* context,
		jvxApiString* comment) const = 0;
	virtual jvxErrorType getSuccess(jvxErrorType* cpId, jvxApiString* message,
		jvxApiString* origin) const =0;

	virtual jvxErrorType JVX_CALLINGCONVENTION getNumberNext(jvxSize* num) const = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION getNext(jvxApiString* str, IjvxCallProt** next, jvxSize idx) const = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION getSelector(jvxApiString* str) const = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION getNumberEntries(jvxSize* num) const = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION getEntry(jvxApiString* str, jvxApiString* message, 
		jvxErrorType* result, IjvxCallProt::jvxCallProtClassification* classification, jvxSize idx) const = 0;

	// ===================================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION clear() = 0;

	virtual void JVX_CALLINGCONVENTION printResult(std::ostream& out, jvxSize level) = 0;
};

// ============================================================================
// ============================================================================
	
class CjvxCallProt_entry
{
public:
	std::string selector;
	std::string message;
	jvxErrorType result;
	IjvxCallProt::jvxCallProtClassification classification;
};

class CjvxCallProt_next
{
public:
	std::string selector;
	IjvxCallProt* ptr;
};

class CjvxCallProt: public IjvxCallProt
{
private:
	struct
	{
		jvxComponentIdentification tpId;
		std::string name;
		std::string conn;
		std::string comment;
		std::string origin;
	} component;

	struct
	{
		jvxErrorType res;
		std::string error_message;
		std::string origin_error;
	} success;

	std::string context;
	std::string call_selector;
	IjvxCallProt* parent;

	std::list<CjvxCallProt_entry> entries;
	std::list<CjvxCallProt_next> nexts;

public:
	CjvxCallProt(IjvxCallProt* par, const char* selector);
	CjvxCallProt();
	CjvxCallProt(const CjvxCallProt& copyme);
	~CjvxCallProt();
	CjvxCallProt& operator=(const CjvxCallProt& copyme);
	virtual jvxErrorType setErrorCode(jvxErrorType res, const char* message, const char* origin)override;
	virtual jvxErrorType setIdentification(jvxComponentIdentification tp, const char* descriptor, 
		const char* conn, const char* origin, const char* context = NULL, const char* comment = NULL)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION addEntry(const char* selector, const char* message, jvxErrorType res, 
		IjvxCallProt::jvxCallProtClassification theClassification)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION addBranch(const char* selector, IjvxCallProt* next)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION getErrorShort(jvxBool* errorHere, jvxApiString* onRetMess, jvxApiString* onRetLoc)override;
	virtual IjvxCallProt* JVX_CALLINGCONVENTION newEntry(const char* selector)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION duplicateFrom(const IjvxCallProt* copyme)override;

	// ===================================================================================
	virtual jvxErrorType getIdentification(jvxComponentIdentification* cpId, jvxApiString* descriptor, 
		jvxApiString* conn, jvxApiString* origin, jvxApiString* context, jvxApiString* comment)const override;
	virtual jvxErrorType getSuccess(jvxErrorType* cpId, jvxApiString* message,
		jvxApiString* origin) const override;

	virtual jvxErrorType JVX_CALLINGCONVENTION getNumberNext(jvxSize* num) const override;
	virtual jvxErrorType JVX_CALLINGCONVENTION getNext(jvxApiString* str, IjvxCallProt** next, jvxSize idx) const override;

	virtual jvxErrorType JVX_CALLINGCONVENTION getNumberEntries(jvxSize* num) const override;
	virtual jvxErrorType JVX_CALLINGCONVENTION getEntry(jvxApiString* str, jvxApiString* message,
		jvxErrorType* result, IjvxCallProt::jvxCallProtClassification* classification, jvxSize idx) const override;

	virtual jvxErrorType JVX_CALLINGCONVENTION getSelector(jvxApiString* str)const override;

	// ===================================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION clear()override;

	virtual void JVX_CALLINGCONVENTION printResult(std::ostream& out, jvxSize level)override;
};
	
#endif