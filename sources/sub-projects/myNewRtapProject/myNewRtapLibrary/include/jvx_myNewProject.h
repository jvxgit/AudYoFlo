#ifndef __JVX_myNewProject_H__
#define __JVX_myNewProject_H__

#include "jvx_dsp_base.h"

class jvx_myNewProject
{
public:
	jvx_myNewProject();

	~jvx_myNewProject();

#ifdef JVX_CC11_ENABLED
	jvx_myNewProject(const jvx_myNewProject&) = delete; // copying forbidden (to make it safe for class to allocate memory)
	void operator=(const jvx_myNewProject&) = delete; // assignment forbidden (same reason)
#endif
	jvxDspBaseErrorType process(jvxData** inputs, jvxData** outputs, jvxSize numChannelsIn, jvxSize numChannelsOut, jvxSize bSize);

	// properties which are independent from other properties can be public
	jvxData myVolume;

private:
	// properties which other properties depend on should be private and only accessed via setter

};




#endif