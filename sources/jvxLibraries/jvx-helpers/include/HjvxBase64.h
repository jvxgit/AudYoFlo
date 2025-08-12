#ifndef __HJVXBASE64_H__
#define __HJVXBASE64_H__

std::string base64_encode(const unsigned char *src, size_t len);
void b64decode(const std::string& in, unsigned char* out, size_t* numBytesOut);

#endif
