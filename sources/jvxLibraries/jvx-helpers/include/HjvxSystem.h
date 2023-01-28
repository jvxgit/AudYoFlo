#ifndef __HJVXSYSTEM_H__
#define __HJVXSYSTEM_H__

jvxTextHelpers* jvxComponentType_str();
const char* jvxComponentType_txt(jvxSize id);
const char* jvxComponentType_txtf(jvxSize id);
jvxErrorType jvxComponentType_decode(jvxComponentType* tp, const std::string & cpName);

jvxTextHelpers* jvxComponentTypeClass_str();
const char* jvxComponentTypeClass_txt(jvxComponentTypeClass cls);
const char* jvxComponentTypeClass_txtf(jvxComponentTypeClass cls);

#endif