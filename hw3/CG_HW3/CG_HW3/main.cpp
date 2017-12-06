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
#include "../GL/glut.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <vector>

#include "Texture.hpp"
#include "Model.hpp"
#include "Shader.hpp"
#include "Input.hpp"

void InitShader()
{
	char *vertexShaderSource = ReadShader("../Resources/vertex.glsl");
	char *fragmentShaderSource = ReadShader("../Resources/fragment.glsl");
	CreateShader(vertexShader, GL_VERTEX_SHADER, vertexShaderSource);
	CreateShader(fragmentShader, GL_FRAGMENT_SHADER, fragmentShaderSource);
	CreateProgram(shaderProgram, 2, vertexShader, fragmentShader);
}

void InitTexture()
{
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	Image* texture = LoadTexture("../Resources/Tile.bmp");
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->sizeX, texture->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, texture->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}


void InitBuffer()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

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

	glEnable(GL_CULL_FACE);
	
	InitTexture();

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
}

glm::mat4 getP()
{
	float fov = 45.0f;
	float aspect = 1.0 / 1.0f; // since window is (512, 512)
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

	glUseProgram(shaderProgram);

	glm::mat4 M(1.0f);
	M = glm::translate(M, glm::vec3(ball_pos[0], ball_pos[1], ball_pos[2]));
	M = glm::rotate(M, ball_rot[0], glm::vec3(1, 0, 0));
	M = glm::rotate(M, ball_rot[1], glm::vec3(0, 1, 0));
	M = glm::rotate(M, ball_rot[2], glm::vec3(0, 0, 1));

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "M"), 1, GL_FALSE, &M[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "V"), 1, GL_FALSE, &getV()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "P"), 1, GL_FALSE, &getP()[0][0]);

	glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), light_pos[0], light_pos[1], light_pos[2]);
	glUniform3f(glGetUniformLocation(shaderProgram, "CameraPos"), eyex, eyey, eyez);
	glm::vec4 worldpos = M * glm::vec4(0.0);
	glUniform3f(glGetUniformLocation(shaderProgram, "WorldPos"), worldpos[0], worldpos[1], worldpos[2]);

	GLint loc = glGetUniformLocation(shaderProgram, "mainTex");
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, TextureID);	
	glUniform1i(loc, 0);
	
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glutSwapBuffers();
	camera_light_ball_move();
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

void idle(void)
{
	glutPostRedisplay();
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

	return 0;
}