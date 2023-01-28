#ifndef __HJVXCOMPONENTPRESELECTION_H__
#define __HJVXCOMPONENTPRESELECTION_H__

#include "jvx-helpers.h"

// ===================================================================
void jvx_getReferenceComponentType(const jvxComponentIdentification& tp, IjvxObject*& theObj, bool addressesStandardComponent, IjvxHost* hostRef);

void jvx_returnReferenceComponentType(const jvxComponentIdentification& tp, IjvxObject* theObj, bool addressesStandardComponent, IjvxHost* hostRef);

//void jvx_requestLinkDataSlaveRef(IjvxDataProcessor* procRef, IjvxLinkDataSlave** slvRefPtr, jvxComponentIdentification* tpRet);
//void jvx_returnLinkDataSlaveRef(IjvxDataProcessor* procRef, IjvxLinkDataSlave* slvRef);

#endif
