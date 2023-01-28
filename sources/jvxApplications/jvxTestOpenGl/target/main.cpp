// Content taken from Tutorial:
// http://duriansoftware.com/joe/An-intro-to-modern-OpenGL.-Chapter-2.1:-Buffers-and-Textures.html

#include <stdlib.h>
#include <glew.h>
#ifdef __APPLE__
#  include <glut.h>
#else
#  include <glut.h>
#endif
#include <iostream>
#include <cassert>

extern void *read_tga(const char *filename, int *width, int *height);

// Vertex buffer:
// [2, -1:1] ------->---------[3, 1:1]
//     |                         |
//     |                         |
//     |                         |
// [0, -1:-1]------->--------[1, 1:-1]
//
static const GLfloat g_vertex_buffer_data[] = 
{
	-1.0f, -1.0f,
	1.0f, -1.0f,
	-1.0f,  1.0f,
	1.0f,  1.0f
};
static const GLushort g_element_buffer_data[] = 
{ 
	0, 1, 2, 3 
};

static struct 
{
	GLuint vertex_buffer, element_buffer;
	GLuint textures[2];

	GLuint vertex_shader, fragment_shader, program;

	struct
	{
		GLint fade_factor;
		GLint textures[2];
	} uniforms;

	struct
	{
		GLint position;
	} attributes;

	GLfloat fade_factor;
} g_resources;

// =========================================================================
// Vertex shader code: 
// "#version 110"
// -> GLSL versions are pretty tightly tied to OpenGL versions; 1.10 is the version that corresponds to OpenGL 2.0.)
// 
// -> GLSL does away with pointers and most of C's sized numeric types, keeping only the bool, int, and float types 
//	  in common, but it adds a suite of vector and matrix types up to four components in length. The vec2 and vec4 
//    types you see here are two- and four-component vectors of floats, respectively.
// 
// -> GLSL also supplies special sampler data types for sampling textures.
// 
// "gl_Position = vec4(position, 0.0, 1.0);"
// -> vertex's screen space position to gl_Position, a predefined variable that GLSL provides for the purpose. In screen space, 
//    the coordinates (–1, –1) and (1, 1) correspond respectively to the lower-left and upper-right corners of the framebuffer; 
//    since our vertex array's vertices already trace that same rectangle, we can directly copy the x and y components from each 
//    vertex's position value as it comes out of the vertex array.
//    The gl_position expects vec4 but our vertices are defines as vec2 -> extend missing dimensions by [0, 1]
//
// "texcoord = position * vec2(0.5) + vec2(0.5);"
// -> The shader then does some math to map our screen-space positions from screen space (–1 to 1) to texture space (0 to 1) 
//    and assigns the result to the vertex's texcoord. 
// 
// Attribute variables (vertex positions)
//				    [1, -1]                   [1,1]
//
// 
//				    [-1,-1]					[-1,1]
//									|
//									|
//						  ======================
//						  |    VERTEX SHADER   |
//						  ======================
//									|
//									|
//		- in screen space -					- texture space -
//		gl_position [-1,-1]..[1,1]			texcoord [0,0]..[1,1]
//			viewport							image
//
// -> Its inputs come from uniform variables, which supply values from the uniform state, and attribute variables, which supply 
//    per-vertex attributes from the vertex array. The shader assigns its per-vertex outputs to varying variables. GLSL predefines 
//    some varying variables to receive special outputs used by the graphics pipeline, including the gl_Position variable we used here. 
// 
static const char* vertex_shader_code = \
	"#version 110										\n" \
	"													\n" \
	"attribute vec2 position;							\n" \
	"													\n" \
	"varying vec2 texcoord;								\n" \
	"													\n" \
	"void main()										\n" \
	"{													\n" \
	"	gl_Position = vec4(position, 0.0, 1.0);			\n" \
	"	texcoord = position * vec2(0.5) + vec2(0.5);	\n" \
	"}\n";

// =========================================================================
// Vertex shader code: 
//
// "varying vec2 texcoord;"
// -> varying variables become inputs here: Each varying variable in the fragment shader is linked to the vertex shader's varying variable 
//    of the same name.
// -> each invocation of the fragment shader receives a rasterized version of the vertex shader's outputs for that varying variable.
//
// "gl_FragColor = ..."
// -> Fragment shaders are also given a different set of predefined gl_* variables. gl_FragColor is the most important, a vec4 to which 
//    the shader assigns the RGBA color value for the fragment. 
// 
// "uniform float fade_factor;"
// "uniform sampler2D textures[2];""	
// -> The fragment shader has access to the same set of uniforms as the vertex shader, but cannot declare or access attribute variables.
// 
// "gl_FragColor = mix(texture2D(textures[0], texcoord), texture2D(textures[1], texcoord),	fade_factor);
// -> Our fragment shader uses GLSL's builtin texture2D function to sample the two textures from uniform state at texcoord. It then calls 
//    the builtin mix function to combine the two texture values based on the current value of the uniform fade_factor: zero gives only 
//    the sample from the first texture, one gives only the second texture's sample, and values in between give us a blend of the two.
// 
static const char* fragment_shader_code = \
	"#version 110									\n" \
	"												\n" \
	"uniform float fade_factor;						\n" \
	"uniform sampler2D textures[2];					\n" \
	"												\n" \
	"varying vec2 texcoord;							\n" \
	"												\n" \
	"void main()									\n" \
	"{												\n" \
	"	gl_FragColor = mix(							\n" \
	"		texture2D(textures[0], texcoord),		\n" \
	"		texture2D(textures[1], texcoord),		\n" \
	"		fade_factor);							\n" \
	"}												\n"; 

// GL_ARRAY_BUFFER to store vertex elements
// GL_ELEMENT_ARRAY_BUFFER to store elements
// Usage:
// Tutorial: DYNAMIC: we intend to write into the buffer frequently; STATIC: we don't ever intend to change the data; STREAM: regularly replace the entire contents of the buffer
// GL_STREAM_DRAW
// - Der Inhalt des Datenspeichers wird einmal durch die Anwendung festgelegt und selten als Quelle für einen GL - Renderbefehl benutzt.
// GL_STREAM_READ
// - Der Inhalt des Datenspeichers wird einmal zum Auslesen von Daten festgelegt und selten durch die Applikation abgefragt.
// GL_STREAM_COPY
// - Der Inhalt des Datenspeichers wird einmal zum Auslesen von Daten festgelegt und selten als Quelle für einen GL - Renderbefehl benutzt.
// GL_STATIC_DRAW
// - Der Inhalt des Datenspeichers wird einmal durch die Anwendung festgelegt und häufig als Quelle für einen GL - Renderbefehl benutzt.
// GL_STATIC_READ
// - Der Inhalt des Datenspeichers wird einmal zum Auslesen von Daten festgelegt und häufig durch die Applikation abgefragt.
// GL_STATIC_COPY
// - Der Inhalt des Datenspeichers wird einmal zum Auslesen von Daten festgelegt und häufig als Quelle für einen GL - Renderbefehl benutzt.
// GL_DYNAMIC_DRAW
// - Der Inhalt des Datenspeichers wird wiederholt durch die Anwendung festgelegt und häufig als Quelle für einen GL - Renderbefehl benutzt.
// GL_DYNAMIC_READ
// - Der Inhalt des Datenspeichers wird wiederholt zum Auslesen von Daten festgelegt und häufig durch die Applikation abgefragt.
// GL_DYNAMIC_COPY
// - Der Inhalt des Datenspeichers wird wiederholt zum Auslesen von Daten festgelegt und häufig als Quelle für einen GL - Renderbefehl benutzt.
static GLuint make_buffer(GLenum target, const void *buffer_data, GLsizei buffer_size) 
{
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(target, buffer);
	glBufferData(target, buffer_size, buffer_data, GL_STATIC_DRAW);
	return buffer;
}

// Texture:
// GL_TEXTURE_1D
// GL_TEXTURE_2D
// GL_TEXTURE_3D
// Texture mapping:
//                                              t-axis (height elements)
//                                                ^
//                                                |
//												[1,0]                [1,1]
//                                                   8   9   10  11
// tex[12] = [0,1,2,3,4,5,6,7,8,9,10,11,11] ->       4   5   6   7
//                                                   0   1   2   3
//                                              [0,1]                [0,1] -> s-axis (width elements)
// 
// 
static GLuint make_texture(const char *filename)
{
	GLuint texture;
	int width, height;
	void *pixels = read_tga(filename, &width, &height);

	if (!pixels)
		return 0;
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// The glTexImage2D (or -1D or -3D) function allocates memory for a texture. Textures can have multiple levels of detail, sampling from 
	// a hierarchy of progressively smaller "mipmaps" when sampled at lower resolutions, but in our case we only supply the base level zero. 
	// The border argument is a relic and should always be zero.
	glTexImage2D(
		GL_TEXTURE_2D, 0,           /* target, level of detail */
		GL_RGB8,                    /* internal format */
		width, height, 0,           /* width, height, border */
		GL_BGR, GL_UNSIGNED_BYTE,   /* external format, type */
		pixels                      /* pixels */
	);
	free(pixels);
	return texture;
}

// PFNGLGETSHADERIVPROC and PFNGLGETSHADERINFOLOGPROC from glew
static void show_info_log(
	GLuint object,
	PFNGLGETSHADERIVPROC glGet__iv,
	PFNGLGETSHADERINFOLOGPROC glGet__InfoLog
)
{
	GLint log_length;
	char *log;

	glGet__iv(object, GL_INFO_LOG_LENGTH, &log_length);
	log = (char*)malloc(log_length);
	glGet__InfoLog(object, log_length, NULL, log);
	std::cout << log << std::endl;
	free(log);
}

static GLuint make_shader(GLenum type, GLchar *source)
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
		show_info_log(shader, glGetShaderiv, glGetShaderInfoLog);
		glDeleteShader(shader);
		assert(0);
		exit(0);
	}
	return shader;
}

static GLuint make_program(GLuint vertex_shader, GLuint fragment_shader)
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
		show_info_log(program, glGetProgramiv, glGetProgramInfoLog);
		glDeleteProgram(program);
		return 0;
	}
	return program;
}

// ==========================================================================
static int make_resources(void)
{
	g_resources.vertex_buffer = make_buffer(
		GL_ARRAY_BUFFER,
		g_vertex_buffer_data,
		sizeof(g_vertex_buffer_data)
	);
	g_resources.element_buffer = make_buffer(
		GL_ELEMENT_ARRAY_BUFFER,
		g_element_buffer_data,
		sizeof(g_element_buffer_data)
	);

	g_resources.textures[0] = make_texture("O:\\jvx\\base\\sources\\jvxApplications\\jvxTestOpenGl\\pics\\hello1.tga");
	g_resources.textures[1] = make_texture("O:\\jvx\\base\\sources\\jvxApplications\\jvxTestOpenGl\\pics\\hello2.tga");

	if (g_resources.textures[0] == 0 || g_resources.textures[1] == 0)
		return 0;

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

	g_resources.uniforms.fade_factor
		= glGetUniformLocation(g_resources.program, "fade_factor");
	g_resources.uniforms.textures[0]
		= glGetUniformLocation(g_resources.program, "textures[0]");
	g_resources.uniforms.textures[1]
		= glGetUniformLocation(g_resources.program, "textures[1]");

	g_resources.attributes.position = glGetAttribLocation(g_resources.program, "position");

	return 1;
}

static void update_fade_factor(void)
{
	int milliseconds = glutGet(GLUT_ELAPSED_TIME);
	g_resources.fade_factor = sinf((float)milliseconds * 0.001f) * 0.5f + 0.5f;
	glutPostRedisplay();
}

static void render(void)
{
	// Clear the framebuffer
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

	//std::cout << "Render" << std::endl;

	// Use the vertex/fragment shader program
	glUseProgram(g_resources.program);

	// Associate the uniform variable to the GPU space
	// glUniform{dim}{type}, where the dim indicates the size of a vector type (1 for an int or float uniform, 
	// 2 for a vec2, etc.), and the type indicates the component type: either i for integer, or f for floating-point
	glUniform1f(g_resources.uniforms.fade_factor, g_resources.fade_factor);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_resources.textures[0]);
	glUniform1i(g_resources.uniforms.textures[0], 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, g_resources.textures[1]);
	glUniform1i(g_resources.uniforms.textures[1], 1);



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

	// Wait for all jobs to complete and draw window content
    glutSwapBuffers();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(400, 300);
    glutCreateWindow("Hello World");
    glutDisplayFunc(&render);
    glutIdleFunc(&update_fade_factor);
	
	 glewInit();
    if (!GLEW_VERSION_2_0) 
	{
		std::cout << "OpenGL 2.0 not available" << std::endl;
        return 1;
    }
	
	if (!make_resources()) 
	{
		std::cout << "Failed to load resources" << std::endl;
        return 1;
    }

    glutMainLoop();
    return 0;
}

