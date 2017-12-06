/*
	CG Homework3 - Bump Mapping
	
	Objective - learning GLSL, glm model datatype(for .obj file) and bump mapping
	
	In this homework, you should load "Ball.obj" and normal map "NormalMap.ppm" with glm.c(done by TA)
	and render the object with color texure and normal mapping with Phong shading(and Phong lighting model of course).
	Please focus on the part with comment like "you may need to do somting here".
	If you don't know how to access vertices information of the model,
	I suggest you refer to glm.c for _GLMmodel structure and glm.h for glmReadOBJ() and glmDraw() function.
	And the infomation printed by print_model_info(model); of glm_helper.h helps as well!
	Finally, please pay attention to the datatype of the variable you use(might be a ID list or value array)
	Good luck!
*/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h> /*for function: offsetof */
#include <math.h>
#include <string.h>
#include "../GL/glew.h"
#include "../GL/glut.h""
#include "../shader_lib/shader.h"
#include "glm/glm.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <sstream>
#include <string>
#include <iterator>
#include <iostream>
#include <stdarg.h>

using glm::vec3;

extern "C"
{
	#include "glm_helper.h"
}

/*you may need to do somting here
you may use the following struct type to perform your single VBO method,
or you can define/declare multiple VBOs for VAO method.
Please feel free to modify it*/
struct Vertex
{
	GLfloat position[3];
	GLfloat normal[3];
	GLfloat texcoord[2];
	GLfloat tangent[3];
	GLfloat bitangent[3];
};
typedef struct Vertex Vertex;

//no need to modify the following function declarations and gloabal variables
void init(void);
void display(void);
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void keyboardup(unsigned char key, int x, int y);
void motion(int x, int y);
void mouse(int button, int state, int x, int y);
void idle(void);
void draw_light_bulb(void);
void camera_light_ball_move();
GLuint load_normal_map(char* name);

namespace
{
	char *obj_file_dir = "../Resources/Ball.obj";
	char *normal_map_dir = "../Resources/NormalMap.ppm";
	GLfloat light_rad = 0.05;//radius of the light bulb
	float eyet = 0.0;//theta in degree
	float eyep = 90.0;//phi in degree
	bool mleft = false;
	bool mright = false;
	bool mmiddle = false;
	bool forward = false;
	bool backward = false;
	bool left = false;
	bool right = false;
	bool up = false;
	bool down = false;
	bool lforward = false;
	bool lbackward = false;
	bool lleft = false;
	bool lright = false;
	bool lup = false;
	bool ldown = false;
	bool bforward = false;
	bool bbackward = false;
	bool bleft = false;
	bool bright = false;
	bool bup = false;
	bool bdown = false;
	bool bx = false;
	bool by = false;
	bool bz = false;
	bool brx = false;
	bool bry = false;
	bool brz = false;
	int mousex = 0;
	int mousey = 0;
}

//you can modify the moving/rotating speed if it's too fast/slow for you
const float speed = 0.005;//camera/light/ball moving speed
const float rotation_speed = 0.05;//ball rotation speed
//you may need to use some of the following variables in your program 
GLuint normalTextureID;//TA has already loaded the normal texture for you
GLMmodel *model;//TA has already loaded the model for you(!but you still need to convert it to VBO(s)!)
float eyex = 0.0;
float eyey = 0.0;
float eyez = 3.0;
GLfloat light_pos[] = { 1, 1, 1 };
GLfloat ball_pos[] = { 0.0, 0.0, 0.0 };
GLfloat ball_rot[] = { 0.0, 0.0, 0.0 };
GLfloat normalWid, normalHei;

std::vector<struct Vertex> vertices;
std::vector<unsigned int> indices;

template<typename Out> void split(const std::string &s, char delim, Out result) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		*(result++) = item;
	}
}

std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}

void LoadObj(char* filename)
{
	using std::string;
	std::vector<std::string> rawData;
	std::vector<glm::vec2> texcoords;
	std::vector<glm::vec3> normals;

	FILE *fp;
	fp = fopen(filename, "r");
	char *buffer = (char *)malloc(sizeof(char) * 4096);
	while (fgets(buffer, 4096, fp) != NULL)
	{
		rawData.push_back(buffer);
	}
	free(buffer);
	fclose(fp);

	for (std::string buffer : rawData)
	{
		if (buffer[0] == 'v' && buffer[1] == ' ') {

			std::vector<std::string> v = split(buffer.substr(string("v ").size(), buffer.length() - string("v ").size()), ' ');

			struct Vertex vert;
			vert.position[0] = std::stof(v[0], nullptr);
			vert.position[1] = std::stof(v[1], nullptr);
			vert.position[2] = std::stof(v[2], nullptr);

			vert.texcoord[0] = 0;
			vert.texcoord[1] = 0;

			vert.normal[0] = 0;
			vert.normal[1] = 0;
			vert.normal[2] = 0;
			
			vert.tangent[0] = 0;
			vert.tangent[1] = 0;
			vert.tangent[2] = 0;

			vert.bitangent[0] = 0;
			vert.bitangent[1] = 0;
			vert.bitangent[2] = 0;

			vertices.push_back(vert);
		}
		else if (buffer[0] == 'v' && buffer[1] == 't') {

			std::vector<std::string> v = split(buffer.substr(string("vt ").size(), buffer.length() - string("vt ").size()), ' ');

			auto coord = glm::vec2(
				std::stof(v[0], nullptr),
				std::stof(v[1], nullptr)
			);

			texcoords.push_back(coord);
		}

		else if (buffer[0] == 'v' && buffer[1] == 'n') {

			std::vector<std::string> v = split(buffer.substr(string("vn ").size(), buffer.length() - string("vn ").size()), ' ');

			auto normal = glm::vec3(
				std::stof(v[0], nullptr),
				std::stof(v[1], nullptr),
				std::stof(v[2], nullptr)
			);

			normal = glm::normalize(normal);

			normals.push_back(normal);
		}

		else if (buffer[0] == 'f') {
			std::vector<std::string> f = split(buffer.substr(string("f ").size(), buffer.length() - string("f ").size()), ' ');

			for (int i = 0; i < 3; ++i) {
				std::vector<std::string> ff = split(f[i], '/');
				int vertexIndex = std::stod(ff[0], nullptr) - 1;
				int UVIndex = std::stod(ff[0], nullptr) - 1;
				int normalIndex = std::stod(ff[0], nullptr) - 1;

				vertices[vertexIndex].texcoord[0] = texcoords[UVIndex].x;
				vertices[vertexIndex].texcoord[1] = texcoords[UVIndex].y;

				if (
					(vertices[vertexIndex].normal[0] + vertices[vertexIndex].normal[1] + vertices[vertexIndex].normal[2]) != 0 
					&& 
					((vertices[vertexIndex].normal[0] + vertices[vertexIndex].normal[1] + vertices[vertexIndex].normal[2]) != (normals[normalIndex].x + normals[normalIndex].y + normals[normalIndex].z))
				)
				{
					// debug point;
				}

				vertices[vertexIndex].normal[0] = normals[normalIndex].x;
				vertices[vertexIndex].normal[1] = normals[normalIndex].y;
				vertices[vertexIndex].normal[2] = normals[normalIndex].z;

				indices.push_back(vertexIndex);
			}

		}
	}

	return;
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	//remember to replace "YourStudentID" with your own student ID
	glutCreateWindow("CG_HW3_0556652");
	glutReshapeWindow(512, 512);

	glewInit();

	init();

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardup);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	glutMainLoop();

	glmDelete(model);
	return 0;
}

unsigned int VAO, VBO, EBO;
unsigned int vertexShader;
unsigned int fragmentShader;
unsigned int shaderProgram;

#define ERROR_MESSAGE_LOG_SIZE 512
int success;
char infoLog[ERROR_MESSAGE_LOG_SIZE];

bool CreateShader(unsigned int &shaderInstance, unsigned int shaderType, const GLchar* shaderSource) {

	shaderInstance = glCreateShader(shaderType);

	// 1 stands for one string in vertexShaderSource
	glShaderSource(shaderInstance, 1, &shaderSource, NULL);
	glCompileShader(shaderInstance);

	glGetShaderiv(shaderInstance, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(shaderInstance, ERROR_MESSAGE_LOG_SIZE, NULL, infoLog);
		// std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
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

char *ReadShader(char * shaderpath)
{
	FILE *fp;
	fp = fopen(shaderpath, "r");
	char *buffer = (char *)malloc(sizeof(char) * 4096);
	char *data = (char *)malloc(sizeof(char) * 4096);
	data[0] = '\0';
	while (fgets(buffer, 4096, fp) != NULL)
	{
		strcat(data, buffer);
	}
	free(buffer);
	fclose(fp);

	return data;
}

void InitShader()
{
	char *vertexShaderSource = ReadShader("../Resources/vertex.glsl");
	char *fragmentShaderSource = ReadShader("../Resources/fragment.glsl");
	CreateShader(vertexShader, GL_VERTEX_SHADER, vertexShaderSource);
	CreateShader(fragmentShader, GL_FRAGMENT_SHADER, fragmentShaderSource);
	CreateProgram(shaderProgram, 2, vertexShader, fragmentShader);
}

void InitBuffer()
{
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
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &(vertices[0]), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &(indices[0]), GL_STATIC_DRAW);

	/*
		Structure of Vertex looks like:
			GLfloat position[3];
			GLfloat normal[3];
			GLfloat texcoord[2];
			GLfloat tangent[3];
			GLfloat bitangent[3];
	*/

	// 0 is the index of vertex attribute			
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void *)0);
	glEnableVertexAttribArray(0);

	// vertex normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void*)offsetof(struct Vertex, position));
	glEnableVertexAttribArray(1);

	// vertex UVs
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void*)offsetof(struct Vertex, normal));
	glEnableVertexAttribArray(2);

	// vertex tangents
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void*)offsetof(struct Vertex, texcoord));
	glEnableVertexAttribArray(3);

	// vertex bitangents
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void*)offsetof(struct Vertex, tangent));
	glEnableVertexAttribArray(4);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void init(void) {

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glEnable(GL_CULL_FACE);
	
	model = glmReadOBJ(obj_file_dir);
	normalTextureID = load_normal_map(normal_map_dir);
	glmUnitize(model);
	glmFacetNormals(model);
	glmVertexNormals(model, 90.0, GL_FALSE);

	glEnable(GL_DEPTH_TEST);
	print_model_info(model);

	//you may need to do somting here(create shaders/program(s) and create vbo(s)/vao from GLMmodel model)

	LoadObj(obj_file_dir);

	InitShader();

	InitBuffer();
}

glm::mat4 getV()
{
	glm::vec3 cameraPos = glm::vec3(eyex, eyey, eyez);
	glm::vec3 cameraFront = glm::vec3(cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180), sin(eyet*M_PI / 180), -1.0 * cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180));
	glm::vec3 cameraRight = glm::vec3(0.0, 1.0, 0.0);
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraRight);
	//gluLookAt(eyex, eyey, eyez, eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180), eyey + sin(eyet*M_PI / 180), eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180), 0.0, 1.0, 0.0);
}

glm::mat4 getP()
{
	float fov = 45.0f;
	float aspect = 1.0 / 1.0f;
	float nearDistance = 0.1f;
	float farDistance = 1000.0f;
	return glm::perspective(glm::radians(fov), aspect, nearDistance, farDistance);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glPushMatrix();
		glUseProgram(0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(eyex, eyey, eyez,eyex+cos(eyet*M_PI/180)*cos(eyep*M_PI / 180), eyey+sin(eyet*M_PI / 180), eyez-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180),0.0, 1.0, 0.0);
		draw_light_bulb();
	glPopMatrix();

	/*
		glPushMatrix();
			glLoadIdentity();
			glTranslatef(ball_pos[0], ball_pos[1], ball_pos[2]);
			glRotatef(ball_rot[0], 1, 0, 0);
			glRotatef(ball_rot[1], 0, 1, 0);
			glRotatef(ball_rot[2], 0, 0, 1);
			glmDraw(model, GLM_TEXTURE);//please delete this line in your final code! It's just a preview of rendered object
		glPopMatrix();
	*/

	glUseProgram(shaderProgram);

	glm::mat4 MVP;
	glm::mat4 M(1.0f);
	M = glm::translate(M, vec3(ball_pos[0], ball_pos[1], ball_pos[2]));
	M = glm::rotate(M, ball_rot[0], vec3(1, 0, 0));
	M = glm::rotate(M, ball_rot[1], vec3(0, 1, 0));
	M = glm::rotate(M, ball_rot[2], vec3(0, 0, 1));

	MVP = getP() * getV() * M;
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, &MVP[0][0]);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glutSwapBuffers();
	camera_light_ball_move();
}

//please implement bump mapping toggle(enable/disable bump mapping) in case 'b'(lowercase)
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
	{	//ESC
		break;
	}
	case 'b'://toggle bump mapping
	{
		//you may need to do somting here
		break;
	}
	case 'd':
	{
		right = true;
		break;
	}
	case 'a':
	{
		left = true;
		break;
	}
	case 'w':
	{
		forward = true;
		break;
	}
	case 's':
	{
		backward = true;
		break;
	}
	case 'q':
	{
		up = true;
		break;
	}
	case 'e':
	{
		down = true;
		break;
	}
	case 't':
	{
		lforward = true;
		break;
	}
	case 'g':
	{
		lbackward = true;
		break;
	}
	case 'h':
	{
		lright = true;
		break;
	}
	case 'f':
	{
		lleft = true;
		break;
	}
	case 'r':
	{
		lup = true;
		break;
	}
	case 'y':
	{
		ldown = true;
		break;
	}
	case 'i':
	{
		bforward = true;
		break;
	}
	case 'k':
	{
		bbackward = true;
		break;
	}
	case 'l':
	{
		bright = true;
		break;
	}
	case 'j':
	{
		bleft = true;
		break;
	}
	case 'u':
	{
		bup = true;
		break;
	}
	case 'o':
	{
		bdown = true;
		break;
	}
	case '7':
	{
		bx = true;
		break;
	}
	case '8':
	{
		by = true;
		break;
	}
	case '9':
	{
		bz = true;
		break;
	}
	case '4':
	{
		brx = true;
		break;
	}
	case '5':
	{
		bry = true;
		break;
	}
	case '6':
	{
		brz = true;
		break;
	}

	//special function key
	case 'z'://move light source to front of camera
	{
		light_pos[0] = eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180);
		light_pos[1] = eyey + sin(eyet*M_PI / 180);
		light_pos[2] = eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180);
		break;
	}
	case 'x'://move ball to front of camera
	{
		ball_pos[0] = eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) * 3;
		ball_pos[1] = eyey + sin(eyet*M_PI / 180) * 5;
		ball_pos[2] = eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180) * 3;
		break;
	}
	case 'c'://reset all pose
	{
		light_pos[0] = 1;
		light_pos[1] = 1;
		light_pos[2] = 1;
		ball_pos[0] = 0;
		ball_pos[1] = 0;
		ball_pos[2] = 0;
		ball_rot[0] = 0;
		ball_rot[1] = 0;
		ball_rot[2] = 0;
		eyex = 0;
		eyey = 0;
		eyez = 3;
		eyet = 0;
		eyep = 90;
		break;
	}
	default:
	{
		break;
	}
	}
}

//no need to modify the following functions
void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.001f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void motion(int x, int y) {
	if (mleft)
	{
		eyep -= (x-mousex)*0.1;
		eyet -= (y - mousey)*0.12;
		if (eyet > 89.9)
			eyet = 89.9;
		else if (eyet < -89.9)
			eyet = -89.9;
		if (eyep > 360)
			eyep -= 360;
		else if (eyep < 0)
			eyep += 360;
	}
	mousex = x;
	mousey = y;
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON)
	{
		if(state == GLUT_DOWN && !mright && !mmiddle)
		{
			mleft = true;
			mousex = x;
			mousey = y;
		}
		else
			mleft = false;
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN && !mleft && !mmiddle)
		{
			mright = true;
			mousex = x;
			mousey = y;
		}
		else
			mright = false;
	}
	else if (button == GLUT_MIDDLE_BUTTON)
	{
		if (state == GLUT_DOWN && !mleft && !mright)
		{
			mmiddle = true;
			mousex = x;
			mousey = y;
		}
		else
			mmiddle = false;
	}
}

void camera_light_ball_move()
{
	GLfloat dx = 0, dy = 0, dz=0;
	if(left|| right || forward || backward || up || down)
	{ 
		if (left)
			dx = -speed;
		else if (right)
			dx = speed;
		if (forward)
			dy = speed;
		else if (backward)
			dy = -speed;
		eyex += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
		eyey += dy*sin(eyet*M_PI / 180);
		eyez += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
		if (up)
			eyey += speed;
		else if (down)
			eyey -= speed;
	}
	if(lleft || lright || lforward || lbackward || lup || ldown)
	{
		dx = 0;
		dy = 0;
		if (lleft)
			dx = -speed;
		else if (lright)
			dx = speed;
		if (lforward)
			dy = speed;
		else if (lbackward)
			dy = -speed;
		light_pos[0] += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
		light_pos[1] += dy*sin(eyet*M_PI / 180);
		light_pos[2] += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
		if (lup)
			light_pos[1] += speed;
		else if(ldown)
			light_pos[1] -= speed;
	}
	if (bleft || bright || bforward || bbackward || bup || bdown)
	{
		dx = 0;
		dy = 0;
		if (bleft)
			dx = -speed;
		else if (bright)
			dx = speed;
		if (bforward)
			dy = speed;
		else if (bbackward)
			dy = -speed;
		ball_pos[0] += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
		ball_pos[1] += dy*sin(eyet*M_PI / 180);
		ball_pos[2] += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
		if (bup)
			ball_pos[1] += speed;
		else if (bdown)
			ball_pos[1] -= speed;
	}
	if(bx||by||bz || brx || bry || brz)
	{
		dx = 0;
		dy = 0;
		dz = 0;
		if (bx)
			dx = -rotation_speed;
		else if (brx)
			dx = rotation_speed;
		if (by)
			dy = rotation_speed;
		else if (bry)
			dy = -rotation_speed;
		if (bz)
			dz = rotation_speed;
		else if (brz)
			dz = -rotation_speed;
		ball_rot[0] += dx;
		ball_rot[1] += dy;
		ball_rot[2] += dz;
	}
}

void draw_light_bulb()
{
	GLUquadric *quad;
	quad = gluNewQuadric();
	glPushMatrix();
	glColor3f(0.4, 0.5, 0);
	glTranslatef(light_pos[0], light_pos[1], light_pos[2]);
	gluSphere(quad, light_rad, 40, 20);
	glPopMatrix();
}

void keyboardup(unsigned char key, int x, int y) {
	switch (key) {
	case 'd':
	{
		right =false;
		break;
	}
	case 'a':
	{
		left = false;
		break;
	}
	case 'w':
	{
		forward = false;
		break;
	}
	case 's':
	{
		backward = false;
		break;
	}
	case 'q':
	{
		up = false;
		break;
	}
	case 'e':
	{
		down = false;
		break;
	}
	case 't':
	{
		lforward = false;
		break;
	}
	case 'g':
	{
		lbackward = false;
		break;
	}
	case 'h':
	{
		lright = false;
		break;
	}
	case 'f':
	{
		lleft = false;
		break;
	}
	case 'r':
	{
		lup = false;
		break;
	}
	case 'y':
	{
		ldown = false;
		break;
	}
	case 'i':
	{
		bforward = false;
		break;
	}
	case 'k':
	{
		bbackward = false;
		break;
	}
	case 'l':
	{
		bright = false;
		break;
	}
	case 'j':
	{
		bleft = false;
		break;
	}
	case 'u':
	{
		bup = false;
		break;
	}
	case 'o':
	{
		bdown = false;
		break;
	}
	case '7':
	{
		bx = false;
		break;
	}
	case '8':
	{
		by = false;
		break;
	}
	case '9':
	{
		bz = false;
		break;
	}
	case '4':
	{
		brx = false;
		break;
	}
	case '5':
	{
		bry = false;
		break;
	}
	case '6':
	{
		brz = false;
		break;
	}

	default:
	{
		break;
	}
	}
}

void idle(void)
{
	glutPostRedisplay();
}

GLuint load_normal_map(char* name)
{
	return glmLoadTexture(name, false, true, true, true, &normalWid, &normalHei);
}
