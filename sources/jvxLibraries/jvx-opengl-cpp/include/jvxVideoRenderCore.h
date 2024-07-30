/* 
 * IMPORTANT HINT: 
 * This class contains all functionality to render videos to an
 * opengl window. But it may be used in two different ways:
 * 1) Only use this header file to pull in a specific opengl API.
 *    in that case this class needs to be derived from a base class that 
 *    offers all opengl functions. I have actually done this with QtOpenGLWidget!
 *    In this case: YOU MUST DEFINE JVX_VIDEO_RENDER_CORE_CLASS
 * 2) Use this class without base class. This class will use the default
 *	  opengl functions and glut + glew
 *    In this case: YOU MUST NOT DEFINE JVX_VIDEO_RENDER_CORE_CLASS
*/
#if JVX_OPENGL_MAP_GLES2

#define GL_UNPACK_SKIP_ROWS GL_UNPACK_SKIP_ROWS_EXT
#define GL_UNPACK_SKIP_PIXELS GL_UNPACK_SKIP_PIXELS_EXT
#define GL_UNPACK_ROW_LENGTH GL_UNPACK_ROW_LENGTH_EXT
#define GL_RGB8 GL_RGB

#endif

// =====================================================================

#if !defined(JVX_VIDEO_RENDER_CORE_CLASS)
extern "C"
{
	extern void jvx_ogl_render();
	extern void jvx_ogl_idle();
};

#include "jvx.h"

// If we use the native glut implementation, we require
// 
#include <stdlib.h>

#ifdef __APPLE__
#  include <GL/glew.h>
#  include <glut.h>
#else
#include <glew.h>
#  include <glut.h>
#  include <freeglut.h>
#endif

#include <iostream>
#endif

#include <cassert>

// No longer in use: 
// 	"#version 110										\n" \

static const char* vertex_shader_code_rgb24 = \
	"													\n" \
	"attribute mediump vec2 position;							\n" \
	"													\n" \
	"varying vec2 texcoord;								\n" \
	"													\n" \
	"void main()										\n" \
	"{													\n" \
	"	gl_Position = vec4(position, 0.0, 1.0);			\n" \
	"	texcoord = position * vec2(0.5) + vec2(0.5);	\n" \
	"}\n";

// No longer in use: 
// "#version 110									\n" \

static const char* fragment_shader_code_view_rgb24 = \
	"												\n" \
	"uniform sampler2D texture;						\n" \
	"uniform mediump int invert_y;						\n" \
	"												\n" \
	"varying mediump vec2 texcoord;					\n" \
	"												\n" \
	"void main()									\n" \
	"{												\n" \
	"	mediump vec2 coord = texcoord; \n" \
	"	if(invert_y) coord.y = 1 - coord.y; \n" \
	"	gl_FragColor =								\n" \
	"		texture2D(texture, coord);			\n" \
	"}												\n";

// Show only the red part:
//"	gl_FragColor[1] = 0;						\n" \
//"	gl_FragColor[2] = 0;						\n" \

// No longer in use: 
// "#version 110									\n" \

static const char* fragment_shader_code_rotate_rgb24 = \
"												\n" \
"uniform sampler2D texture;						\n" \
"uniform mediump float border_x;						\n" \
"uniform mediump float border_y;						\n" \
"uniform mediump float offset_x;						\n" \
"												\n" \
"varying mediump vec2 texcoord;							\n" \
"												\n" \
"void main()									\n" \
"{												\n" \
"	mediump vec2 coord = texcoord; \n" \
"	if( 										\n" \
"		(coord.x < border_x) || (coord.x > (1.0-border_x))	||	\n" \
"		(coord.y < border_y) || (coord.y > (1.0-border_y)))		\n" \
"	{											\n" \
"		gl_FragColor = vec4(0,0,0,1);			\n" \
"   }											\n" \
"	else										\n" \
"	{											\n" \
"		coord = vec2((((texcoord.y-border_y) * 1.0 / (1.0 - 2.0 * border_y)) ), ((texcoord.x-border_x) * 1.0 / (1.0 - 2.0 * border_x)) + offset_x); \n" \
"		gl_FragColor =							\n" \
"			texture2D(texture, coord);			\n" \
"	}											\n" \
"}												\n";

#if 0
static const char* fragment_shader_code_rotate_rgb24 = \
"#version 110									\n" \
"												\n" \
"uniform sampler2D texture;						\n" \
"uniform float border;							\n" \
"												\n" \
"varying vec2 texcoord;							\n" \
"												\n" \
"void main()									\n" \
"{												\n" \
"	vec2 coord = texcoord; \n" \
"	if( 										\n" \
"		(coord.x < border) || (coord.x > (1.0 - border))	||	\n" \
"		(coord.y < border) || (coord.y > (1.0 - border)))		\n" \
"	{											\n" \
"		gl_FragColor = vec4(0,0,0,1);			\n" \
"   }											\n" \
"	else										\n" \
"	{											\n" \
"		coord = vec2(((texcoord.y-border) * 1.0 / (1-border-border)), ((texcoord.x-border) * 1.0 / (1-border-border))); \n" \
"		gl_FragColor =							\n" \
"			texture2D(texture, coord);			\n" \
"	}											\n" \
"}												\n";
#endif

typedef enum
{
	JVX_GL_RENDER_STRAIGHT,
	JVX_GL_RENDER_ROTATE_ANGLE_OFFSET_FOLLOW
} jvxOpenGlRendering;

struct cfgRotateAngleOffsetFollow
{
	jvxData border_x;
	jvxData border_y;
	jvxData offset_x;
	jvxSize update_offset_y;
	jvxSize update_num_y;
} ;

struct cfgRenderStraight
{
	jvxCBool invert_y = false;
};

//"	vec2 coord = vec2((texcoord.x + 0.1) * 0.9/1.1, texcoord.y + 0.1 * 0.9/1.1); \n" \
//"	coord = vec2((texcoord.x - 0.5) * (Resolution.x / Resolution.y), texcoord.y - 0.5) * mat2(cos_factor, sin_factor, -sin_factor, cos_factor);


#if defined(JVX_VIDEO_RENDER_CORE_CLASS)
#define JVXVIDEORENDERCORE jvxVideoRenderCore_bc
#else
#define JVXVIDEORENDERCORE jvxVideoRenderCore_nobc
#endif

class JVXVIDEORENDERCORE
#if defined(JVX_VIDEO_RENDER_CORE_CLASS)
	: public JVX_VIDEO_RENDER_CORE_CLASS
#endif

{
private:
	struct
	{
		GLuint vertex_buffer, element_buffer;
		GLuint texture;

		GLuint vertex_shader, fragment_shader, program;

		struct
		{
			GLint texture;
			GLint border_x;
			GLint border_y;
			GLint offset_x;

			GLint invert_y;
		} uniforms;

		struct
		{
			GLint position;
		} attributes;
		
		GLfloat border_x;
		GLfloat border_y;
		GLfloat offset_x;
		GLint invert_y;

	} g_resources;

	GLfloat* g_vertex_buffer_data;
	jvxSize g_vertex_buffer_data_size;

	GLushort* g_element_buffer_data;
	jvxSize g_element_buffer_data_size;

	jvxSize myWidth;
	jvxSize myHeight;
	jvxDataFormat myForm;
	jvxDataFormatGroup mySubform;
	jvxSize myNumChannels;
	jvxSize myNumBufs;
	jvxSize szOnePixel;
	jvxSize myFldSize;
	jvxByte*** bufsAllocated;
	jvxBool bufFromExt;
	jvxOpenGlRendering myRenderOperation;
	// 	glutLeaveMainLoop();

	cfgRotateAngleOffsetFollow varCfgRotateAngleOffsetFollow;
public:

	JVXVIDEORENDERCORE()
	{
		g_vertex_buffer_data = NULL;
		g_element_buffer_data = NULL;
		myWidth = 0;
		myHeight = 0;
		myForm = JVX_DATAFORMAT_NONE;
		mySubform = JVX_DATAFORMAT_GROUP_NONE;
		g_resources.program = 0;
		bufsAllocated = NULL;
	};

	jvxErrorType initialize(int width, int height, jvxByte**** bufs, jvxSize* szFld, jvxSize numChannels,
		jvxSize numBufs, jvxDataFormat form, jvxDataFormatGroup subform,
		jvxOpenGlRendering renderOperation, jvxHandle* cfgSpecific,
		const char* txtToShow, jvxByte* extBuf, jvxSize extSz)
	{
		jvxSize i;
		jvxErrorType res = system_initialize(txtToShow);

		myWidth = width;
		myHeight = height;
		myForm = form;
		mySubform = subform;
		myFldSize = 0;
		myNumChannels = numChannels;
		myNumBufs = numBufs;

		myRenderOperation = renderOperation;

		const char* v_render_prog = vertex_shader_code_rgb24;
		const char* f_render_prog = fragment_shader_code_view_rgb24;
		
		varCfgRotateAngleOffsetFollow.border_x = 0.0;
		varCfgRotateAngleOffsetFollow.border_y = 0.0;
		varCfgRotateAngleOffsetFollow.offset_x = 0.0;

		switch (myRenderOperation)
		{
		case JVX_GL_RENDER_STRAIGHT:
			break;
		case JVX_GL_RENDER_ROTATE_ANGLE_OFFSET_FOLLOW:
			f_render_prog = fragment_shader_code_rotate_rgb24;
			if (cfgSpecific)
			{
				cfgRotateAngleOffsetFollow* myPtr = (cfgRotateAngleOffsetFollow*)cfgSpecific;
				varCfgRotateAngleOffsetFollow = *myPtr;
			}
			break;
		}

		g_vertex_buffer_data = new GLfloat[8];
		g_vertex_buffer_data[0] = -1.0f;
		g_vertex_buffer_data[1] = -1.0f;
		g_vertex_buffer_data[2] = 1.0f;
		g_vertex_buffer_data[3] = -1.0f;
		g_vertex_buffer_data[4] = -1.0f;
		g_vertex_buffer_data[5] = 1.0f;
		g_vertex_buffer_data[6] = 1.0f;
		g_vertex_buffer_data[7] = 1.0f;
		g_vertex_buffer_data_size = 8 * sizeof(GLfloat);

		g_element_buffer_data = new GLushort[4];
		for (i = 0; i < 4; i++)
		{
			g_element_buffer_data[i] = (GLushort)i;
		}
		g_element_buffer_data_size = 4 * sizeof(GLushort);

		if (!make_resources(myWidth, myHeight, bufs, szFld, numChannels, numBufs, myForm, mySubform, v_render_prog, 
			f_render_prog, extBuf, extSz))
		{
			return JVX_ERROR_INTERNAL;
		}

		return JVX_NO_ERROR;
		// glutMainLoop(); <- no
	};

	jvxErrorType terminate()
	{
		jvxSize i, j;

#if defined(JVX_VIDEO_RENDER_CORE_CLASS)
		if (g_resources.program == 0)
		{
			release_programm_unassign();
		}
		else
		{
#endif
			glDeleteProgram(g_resources.program);
			glDeleteShader(g_resources.vertex_shader);
			glDeleteShader(g_resources.fragment_shader);

			g_resources.program = 0;
			g_resources.vertex_shader = 0;
			g_resources.fragment_shader = 0;

#if defined(JVX_VIDEO_RENDER_CORE_CLASS)
		}
#endif


		if (bufsAllocated)
		{
			if (bufFromExt)
			{
				for (i = 0; i < myNumBufs; i++)
				{
					for (j = 0; j < myNumChannels; j++)
					{
						//JVX_DSP_SAFE_DELETE_FIELD(bufsAllocated[i][j]); <- no, do not deallocate, buffers are from extern
					}
					JVX_DSP_SAFE_DELETE_FIELD(bufsAllocated[i]);
				}
			}
			else
			{
				for (i = 0; i < myNumBufs; i++)
				{
					for (j = 0; j < myNumChannels; j++)
					{
						JVX_DSP_SAFE_DELETE_FIELD(bufsAllocated[i][j]);
					}
					JVX_DSP_SAFE_DELETE_FIELD(bufsAllocated[i]);
				}
			}
			JVX_DSP_SAFE_DELETE_FIELD(bufsAllocated);
			bufsAllocated = NULL;
		}

		g_resources.uniforms.border_x = 0;
		g_resources.uniforms.border_y = 0;
		g_resources.uniforms.offset_x = 0;	
		g_resources.uniforms.invert_y = 0;
		g_resources.uniforms.texture = 0;
		g_resources.attributes.position = 0;

		return JVX_NO_ERROR;
	};

	// =====================================================================

	void prepare_render(const jvxByte** buf, jvxHandle* cfgSpecific = NULL)
	{
		jvxSize offset_y = 0;
		jvxSize num_values = myHeight;
		jvxSize pixOffset = 0;

		// Clear the framebuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//std::cout << "Render" << std::endl;

		if (g_resources.program == 0)
		{
#if defined(JVX_VIDEO_RENDER_CORE_CLASS)
			start_rendering_program();
#else
			assert(0);
#endif
		}
		else
		{
			// Use the vertex/fragment shader program
			glUseProgram(g_resources.program);
		}
		switch (myRenderOperation)
		{
		case JVX_GL_RENDER_ROTATE_ANGLE_OFFSET_FOLLOW:

			g_resources.border_x = (GLfloat)varCfgRotateAngleOffsetFollow.border_x;
			g_resources.border_y = (GLfloat)varCfgRotateAngleOffsetFollow.border_y;
			g_resources.offset_x = (GLfloat)varCfgRotateAngleOffsetFollow.offset_x;
			if (cfgSpecific)
			{
				cfgRotateAngleOffsetFollow* varCfgRotateAngleOffsetFollow = (cfgRotateAngleOffsetFollow*)cfgSpecific;
				g_resources.border_x = (GLfloat)varCfgRotateAngleOffsetFollow->border_x;
				g_resources.border_y = (GLfloat)varCfgRotateAngleOffsetFollow->border_y;
				g_resources.offset_x = (GLfloat)varCfgRotateAngleOffsetFollow->offset_x;
				offset_y = varCfgRotateAngleOffsetFollow->update_offset_y;
				num_values = varCfgRotateAngleOffsetFollow->update_num_y;
			}

			glUniform1f(g_resources.uniforms.border_x, g_resources.border_x);
			glUniform1f(g_resources.uniforms.border_y, g_resources.border_y);

			//std::cout << "Offset x: " << g_resources.offset_x << std::endl;
			glUniform1f(g_resources.uniforms.offset_x, g_resources.offset_x);
			break;
		case JVX_GL_RENDER_STRAIGHT:
			g_resources.invert_y = 0;
			if (cfgSpecific)
			{
				cfgRenderStraight* optVar = (cfgRenderStraight*)cfgSpecific;
				g_resources.invert_y = optVar->invert_y;
			}
			glUniform1i(g_resources.uniforms.invert_y, g_resources.invert_y);
			break;
		default:
		  break;
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, g_resources.texture);
		glUniform1i(g_resources.uniforms.texture, 0);

		// Found this for odd matrix dimensions (e.g. 513 x 1024) here: https://stackoverflow.com/questions/7380773/glteximage2d-segfault-related-to-width-height
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
		glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

		switch (myRenderOperation)
		{
		case JVX_GL_RENDER_ROTATE_ANGLE_OFFSET_FOLLOW:
			// Copy buffer from camera to openGL texture
			pixOffset = offset_y * myWidth * szOnePixel;
			glTexSubImage2D(
				GL_TEXTURE_2D,
				0,
				0,
				(GLint)offset_y,
				(GLsizei)myWidth, (GLsizei)num_values,
				GL_BGR, GL_UNSIGNED_BYTE,   /* external format, type */
				buf[0] + pixOffset                  /* pixels */
			);
			break;
		default:
			// Copy buffer from camera to openGL texture
			glTexImage2D(
				GL_TEXTURE_2D, 0,           /* target, level of detail */
				GL_RGB8,                    /* internal format */
				(GLsizei)myWidth, (GLsizei)myHeight, 0,           /* width, height, border */
				GL_RGB, GL_UNSIGNED_BYTE,   /* external format, type */
				buf[0]                     /* pixels */
			);
		}

		glBindBuffer(GL_ARRAY_BUFFER, g_resources.vertex_buffer);
		glVertexAttribPointer(
			g_resources.attributes.position,  /* attribute */
			2,                                /* size */
			GL_FLOAT,                         /* type */
			GL_FALSE,                         /* normalized? */
			sizeof(GLfloat) * 2,                /* stride */
			(void*)0                          /* array buffer offset */
		);
		glEnableVertexAttribArray(g_resources.attributes.position);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_resources.element_buffer);
		glDrawElements(
			GL_TRIANGLE_STRIP,  /* mode */
			4,                  /* count */
			GL_UNSIGNED_SHORT,  /* type */
			(void*)0            /* element array buffer offset */
		);

		glDisableVertexAttribArray(g_resources.attributes.position);
		
		if (g_resources.program == 0)
		{
#if defined(JVX_VIDEO_RENDER_CORE_CLASS)
			stop_rendering_program();
#else
			assert(0);
#endif
		}
	};

	// =================================================================

#if !defined(JVX_VIDEO_RENDER_CORE_CLASS)
	void trigger_render();
	void finalize_render();
	jvxErrorType system_initialize(const char* txtToShow);
	void system_passcontrol();
	void system_regaincontrol();

#else

	virtual jvxErrorType system_initialize(const char* txtToShow) = 0;
	virtual jvxErrorType system_passcontrol() = 0;

	virtual jvxErrorType create_programm_assign(
		const char* vertex_shader_code,
		const char*	fragment_shader_code,
		jvxOpenGlRendering myRenderOperation,
		GLint* position, GLint* texture,
		GLint* border_x, GLint* border_y, 
		GLint* offset_x, GLint* invert_y) = 0;
	virtual jvxErrorType release_programm_unassign() = 0;
	virtual jvxErrorType start_rendering_program() = 0;
	virtual jvxErrorType stop_rendering_program() = 0;

#endif

private:

	GLuint make_buffer(GLenum target, const void *buffer_data, GLsizei buffer_size)
	{
		GLuint buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(target, buffer);
		glBufferData(target, buffer_size, buffer_data, GL_STATIC_DRAW);
		return buffer;
	};

	GLuint make_texture_buffer(jvxByte* ptrExt, jvxSize extSz)
	{
		jvxSize i,j;
		GLuint texture;

		szOnePixel = jvxDataFormatGroup_size[mySubform];
		myFldSize = szOnePixel * myWidth * myHeight;
		if (ptrExt == NULL)
		{
			bufFromExt = false;
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(bufsAllocated, jvxByte**, myNumBufs);
			for (i = 0; i < myNumBufs; i++)
			{
				JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(bufsAllocated[i], jvxByte*, myNumChannels);
				for (j = 0; j < myNumChannels; j++)
				{
					JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(bufsAllocated[i][j], jvxByte, myFldSize);
				}
			}
		}
		else
		{
			bufFromExt = true;
			jvxSize cmplFldSz = myFldSize * myNumBufs * myNumChannels;
			assert(cmplFldSz == extSz);
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(bufsAllocated, jvxByte**, myNumBufs);
			for (i = 0; i < myNumBufs; i++)
			{
				JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(bufsAllocated[i], jvxByte*, myNumChannels);
				for (j = 0; j < myNumChannels; j++)
				{
					bufsAllocated[i][j] = ptrExt;
					ptrExt += myFldSize;
				}
			}
		}
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		// Interpolation parameters
		// GL_TEXTURE_MIN_FILTER and GL_TEXTURE_MAG_FILTER control how in - between sample points are treated when the texture is sampled 
		// at a resolution lower and higher than its native resolution,
		// GL_LINEAR to tell the GPU to use linear interpolation to smoothly blend the four elements closest to the sample point. If the 
		// user resizes our window, the texture image will then scale smoothly. Setting the filters to GL_NEAREST would tell the GPU to 
		// return the texture element closest to the sample point, leading to blocky, pixelated scaling.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// The GL_TEXTURE_WRAP_S and GL_TEXTURE_WRAP_T parameters control how coordinates beyond the zero - to - one range on their respective 
		// axes are treated; in our case, we don't plan to sample outside that range, so we use GL_CLAMP_TO_EDGE, which clamps coordinates 
		// below zero to zero, and above one to one. A wrap value of GL_WRAP for one or both axes would cause the texture image to be repeated 
		// infinitely through texture space along the wrapped axes. 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT/*GL_CLAMP_TO_EDGE*/);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT/*GL_CLAMP_TO_EDGE*/);

		// Found this for odd matrix dimensions (e.g. 513 x 1024) here: https://stackoverflow.com/questions/7380773/glteximage2d-segfault-related-to-width-height
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
		glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

		// The glTexImage2D (or -1D or -3D) function allocates memory for a texture. Textures can have multiple levels of detail, sampling from 
		// a hierarchy of progressively smaller "mipmaps" when sampled at lower resolutions, but in our case we only supply the base level zero. 
		// The border argument is a relic and should always be zero.
		glTexImage2D(
			GL_TEXTURE_2D, 0,				/* target, level of detail */
			GL_RGB8,						/* internal format */
			(GLsizei)myWidth, (GLsizei)myHeight, 0,           /* width, height, border */
			GL_BGR, GL_UNSIGNED_BYTE,		/* external format, type */
			bufsAllocated[0][0]                   /* pixels */
		);
		return texture;
	};

  void show_info_log_shader(GLuint object)
  {
    GLint log_length = 0;
    char *log = NULL;
    glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
    if(log_length)
      {
	log = (char*)malloc(log_length);
	glGetShaderInfoLog( object, log_length, NULL, log);
	std::cout << log << std::endl;
	free(log);
      }
  };
  void show_info_log_program(GLuint object)
  {
    GLint log_length = 0;
    char *log = NULL;
    glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
    if(log_length)
      {
	log = (char*)malloc(log_length);
	glGetProgramInfoLog( object, log_length, NULL, log);
	std::cout << log << std::endl;
	free(log);
      }
  };
  
	GLuint make_shader(GLenum type, GLchar *source)
	{
		GLint length = -1;
		GLuint shader;
		GLint shader_ok;

		// type = GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
		shader = glCreateShader(type);

		// 1 entry in following arraies, each entry with a char buffer and a length
		// Each element in the length array may contain the length of the corresponding string (the null character is not 
		// counted as part of the string length) or a value less than 0 to indicate that the string is null terminated
		glShaderSource(shader, 1, (const GLchar**)&source, &length);
		glCompileShader(shader);

		glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);
		if (!shader_ok)
		{
			fprintf(stderr, "Failed to compile %s:\n", source);
//#if !defined(JVX_VIDEO_RENDER_CORE_CLASS) <- did I insert this ifdef for Mac OSX??
			show_info_log_shader(shader);			
//#endif
			glDeleteShader(shader);
			assert(0);
			exit(0);
		}
		return shader;
	};

	GLuint make_program(GLuint vertex_shader, GLuint fragment_shader)
	{
		GLint program_ok;

		GLuint program = glCreateProgram();
		glAttachShader(program, vertex_shader);
		glAttachShader(program, fragment_shader);
		glLinkProgram(program);

		glGetProgramiv(program, GL_LINK_STATUS, &program_ok);
		if (!program_ok)
		{
			fprintf(stderr, "Failed to link shader program:\n");
//#if !defined(JVX_VIDEO_RENDER_CORE_CLASS)<- did I insert this ifdef for Mac OSX??
			show_info_log_program(program);
//#endif
			glDeleteProgram(program);
			return 0;
		}
		return program;
	};

	// ==========================================================================
	int make_resources(jvxSize width, jvxSize height, jvxByte****ptr, jvxSize* szFld, jvxSize numChannels, jvxSize numBufs, 
		jvxDataFormat form, jvxDataFormatGroup subform, const char* vertex_shader_code, const char* fragment_shader_code,
		jvxByte* extPtr, jvxSize szExt)
	{
#if defined(JVX_VIDEO_RENDER_CORE_CLASS)
		jvxErrorType errProg;
#endif
		g_resources.vertex_buffer = make_buffer(
			GL_ARRAY_BUFFER,
			g_vertex_buffer_data,
			(GLsizei)g_vertex_buffer_data_size
		);
		g_resources.element_buffer = make_buffer(
			GL_ELEMENT_ARRAY_BUFFER,
			g_element_buffer_data,
			(GLsizei)g_element_buffer_data_size
		);

		g_resources.texture = make_texture_buffer(extPtr, szExt);
		if (ptr)
		{
			*ptr = bufsAllocated;
		}
		if (szFld)
		{
			*szFld = myFldSize;
		}

		if (g_resources.texture == 0)
			return 0;

#if defined(JVX_VIDEO_RENDER_CORE_CLASS)

		// Create program in a different context
		errProg = this->create_programm_assign(
			vertex_shader_code,
			fragment_shader_code,
			myRenderOperation,
			&g_resources.attributes.position,
			&g_resources.uniforms.texture,

			&g_resources.uniforms.border_x,
			&g_resources.uniforms.border_y,
			&g_resources.uniforms.offset_x,
			&g_resources.uniforms.invert_y
		);
		if (errProg == JVX_ERROR_UNSUPPORTED)
		{
#endif

			/* make buffers and textures ... */
			g_resources.vertex_shader = make_shader(GL_VERTEX_SHADER, (GLchar*)vertex_shader_code);
			if (g_resources.vertex_shader == 0)
				return 0;

			g_resources.fragment_shader = make_shader(GL_FRAGMENT_SHADER, (GLchar*)fragment_shader_code);
			if (g_resources.fragment_shader == 0)
				return 0;

			g_resources.program = make_program(g_resources.vertex_shader, g_resources.fragment_shader);
			if (g_resources.program == 0)
				return 0;

			switch (myRenderOperation)
			{
			case JVX_GL_RENDER_ROTATE_ANGLE_OFFSET_FOLLOW:
				g_resources.uniforms.border_x
					= glGetUniformLocation(g_resources.program, "border_x");
				g_resources.uniforms.border_y
					= glGetUniformLocation(g_resources.program, "border_y");
				g_resources.uniforms.offset_x
					= glGetUniformLocation(g_resources.program, "offset_x");
				break;
			case JVX_GL_RENDER_STRAIGHT:
				g_resources.uniforms.invert_y
					= glGetUniformLocation(g_resources.program, "invert_y");
			default:
			  break;
			}
			g_resources.uniforms.texture
				= glGetUniformLocation(g_resources.program, "texture");

			g_resources.attributes.position = glGetAttribLocation(g_resources.program, "position");
#if defined(JVX_VIDEO_RENDER_CORE_CLASS)
		}
#endif
		return 1;
	};
};

// 	jvxErrorType system_passcontrol()
// ===========================================

