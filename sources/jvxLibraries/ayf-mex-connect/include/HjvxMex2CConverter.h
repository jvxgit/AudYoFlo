#ifndef __HJVXMEX2CCONVERTER_H__
#define __HJVXMEX2CCONVERTER_H__

#define JVX_SIZE_2_MAT_IDX(idx) (mwIndex) idx

static std::string jvx_mex_2_cstring(const mxArray* phs)
{
	jvxSize bufLen = mxGetM(phs)*mxGetN(phs)*sizeof(char)+1;
	char* buf = new char[bufLen];
	mxGetString(phs, buf, JVX_SIZE_INT(bufLen));
	std::string str = buf;
	delete[](buf);
	return(str);
}

#endif