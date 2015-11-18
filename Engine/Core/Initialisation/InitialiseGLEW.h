#pragma once
#include <iostream>
#include "../../Dependencies/glew/glew.h"
#include "../../Dependencies/freeglut/freeglut.h"

namespace Core
{
	namespace Initialisation
	{
		class InitialiseGLEW
		{
		public:
			static void Initialise();
		};
	}
}
