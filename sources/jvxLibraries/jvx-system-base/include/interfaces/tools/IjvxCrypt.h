#ifndef __IJVXCRYPT_H__
#define __IJVXCRYPT_H__

JVX_INTERFACE IjvxCrypt: public IjvxObject
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxCrypt(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* theHostRef) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION encrypt(jvxByte* fldIn, jvxSize numElmsIn, jvxByte** byteFldOnReturn, jvxSize* numBytesOnReturn) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION decrypt(jvxByte* fldIn, jvxSize numElmsIn, jvxByte** byteFldOnReturn, jvxSize* numBytesOnReturn) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION release(jvxByte* releaseMe) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION terminate() = 0;

};

#endif
