#include "Dependancies\glew\glew.h"
#include "Dependancies\freeglut\freeglut.h"
#include <iostream>


void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 0.0, 0.0, 1.0);//clear red

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

void InitialiseWindow(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 500);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Engine");


	InitialiseGLEW();

	glEnable(GL_DEPTH_TEST);

	// register callbacks
	glutDisplayFunc(RenderScene);

	glutMainLoop();
}


int main(int argc, char **argv)
{
	InitialiseWindow(argc, argv);
	return 0;
}





