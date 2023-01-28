#ifndef __JVX_READWRITE_FILE__H__
#define __JVX_READWRITE_FILE__H__

jvxErrorType
jvx_writeContentToFile(std::string fName, const std::string& writeContentStr, IjvxCrypt* theCryptor = NULL);

jvxErrorType
jvx_readContentFromFile(std::string fName, std::string& storeContent, IjvxCrypt* theCryptor = NULL);

#endif
