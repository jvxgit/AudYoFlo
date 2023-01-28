#ifndef __CJVXDATACONNECTIONS_TYPES_H__
#define __CJVXDATACONNECTIONS_TYPES_H__

template <typename T>
struct oneEntrySomething
{
	T* ptr;
	//std::string register_descr;
	jvxSize uId;
	jvxSize refCnt;
};

#endif