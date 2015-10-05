#include "Dependancies\glew\glew.h"
#include "Dependancies\freeglut\freeglut.h"
#include <iostream>

#include "Core\ShaderManager.h"

GLuint program;

void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 0.0, 0.0, 1.0);//clear red

	//use the created program
	glUseProgram(program);

	//draw 3 vertices as triangles
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glutSwapBuffers();
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
}

void InitialiseShaders()
{
	Core::ShaderManager shaderManager;
	program = shaderManager.CreateProgram("Shaders\\basicTriangle.vert", "Shaders\\basicGreen.frag");
}

void Initialise(int argc, char **argv)
{
	InitialiseGLUTWindow(argc, argv);
	InitialiseGLEW();

	glEnable(GL_DEPTH_TEST);
	InitialiseShaders();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// register callbacks
	glutDisplayFunc(RenderScene);

	glutMainLoop();
}

int main(int argc, char **argv)
{
	Initialise(argc, argv);
	glDeleteProgram(program);
	return 0;
}





