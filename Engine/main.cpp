#include "Dependancies\glew\glew.h"
#include "Dependancies\freeglut\freeglut.h"
#include "Dependancies\glm\glm.hpp"
#include "Core\ShaderManager.h"
#include "Core\GameModels.h"

#include <iostream>

GameModels* gameModels;
GLuint program;

void RenderScene()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);//clear red
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glBindVertexArray(gameModels->GetModel("triangle1"));

	//use the created program
	glUseProgram(program);

	//draw 3 vertices as triangles
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glutSwapBuffers();
}

void CloseCallback()
{
	std::cout << "GLUT:\t Finished" << std::endl;
	glutLeaveMainLoop();
}


void InitialiseGLEW()
{
	glewInit();
	if (glewIsSupported("GL_VERSION_4_5"))
	{
		std::cout << " GLEW Version is 4.5\n ";
	}
	else
	{
		std::cout << "GLEW 4.5 not supported\n ";
	}
}

void InitialiseGLUTWindow(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 500);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Engine");
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
}

void InitialiseShaders()
{
	Core::ShaderManager shaderManager;
	program = shaderManager.CreateProgram("Shaders\\basicPositionColor.vert", "Shaders\\basicColor.frag");
}

void Initialise(int argc, char **argv)
{
	InitialiseGLUTWindow(argc, argv);
	InitialiseGLEW();

	gameModels = new GameModels();
	gameModels->CreateTriangleModel("triangle1");


	glEnable(GL_DEPTH_TEST);
	InitialiseShaders();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// register callbacks
	glutDisplayFunc(RenderScene);
	glutCloseFunc(CloseCallback);

	glutMainLoop();
}

int main(int argc, char **argv)
{
	Initialise(argc, argv);

	//cleanup
	delete gameModels;
	glDeleteProgram(program);
	return 0;
}





