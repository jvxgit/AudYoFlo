// https://stackoverflow.com/questions/342409/how-do-i-base64-encode-decode-in-c
#include "jvx-helpers.h"
#include <string>

static const unsigned char base64_table[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
* base64_encode - Base64 encode
* @src: Data to be encoded
* @len: Length of the data to be encoded
* @out_len: Pointer to output length variable, or %NULL if not used
* Returns: Allocated buffer of out_len bytes of encoded data,
* or empty string on failure
*/
std::string base64_encode(const unsigned char *src, size_t len)
{
    unsigned char *out, *pos;
    const unsigned char *end, *in;

    size_t olen;

    olen = 4*((len + 2) / 3); /* 3-byte blocks to 4-byte */

    if (olen < len)
        return std::string(); /* integer overflow */

    std::string outStr;
    outStr.resize(olen);
    out = (unsigned char*)&outStr[0];

    end = src + len;
    in = src;
    pos = out;
    while (end - in >= 3) {
        *pos++ = base64_table[in[0] >> 2];
        *pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
        *pos++ = base64_table[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
        *pos++ = base64_table[in[2] & 0x3f];
        in += 3;
    }

    if (end - in) {
        *pos++ = base64_table[in[0] >> 2];
        if (end - in == 1) {
            *pos++ = base64_table[(in[0] & 0x03) << 4];
            *pos++ = '=';
        }
        else {
            *pos++ = base64_table[((in[0] & 0x03) << 4) |
                (in[1] >> 4)];
            *pos++ = base64_table[(in[1] & 0x0f) << 2];
        }
        *pos++ = '=';
    }

    return outStr;
}

static const int B64index[256] = { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62, 63, 62, 62, 63, 52, 53, 54, 55,
56, 57, 58, 59, 60, 61,  0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,
7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,
0,  0,  0, 63,  0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51 };

void b64decode(const std::string& in, unsigned char* out, size_t* numBytesOut)
{
    unsigned char* p = (unsigned char*)in.c_str();
	size_t len = in.size();
	size_t cnt = 0;
	size_t dnt = 0;
	jvxSize lent = len / 4;
	jvxSize lenr = lent * 4;
	jvxSize lend = len - lenr;
	jvxSize lenw = 0;
	jvxSize lenout = lent * 3;
	if (in[len - 1] == '=')
	{
		lenw++;
	}
	if (in[len - 2] == '=')
	{
		lenw++;
	}

	lenout -= lenw;
	*numBytesOut = JVX_MIN(lenout, *numBytesOut);

	if (out)
	{
		for(; cnt < lenr; cnt += 4)
		{
			jvxUInt32 n = B64index[p[cnt]] << 18 | B64index[p[cnt + 1]] << 12 | B64index[p[cnt + 2]] << 6 | B64index[p[cnt + 3]];
			if (dnt < *numBytesOut)
			{
				out[dnt++] = n >> 16;
			}
			if (dnt < *numBytesOut)
			{
				out[dnt++] = n >> 8 & 0xFF;
			}
			if (dnt < *numBytesOut)
			{
				out[dnt++] = n & 0xFF;
			}
		}
	}
	
}