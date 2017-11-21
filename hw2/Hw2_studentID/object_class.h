#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "../GL/glut.h"

#include <glm/glm.hpp>

using namespace std;
using namespace glm;


class object_class
{
public:
	object_class(const char* in_filename);	//load file
	~object_class();
	void draw();							//draw this object
	void scaling(float x, float y, float z);

	/* you should focus on this two functions */
	void Stencil(float*, const float*, int);
	void Transform_Light(GLfloat* , GLfloat* );

	float* scale;
	float* position;
	float* rotation;
	float* rotate_speed;
	float* light_at_obj;

	int points_count;		//有多少點
	int plane_count;		//有多少三角形平面
	double** pos;			//紀錄local?Em local address of points
	int** plane;			//紀錄哪些點(編號)組成平面 
	double** n;					//紀錄每個面三個點的法向量，用在繪圖 normal vector of every points
	double** planeEq;		//紀錄平面方程式，非常op啊

};

object_class::object_class(const char* in_filename)
{
	std::ifstream fin;
	fin.open(in_filename, std::ios::in);
	if (!fin.is_open()) { std::cout << "object open failed.\n"; system("pause"); exit(-1); }
	using namespace std;
	fin >> points_count;
	pos = (double**)operator new(points_count * sizeof(double));
	for (int i = 0; i < points_count; i++)
	{
		pos[i] = (double*)operator new(3 * sizeof(double));//3 means x y z
	}
	for (int i = 0; i < points_count; i++)
	{
		fin >> pos[i][0];//x
		fin >> pos[i][1];//y
		fin >> pos[i][2];//z
	}
	fin >> plane_count;
	plane = (int**)operator new(plane_count * sizeof(int));
	n = (double**)operator new(plane_count * sizeof(double));
	for (int i = 0; i < plane_count; i++)
	{
		plane[i] = (int*)operator new(3 * sizeof(int));
		n[i] = (double*)operator new(3 * 3 * sizeof(double));//每個點有個法向量 3*3(xyz)一共9個
	}
	for (int i = 0; i < plane_count; i++)
	{
		fin >> plane[i][0] >> plane[i][1] >> plane[i][2];
		plane[i][0]--;
		plane[i][1]--;
		plane[i][2]--;
		fin >> n[i][0] >> n[i][1] >> n[i][2];
		fin >> n[i][3] >> n[i][4] >> n[i][5];
		fin >> n[i][6] >> n[i][7] >> n[i][8];
	}

	planeEq = (double**) operator new(plane_count * sizeof(double));
	for (int i = 0; i<plane_count; i++)
	{
		planeEq[i] = (double*)operator new(4 * sizeof(double));
	}
	for (int i = 0; i<plane_count; i++)
	{
		double x1 = pos[plane[i][0]][0];
		double y1 = pos[plane[i][0]][1];
		double z1 = pos[plane[i][0]][2];
		double x2 = pos[plane[i][1]][0];
		double y2 = pos[plane[i][1]][1];
		double z2 = pos[plane[i][1]][2];
		double x3 = pos[plane[i][2]][0];
		double y3 = pos[plane[i][2]][1];
		double z3 = pos[plane[i][2]][2];
		//ax+by+cz+d=0
		planeEq[i][0] = y1*(z2 - z3) + y2*(z3 - z1) + y3*(z1 - z2);//a
		planeEq[i][1] = z1*(x2 - x3) + z2*(x3 - x1) + z3*(x1 - x2);//b
		planeEq[i][2] = x1*(y2 - y3) + x2*(y3 - y1) + x3*(y1 - y2);//c
		planeEq[i][3] = -(x1*(y2*z3 - y3*z2) + x2*(y3*z1 - y1*z3) + x3*(y1*z2 - y2*z1));
	}

	scale = (float*)operator new(3 * sizeof(float));
	position = (float*)operator new(3 * sizeof(float));
	rotation = (float*)operator new(3 * sizeof(float));
	rotate_speed = (float*)operator new(3 * sizeof(float));
	for (int i = 0; i<3; i++) {
		scale[i] = 1;
		position[i] = rotation[i] = rotate_speed[i] = 0;
	}
	light_at_obj = (float*)operator new(4 * sizeof(float));
}

object_class::~object_class()
{
	for (int i = 0; i < points_count; i++)
	{
		operator delete(pos[i]);
	}
	operator delete(pos);
	for (int i = 0; i < plane_count; i++)
	{
		operator delete(n[i]);
		operator delete(plane[i]);
		operator delete(planeEq[i]);
	}
	operator delete(n);
	operator delete(plane);
	operator delete(planeEq);

	operator delete(scale);
	operator delete(position);
	operator delete(rotation);
	operator delete(rotate_speed);
	operator delete(light_at_obj);
}

void object_class::scaling(float x, float y, float z) {
	scale[0] = x; scale[1] = y, scale[2] = z;
}

void object_class::draw()
{
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < plane_count; i++)
	{
		//if(i==4||i==5)
		for (int j = 0; j < 3; j++)
		{
			glNormal3d(n[i][j * 3 + 0], n[i][j * 3 + 1], n[i][j * 3 + 2]);				//這個點的法向量
			glVertex3d(pos[plane[i][j]][0], pos[plane[i][j]][1], pos[plane[i][j]][2]);	//這個平面的第j個點
		}
	}
	glEnd();
}

void object_class::Transform_Light(GLfloat* global_Light, GLfloat* mat)
{
	for (int i = 0; i < 3; i++) // 新的光點
		light_at_obj[i] = mat[i] * global_Light[0] + mat[4 + i] * global_Light[1] + mat[8 + i] * global_Light[2] + mat[12 + i];
}

void object_class::Stencil(float* global_Light, const float* ObjectRotation, int DebugFlag = 0) {

	// Calculate global_light in Object Coordinate and Store result in light_at_obj
	GLfloat ObjectRotation_M[16];
	glPushMatrix();
		glLoadIdentity();
	
		glRotatef(-ObjectRotation[2], 0, 0, 1);
		glRotatef(-ObjectRotation[1], 0, 1, 0);
		glRotatef(-ObjectRotation[0], 1, 0, 0);
		glTranslatef(-position[0], -position[1], -position[2]);
		glScalef(1.0/scale[0], 1.0/scale[1], 1.0/scale[2]);

		glGetFloatv(GL_MODELVIEW_MATRIX, ObjectRotation_M);
	glPopMatrix();

	Transform_Light(global_Light, ObjectRotation_M);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	glDisable(GL_LIGHTING);

	glCullFace(GL_BACK);

	if (DebugFlag == 0){
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}
	else{
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	}

	for (int i = 0; i < plane_count; i++)
	{
		//glLoadIdentity();

		double a = planeEq[i][0];
		double b = planeEq[i][1];
		double c = planeEq[i][2];
		double d = planeEq[i][4];

		double result = a * light_at_obj[0] + b * light_at_obj[1] + c * light_at_obj[2] + d;

		// Cull Face Back toward Light
		if (result <= 0) {
			continue;
		}

		if (DebugFlag == 2) {
			// Draw Object For Debug

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			
			glDisable(GL_STENCIL_TEST);
			
			glColor3f(1.0, 1.0, 1.0);
			glBegin(GL_TRIANGLES);
				for (int j = 0; j < 3; j++)
				{
					glNormal3d(n[i][j * 3 + 0], n[i][j * 3 + 1], n[i][j * 3 + 2]);//這個點的法向量
					glVertex3d(pos[plane[i][j]][0], pos[plane[i][j]][1], pos[plane[i][j]][2]);//這個平面的第j個點
				}
			glEnd();

			glEnable(GL_STENCIL_TEST);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		}
		
		// Start Stencil Test
		for (int count = 0; count < 2; ++count)
		{
			if (count == 0) {
				glFrontFace(GL_CW);

				glEnable(GL_STENCIL_TEST);
				glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glStencilMask(0xFF);

				glDepthMask(false);
			}
			else { // count == 1
				glFrontFace(GL_CCW);

				glEnable(GL_STENCIL_TEST);
				glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glStencilMask(0xFF);

				glDepthMask(false);			
			}

			// Loop Through Triangles for calculating three ray-interscetion
			for (int j = 0; j < 3; j++)
			{
				GLfloat p1[3] = { pos[plane[i][j]][0], pos[plane[i][j]][1], pos[plane[i][j]][2] };
				GLfloat p2[3];

				// Setup point 2: p1 & p2, p2 & p3, p3 & p1
				if (j == 2) {
					p2[0] = pos[plane[i][0]][0];
					p2[1] = pos[plane[i][0]][1];
					p2[2] = pos[plane[i][0]][2];
				}
				else {
					p2[0] = pos[plane[i][j + 1]][0];
					p2[1] = pos[plane[i][j + 1]][1];
					p2[2] = pos[plane[i][j + 1]][2];
				}

				GLfloat lightVec1[3] = { p1[0] - light_at_obj[0], p1[1] - light_at_obj[1], p1[2] - light_at_obj[2] };
				GLfloat lightVec2[3] = { p2[0] - light_at_obj[0], p2[1] - light_at_obj[1], p2[2] - light_at_obj[2] };

				// Maximum Length Of Ray
				float factor = 50;
				GLfloat intersectPoint1[3] = { light_at_obj[0] + (lightVec1[0] * factor), light_at_obj[1] + (lightVec1[1] * factor), light_at_obj[2] + (lightVec1[2] * factor) };
				GLfloat intersectPoint2[3] = { light_at_obj[0] + (lightVec2[0] * factor), light_at_obj[1] + (lightVec2[1] * factor), light_at_obj[2] + (lightVec2[2] * factor) };

				if (DebugFlag == 0) {
					// Setup Color For Debug
					if (j == 0) {
						glColor3f(0.0, 1.0, 0.0);
					}
					else if (j == 1) {
						glColor3f(0.0, 0.0, 1.0);
					}
					else {
						glColor3f(1.0, 0.0, 0.0);
					}
				}
				else if (DebugFlag == 2) {
					// Draw Light in Object Coordinate For Debug
					glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
					glDisable(GL_STENCIL_TEST);
					glColor3f(1.0, 0.0, 0.0);
					glPushMatrix();
					glTranslatef(light_at_obj[0], light_at_obj[1], light_at_obj[2]);
					glutSolidSphere(.5, 20, 20);
					glPopMatrix();
					glEnable(GL_STENCIL_TEST);
					glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
				}				

				// Draw Shadow Volume Area
				glPushMatrix();
					glBegin(GL_QUADS);
						glVertex3f(p1[0], p1[1], p1[2]);
						glVertex3f(p2[0], p2[1], p2[2]);
						glVertex3f(intersectPoint2[0], intersectPoint2[1], intersectPoint2[2]);
						glVertex3f(intersectPoint1[0], intersectPoint1[1], intersectPoint1[2]);
					glEnd();
				glPopMatrix();

			} // End of j-Loop
		} // End of Count-Loop
	} // End of i-Loop


	// Restore parameters	
	glFrontFace(GL_CCW);

	glEnable(GL_LIGHTING);

	glDepthMask(true);
	
	glStencilMask(0x00);
	glDisable(GL_STENCIL_TEST);
	
	glDisable(GL_LIGHTING);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}