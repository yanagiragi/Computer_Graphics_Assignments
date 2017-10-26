#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>

//#include "Texture.cpp"

#define TEXTURE_NUM 6 

// Light sorce parameter
float LightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };			// Light position
float LightAmb[] = { 1.0f, 1.0f, 1.0f, 1.0f };			// Ambient Light Values
float LightDif[] = { 1.0f, 1.0f, 1.0f, 1.0f };			// Diffuse Light Values
float LightSpc[] = { 1.0f, 1.0f, 1.0f, 1.0f };			// Specular Light Values

struct Image {
	unsigned long sizeX;
	unsigned long sizeY;
	char *data;
};
typedef struct Image Image;

char* texture_name[TEXTURE_NUM] = {
	"../Resource/sun.bmp",
	"../Resource/mercury.bmp",
	"../Resource/earth.bmp",
	"../Resource/moon.bmp",
	"../Resource/jupiter.bmp",
	"../Resource/europa.bmp"
};

GLuint texture[TEXTURE_NUM];
GLUquadricObj* quad = gluNewQuadric();				// glu quadratic object for drawing shape like sphere and cylinder
unsigned int frame = 0;

// 24-bit bmp loading function, no need to modify it
int ImageLoad(char *filename, Image *image) 
{
	FILE *file;
	unsigned long size; // size of the image in bytes.
	unsigned long i; // standard counter.
	unsigned short int planes; // number of planes in image (must be 1)
	unsigned short int bpp; // number of bits per pixel (must be 24)
	char temp; // temporary color storage for bgr-rgb conversion.
			   // make sure the file is there.
	if ((file = fopen(filename, "rb")) == NULL) {
		printf("File Not Found : %s\n", filename);
		return 0;
	}

	// seek through the bmp header, up to the width/height:
	fseek(file, 18, SEEK_CUR);
	// read the width
	if ((i = fread(&image->sizeX, 4, 1, file)) != 1) {
		printf("Error reading width from %s.\n", filename);
		return 0;
	}

	//printf("Width of %s: %lu\n", filename, image->sizeX);
	// read the height
	if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {
		printf("Error reading height from %s.\n", filename);
		return 0;
	}
	//printf("Height of %s: %lu\n", filename, image->sizeY);
	// calculate the size (assuming 24 bits or 3 bytes per pixel).
	size = image->sizeX * image->sizeY * 3;
	// read the planes
	if ((fread(&planes, 2, 1, file)) != 1) {
		printf("Error reading planes from %s.\n", filename);
		return 0;
	}
	if (planes != 1) {
		printf("Planes from %s is not 1: %u\n", filename, planes);
		return 0;
	}
	// read the bitsperpixel
	if ((i = fread(&bpp, 2, 1, file)) != 1) {
		printf("Error reading bpp from %s.\n", filename);
		return 0;
	}
	if (bpp != 24) {
		printf("Bpp from %s is not 24: %u\n", filename, bpp);
		return 0;
	}
	// seek past the rest of the bitmap header.
	fseek(file, 24, SEEK_CUR);
	// read the data.
	image->data = (char *)malloc(size);
	if (image->data == NULL) {
		printf("Error allocating memory for color-corrected image data");
		return 0;
	}
	if ((i = fread(image->data, size, 1, file)) != 1) {
		printf("Error reading image data from %s.\n", filename);
		return 0;
	}
	for (i = 0; i<size; i += 3) { 
		// reverse all of the colors. (bgr -> rgb)
		temp = image->data[i];
		image->data[i] = image->data[i + 2];
		image->data[i + 2] = temp;
	}
	// we're done.
	return 1;
}

// memory allocation and file reading for an Image datatype, no need to modify it
Image * loadTexture(char *filename)
{
	Image *image;
	// allocate space for texture
	image = (Image *)malloc(sizeof(Image));
	if (image == NULL) {
		printf("Error allocating space for image");
		getchar();
		exit(0);
	}
	if (!ImageLoad(filename, image)) {
		getchar();
		exit(1);
	}
	return image;
}

void Init()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);					// set what color should be used when we clean the color buffer
	glEnable(GL_LIGHT0);								// Enable Light1
	glEnable(GL_LIGHTING);								// Enable Lighting
						  
	glLightfv(GL_LIGHT0, GL_POSITION, LightPos);		// Set Light1 Position, this setting function should be at another place
												
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmb);			// Set Light1 Ambience
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDif);			// Set Light1 Diffuse
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpc);		// Set Light1 Specular
	
	// since a light source is also an object in the 3D scene, we set its position in the display function after gluLookAt()
	// you should know that straight texture mapping(without modifying shader) may not have shading effect	
	// you need to tweak certain parameter(when building texture) to obtain a lit and textured object
	
	glShadeModel(GL_SMOOTH);							// shading model

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);	// you can choose which part of lighting model should be modified by texture mapping
	//glEnable(GL_COLOR_MATERIAL);						// enable this parameter to use glColor() as material of lighting model

	// please load all the textures here
	// use Image* loadTexture(file_directory) function to obtain char* data and size info.

	// Create one OpenGL texture
	for (int i = 0; i < TEXTURE_NUM; ++i) {
		// GLuint textureID;
		glGenTextures(1, &(texture[i]));

		// "Bind" the newly created texture : all future texture functions will modify this texture
		glBindTexture(GL_TEXTURE_2D, texture[i]);

		Image* texture = loadTexture(texture_name[i]);

		// Give the image to OpenGL
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->sizeX, texture->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, texture->data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
}

void DrawPlanet(int index) 
{
	if (index == 0) {
		return;
	}

	glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture[index]);

		// Rotate Self
		//glTranslatef(0.0f, 0.0f, 0.0f);
		//glRotatef(90.0f + frame, 0.0f, 0.5f, 0.0f);

		// Rotate Around axis
		//glPushMatrix();
		if (index == 2) {
			glRotatef(90.0f + frame, 0.0f, 1.0f, 0.0f);
			glTranslatef(index * 5, 0.0f, 0.0f);
		}
		else {
			glRotatef(90.0f + frame, 1.0f, 0.0f, 0.0f);
			glTranslatef(0.0f, index * 5, 0.0f);

		}
		

		//glPopMatrix();

		//glTranslatef(index * 5, 0.0f, 0.0f);

		gluQuadricTexture(quad, GL_TRUE);
		gluSphere(quad, 2, 30, 30);

		glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

//callback function for drawing a frame
void Display(void)
{
	++frame;
	printf("\rFrame = %d.", frame);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0, 20, 0, 0, 0, 0, 1, 0);			// set the view part of modelview matrix
	
	// please draw the scene in the following region
	// here are some example objects without texture
	// please notice how they look like without rotation

	//glDisable(GL_TEXTURE_2D);							// when you draw something without texture, be sure to disable GL_TEXTURE_2D
	glLightfv(GL_LIGHT0, GL_POSITION, LightPos);		// Set Light1 Position, this setting function should be at another place

	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmb);			// Set Light1 Ambience
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDif);			// Set Light1 Diffuse
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpc);		// Set Light1 Specular

	glPushMatrix();
	for (int i = 0; i < 3; ++i) {
		DrawPlanet(i);
	}
	glPopMatrix();
	
	
	glPushMatrix();
		glTranslatef(-5.0, -5.0, 0.0);
		glColor3f(1.0, 0.0, 1.0);
		//glutSolidCone(2, 5, 20, 20);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(7.0, 0.0, -5.0);
		glColor3f(1.0, 1.0, 0.0);
		//gluCylinder(quad, 1, 1, 3, 20, 20);
	glPopMatrix();

	

	glutSwapBuffers();									// swap the drawn buffer to the window
}