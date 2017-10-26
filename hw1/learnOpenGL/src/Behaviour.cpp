//#include <GL/glut.h>
#include "GL/glut.h"
#include <GL/glu.h>
#include <GL/gl.h>

//#include "Texture.cpp"

#define TEXTURE_NUM 6

#define SUN_INDEX 0
#define MECURY_INDEX 1
#define EARTH_INDEX 2
#define JUPITER_INDEX 3
#define EURPOPA_INDEX 4
#define MOON_INDEX 5

#pragma region  StructDecarations
struct Vector3 {
	float x, y, z;
};
typedef struct Vector3 Vector3;

struct Image {
	unsigned long sizeX;
	unsigned long sizeY;
	char *data;
};
typedef struct Image Image;


struct Planet {
	char *TextureName;
	GLuint Texture2D;

	Vector3 RotateSelfAxis;
	float RotateSelfAngle;

	Vector3 RotateRespectAxis;
	float RotateRespectAxisRotateAngle;
	float RotateRespectAngle;

	float Radius;

	float Translation; // Distance to rotate center
	float TranslationMin, TranslationMax; // Distance to rotate center
};
typedef struct Planet Planet;
#pragma endregion

Planet planets[TEXTURE_NUM];
GLUquadricObj* quad = gluNewQuadric();					// glu quadratic object for drawing shape like sphere and cylinder
unsigned int frame = 0;
bool frameStop = false;
bool debugMode = false;
int nowIndex = 0;

// Light sorce parameter
float LightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };			// Light position
float LightAmb[] = { 1.0f, 1.0f, 1.0f, 1.0f };			// Ambient Light Values
float LightDif[] = { 1.0f, 1.0f, 1.0f, 1.0f };			// Diffuse Light Values
float LightSpc[] = { 1.0f, 1.0f, 1.0f, 1.0f };			// Specular Light Values

/*================================================================================================================================*/
void setupPlanetTexture(int index);
void setupPlanets();
int ImageLoad(char *filename, Image *image);
Image * loadTexture(char *filename);
void Init();
void DrawPlanet(int index);
void Display(void);
void keyboard(unsigned char key, int x, int y);

void setupPlanetTexture(int index)
{
	// GLuint textureID;
	glGenTextures(1, &(planets[index].Texture2D));

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, planets[index].Texture2D);

	Image* texture = loadTexture(planets[index].TextureName);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->sizeX, texture->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, texture->data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void setupPlanets()
{
	Vector3 bufferVector3;

	// Sun
	planets[SUN_INDEX].TextureName = "../Resource/sun.bmp";
	planets[SUN_INDEX].Radius = 2.0f;
	bufferVector3.x = 0.0f;
	bufferVector3.y = 0.0f;
	bufferVector3.z = 0.0f;
	planets[SUN_INDEX].Translation = 0.0f;
	planets[SUN_INDEX].TranslationMin = planets[SUN_INDEX].TranslationMax = 0.0f;
	planets[SUN_INDEX].RotateRespectAxis = bufferVector3;
	planets[SUN_INDEX].RotateRespectAngle = 0.0f;
	planets[SUN_INDEX].RotateSelfAngle = 1.0f;
	bufferVector3.x = 0.0f;
	bufferVector3.y = 1.0f;
	bufferVector3.z = 0.0f;
	planets[SUN_INDEX].RotateSelfAxis = bufferVector3;
	planets[SUN_INDEX].RotateRespectAxisRotateAngle = 0.0f;
	

	// Mecury
	planets[MECURY_INDEX].TextureName = "../Resource/mercury.bmp";
	planets[MECURY_INDEX].Radius = 0.5f;
	
	planets[MECURY_INDEX].Translation = 4.0f;
	planets[MECURY_INDEX].TranslationMin = 4.0f;
	planets[MECURY_INDEX].TranslationMax = 15.0f;
	bufferVector3.x = 0.0f;
	bufferVector3.y = 1.0f;
	bufferVector3.z = 0.0f;
	planets[MECURY_INDEX].RotateRespectAxis = bufferVector3;
	planets[MECURY_INDEX].RotateRespectAngle = 1.0f;
	planets[MECURY_INDEX].RotateRespectAxisRotateAngle = 45.0f;

	bufferVector3.x = 0.0f;
	bufferVector3.y = 1.0f;
	bufferVector3.z = 0.0f;
	planets[MECURY_INDEX].RotateSelfAxis = bufferVector3;	
	planets[MECURY_INDEX].RotateSelfAngle = 0.5f;

	// Earth
	planets[EARTH_INDEX].TextureName = "../Resource/earth.bmp";
	planets[EARTH_INDEX].Radius = 1.0f;

	planets[EARTH_INDEX].Translation = 5.0f;
	planets[EARTH_INDEX].TranslationMin = 4.0f;
	planets[EARTH_INDEX].TranslationMax = 10.0f;
	bufferVector3.x = 0.0f;
	bufferVector3.y = 1.0f;
	bufferVector3.z = 0.0f;

	planets[EARTH_INDEX].RotateRespectAxis = bufferVector3;
	planets[EARTH_INDEX].RotateRespectAngle = 1.0f;
	planets[EARTH_INDEX].RotateRespectAxisRotateAngle = 30.0f;

	bufferVector3.x = 0.0f;
	bufferVector3.y = 1.0f;
	bufferVector3.z = 0.0f;
	planets[EARTH_INDEX].RotateSelfAxis = bufferVector3;
	planets[EARTH_INDEX].RotateSelfAngle = 1.0f;

	// Jupiter
	planets[JUPITER_INDEX].TextureName = "../Resource/jupiter.bmp";
	planets[JUPITER_INDEX].Radius = 1.5f;

	planets[JUPITER_INDEX].Translation = 11.0f;
	planets[JUPITER_INDEX].TranslationMin = 10.0f;
	planets[JUPITER_INDEX].TranslationMax = 15.0f;
	bufferVector3.x = 0.0f;
	bufferVector3.y = 1.0f;
	bufferVector3.z = 0.0f;

	planets[JUPITER_INDEX].RotateRespectAxis = bufferVector3;
	planets[JUPITER_INDEX].RotateRespectAngle = 1.0f;
	planets[JUPITER_INDEX].RotateRespectAxisRotateAngle = 10.0f;

	bufferVector3.x = 0.0f;
	bufferVector3.y = 1.0f;
	bufferVector3.z = 0.0f;
	planets[JUPITER_INDEX].RotateSelfAxis = bufferVector3;
	planets[JUPITER_INDEX].RotateSelfAngle = 0.3f;

	// europa
	planets[EURPOPA_INDEX].TextureName = "../Resource/europa.bmp";

	// moon
	planets[MOON_INDEX].TextureName = "../Resource/moon.bmp";


	for (int i = 0; i < TEXTURE_NUM; ++i) {
		setupPlanetTexture(i);
	}
}

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
	//glClearColor(0.0, 0.0, 0.0, 1.0);					// set what color should be used when we clean the color buffer
	glClearColor(0.07f, 0.447f, 0.05f, 1.0);					// set what color should be used when we clean the color buffer
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
	glEnable(GL_COLOR_MATERIAL);						// enable this parameter to use glColor() as material of lighting model

	setupPlanets();
}

void DrawPlanet(int index)
{
	glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, planets[index].Texture2D);

		// Rotate Around axis
		glRotatef(planets[index].RotateRespectAxisRotateAngle, 0.0f, 0.0f, 1.0f);
		glRotatef(planets[index].RotateRespectAngle * (float)(frame), planets[index].RotateRespectAxis.x, planets[index].RotateRespectAxis.y, planets[index].RotateRespectAxis.z);
		glTranslatef(planets[index].Translation, 0.0f, 0.0f);

		// Rotate Self
		glTranslatef(0.0f, 0.0f, 0.0f);
		glRotatef(planets[index].RotateSelfAngle * (float)(frame), planets[index].RotateSelfAxis.x, planets[index].RotateSelfAxis.y, planets[index].RotateSelfAxis.z);

		gluQuadricTexture(quad, GL_TRUE);
		gluSphere(quad, planets[index].Radius, 30, 30);

		if (debugMode) {
			// Debug Line Draw
			glPushMatrix();

			float baseRad = 0.03f, baseHeight = 5.0f;
			glDisable(GL_TEXTURE_2D);

			glTranslatef(0.0f, 0.0f, 0.0f);

			glColor3f(1.0f, 0.0f, 0.0f);
			glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
			gluCylinder(quad, baseRad, baseRad, baseHeight, 10, 5);

			glColor3f(0.0f, 1.0f, 0.0f);
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			gluCylinder(quad, baseRad, baseRad, baseHeight, 10, 5);

			glColor3f(0.0f, 0.0f, 1.0f);
			glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
			gluCylinder(quad, baseRad, baseRad, baseHeight, 10, 5);

			glColor3f(1.0f, 1.0f, 1.0f);
			glEnable(GL_TEXTURE_2D);
			glPopMatrix();
		}

		glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

//callback function for drawing a frame
void Display(void)
{
	if (!frameStop) {
		++frame;
		printf("[Info] Frame = %d\r", frame);
	}
	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0, 20, 0, 0, 0, 0, 1, 0);			// set the view part of modelview matrix
	
	glDisable(GL_TEXTURE_2D);

	glLightfv(GL_LIGHT0, GL_POSITION, LightPos);		// Set Light1 Position, this setting function should be at another place
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmb);			// Set Light1 Ambience
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDif);			// Set Light1 Diffuse
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpc);		// Set Light1 Specular
	//glEnable(GL_TEXTURE_2D);

	glPushMatrix();
		for (int i = 0; i < 4; ++i) {
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
	glDisable(GL_TEXTURE_2D);							// when you draw something without texture, be sure to disable GL_TEXTURE_2D


	glutSwapBuffers();									// swap the drawn buffer to the window
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case 'a':
			printf("[Info] ID = %d, RotateRespectAxisRotateAngle = %f\n", nowIndex, planets[nowIndex].RotateRespectAxisRotateAngle);
			if (planets[nowIndex].RotateRespectAxisRotateAngle >= 45.0f) {
				return;
			}
			else {
				planets[nowIndex].RotateRespectAxisRotateAngle += 1.0f;
				//planets[MECURY_INDEX].RotateRespectAxisRotateAngle -= 0.1f;
			}
			break;

		case 'd':		
			printf("[Info] ID = %d, RotateRespectAxisRotateAngle = %f\n", nowIndex, planets[nowIndex].RotateRespectAxisRotateAngle);
			if (planets[nowIndex].RotateRespectAxisRotateAngle <= -45.0f) {
				return;
			}
			else {
				planets[nowIndex].RotateRespectAxisRotateAngle -= 1.0f;
			}
			break;

		case 'w':
			printf("[Info] ID = %d, Translation = %f\n", nowIndex, planets[nowIndex].Translation);
			if (planets[nowIndex].Translation >= planets[nowIndex].TranslationMax) {
				return;
			}
			else {
				planets[nowIndex].Translation += 0.5f;
			}
			break;

		case 's':
			printf("[Info] ID = %d, Translation = %f\n", nowIndex, planets[nowIndex].Translation);
			if (planets[nowIndex].Translation <= planets[nowIndex].TranslationMin) {
				return;
			}
			else {
				planets[nowIndex].Translation -= 0.5f;
			}
			break;
		
		case 'q':
			printf("[Info] ID = %d, Translation = %f\n", nowIndex, planets[nowIndex].RotateRespectAngle);

			planets[nowIndex].RotateRespectAngle += 0.1f;
			break;
		
		case 'e':
			printf("[Info] ID = %d, Translation = %f\n", nowIndex, planets[nowIndex].RotateRespectAngle);
			
			if (planets[nowIndex].RotateRespectAngle > 0.0f) {
				planets[nowIndex].RotateRespectAngle -= 0.1f;
			}
			else
			{
				planets[nowIndex].RotateRespectAngle = 0.0f;
			}

			break;
		
		case 'z':
			printf("[Info] ID = %d, Debug Mode = %s\n", nowIndex, debugMode ? "On" : "Off");
			debugMode = !debugMode;
			break;

		case 'p':
			if (!frameStop)
				printf("\n"); // eat /r
			printf("[Info] ID = %d, Frame Mode = %s\n", nowIndex, frameStop ? "Count" : "Stop");
			frameStop = !frameStop;
			break;
		
		case '1':
			printf("[Info] ID = %d changed to Index %d\n", nowIndex, 0);
			nowIndex = 0;
			break;

		case '2':
			printf("[Info] ID = %d changed to Index %d\n", nowIndex, 1);
			nowIndex = 1;
			break;

		case '3':
			printf("[Info] ID = %d changed to Index %d\n", nowIndex, 2);
			nowIndex = 2;
			break;

		case '4':
			printf("[Info] ID = %d changed to Index %d\n", nowIndex, 3);
			nowIndex = 3;
			break;


		default:
			break;
	}
	glutPostRedisplay(); /* this redraws the scene without
						 waiting for the display callback so that any changes appear
						 instantly */
}