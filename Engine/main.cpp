#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include "Dependencies\glm\glm.hpp"

#include "Core\Initialise\InitialiseGLUT.h"
#include "Managers\SceneManager.h"

#include <iostream>


using namespace Core;
using namespace Initialisation;

void InitialiseModels()
{

}

int main(int argc, char **argv)
{
	WindowInfo window(std::string("in2gpu OpenGL Beginner Tutorial "),
		400, 200,//position
		800, 600, //size
		true);//resizable

	ContextInfo context(4, 5, true);
	FramebufferInfo frameBufferInfo(true, true, true, true);

	InitialiseGLUT::Initialise(window, context, frameBufferInfo);

	IListener* scene = new Managers::SceneManager();
	Initialisation::InitialiseGLUT::SetListener(scene);

	InitialiseGLUT::Run();

	//cleanup
	delete scene;
	return 0;
}





