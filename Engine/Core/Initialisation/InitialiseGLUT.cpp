#include "InitialiseGLUT.h"
#include "../../Core/Utils.h"
#include <memory>

using namespace Core::Initialisation;

std::shared_ptr<Core::IListener> InitialiseGLUT::listener = nullptr;
WindowInfo InitialiseGLUT::windowInformation;

void InitialiseGLUT::Initialise(const WindowInfo& windowInfo,
	const ContextInfo& contextInfo,
	const FramebufferInfo& framebufferInfo)
{
	windowInformation = windowInfo;

	//we need to create these fake arguments
	int fakeargc = 1;
	char *fakeargv[] = { "fake", nullptr };
	glutInit(&fakeargc, fakeargv);

	if (contextInfo.core)
	{
		glutInitContextVersion(contextInfo.major_version,
			contextInfo.minor_version);
		glutInitContextProfile(GLUT_CORE_PROFILE);
	}
	else
	{
		//version doesn't matter in Compatibility mode
		glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	}

	//Support for glDebugMessageCallback in debug builds
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE
	#if _DEBUG
		| GLUT_DEBUG
	#endif
		);


	glutInitDisplayMode(framebufferInfo.flags);
	glutInitWindowPosition(windowInfo.position_x, windowInfo.position_y);
	glutInitWindowSize(windowInfo.width, windowInfo.height);

	if (windowInfo.useGameMode)
	{
		glutGameModeString(windowInfo.gameModeString.c_str());
		glutEnterGameMode();
	}

	glutCreateWindow(windowInfo.name.c_str());

	std::cout << "GLUT:initialized" << std::endl;
	//these callbacks are used for rendering
	glutIdleFunc(IdleCallback);
	glutCloseFunc(CloseCallback);
	glutDisplayFunc(DisplayCallback);
	glutReshapeFunc(ResizeCallback);

	glutKeyboardFunc(ProcessNormalKeysCallback);
	glutKeyboardUpFunc(ProcessNormalKeysUpCallback);
	glutSpecialFunc(ProcessSpecialKeysCallback);

	glutMouseFunc(ProcessMouseStateCallback);
	glutMotionFunc(ProcessMouseActiveMoveCallback);
	glutPassiveMotionFunc(ProcessMousePassiveMoveCallback);
	glutEntryFunc(ProcessMouseWindowEntryCallback);

#if _DEBUG
	if (glDebugMessageCallback) 
	{
		std::cout << "Register OpenGL debug callback " << std::endl;
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(ProcessErrorCallback, nullptr);
		GLuint unusedIds = 0;
		glDebugMessageControl(GL_DONT_CARE,
			GL_DONT_CARE,
			GL_DONT_CARE,
			0,
			&unusedIds,
			true);
	}
	else
		std::cout << "glDebugMessageCallback not available" << std::endl;
#endif


	//init GLEW
	InitialiseGLEW::Initialise();

	//cleanup
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	//our method to display some info. Needs contextInfo and windowinfo
	PrintOpenGLInfo(windowInfo, contextInfo);
	Check_GLError();
}

//starts the rendering Loop
void InitialiseGLUT::Run()
{
	std::cout << "GLUT:\t Start Running " << std::endl;
	glutMainLoop();
}

void InitialiseGLUT::IdleCallback()
{
	//do nothing, just redisplay
	glutPostRedisplay();
}

void InitialiseGLUT::Close()
{
	std::cout << "GLUT:\t Finished" << std::endl;
	glutLeaveMainLoop();
}

void InitialiseGLUT::CloseCallback()
{
	Close();
}

void InitialiseGLUT::DisplayCallback()
{
	//check for NULL
	if (listener)
	{
		listener->notifyBeginFrame();
		listener->notifyDisplayFrame();
	}

	glutSwapBuffers();
	
	if(listener)
		listener->notifyEndFrame();
}

void InitialiseGLUT::ResizeCallback(int width, int height)
{
	if (windowInformation.isResizable == true)
	{
		if (listener)
		{
			listener->notifyReshape(width, height, windowInformation.width, 
				windowInformation.height);
		}
		windowInformation.width = width;
		windowInformation.height = height;
	}
}

void InitialiseGLUT::ProcessNormalKeysCallback(unsigned char key, int x, int y) 
{
	if (listener)
		listener->notifyProcessNormalKeys(key, x, y);
}

void InitialiseGLUT::ProcessNormalKeysUpCallback(unsigned char key, int x, int y)
{
	if (listener)
		listener->notifyProcessNormalKeysUp(key, x, y);
}

void InitialiseGLUT::ProcessSpecialKeysCallback(int key, int x, int y)
{
	if(listener)
		listener->notifyProcessSpecialKeys(key, x, y);
}

void InitialiseGLUT::ProcessMouseStateCallback(int button, int state, int x, int y)
{
	if (listener)
		listener->notifyProcessMouseState(button, state, x, y);
}

void InitialiseGLUT::ProcessMouseActiveMoveCallback(int x, int y)
{
	if (listener)
		listener->notifyProcessMouseActiveMove(x, y);
}

void InitialiseGLUT::ProcessMousePassiveMoveCallback(int x, int y)
{
	if (listener)
		listener->notifyProcessMousePassiveMove(x, y);
}

void InitialiseGLUT::ProcessMouseWindowEntryCallback(int state)
{
	if (listener)
		listener->notifyProcessMouseWindowEntryCallback(state);
}

void InitialiseGLUT::EnterFullscreen()
{
	glutFullScreen();
}

void InitialiseGLUT::ExitFullscreen()
{
	glutLeaveFullScreen();
}

void InitialiseGLUT::PrintOpenGLInfo(const WindowInfo& windowInfo,
	const ContextInfo& contextInfo) {

	const unsigned char* renderer = glGetString(GL_RENDERER);
	const unsigned char* vendor = glGetString(GL_VENDOR);
	const unsigned char* version = glGetString(GL_VERSION);

	std::cout << "******************************************************               ************************" << std::endl;
	std::cout << "GLUT:Initialise" << std::endl;
	std::cout << "GLUT:\tVendor : " << vendor << std::endl;
	std::cout << "GLUT:\tRenderer : " << renderer << std::endl;
	std::cout << "GLUT:\tOpenGl version: " << version << std::endl;
}

void InitialiseGLUT::SetListener(std::shared_ptr<Core::IListener>& iListener)
{
	listener = iListener;
}
void APIENTRY InitialiseGLUT::ProcessErrorCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	if (listener)
		listener->notifyErrorCallback(source, type, id, severity, length, message, userParam);
}