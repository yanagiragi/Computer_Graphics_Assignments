/*
*	HW1: A Simple Solar System
*	Author: yanagiragi
*	Course: NCTU Computer Graphics
*/
#include "glew.h"
#include "glut.h"
#include <GL/glu.h>
#include <GL/gl.h>

#include <stdlib.h>
#include <iostream>

#include "Behaviour.cpp"

// time parameter for helping coordinate your animation, you may utilize it to help perform animation
#define deltaTime 10
double time = 0.0;

// callback funtion as a timer, no need to modify it
void Tick(int id) {
	double d = deltaTime / 1000.0;
	time += d;
	glutPostRedisplay();
	glutTimerFunc(deltaTime, Tick, 0);							// 100ms for time step size
}

// callback function when the window size is changed, no need to modify it
void WindowSize(int w, int h)
{
	glViewport(0, 0, w, h);										// changing the buffer size the same to the window size
	glMatrixMode(GL_PROJECTION);								// choose the projection matrix
	glLoadIdentity();
	gluPerspective(60.0, (double)w / (double)h, 1.0, 1000.0);	// set the projection matrix as perspective mode
	glMatrixMode(GL_MODELVIEW);									// it is suggested that modelview matrix is chosen if nothing specific being performed
	glLoadIdentity();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);										// glut function for simplifying OpenGL initialization
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);	// demanding: double-framed buffer | RGB colorbuffer | depth buffer
	glutInitWindowPosition(100, 50);							// set the initial window position
	glutInitWindowSize(800, 600);								// set the initial window size
								 
	glutCreateWindow("CG_HW1_0556652");

	glutDisplayFunc(Display);									// callback funtion for drawing a frame
	glutReshapeFunc(WindowSize);								// callback function when the window size is changed
	glutTimerFunc(deltaTime, Tick, 0);							// timer function
	glutKeyboardFunc(keyboard);
									  
	Init();														// self-defined initialization function for the elegance of your code
	printf("FPS = %f\n", 1.0f / deltaTime * 1000.0f);

	glutMainLoop();

	Destroy();
	return 0;
}