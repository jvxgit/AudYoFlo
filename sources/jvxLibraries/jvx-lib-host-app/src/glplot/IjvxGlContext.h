#ifndef __IJVXGLCONTEXT_H__
#define __IJVXGLCONTEXT_H__

#include <string>

/*
 * Platform specific offscreen OpenGL context. All functions are called from the render thread
 * of CjvxGlPlotRenderer - the context is created, used and destroyed in that thread.
 */
class IjvxGlContext
{
public:
	virtual ~IjvxGlContext() {};

	// Create the context and make it current in the calling thread
	virtual bool create(std::string& errDescr) = 0;

	// Release the context
	virtual void destroy() = 0;

	// Lookup of OpenGL functions
	virtual void* get_proc_address(const char* name) = 0;

	// Called periodically in the render loop, e.g. to process window messages of a hidden window
	virtual void process_events() {};
};

// Implemented once per platform
IjvxGlContext* jvx_gl_context_allocate();
void jvx_gl_context_deallocate(IjvxGlContext* ctx);

#endif
