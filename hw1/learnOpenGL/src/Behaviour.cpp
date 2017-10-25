#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>

//#include "Texture.cpp"

// Light sorce parameter
float LightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };			// Light position
float LightAmb[] = { 0.0f, 0.0f, 0.0f, 1.0f };			// Ambient Light Values
float LightDif[] = { 1.0f, 1.0f, 1.0f, 1.0f };			// Diffuse Light Values
float LightSpc[] = { 1.0f, 1.0f, 1.0f, 1.0f };			// Specular Light Values

void Init()
{
	glClearColor(1.0, 0.0, 0.0, 1.0);					// set what color should be used when we clean the color buffer
	glEnable(GL_LIGHT1);								// Enable Light1
	glEnable(GL_LIGHTING);								// Enable Lighting
						  
	glLightfv(GL_LIGHT1, GL_POSITION, LightPos);		// Set Light1 Position, this setting function should be at another place
												
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmb);			// Set Light1 Ambience
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDif);			// Set Light1 Diffuse
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpc);		// Set Light1 Specular
	
	// since a light source is also an object in the 3D scene, we set its position in the display function after gluLookAt()
	// you should know that straight texture mapping(without modifying shader) may not have shading effect	
	// you need to tweak certain parameter(when building texture) to obtain a lit and textured object
	
	glShadeModel(GL_SMOOTH);							// shading model

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);	// you can choose which part of lighting model should be modified by texture mapping
	glEnable(GL_COLOR_MATERIAL);						// enable this parameter to use glColor() as material of lighting model

	// please load all the textures here
	// use Image* loadTexture(file_directory) function to obtain char* data and size info.

}

//callback function for drawing a frame
void Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0, 20, 0, 0, 0, 0, 1, 0);			// set the view part of modelview matrix
	GLUquadricObj* quad = gluNewQuadric();				// glu quadratic object for drawing shape like sphere and cylinder

	// please draw the scene in the following region
	// here are some example objects without texture
	// please notice how they look like without rotation

	glDisable(GL_TEXTURE_2D);							// when you draw something without texture, be sure to disable GL_TEXTURE_2D
	glPushMatrix();
	glTranslatef(0.0, 3.0, 0.0);
	glColor3f(0.0, 1.0, 1.0);
	gluSphere(quad, 2, 20, 20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-5.0, -5.0, 0.0);
	glColor3f(1.0, 0.0, 1.0);
	glutSolidCone(2, 5, 20, 20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(7.0, 0.0, -5.0);
	glColor3f(1.0, 1.0, 0.0);
	gluCylinder(quad, 1, 1, 3, 20, 20);
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);


	glutSwapBuffers();									// swap the drawn buffer to the window
}