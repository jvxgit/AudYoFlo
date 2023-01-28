#ifndef __IJVXCONNECTIONITERATOR_H__
#define __IJVXCONNECTIONITERATOR_H__

JVX_INTERFACE IjvxConnectionIterator
{
public:
  virtual ~IjvxConnectionIterator() {};

  virtual jvxErrorType JVX_CALLINGCONVENTION number_next(jvxSize* num) = 0;
  virtual jvxErrorType JVX_CALLINGCONVENTION reference_next(jvxSize idx, IjvxConnectionIterator** next) = 0;
  virtual jvxErrorType JVX_CALLINGCONVENTION reference_component(
	  jvxComponentIdentification* cpTp, 
	  jvxApiString* modName,
	  jvxApiString* lContext) = 0;
};

#endif
