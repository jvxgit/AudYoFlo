#ifndef __CJVXDATALOGGER_H__
#define __CJVXDATALOGGER_H__

#include <string>
#include <vector>

#include "jvx.h"
#include "common/CjvxObject.h"

#define JVX_SIMPLE_CRYPT_TOKEN "CRYP0"

class CjvxSimpleDotfuscation: public IjvxCrypt, public CjvxObject
{
public:

	/** Enum for the datatype for stored datachunk fields, can be enhanced in the future
	 *///===================================================
private:


	struct
	{
		std::string output;
	} memory;



public:

	//! Constructor: Do nothing
	CjvxSimpleDotfuscation(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	// =====================================================================================
	// Interface API
	// =====================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* theHostRef)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION encrypt(jvxByte* fldIn, jvxSize numElmsIn, jvxByte** byteFldOnReturn, jvxSize* numBytesOnReturn) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION decrypt(jvxByte* fldIn, jvxSize numElmsIn, jvxByte** byteFldOnReturn, jvxSize* numBytesOnReturn) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION release(jvxByte* releaseMe) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION terminate() override;

	// =====================================================================================

	//! Destructor
	~CjvxSimpleDotfuscation(void){};

#include "codeFragments/simplify/jvxObjects_simplify.h"

};

#endif
