#ifndef __JVX_DEBUG_H__
#define __JVX_DEBUG_H__

#ifdef JVX_GLOBAL_BUFFERING_VERBOSE

// We may tune the debug behavior here!
// #define JVX_RESERVE_PIPELINE_VERBOSE
// #define JVX_GROUP_INTERCEPTOR_BUFFERING_VERBOSE
#define JVX_COMPONENT_BUFFERING_OUTPUT
#endif

#ifdef JVX_COMPONENT_BUFFERING_OUTPUT
#define JVX_GLOBAL_BUFFERING_OUTPUT(mname, fname, lineno, idx_in, idx_out) \
	std::cout << "!!! !!! !!! " << mname << " -- " << fname << ", " << lineno << ": Processing from buffer <" << idx_in << "> to buffer <" << idx_out << ">." << std::endl;
#else
#define JVX_GLOBAL_BUFFERING_OUTPUT(mname, fname, lineno, idx_in, idx_out) 
#endif

#endif
