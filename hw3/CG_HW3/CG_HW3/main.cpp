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

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h> /*for function: offsetof */
#include <math.h>
#include <string.h>
#include "../GL/glew.h"
#include "../GL/glut.h"
#include "glm/glm.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <vector>

#include "Model.hpp"
#include "Shader.hpp"
#include "Input.hpp"

GLuint TextureID, normalTextureID;
GLfloat normalWid, normalHei;

void InitShader()
{
	char *vertexShaderSource = ReadShader("../Resources/vertex.glsl");
	char *fragmentShaderSource = ReadShader("../Resources/fragment.glsl");
	CreateShader(vertexShader, GL_VERTEX_SHADER, vertexShaderSource);
	CreateShader(fragmentShader, GL_FRAGMENT_SHADER, fragmentShaderSource);
	CreateProgram(shaderProgram, 2, vertexShader, fragmentShader);
}

GLuint load_normal_map(char* name)
{
	return glmLoadTexture(name, false, true, true, true, &normalWid, &normalHei);
}

void InitTexture()
{
	TextureID = load_normal_map(main_tex_dir);

	normalTextureID = load_normal_map(normal_map_dir);
}


void InitBuffer()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(5, VBO);

	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), &(positions[0]), GL_STATIC_DRAW);
	// 0 is the index of vertex attribute			
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &(normals[0]), GL_STATIC_DRAW);
	// vertex normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(glm::vec2), &(texcoords[0]), GL_STATIC_DRAW);
	// vertex UVs
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), &(tangents[0]), GL_STATIC_DRAW);
	// vertex UVs
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
	glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(glm::vec3), &(bitangents[0]), GL_STATIC_DRAW);
	// vertex UVs
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// setup VAO
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
	++frame;

	if (startBouncing)
	{
		realFrame += 0.05;
	}

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
	glUniform1i (glGetUniformLocation(shaderProgram, "isBump"), isBump);
	glUniform1f(glGetUniformLocation(shaderProgram, "_BumpScale"), bumpScale);
	glUniform1f(glGetUniformLocation(shaderProgram, "frame"), realFrame);
	glUniform1i(glGetUniformLocation(shaderProgram, "bouncing"), (int)startBouncing);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, TextureID);	
	glUniform1i(glGetUniformLocation(shaderProgram, "mainTex"), 0);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, normalTextureID);
	glUniform1i(glGetUniformLocation(shaderProgram, "bumpTex"), 1);
	
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, model->numtriangles * 3);

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