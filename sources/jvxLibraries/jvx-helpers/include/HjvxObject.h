#ifndef __HJVXOBJECTS_H__
#define __HJVXOBJECTS_H__

#include <string>
#include <vector>

#define HJVXOBJECTS_GETNAME(a,b,c) {jvxString* a; b->getName(&a); c = a->bString; b->deallocate(a);}
#define HJVXOBJECTS_GETDESCRIPTION(a,b,c) {jvxString* a; b->getDescription(&a); c = a->bString; b->deallocate(a);}
#define HJVXOBJECTS_GETERROR(a,b,c,d,e,f) {jvxError* a; b->getLastError(&a); c = a->errorType; d = a->errorCode; e = a->errorLevel; f = a->errorDescription; b->deallocate(a);}
#define HJVXOBJECTS_GETNAMECHANNELIN(a,b,c,d) {jvxString* a; b->getNameChannelInputDevice(c,&a); d = a->bString; b->deallocate(a);}
#define HJVXOBJECTS_GETNAMECHANNELOUT(a,b,c,d) {jvxString* a; b->getNameChannelOutputDevice(c,&a); d = a->bString; b->deallocate(a);}

#endif
