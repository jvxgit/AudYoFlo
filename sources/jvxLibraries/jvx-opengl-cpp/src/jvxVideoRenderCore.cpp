#if defined(JVX_VIDEO_RENDER_CORE_CLASS)
#error "Micsonfigured native render library"
#endif
#include "jvxVideoRenderCore.h"

void 
JVXVIDEORENDERCORE::trigger_render()
{
	glutPostRedisplay();
};

void 
JVXVIDEORENDERCORE::finalize_render()
{
	// Wait for all jobs to complete and draw window content
	glutSwapBuffers();
};

jvxErrorType 
JVXVIDEORENDERCORE::system_initialize(const char* txtToShow)
{
	int argc = 0;
	char** argv = NULL;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(400, 300);
	glutCreateWindow(txtToShow);
	glutDisplayFunc(&jvx_ogl_render);
	glutIdleFunc(&jvx_ogl_idle);

	glewInit();
	if (!GLEW_VERSION_2_0)
	{
		std::cout << "OpenGL 2.0 not available" << std::endl;
		return JVX_ERROR_INVALID_SETTING;
	}
	return JVX_NO_ERROR;
};

void 
JVXVIDEORENDERCORE::system_passcontrol()
{
	glutMainLoop();
};

void 
JVXVIDEORENDERCORE::system_regaincontrol()
{
#ifdef JVX_OS_MACOSX
  std::cout << "GLUT implementation of Mac OSX does not support:" << std::endl;
  std::cout << "--> GLUT_ACTION_ON_WINDOW_CLOSE" << std::endl;
  std::cout << "--> glutLeaveMainLoop" << std::endl;
  assert(0);
#else
  
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	glutLeaveMainLoop();
#endif
};

