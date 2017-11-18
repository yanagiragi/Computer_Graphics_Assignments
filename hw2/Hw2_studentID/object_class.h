#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "../GL/glut.h"

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class object_class
{
public:
	object_class(const char* in_filename);//load file
	~object_class();
	void draw();//draw this object
	void scaling(float x, float y, float z);
	
	/* you should focus on this two functions */
	void draw_shadow_poly(const double* CamaraPos);
	void local_light(float* global_light);

	bool isIntersect(float origin, glm::vec3 direction, int planeID);

	float* scale;
	float* position;
	float* rotation;
	float* rotate_speed;
	float* light_at_obj;

	int points_count;//有多少點
	int plane_count;//有多少三角形平面
	double** pos;//紀錄local位置 local address of points
	int** plane;//紀錄哪些點(編號)組成平面 
	double** n;//紀錄每個面三個點的法向量，用在繪圖 normal vector of every points
	double** planeEq;//紀錄平面方程式，非常op啊

	GLfloat *InverseMatrix(GLfloat *inputF)
	{
		mat4 inputM = make_mat4(inputF);
		mat4 inverseInputM = inverse(inputM);

		float *inverseInputFP = value_ptr(inverseInputM);
		GLfloat inverseInputF[16];
		for (int i = 0; i < 16; ++i)
			inverseInputF[i] = inverseInputFP[i];

		return inverseInputF;
	}

};

object_class::object_class(const char* in_filename)
{
	std::ifstream fin;
	fin.open(in_filename, std::ios::in);
	if (!fin.is_open()){ std::cout << "object open failed.\n"; system("pause"); exit(-1); }
	using namespace std;
	fin >> points_count;
	pos = (double**)operator new(points_count*sizeof(double));
	for (int i = 0; i < points_count; i++)
	{
		pos[i] = (double*)operator new(3*sizeof(double));//3 means x y z
	}
	for (int i = 0; i < points_count; i++)
	{
		fin >> pos[i][0];//x
		fin >> pos[i][1];//y
		fin >> pos[i][2];//z
	}
	fin >> plane_count;
	plane = (int**)operator new(plane_count*sizeof(int));
	n = (double**)operator new(plane_count*sizeof(double));
	for (int i = 0; i < plane_count; i++)
	{
		plane[i] = (int*)operator new(3 * sizeof(int));
		n[i] = (double*)operator new(3*3 * sizeof(double));//每個點有個法向量 3*3(xyz)一共9個
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

	planeEq=(double**) operator new(plane_count*sizeof(double));
	for(int i=0;i<plane_count;i++)
	{
		planeEq[i]=(double*)operator new(4*sizeof(double));
	}
	for(int i=0;i<plane_count;i++)
	{
		double x1=pos[plane[i][0]][0];
		double y1=pos[plane[i][0]][1];
		double z1=pos[plane[i][0]][2];
		double x2=pos[plane[i][1]][0];
		double y2=pos[plane[i][1]][1];
		double z2=pos[plane[i][1]][2];
		double x3=pos[plane[i][2]][0];
		double y3=pos[plane[i][2]][1];
		double z3=pos[plane[i][2]][2];
		//ax+by+cz+d=0
		planeEq[i][0]=y1*(z2-z3) + y2*(z3-z1) + y3*(z1-z2);//a
		planeEq[i][1]=z1*(x2-x3) + z2*(x3-x1) + z3*(x1-x2);//b
		planeEq[i][2]=x1*(y2-y3) + x2*(y3-y1) + x3*(y1-y2);//c
		planeEq[i][3]=-(x1*(y2*z3-y3*z2) + x2*(y3*z1-y1*z3) + x3*(y1*z2-y2*z1));
	}

	scale = (float*)operator new(3*sizeof(float));
	position = (float*)operator new(3*sizeof(float));
	rotation = (float*)operator new(3*sizeof(float));
	rotate_speed = (float*)operator new(3*sizeof(float));
	for(int i=0; i<3; i++){
		scale[i] = 1;
		position[i] = rotation[i] = rotate_speed[i] = 0;
	}
	light_at_obj = (float*)operator new(4*sizeof(float));
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

void object_class::scaling(float x, float y, float z){
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
			glNormal3d(n[i][j * 3 + 0], n[i][j * 3 + 1], n[i][j * 3 + 2]);//這個點的法向量
			glVertex3d(pos[plane[i][j]][0], pos[plane[i][j]][1], pos[plane[i][j]][2]);//這個平面的第j個點
		}
	}
	glEnd();
}

void object_class::draw_shadow_poly(const double* CamaraPos)
{
	/* You may need to do something here */
	
}

void object_class::local_light(float* global_Light){
	
	vec3 lightPosV3 = vec3(global_Light[0], global_Light[1], global_Light[2]);

	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);

	glPushMatrix();
		glLoadIdentity();//重設為單位矩陣，畫房間
		glTranslatef(global_Light[0], global_Light[1], global_Light[2]);

		// get current matrix
		GLfloat currentMatF[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, currentMatF);

		// get inverse of current matrix
		GLfloat *inverseCurrentMatF = InverseMatrix(currentMatF);

		// apply inverse of current matrix to identity and Draw Scene
		glLoadIdentity();
		glMultMatrixf(inverseCurrentMatF);

		
		//for (int i = 0; i < plane_count; i++)
		for (int i = 0; i < 1; i++)
		{
			double a = planeEq[i][0];
			double b = planeEq[i][1];
			double c = planeEq[i][2];
			double d = planeEq[i][4];

			double result = a * global_Light[0] + b * global_Light[1] + c * global_Light[2] + d;

			if (result <= 0) {
				continue;
			}

			glColor3f(1.0, 1.0, 1.0);
			glBegin(GL_TRIANGLES);
				for (int j = 0; j < 3; j++)
				{				
						glNormal3d(n[i][j * 3 + 0], n[i][j * 3 + 1], n[i][j * 3 + 2]);//這個點的法向量
						glVertex3d(pos[plane[i][j]][0], pos[plane[i][j]][1], pos[plane[i][j]][2]);//這個平面的第j個點
				}
			glEnd();

			for (int j = 0; j < 3; j++)
			{
				vec3 p1 = vec3(pos[plane[i][j]][0], pos[plane[i][j]][1], pos[plane[i][j]][2]);
				vec3 p2;

				// Setup point 2: p1 & p2, p2 & p3, p3 & p1
				if (j == 2) {
					p2 = vec3(pos[plane[i][0]][0], pos[plane[i][0]][1], pos[plane[i][0]][2]);
				}
				else {
					p2 = vec3(pos[plane[i][j + 1]][0], pos[plane[i][j + 1]][1], pos[plane[i][j + 1]][2]);
				}

				vec3 lightVec1 = p1 - lightPosV3;
				vec3 lightVec2 = p2 - lightPosV3;
				
				// x + y + 0 * z + 0 = 100
				vec4 groundEq(0, 0, 1, 100);

				// Hmmm... I Should Consider as plane formula = planeEq shift long distance,
				// Or may can't deal with different direction of light
				
				double t1 = ((groundEq.x * lightPosV3.x + groundEq.y * lightPosV3.y + groundEq.z * lightPosV3.z + groundEq.z + groundEq.w) * -1.0) / (lightVec1.x + lightVec1.y + lightVec1.z);
				vec3 intersectPoint1 = lightPosV3 + vec3(lightVec1.x * t1, lightVec1.y * t1, lightVec1.z * t1);

				double t2 = ((groundEq.x * lightPosV3.x + groundEq.y * lightPosV3.y + groundEq.z * lightPosV3.z + groundEq.z + groundEq.w) * -1.0) / (lightVec2.x + lightVec2.y + lightVec2.z);
				vec3 intersectPoint2 = lightPosV3 + vec3(lightVec2.x * t2, lightVec2.y * t2, lightVec2.z * t2);

				/* // Draw Light Position
				glColor3f(1.0, 0.0, 0.0);
				glPushMatrix();
					glTranslatef(lightPosV3.x, lightPosV3.y, lightPosV3.z);
					glutSolidSphere(.5, 20, 20);
				glPopMatrix();
				*/

				/* // Draw p1 Position
				glColor3f(0.0, 1.0, 0.0);
				glPushMatrix();
					glTranslatef(p1.x, p1.y, p1.z);
					glutSolidSphere(.5, 20, 20);				
				glPopMatrix();
				*/

				/* // Draw p2 Position
				glColor3f(0.0, 1.0, 0.0);
				glPushMatrix();
					glTranslatef(p2.x, p2.y, p2.z);
					glutSolidSphere(.5, 20, 20);
				glPopMatrix();*/
				
				/* // Draw intersectPoint1 Position
				glColor3f(0.0, 0.0, 1.0);
				glPushMatrix();
					glTranslatef(intersectPoint1.x, intersectPoint1.y, intersectPoint1.z);
					glutSolidSphere(.5, 20, 20);
				glPopMatrix();
				*/

				/* // Draw intersectPoint2 Position
				glColor3f(0.0, 0.0, 1.0);
				glPushMatrix();
					glTranslatef(intersectPoint2.x, intersectPoint2.y, intersectPoint2.z);
					glutSolidSphere(.5, 20, 20);
				glPopMatrix();
				*/

				/* // Draw Triangle form lightPos, p1, p2
				glColor3f(1.0, 1.0, 0.0);
				glBegin(GL_TRIANGLES);
					glVertex3f(lightPosV3.x, lightPosV3.y, lightPosV3.z);
					glVertex3f(p1.x, p1.y, p1.z);
					glVertex3f(p2.x, p2.y, p2.z);					
				glEnd();
				*/

				if (j == 0) {
					glColor3f(0.0, 1.0, 0.0);
				}
				else if (j == 1) {
					glColor3f(0.0, 0.0, 1.0);
				}
				else {
					glColor3f(1.0, 0.0, 0.0);
				}
				// Draw Triangle of Shadow Volume
				glBegin(GL_TRIANGLES);
				glVertex3f(lightPosV3.x, lightPosV3.y, lightPosV3.z);
				glVertex3f(intersectPoint1.x, intersectPoint1.y, intersectPoint1.z);
				glVertex3f(intersectPoint2.x, intersectPoint2.y, intersectPoint2.z);
				glEnd();

				//glNormal3d(n[i][j * 3 + 0], n[i][j * 3 + 1], n[i][j * 3 + 2]);//這個點的法向量
				//glVertex3d(pos[plane[i][j]][0], pos[plane[i][j]][1], pos[plane[i][j]][2]);//這個平面的第j個點
			}
		}

	glPopMatrix();

	glEnable(GL_LIGHTING);
}

bool object_class::isIntersect(float origin, glm::vec3 direction, int planeID)
{
	return false;
}