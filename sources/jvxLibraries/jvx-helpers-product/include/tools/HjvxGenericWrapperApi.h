#ifndef __HJVXGENERICWRAPPERAPI_H__
#define __HJVXGENERICWRAPPERAPI_H__

bool
jvx_genw_detectLeadingToken(std::string nameIn, std::string& nmChan, std::string searchme);

jvxErrorType
jvx_genw_decodeChannelName(std::string nameIn,  jvxGenericWrapperChannelType& purpChan, std::string& nmChan, 	/*int& id_global,*/ int& id_rel_device);

jvxErrorType
jvx_genw_encodeChannelName(std::string& nameOut, jvxGenericWrapperChannelType purpChan, std::string nmChan, std::string fnmChan,	int id_rel_device/*, int id_global*/);

#endif