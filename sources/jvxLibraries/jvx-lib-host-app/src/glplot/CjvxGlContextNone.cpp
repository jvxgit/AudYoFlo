#include "IjvxGlContext.h"

/*
 * Platforms without offscreen OpenGL context implementation (yet): plot renderers report an
 * error on start. For Linux, an EGL based context (surfaceless or pbuffer) is the counterpart of
 * CjvxGlContextWgl.
 */

IjvxGlContext*
jvx_gl_context_allocate()
{
	return nullptr;
}

void
jvx_gl_context_deallocate(IjvxGlContext* ctx)
{
	delete ctx;
}
