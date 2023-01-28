#ifndef __HJVXHOSTANDPROPERTIES_H__
#define __HJVXHOSTANDPROPERTIES_H__

jvxErrorType jvx_getReferencePropertiesObject(IjvxHost* theHost, jvx_propertyReferenceTriple* theTriple, const jvxComponentIdentification& tp);

jvxErrorType jvx_returnReferencePropertiesObject(IjvxHost* theHost, jvx_propertyReferenceTriple* theTriple, const jvxComponentIdentification& tp);

void jvx_findPropertyCategories_filter(std::vector<std::string>& propsCategory, IjvxAccessProperties* theProps, std::string filter);

void jvx_findPropertyCategories(std::vector<std::string>& propsCategory, jvx_propertyReferenceTriple& theTriple, std::string filter);

#endif