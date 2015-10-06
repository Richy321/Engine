#include "InitialiseGLUT.h"

using namespace Core::Initialisation;

Core::IListener* InitialiseGLUT::listener = NULL;
Core::WindowInfo InitialiseGLUT::windowInformation;

void InitialiseGLUT::Initialise(const Core::WindowInfo& windowInfo,
	const Core::ContextInfo& contextInfo,
	const Core::FramebufferInfo& framebufferInfo)
{
	windowInformation = windowInfo;//add this line

	//we need to create these fake arguments
	int fakeargc = 1;
	char *fakeargv[] = { "fake", NULL };
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

	glutInitDisplayMode(framebufferInfo.flags);
	glutInitWindowPosition(windowInfo.position_x, windowInfo.position_y);
	glutInitWindowSize(windowInfo.width, windowInfo.height);

	glutCreateWindow(windowInfo.name.c_str());

	std::cout << "GLUT:initialized" << std::endl;
	//these callbacks are used for rendering
	glutIdleFunc(IdleCallback);
	glutCloseFunc(CloseCallback);
	glutDisplayFunc(DisplayCallback);
	glutReshapeFunc(ResizeCallback);

	//init GLEW, this can be called in main.cpp
	Initialisation::InitialiseGLEW::Initialise();

	//cleanup
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	//our method to display some info. Needs contextInfo and windowinfo
	PrintOpenGLInfo(windowInfo, contextInfo);
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
			listener->notifyReshape(width,
				height,
				windowInformation.width,
				windowInformation.height);
		}
		windowInformation.width = width;
		windowInformation.height = height;
	}
}

void InitialiseGLUT::EnterFullscreen()
{
	glutFullScreen();
}

void InitialiseGLUT::ExitFullscreen()
{
	glutLeaveFullScreen();
}

void InitialiseGLUT::PrintOpenGLInfo(const Core::WindowInfo& windowInfo,
	const Core::ContextInfo& contextInfo) {

	const unsigned char* renderer = glGetString(GL_RENDERER);
	const unsigned char* vendor = glGetString(GL_VENDOR);
	const unsigned char* version = glGetString(GL_VERSION);

	std::cout << "******************************************************               ************************" << std::endl;
	std::cout << "GLUT:Initialise" << std::endl;
	std::cout << "GLUT:\tVendor : " << vendor << std::endl;
	std::cout << "GLUT:\tRenderer : " << renderer << std::endl;
	std::cout << "GLUT:\tOpenGl version: " << version << std::endl;
}

void InitialiseGLUT::SetListener(Core::IListener*& iListener)
{
	listener = iListener;
}