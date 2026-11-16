#include "jvxGlFunctions.h"
#include <string>

template <typename T>
static bool jvx_gl_load_one(T& fptr, const char* name, jvxGlGetProcAddress loader, void* priv,
	const char** missingName, const char* nameAlt = nullptr)
{
	fptr = reinterpret_cast<T>(loader(name, priv));
	if ((fptr == nullptr) && nameAlt)
	{
		fptr = reinterpret_cast<T>(loader(nameAlt, priv));
	}
	if (fptr == nullptr)
	{
		if (missingName && (*missingName == nullptr))
		{
			*missingName = name;
		}
		return false;
	}
	return true;
}

bool
CjvxGlFunctions::jvx_gl_load_functions(jvxGlGetProcAddress loader, void* priv, const char** missingName)
{
	bool ok = true;
	if (missingName)
	{
		*missingName = nullptr;
	}

#define JVX_GL_LOAD(fld) ok = jvx_gl_load_one(jvxgl.fld, "gl" #fld, loader, priv, missingName) && ok
#define JVX_GL_LOAD_ALT(fld, alt) ok = jvx_gl_load_one(jvxgl.fld, "gl" #fld, loader, priv, missingName, alt) && ok

	JVX_GL_LOAD(ActiveTexture);
	JVX_GL_LOAD(AttachShader);
	JVX_GL_LOAD(BindBuffer);
	JVX_GL_LOAD(BindTexture);
	JVX_GL_LOAD(BufferData);
	JVX_GL_LOAD(Clear);
	JVX_GL_LOAD(ClearColor);
	JVX_GL_LOAD(CompileShader);
	JVX_GL_LOAD(CreateProgram);
	JVX_GL_LOAD(CreateShader);
	JVX_GL_LOAD(DeleteBuffers);
	JVX_GL_LOAD(DeleteProgram);
	JVX_GL_LOAD(DeleteShader);
	JVX_GL_LOAD(DeleteTextures);
	JVX_GL_LOAD(DisableVertexAttribArray);
	JVX_GL_LOAD(DrawElements);
	JVX_GL_LOAD(EnableVertexAttribArray);
	JVX_GL_LOAD(Finish);
	JVX_GL_LOAD(GenBuffers);
	JVX_GL_LOAD(GenTextures);
	JVX_GL_LOAD(GetAttribLocation);
	JVX_GL_LOAD(GetError);
	JVX_GL_LOAD(GetIntegerv);
	JVX_GL_LOAD(GetProgramInfoLog);
	JVX_GL_LOAD(GetProgramiv);
	JVX_GL_LOAD(GetShaderInfoLog);
	JVX_GL_LOAD(GetShaderiv);
	JVX_GL_LOAD(GetString);
	JVX_GL_LOAD(GetUniformLocation);
	JVX_GL_LOAD(LinkProgram);
	JVX_GL_LOAD(PixelStorei);
	JVX_GL_LOAD(ReadPixels);
	JVX_GL_LOAD(ShaderSource);
	JVX_GL_LOAD(TexImage2D);
	JVX_GL_LOAD(TexParameteri);
	JVX_GL_LOAD(TexSubImage2D);
	JVX_GL_LOAD(Uniform1f);
	JVX_GL_LOAD(Uniform1i);
	JVX_GL_LOAD(Uniform4f);
	JVX_GL_LOAD(UseProgram);
	JVX_GL_LOAD(VertexAttribPointer);
	JVX_GL_LOAD(Viewport);

	JVX_GL_LOAD_ALT(GenFramebuffers, "glGenFramebuffersEXT");
	JVX_GL_LOAD_ALT(DeleteFramebuffers, "glDeleteFramebuffersEXT");
	JVX_GL_LOAD_ALT(BindFramebuffer, "glBindFramebufferEXT");
	JVX_GL_LOAD_ALT(FramebufferTexture2D, "glFramebufferTexture2DEXT");
	JVX_GL_LOAD_ALT(CheckFramebufferStatus, "glCheckFramebufferStatusEXT");

#undef JVX_GL_LOAD
#undef JVX_GL_LOAD_ALT

	return ok;
}
