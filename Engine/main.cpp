#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include "Dependencies\glm\glm.hpp"

#include "Core\Initialisation\InitialiseGLUT.h"
#include "Managers\SceneManager.h"

#include "Scenes/TestScene.h"
#include "Scenes/ServerScene.h"
#include "Scenes/ClientScene.h"

using namespace Core;
using namespace Initialisation;

//Force use of dedicated graphics card rather than embedded
extern "C" { _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001; }

int main(int argc, char **argv)
{
	WindowInfo window(std::string("Engine"),
		400, 200,//position
		800, 600, //size
		true);//resizable

	ContextInfo context(4, 5, true);
	FramebufferInfo frameBufferInfo(true, true, true, true);

	InitialiseGLUT::Initialise(window, context, frameBufferInfo);


#if IS_SERVER
	IListener* scene = new ServerScene(window);
#else
	//IListener* scene = new TestScene(window);
	IListener* scene = new ClientScene(window);
#endif
	scene->Initialise();
	InitialiseGLUT::SetListener(scene);

	InitialiseGLUT::Run();

	//cleanup
	delete scene;
	return 0;
}





