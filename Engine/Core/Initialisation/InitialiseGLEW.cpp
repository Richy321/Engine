#include "InitialiseGLEW.h"
using namespace Core;
using namespace Core::Initialisation;

void InitialiseGLEW::Initialise() {

	glewExperimental = true;
	if (glewInit() == GLEW_OK)
	{
		std::cout << "GLEW: Initialize - " << glewGetString(GLEW_VERSION) << std::endl;
	}
}