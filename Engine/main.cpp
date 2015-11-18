#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include "Dependencies\glm\glm.hpp"

#include "Core\Initialise\InitialiseGLUT.h"
#include "Managers\SceneManager.h"

#include "Scenes/TestScene.h"

using namespace Core;
using namespace Initialisation;

int main(int argc, char **argv)
{
	WindowInfo window(std::string("Engine"),
		400, 200,//position
		800, 600, //size
		true);//resizable

	ContextInfo context(4, 5, true);
	FramebufferInfo frameBufferInfo(true, true, true, true);

	InitialiseGLUT::Initialise(window, context, frameBufferInfo);

	IListener* scene = new TestScene(window);
	scene->Initialise();
	
	InitialiseGLUT::SetListener(scene);

	InitialiseGLUT::Run();

	//cleanup
	delete scene;
	return 0;
}





