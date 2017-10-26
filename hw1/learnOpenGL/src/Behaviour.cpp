//#include "main.cpp"

#include <iostream>
#include <stdarg.h>
#include <string>
#include <vector>

#include <fstream>
#include <sstream>

//#include "Texture.cpp"
#define MAX_STRING_LENGTH 1024
#define TEXTURE_NUM 9
#define UTAH_VERTEX_COUNT 1292
#define UTAH_FACE_COUNT 2464

#define SUN_INDEX 0
#define MECURY_INDEX 1
#define EARTH_INDEX 2
#define JUPITER_INDEX 3
#define EURPOPA_INDEX 4
#define MOON_INDEX 5
#define MYSTAR_INDEX 6
#define SAT_EARTH_INDEX 7
#define SAT_JUNIPTER_INDEX 8

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

struct Obj {
	Vector3 positions[UTAH_VERTEX_COUNT];
	Vector3 faces[UTAH_FACE_COUNT];
	// In this case:
	// face: 2464
	// Vertex: 1292
};
typedef Obj Obj;

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

Obj teapot;
float vertices[UTAH_VERTEX_COUNT * 3];
unsigned int indices[UTAH_FACE_COUNT * 3];

Planet planets[TEXTURE_NUM];
GLUquadricObj* quad = gluNewQuadric();					// glu quadratic object for drawing shape like sphere and cylinder
unsigned int frame = 0;
bool frameStop = false;
bool debugMode = false;
int nowIndex = 0;

Vector3 CameraCenter;
float CameraDistance = 20.0f;

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
void DrawSatellite(int index);
void DrawCube();
void Display(void);
void ParseObj();
void DrawObj_Alter();
void keyboard(unsigned char key, int x, int y);

void DrawObj() {

	glPushMatrix();
	// Rotate Around axis
	glRotatef(planets[MYSTAR_INDEX].RotateRespectAxisRotateAngle, 0.0f, 0.0f, 1.0f);
	glRotatef(planets[MYSTAR_INDEX].RotateRespectAngle * (float)(frame), planets[MYSTAR_INDEX].RotateRespectAxis.x, planets[MYSTAR_INDEX].RotateRespectAxis.y, planets[MYSTAR_INDEX].RotateRespectAxis.z);
	glTranslatef(planets[MYSTAR_INDEX].Translation, 0.0f, 0.0f);

	// Rotate Self
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(planets[MYSTAR_INDEX].RotateSelfAngle * (float)(frame), planets[MYSTAR_INDEX].RotateSelfAxis.x, planets[MYSTAR_INDEX].RotateSelfAxis.y, planets[MYSTAR_INDEX].RotateSelfAxis.z);

	DrawObj_Alter();

	/*
	for (int i = 0; i < 2464; ++i) {
		glBegin(GL_TRIANGLES);

			Vector3 v1 = teapot.positions[(int)(teapot.faces[i].x) - 1];
			Vector3 v2 = teapot.positions[(int)(teapot.faces[i].y) - 1];
			Vector3 v3 = teapot.positions[(int)(teapot.faces[i].z) - 1];
			
			glVertex3f(v1.x, v1.y, v1.z);
			glVertex3f(v2.x, v2.y, v2.z);
			glVertex3f(v3.x, v3.y, v3.z);

		glEnd();
	}*/

	glPopMatrix();
}

void loadObj(char *filename) {

	int vertexCount = 0, faceCount = 0;
	FILE *fp;
	char *buffer = (char *)malloc(sizeof(char) * MAX_STRING_LENGTH);
	std::size_t sz;

	fp = fopen(filename, "r");
	
	
	while (fgets(buffer, MAX_STRING_LENGTH, fp) != NULL) {
		if (buffer[0] == 'v') {
			
			std::vector<float> vertex;

			char *res = strtok(buffer, " ");
			while (res != NULL) {
				res = strtok(NULL, " ");
				if (res != NULL) {
					float f = std::stof(res, &sz);
					//printf("%f\n", f);
					vertex.push_back(f);
				}
			}

			Vector3 v;
			v.x = vertex[0];
			v.y = vertex[1];
			v.z = vertex[2];

			teapot.positions[vertexCount] = v;

			++vertexCount;

		}
		else if (buffer[0] == 'f') {
			std::vector<float> vertex;

			char *res = strtok(buffer, " ");
			while (res != NULL) {
				res = strtok(NULL, " ");
				if (res != NULL) {
					float f = std::stof(res, &sz);
					vertex.push_back(f);
				}
			}

			Vector3 f;
			f.x = vertex[0];
			f.y = vertex[1];
			f.z = vertex[2];

			teapot.faces[faceCount] = f;

			++faceCount;
		}
	}

	free(buffer);
}

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
	
	#pragma region sunSetting
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
	#pragma endregion	

	#pragma region MecurySetting
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
	#pragma endregion

	#pragma region earthSetting
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
	#pragma endregion

	#pragma region Juitper
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
	#pragma endregion

	// europa
	planets[EURPOPA_INDEX].TextureName = "../Resource/europa.bmp";
	planets[EURPOPA_INDEX].Radius = 0.5f;

	planets[EURPOPA_INDEX].Translation = 2.5f;
	planets[EURPOPA_INDEX].TranslationMin = 1.0f;
	planets[EURPOPA_INDEX].TranslationMax = 3.5f;

	bufferVector3.x = 0.0f;
	bufferVector3.y = 1.0f;
	bufferVector3.z = 0.0f;
	planets[EURPOPA_INDEX].RotateRespectAxis = bufferVector3;
	planets[EURPOPA_INDEX].RotateRespectAngle = 1.0f;
	planets[EURPOPA_INDEX].RotateRespectAxisRotateAngle = 30.0f;

	bufferVector3.x = 0.0f;
	bufferVector3.y = 1.0f;
	bufferVector3.z = 0.0f;
	planets[EURPOPA_INDEX].RotateSelfAxis = bufferVector3;
	planets[EURPOPA_INDEX].RotateSelfAngle = 0.3f;

	// moon
	planets[MOON_INDEX].TextureName = "../Resource/moon.bmp";
	planets[MOON_INDEX].Radius = 0.5f;

	planets[MOON_INDEX].Translation = 2.0f;
	planets[MOON_INDEX].TranslationMin = 1.0f;
	planets[MOON_INDEX].TranslationMax = 3.0f;

	bufferVector3.x = 0.0f;
	bufferVector3.y = 1.0f;
	bufferVector3.z = 0.0f;
	planets[MOON_INDEX].RotateRespectAxis = bufferVector3;
	planets[MOON_INDEX].RotateRespectAngle = 1.0f;
	planets[MOON_INDEX].RotateRespectAxisRotateAngle = 10.0f;

	bufferVector3.x = 0.0f;
	bufferVector3.y = 1.0f;
	bufferVector3.z = 0.0f;
	planets[MOON_INDEX].RotateSelfAxis = bufferVector3;
	planets[MOON_INDEX].RotateSelfAngle = 0.0f; // Always face earth

	// For Debug
	// planets[EARTH_INDEX].RotateSelfAngle = 0.0f;
	// planets[MOON_INDEX].RotateSelfAngle = 0.0f;

	// planets[JUNIPTER_INDEX].RotateSelfAngle = 0.0f;
	// planets[EURPOPA_INDEX].RotateSelfAngle = 0.0f;

	// Satelitte 1
	planets[SAT_EARTH_INDEX].TextureName = "../Resource/sat1.bmp";
	planets[SAT_EARTH_INDEX].Radius = 0.5f;

	planets[SAT_EARTH_INDEX].Translation = 1.5f;
	planets[SAT_EARTH_INDEX].TranslationMin = 1.5f;
	planets[SAT_EARTH_INDEX].TranslationMax = 2.0f;

	bufferVector3.x = 0.0f;
	bufferVector3.y = 1.0f;
	bufferVector3.z = 0.0f;
	planets[SAT_EARTH_INDEX].RotateRespectAxis = bufferVector3;
	planets[SAT_EARTH_INDEX].RotateRespectAngle = 0.0f;
	planets[SAT_EARTH_INDEX].RotateRespectAxisRotateAngle = 0.0f;

	bufferVector3.x = 0.0f;
	bufferVector3.y = 1.0f;
	bufferVector3.z = 0.0f;
	planets[SAT_EARTH_INDEX].RotateSelfAxis = bufferVector3;
	planets[SAT_EARTH_INDEX].RotateSelfAngle = 0.0f; // Always face earth

	// Satelitte 2
	planets[SAT_JUNIPTER_INDEX].TextureName = "../Resource/sat1.bmp"; // However we load it but unused
	planets[SAT_JUNIPTER_INDEX].Radius = 0.5f;

	planets[SAT_JUNIPTER_INDEX].Translation = 2.0f;
	planets[SAT_JUNIPTER_INDEX].TranslationMin = 2.0f;
	planets[SAT_JUNIPTER_INDEX].TranslationMax = 2.5f;

	bufferVector3.x = 0.0f;
	bufferVector3.y = 1.0f;
	bufferVector3.z = 0.0f;
	planets[SAT_JUNIPTER_INDEX].RotateRespectAxis = bufferVector3;
	planets[SAT_JUNIPTER_INDEX].RotateRespectAngle = 1.0f;
	planets[SAT_JUNIPTER_INDEX].RotateRespectAxisRotateAngle = 10.0f;

	bufferVector3.x = 0.0f;
	bufferVector3.y = 1.0f;
	bufferVector3.z = 0.0f;
	planets[SAT_JUNIPTER_INDEX].RotateSelfAxis = bufferVector3;
	planets[SAT_JUNIPTER_INDEX].RotateSelfAngle = 0.0f; // Always face earth

	// MyStar
	planets[MYSTAR_INDEX].TextureName = "../Resource/my.bmp";
	planets[MYSTAR_INDEX].Radius = 1.0f;

	planets[MYSTAR_INDEX].Translation = 8.0f;
	planets[MYSTAR_INDEX].TranslationMin = 4.0f;
	planets[MYSTAR_INDEX].TranslationMax = 10.0f;
	bufferVector3.x = 0.0f;
	bufferVector3.y = 1.0f;
	bufferVector3.z = 0.0f;

	planets[MYSTAR_INDEX].RotateRespectAxis = bufferVector3;
	planets[MYSTAR_INDEX].RotateRespectAngle = 1.0f;
	planets[MYSTAR_INDEX].RotateRespectAxisRotateAngle = 70.0f;

	bufferVector3.x = 0.0f;
	bufferVector3.y = 1.0f;
	bufferVector3.z = 0.0f;
	planets[MYSTAR_INDEX].RotateSelfAxis = bufferVector3;
	planets[MYSTAR_INDEX].RotateSelfAngle = 1.0f;

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
	GLenum err = glewInit();

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

	//glEnable(GL_COLOR_MATERIAL);						// enable this parameter to use glColor() as material of lighting model

	setupPlanets();

	loadObj("../Resource/teapot.obj");
	
	ParseObj();
}

void CreateSatelitte() {

	float uniformScale = 0.1f;
	float satelliteRadius = 1.0f;

	glPushMatrix();

		glTranslatef(0.0f, 0.5f, 0.0f);

		glScalef(uniformScale, uniformScale, uniformScale);

		// Draw Main Body
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		glPushMatrix();
		glRotatef(180.0f, 0.0f, 1.0f, 0.0f);

		glPushMatrix();
		glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
		glTranslatef(0.0f, 0.0f, 0.5f);
		gluDisk(quad, 0.0f, satelliteRadius, 20, 5);
		glPopMatrix();

		glTranslatef(0.0f, 0.0f, -1.0f * satelliteRadius * 4.0f + 0.5f);
		gluCylinder(quad, satelliteRadius, satelliteRadius, satelliteRadius * 4.0f, 20, 5);

		glPushMatrix();
		glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
		gluDisk(quad, 0.0f, satelliteRadius, 20, 5);
		glPopMatrix();

		glPopMatrix();

		// Draw
		glPushMatrix();
			glTranslatef(-5.0f, 0.0f, 0.0f);
			glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
			glTranslatef(5.0f, 0.0f, 0.0f);
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			glTranslatef(-5.0f, 0.0f, 0.3f);
			glScalef(10.0f, 3.2f, 0.5f);
			DrawCube();
		glPopMatrix();

		glTranslatef(-2.2f, 0.0f, 2.2f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
		glTranslatef(0.0f, 0.5f, 0.0f);
		glutSolidCone(satelliteRadius * 2.0f, satelliteRadius * 1.2f, 20, 5);
		gluDisk(quad, 0.0f, satelliteRadius * 2.0f, 20, 5);

	glPopMatrix();
}

void DrawSatellite(int index)
{
	glPushMatrix();

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, planets[index].Texture2D);
		gluQuadricTexture(quad, GL_TRUE);
	
		if (index == SAT_EARTH_INDEX) {
		
			glRotatef(planets[EARTH_INDEX].RotateRespectAxisRotateAngle, 0.0f, 0.0f, 1.0f);
			glRotatef(planets[EARTH_INDEX].RotateRespectAngle * (float)(frame), planets[EARTH_INDEX].RotateRespectAxis.x, planets[EARTH_INDEX].RotateRespectAxis.y, planets[EARTH_INDEX].RotateRespectAxis.z);
			glTranslatef(planets[EARTH_INDEX].Translation, 0.0f, 0.0f);

			// Rotate Self
			glTranslatef(0.0f, 0.0f, 0.0f);
			glRotatef(planets[EARTH_INDEX].RotateSelfAngle * (float)(frame), planets[EARTH_INDEX].RotateSelfAxis.x, planets[EARTH_INDEX].RotateSelfAxis.y, planets[EARTH_INDEX].RotateSelfAxis.z);
		
		}
		else if (index == SAT_JUNIPTER_INDEX) {
			glRotatef(planets[JUPITER_INDEX].RotateRespectAxisRotateAngle, 0.0f, 0.0f, 1.0f);
			glRotatef(planets[JUPITER_INDEX].RotateRespectAngle * (float)(frame), planets[JUPITER_INDEX].RotateRespectAxis.x, planets[JUPITER_INDEX].RotateRespectAxis.y, planets[JUPITER_INDEX].RotateRespectAxis.z);
			glTranslatef(planets[JUPITER_INDEX].Translation, 0.0f, 0.0f);

			// Rotate Self
			glTranslatef(0.0f, 0.0f, 0.0f);
			glRotatef(planets[JUPITER_INDEX].RotateSelfAngle * (float)(frame), planets[JUPITER_INDEX].RotateSelfAxis.x, planets[JUPITER_INDEX].RotateSelfAxis.y, planets[JUPITER_INDEX].RotateSelfAxis.z);
			glDisable(GL_TEXTURE_2D); 
		}

		// Rotate Around axis
		glRotatef(planets[index].RotateRespectAxisRotateAngle, 0.0f, 0.0f, 1.0f);
		glRotatef(planets[index].RotateRespectAngle * (float)(frame), planets[index].RotateRespectAxis.x, planets[index].RotateRespectAxis.y, planets[index].RotateRespectAxis.z);
		glTranslatef(planets[index].Translation, 0.0f, 0.0f);

		// Rotate Self
		glTranslatef(0.0f, 0.0f, 0.0f);
		glRotatef(planets[index].RotateSelfAngle * (float)(frame), planets[index].RotateSelfAxis.x, planets[index].RotateSelfAxis.y, planets[index].RotateSelfAxis.z);

		CreateSatelitte();

		if (index == SAT_JUNIPTER_INDEX) 
		{
			glEnable(GL_TEXTURE_2D); // Restore what's we've done
		}

	glPopMatrix();

	glDisable(GL_LIGHTING);

}

void DrawCube()
{
	glBegin(GL_QUADS);
	
	// front
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, -1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	// back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, -1.0f);
	
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, -1.0f);

	glTexCoord2f(1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);

	glTexCoord2f(0.0f, -1.0f);
	glVertex3f(0.0f, 1.0f, -1.0f);
	// right
	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);
	// left
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, -1.0f);
	glVertex3f(0.0f, 1.0f, -1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	// top
	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(0.0f, 1.0f, -1.0f);
	// bottom
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, -1.0f);
	glVertex3f(0.0f, 0.0f, -1.0f);
	glEnd();
}

void DrawPlanet(int index)
{
	glPushMatrix();

		if (index == 0) {
			glDisable(GL_LIGHTING);
		}

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, planets[index].Texture2D);

		if (index == EURPOPA_INDEX) { // Europia
			glRotatef(planets[JUPITER_INDEX].RotateRespectAxisRotateAngle, 0.0f, 0.0f, 1.0f);
			glRotatef(planets[JUPITER_INDEX].RotateRespectAngle * (float)(frame), planets[JUPITER_INDEX].RotateRespectAxis.x, planets[JUPITER_INDEX].RotateRespectAxis.y, planets[JUPITER_INDEX].RotateRespectAxis.z);
			glTranslatef(planets[JUPITER_INDEX].Translation, 0.0f, 0.0f);

			// Rotate Self
			glTranslatef(0.0f, 0.0f, 0.0f);
			glRotatef(planets[JUPITER_INDEX].RotateSelfAngle * (float)(frame), planets[JUPITER_INDEX].RotateSelfAxis.x, planets[JUPITER_INDEX].RotateSelfAxis.y, planets[JUPITER_INDEX].RotateSelfAxis.z);
		}
		else if (index == MOON_INDEX) { // Moon
			glRotatef(planets[EARTH_INDEX].RotateRespectAxisRotateAngle, 0.0f, 0.0f, 1.0f);
			glRotatef(planets[EARTH_INDEX].RotateRespectAngle * (float)(frame), planets[EARTH_INDEX].RotateRespectAxis.x, planets[EARTH_INDEX].RotateRespectAxis.y, planets[EARTH_INDEX].RotateRespectAxis.z);
			glTranslatef(planets[EARTH_INDEX].Translation, 0.0f, 0.0f);

			// Rotate Self
			glTranslatef(0.0f, 0.0f, 0.0f);
			glRotatef(planets[EARTH_INDEX].RotateSelfAngle * (float)(frame), planets[EARTH_INDEX].RotateSelfAxis.x, planets[EARTH_INDEX].RotateSelfAxis.y, planets[EARTH_INDEX].RotateSelfAxis.z);
		}

		// Rotate Around axis
		glRotatef(planets[index].RotateRespectAxisRotateAngle, 0.0f, 0.0f, 1.0f);
		glRotatef(planets[index].RotateRespectAngle * (float)(frame), planets[index].RotateRespectAxis.x, planets[index].RotateRespectAxis.y, planets[index].RotateRespectAxis.z);
		glTranslatef(planets[index].Translation, 0.0f, 0.0f);

		// Rotate Self
		glTranslatef(0.0f, 0.0f, 0.0f);
		glRotatef(planets[index].RotateSelfAngle * (float)(frame), planets[index].RotateSelfAxis.x, planets[index].RotateSelfAxis.y, planets[index].RotateSelfAxis.z);

		gluQuadricTexture(quad, GL_TRUE);
		
		if (index == EARTH_INDEX) {
			glScalef(1.2f, 1.0f, 1.0f);
		}
		else {
			glScalef(1.0f, 1.0f, 1.0f);
		}

		gluSphere(quad, planets[index].Radius, 30, 30);

		if (debugMode) {
			// Debug Line Draw
			glDisable(GL_LIGHTING);
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
			glEnable(GL_LIGHTING);
		}

		glDisable(GL_TEXTURE_2D);
		
		if (index == 0) {
			glEnable(GL_LIGHTING);
		}
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
	
	gluLookAt(0.0, 0, CameraDistance, CameraCenter.x, CameraCenter.y, CameraCenter.z, 0, 1, 0);			// set the view part of modelview matrix
	
	glLightfv(GL_LIGHT0, GL_POSITION, LightPos);		// Set Light1 Position, this setting function should be at another place

	DrawObj();
	
	glPushMatrix();
		for (int i = 0; i < TEXTURE_NUM - 2; ++i) {
			
			if (i == MYSTAR_INDEX) { // skip this
				continue;
			}

			DrawPlanet(i);
		}
		DrawSatellite(SAT_EARTH_INDEX);
		DrawSatellite(SAT_JUNIPTER_INDEX);		
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
				printf("\n"); // eat '/r'
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
		
		case '5':
			printf("[Info] ID = %d changed to Index %d\n", nowIndex, 4);
			nowIndex = 4;
			break;

		case '6':
			printf("[Info] ID = %d changed to Index %d\n", nowIndex, 5);
			nowIndex = 5;
			break;

		case 'i':
			CameraCenter.z += 0.5f;
			break;

		case 'k':
			CameraCenter.z -= 0.5f;
			break;
		case 'u':
			CameraDistance += 2.0f;
			break;
		case 'o':
			CameraDistance -= 2.0f;
			break;
		default:
			break;
	}

	glutPostRedisplay(); // this redraws the scene without waiting for the display callback so that any changes appear instantly 
}

#define ERROR_MESSAGE_LOG_SIZE 512

int success;
char infoLog[ERROR_MESSAGE_LOG_SIZE];

char* vertexShaderSource = "";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec4 c;\n"
"void main()\n"
"{\n"
"   FragColor = c;//vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
"}\n\0";

unsigned int VAO, VBO, EBO;
unsigned int vertexShader;
unsigned int fragmentShader;
unsigned int shaderProgram;


void DrawObj_Alter()
{
	float time = frame;
	float greenValue = (sin(time) / 2.0f) + 0.5f;
	int vertexColorLocationM = glGetUniformLocation(shaderProgram, "matrixM");
	int vertexColorLocationV = glGetUniformLocation(shaderProgram, "matrixV");
	int vertexColorLocationP = glGetUniformLocation(shaderProgram, "matrixP");

	GLfloat matrixM[16];
	GLfloat matrixP[16];
	glGetFloatv(GL_PROJECTION_MATRIX, matrixP);

	glPushMatrix();
	//glGetFloatv(GL_MODELVIEW_MATRIX, matrixMV);
	
	//glTranslatef(2.7357f, -7.51639f, 20.13f);
	//glGetFloatv(GL_MODELVIEW_MATRIX, matrixMV);

	glLoadIdentity();
	//gluLookAt(0.0, 0, 20, 0, 0, 0, 0, 1, 0);			// set the view part of modelview matrix

	
	//glRotatef(planets[MYSTAR_INDEX].RotateRespectAxisRotateAngle, 0.0f, 0.0f, 1.0f);
	//glRotatef(planets[MYSTAR_INDEX].RotateRespectAngle * (float)(frame), planets[MYSTAR_INDEX].RotateRespectAxis.x, planets[MYSTAR_INDEX].RotateRespectAxis.y, planets[MYSTAR_INDEX].RotateRespectAxis.z);
	//glTranslatef(planets[MYSTAR_INDEX].Translation, 0.0f, 0.0f);
	//glTranslatef(0.3f, 0.0f, 0.0f);
	//glTranslatef(0.0f, 0.0f, 0.4f);

	glScalef(0.5f, 0.5f, 0.5f);
	//glRotatef(1.0f * frame, 0.0f, 1.0f, 0.0f);

	glGetFloatv(GL_MODELVIEW_MATRIX, matrixM);

	//glTranslatef(0.0f, 0.0f, 0.0f);

	//gluSphere(quad, 2.0f, 20, 5);

	//glLoadIdentity();
	gluLookAt(0.0, 0, 20, 0, 0, 0, 0, 1, 0);			// set the view part of modelview matrix
	GLfloat matrixV[16];
	//glGetFloatv(GL_MODELVIEW_MATRIX, matrixV);

	glLoadIdentity();
	//gluPerspective(60.0f, 4.0f / 3.0f, 5.0f, 100.0f);
	//GLfloat matrixP[16];
	//glGetFloatv(GL_PROJECTION_MATRIX, matrixP);

	//glMultMatrixf(matrixP);
	//glGetFloatv(GL_MODELVIEW_MATRIX, matrixMV);

	glUseProgram(shaderProgram);
	//glUniform4fv(vertexColorLocation, 0, matrix);
	glUniformMatrix4fv(vertexColorLocationM, 1, GL_FALSE, matrixM);
	glUniformMatrix4fv(vertexColorLocationV, 1, GL_FALSE, matrixV);
	glUniformMatrix4fv(vertexColorLocationP, 1, GL_FALSE, matrixP);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);

	glPopMatrix();
	glUseProgram(0);
}

bool CreateShader(unsigned int &shaderInstance, unsigned int shaderType, const GLchar* shaderSource) {

	shaderInstance = glCreateShader(shaderType);

	// 1 stands for one string in vertexShaderSource
	glShaderSource(shaderInstance, 1, &shaderSource, NULL);
	glCompileShader(shaderInstance);

	glGetShaderiv(shaderInstance, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(shaderInstance, ERROR_MESSAGE_LOG_SIZE, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	return success;
}

bool CreateProgram(unsigned int &shaderProgramInstance, int n_args, ...)
{
	shaderProgramInstance = glCreateProgram();

	va_list ap;
	va_start(ap, n_args);
	for (int i = 0; i < n_args; i++) {
		int shaderID = va_arg(ap, unsigned int);
		glAttachShader(shaderProgramInstance, shaderID);
	}
	va_end(ap);

	glLinkProgram(shaderProgramInstance);

	glGetProgramiv(shaderProgramInstance, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgramInstance, ERROR_MESSAGE_LOG_SIZE, NULL, infoLog);
		std::cout << "ERROR::SHADER::LINK_FAILED\n" << infoLog << std::endl;
	}
	else {
		va_start(ap, n_args);
		for (int i = 0; i < n_args; i++) {
			int shaderID = va_arg(ap, unsigned int);
			glDeleteShader(shaderID);
		}
		va_end(ap);
	}

	return success;
}


void Destroy() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void ParseObj() {

	for (int i = 0; i < UTAH_VERTEX_COUNT; ++i) {
		vertices[i * 3 + 0] = teapot.positions[i].x;
		vertices[i * 3 + 1] = teapot.positions[i].y;
		vertices[i * 3 + 2] = teapot.positions[i].z;
	}

	for (int i = 0; i < UTAH_FACE_COUNT; ++i) {
		indices[i * 3 + 0] = teapot.faces[i].x - 1;
		indices[i * 3 + 1] = teapot.faces[i].y - 1;
		indices[i * 3 + 2] = teapot.faces[i].z - 1;
	}

	//vertexShaderSource = std::ifstream
	
	vertexShaderSource = (char *) malloc(sizeof(char) * MAX_STRING_LENGTH);
	vertexShaderSource[0] = '\0';

	char *buf = (char *)malloc(sizeof(char) * MAX_STRING_LENGTH);
	FILE* fp = fopen("../learnOpenGL/vertex.glsl", "r");
	while (fgets(buf, MAX_STRING_LENGTH, fp) != NULL)
	{
		strcat(vertexShaderSource, buf);
	}
	free(buf);

	CreateShader(vertexShader, GL_VERTEX_SHADER, vertexShaderSource);
	CreateShader(fragmentShader, GL_FRAGMENT_SHADER, fragmentShaderSource);

	CreateProgram(shaderProgram, 2, vertexShader, fragmentShader);

	// VAO = vertex attribute object, 1 stands for one buffer
	glGenVertexArrays(1, &VAO);
	// VBO = vertex buffer object
	glGenBuffers(1, &VBO);
	// EBO = element buffer object
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// vertex buffer type: GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	// copy vertex data into buffer's memory
	// GL_STATIC_DRAW: the data will most likely not change at all or very rarely.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	// 0 is the index of vertex attribute
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}