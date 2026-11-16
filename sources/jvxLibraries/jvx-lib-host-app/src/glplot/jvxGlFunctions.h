#ifndef __JVXGLFUNCTIONS_H__
#define __JVXGLFUNCTIONS_H__

/*
 * Minimal OpenGL function table without any UI toolkit (no Qt, no GLEW, no GLUT).
 *
 * jvxVideoRenderCore can be derived from a base class that provides all OpenGL functions as
 * member functions (see the hint at the top of jvxVideoRenderCore.h). In the Qt world, this is
 * QOpenGLFunctions (CjvxMyGlWidget). Here, CjvxGlFunctions takes that role: all functions used by
 * jvxVideoRenderCore (and the offscreen rendering of CjvxGlPlotRenderer) are loaded via the
 * getProcAddress function of the platform specific context (WGL, EGL, ...).
 *
 * Usage:
 *   #define JVX_VIDEO_RENDER_CORE_CLASS CjvxGlFunctions
 *   #include "jvxVideoRenderCore.h"
 */

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <cstddef>

#ifdef APIENTRY
#define JVX_GL_APIENTRY APIENTRY
#else
#define JVX_GL_APIENTRY
#endif

// =====================================================================
// Types and constants beyond OpenGL 1.1 (gl.h on Windows only covers 1.1)
// =====================================================================

typedef char jvxGLchar;
typedef std::ptrdiff_t jvxGLsizeiptr;

#ifndef GL_VERSION_2_0
typedef char GLchar;
#endif

#ifndef GL_BGR
#define GL_BGR 0x80E0
#endif
#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif
#ifndef GL_TEXTURE0
#define GL_TEXTURE0 0x84C0
#endif
#ifndef GL_TEXTURE1
#define GL_TEXTURE1 0x84C1
#endif
#ifndef GL_ARRAY_BUFFER
#define GL_ARRAY_BUFFER 0x8892
#endif
#ifndef GL_ELEMENT_ARRAY_BUFFER
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#endif
#ifndef GL_STATIC_DRAW
#define GL_STATIC_DRAW 0x88E4
#endif
#ifndef GL_FRAGMENT_SHADER
#define GL_FRAGMENT_SHADER 0x8B30
#endif
#ifndef GL_VERTEX_SHADER
#define GL_VERTEX_SHADER 0x8B31
#endif
#ifndef GL_COMPILE_STATUS
#define GL_COMPILE_STATUS 0x8B81
#endif
#ifndef GL_LINK_STATUS
#define GL_LINK_STATUS 0x8B82
#endif
#ifndef GL_INFO_LOG_LENGTH
#define GL_INFO_LOG_LENGTH 0x8B84
#endif
#ifndef GL_FRAMEBUFFER
#define GL_FRAMEBUFFER 0x8D40
#endif
#ifndef GL_COLOR_ATTACHMENT0
#define GL_COLOR_ATTACHMENT0 0x8CE0
#endif
#ifndef GL_FRAMEBUFFER_COMPLETE
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#endif
#ifndef GL_RGBA8
#define GL_RGBA8 0x8058
#endif

// Loader as provided by the platform specific context
typedef void* (*jvxGlGetProcAddress)(const char* name, void* priv);

class CjvxGlFunctions
{
public:

	// Function pointer table
	struct
	{
		void (JVX_GL_APIENTRY* ActiveTexture)(GLenum) = nullptr;
		void (JVX_GL_APIENTRY* AttachShader)(GLuint, GLuint) = nullptr;
		void (JVX_GL_APIENTRY* BindBuffer)(GLenum, GLuint) = nullptr;
		void (JVX_GL_APIENTRY* BindTexture)(GLenum, GLuint) = nullptr;
		void (JVX_GL_APIENTRY* BufferData)(GLenum, jvxGLsizeiptr, const void*, GLenum) = nullptr;
		void (JVX_GL_APIENTRY* Clear)(GLbitfield) = nullptr;
		void (JVX_GL_APIENTRY* ClearColor)(GLfloat, GLfloat, GLfloat, GLfloat) = nullptr;
		void (JVX_GL_APIENTRY* CompileShader)(GLuint) = nullptr;
		GLuint (JVX_GL_APIENTRY* CreateProgram)() = nullptr;
		GLuint (JVX_GL_APIENTRY* CreateShader)(GLenum) = nullptr;
		void (JVX_GL_APIENTRY* DeleteBuffers)(GLsizei, const GLuint*) = nullptr;
		void (JVX_GL_APIENTRY* DeleteProgram)(GLuint) = nullptr;
		void (JVX_GL_APIENTRY* DeleteShader)(GLuint) = nullptr;
		void (JVX_GL_APIENTRY* DeleteTextures)(GLsizei, const GLuint*) = nullptr;
		void (JVX_GL_APIENTRY* DisableVertexAttribArray)(GLuint) = nullptr;
		void (JVX_GL_APIENTRY* DrawElements)(GLenum, GLsizei, GLenum, const void*) = nullptr;
		void (JVX_GL_APIENTRY* EnableVertexAttribArray)(GLuint) = nullptr;
		void (JVX_GL_APIENTRY* Finish)() = nullptr;
		void (JVX_GL_APIENTRY* GenBuffers)(GLsizei, GLuint*) = nullptr;
		void (JVX_GL_APIENTRY* GenTextures)(GLsizei, GLuint*) = nullptr;
		GLint (JVX_GL_APIENTRY* GetAttribLocation)(GLuint, const jvxGLchar*) = nullptr;
		GLenum (JVX_GL_APIENTRY* GetError)() = nullptr;
		void (JVX_GL_APIENTRY* GetIntegerv)(GLenum, GLint*) = nullptr;
		void (JVX_GL_APIENTRY* GetProgramInfoLog)(GLuint, GLsizei, GLsizei*, jvxGLchar*) = nullptr;
		void (JVX_GL_APIENTRY* GetProgramiv)(GLuint, GLenum, GLint*) = nullptr;
		void (JVX_GL_APIENTRY* GetShaderInfoLog)(GLuint, GLsizei, GLsizei*, jvxGLchar*) = nullptr;
		void (JVX_GL_APIENTRY* GetShaderiv)(GLuint, GLenum, GLint*) = nullptr;
		const GLubyte* (JVX_GL_APIENTRY* GetString)(GLenum) = nullptr;
		GLint (JVX_GL_APIENTRY* GetUniformLocation)(GLuint, const jvxGLchar*) = nullptr;
		void (JVX_GL_APIENTRY* LinkProgram)(GLuint) = nullptr;
		void (JVX_GL_APIENTRY* PixelStorei)(GLenum, GLint) = nullptr;
		void (JVX_GL_APIENTRY* ReadPixels)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, void*) = nullptr;
		void (JVX_GL_APIENTRY* ShaderSource)(GLuint, GLsizei, const jvxGLchar* const*, const GLint*) = nullptr;
		void (JVX_GL_APIENTRY* TexImage2D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void*) = nullptr;
		void (JVX_GL_APIENTRY* TexParameteri)(GLenum, GLenum, GLint) = nullptr;
		void (JVX_GL_APIENTRY* TexSubImage2D)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void*) = nullptr;
		void (JVX_GL_APIENTRY* Uniform1f)(GLint, GLfloat) = nullptr;
		void (JVX_GL_APIENTRY* Uniform1i)(GLint, GLint) = nullptr;
		void (JVX_GL_APIENTRY* Uniform4f)(GLint, GLfloat, GLfloat, GLfloat, GLfloat) = nullptr;
		void (JVX_GL_APIENTRY* UseProgram)(GLuint) = nullptr;
		void (JVX_GL_APIENTRY* VertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void*) = nullptr;
		void (JVX_GL_APIENTRY* Viewport)(GLint, GLint, GLsizei, GLsizei) = nullptr;

		// Framebuffer objects (OpenGL 3.0 / ARB_framebuffer_object, EXT variant as fallback)
		void (JVX_GL_APIENTRY* GenFramebuffers)(GLsizei, GLuint*) = nullptr;
		void (JVX_GL_APIENTRY* DeleteFramebuffers)(GLsizei, const GLuint*) = nullptr;
		void (JVX_GL_APIENTRY* BindFramebuffer)(GLenum, GLuint) = nullptr;
		void (JVX_GL_APIENTRY* FramebufferTexture2D)(GLenum, GLenum, GLenum, GLuint, GLint) = nullptr;
		GLenum (JVX_GL_APIENTRY* CheckFramebufferStatus)(GLenum) = nullptr;
	} jvxgl;

	// Load all functions. Returns false if a function is missing; missingName holds the first missing function.
	bool jvx_gl_load_functions(jvxGlGetProcAddress loader, void* priv, const char** missingName = nullptr);

	// =====================================================================
	// The OpenGL API as member functions. Unqualified calls in jvxVideoRenderCore resolve to
	// these members (same mechanism as with QOpenGLFunctions).
	// =====================================================================
	void glActiveTexture(GLenum t) { jvxgl.ActiveTexture(t); }
	void glAttachShader(GLuint p, GLuint s) { jvxgl.AttachShader(p, s); }
	void glBindBuffer(GLenum t, GLuint b) { jvxgl.BindBuffer(t, b); }
	void glBindTexture(GLenum t, GLuint tex) { jvxgl.BindTexture(t, tex); }
	void glBufferData(GLenum t, jvxGLsizeiptr sz, const void* d, GLenum u) { jvxgl.BufferData(t, sz, d, u); }
	void glClear(GLbitfield m) { jvxgl.Clear(m); }
	void glClearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) { jvxgl.ClearColor(r, g, b, a); }
	void glCompileShader(GLuint s) { jvxgl.CompileShader(s); }
	GLuint glCreateProgram() { return jvxgl.CreateProgram(); }
	GLuint glCreateShader(GLenum t) { return jvxgl.CreateShader(t); }
	void glDeleteBuffers(GLsizei n, const GLuint* b) { jvxgl.DeleteBuffers(n, b); }
	void glDeleteProgram(GLuint p) { jvxgl.DeleteProgram(p); }
	void glDeleteShader(GLuint s) { jvxgl.DeleteShader(s); }
	void glDeleteTextures(GLsizei n, const GLuint* t) { jvxgl.DeleteTextures(n, t); }
	void glDisableVertexAttribArray(GLuint i) { jvxgl.DisableVertexAttribArray(i); }
	void glDrawElements(GLenum m, GLsizei c, GLenum t, const void* i) { jvxgl.DrawElements(m, c, t, i); }
	void glEnableVertexAttribArray(GLuint i) { jvxgl.EnableVertexAttribArray(i); }
	void glFinish() { jvxgl.Finish(); }
	void glGenBuffers(GLsizei n, GLuint* b) { jvxgl.GenBuffers(n, b); }
	void glGenTextures(GLsizei n, GLuint* t) { jvxgl.GenTextures(n, t); }
	GLint glGetAttribLocation(GLuint p, const jvxGLchar* n) { return jvxgl.GetAttribLocation(p, n); }
	GLenum glGetError() { return jvxgl.GetError(); }
	void glGetIntegerv(GLenum p, GLint* d) { jvxgl.GetIntegerv(p, d); }
	void glGetProgramInfoLog(GLuint p, GLsizei b, GLsizei* l, jvxGLchar* i) { jvxgl.GetProgramInfoLog(p, b, l, i); }
	void glGetProgramiv(GLuint p, GLenum n, GLint* v) { jvxgl.GetProgramiv(p, n, v); }
	void glGetShaderInfoLog(GLuint s, GLsizei b, GLsizei* l, jvxGLchar* i) { jvxgl.GetShaderInfoLog(s, b, l, i); }
	void glGetShaderiv(GLuint s, GLenum n, GLint* v) { jvxgl.GetShaderiv(s, n, v); }
	const GLubyte* glGetString(GLenum n) { return jvxgl.GetString(n); }
	GLint glGetUniformLocation(GLuint p, const jvxGLchar* n) { return jvxgl.GetUniformLocation(p, n); }
	void glLinkProgram(GLuint p) { jvxgl.LinkProgram(p); }
	void glPixelStorei(GLenum p, GLint v) { jvxgl.PixelStorei(p, v); }
	void glReadPixels(GLint x, GLint y, GLsizei w, GLsizei h, GLenum f, GLenum t, void* d) { jvxgl.ReadPixels(x, y, w, h, f, t, d); }
	void glShaderSource(GLuint s, GLsizei c, const jvxGLchar* const* str, const GLint* l) { jvxgl.ShaderSource(s, c, str, l); }
	void glTexImage2D(GLenum t, GLint l, GLint i, GLsizei w, GLsizei h, GLint b, GLenum f, GLenum ty, const void* p) { jvxgl.TexImage2D(t, l, i, w, h, b, f, ty, p); }
	void glTexParameteri(GLenum t, GLenum p, GLint v) { jvxgl.TexParameteri(t, p, v); }
	void glTexSubImage2D(GLenum t, GLint l, GLint x, GLint y, GLsizei w, GLsizei h, GLenum f, GLenum ty, const void* p) { jvxgl.TexSubImage2D(t, l, x, y, w, h, f, ty, p); }
	void glUniform1f(GLint l, GLfloat v) { jvxgl.Uniform1f(l, v); }
	void glUniform1i(GLint l, GLint v) { jvxgl.Uniform1i(l, v); }
	void glUniform4f(GLint l, GLfloat a, GLfloat b, GLfloat c, GLfloat d) { jvxgl.Uniform4f(l, a, b, c, d); }
	void glUseProgram(GLuint p) { jvxgl.UseProgram(p); }
	void glVertexAttribPointer(GLuint i, GLint s, GLenum t, GLboolean n, GLsizei st, const void* p) { jvxgl.VertexAttribPointer(i, s, t, n, st, p); }
	void glViewport(GLint x, GLint y, GLsizei w, GLsizei h) { jvxgl.Viewport(x, y, w, h); }

	void glGenFramebuffers(GLsizei n, GLuint* f) { jvxgl.GenFramebuffers(n, f); }
	void glDeleteFramebuffers(GLsizei n, const GLuint* f) { jvxgl.DeleteFramebuffers(n, f); }
	void glBindFramebuffer(GLenum t, GLuint f) { jvxgl.BindFramebuffer(t, f); }
	void glFramebufferTexture2D(GLenum t, GLenum a, GLenum tt, GLuint tex, GLint l) { jvxgl.FramebufferTexture2D(t, a, tt, tex, l); }
	GLenum glCheckFramebufferStatus(GLenum t) { return jvxgl.CheckFramebufferStatus(t); }
};

#endif
