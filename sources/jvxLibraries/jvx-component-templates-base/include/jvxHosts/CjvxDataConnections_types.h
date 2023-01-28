#ifndef __CJVXDATACONNECTIONS_TYPES_H__
#define __CJVXDATACONNECTIONS_TYPES_H__

template <typename T>
struct oneEntrySomething
{
	T* ptr = nullptr;
	//std::string register_descr;
	jvxSize uId = JVX_SIZE_UNSELECTED;
	jvxSize refCnt = 0;
};

#endif