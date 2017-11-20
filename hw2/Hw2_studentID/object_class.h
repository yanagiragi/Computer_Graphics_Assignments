#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "../GL/glut.h"

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

unsigned int gggdata[800][600];

class object_class
{
public:
	object_class(const char* in_filename);//load file
	~object_class();
	void draw();//draw this object
	void scaling(float x, float y, float z);
	
	/* you should focus on this two functions */
	void draw_shadow_poly(const double* , double );
	void local_light(float*, const float*, int);

	void object_class::Transform_Light(float*, GLfloat*);
	GLfloat *Transform(GLfloat*, GLfloat*);

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

void object_class::draw_shadow_poly(const double* CameraPos,double CameraYaw)
{
	/* You may need to do something here */
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glCullFace(GL_FRONT);
	
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_STENCIL_TEST);
	glStencilMask(0x00);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	//glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
	glStencilFunc(GL_EQUAL, 1, 0xFF);
	//glStencilFunc(GL_ALWAYS, 1, 0xFF);

	//glStencilFunc(GL_GREATER, 1, 0xFF);
	//glStencilFunc(GL_EQUAL, 1, 0xFF);
	//glStencilFunc(GL_GEQUAL, , 0xFF);

	glPushMatrix();
		glLoadIdentity();
		glTranslatef(CameraPos[0], CameraPos[1], CameraPos[2]);
		glColor4f(0.0, 0.0, 0.0, 0.5);
		glutSolidSphere(0.01, 20, 20);
	glPopMatrix();

	// Restore parameters
	glCullFace(GL_BACK);

	glDisable(GL_STENCIL_TEST);
	glStencilMask(0xFF);
	
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	
	/*glClear(GL_STENCIL_BUFFER_BIT);	//清理緩衝區
	glDisable(GL_LIGHTING);
	
	glCullFace(GL_FRONT);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilMask(0x00);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilFunc(GL_LESS, 0, 0xFF);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(CameraPos[0], CameraPos[1], CameraPos[2]);
	glColor4f(1.0, 0.0, 1.0, 1.0);
	glutSolidSphere(0.01, 20, 20);
	glPopMatrix();

	glCullFace(GL_FRONT);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilMask(0x00);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilFunc(GL_GREATER, 0, 0xFF);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(CameraPos[0], CameraPos[1], CameraPos[2]);
	glColor4f(1.0, 0.0, 0.0, 1.0);
	glutSolidSphere(0.01, 20, 20);
	glPopMatrix();

	glCullFace(GL_FRONT);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilMask(0x00);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilFunc(GL_GREATER, 1, 0xFF);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(CameraPos[0], CameraPos[1], CameraPos[2]);
	glColor4f(0.0, 1.0, 0.0, 1.0);
	glutSolidSphere(0.01, 20, 20);
	glPopMatrix();

	glCullFace(GL_FRONT);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilMask(0x00);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilFunc(GL_GREATER, 2, 0xFF);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(CameraPos[0], CameraPos[1], CameraPos[2]);
	glColor4f(0.0, 0.0, 1.0, 1.0);
	glutSolidSphere(0.01, 20, 20);
	glPopMatrix();*/
	
	//glStencilMask(0xff);
	//glClear(GL_STENCIL_BUFFER_BIT);

	
	glReadPixels(0, 0, 800, 600, GL_STENCIL_INDEX, GL_UNSIGNED_INT, gggdata);

	double sum = 0;
	for (int i = 0; i < 800; ++i) {
		for (int j = 0; j < 600; ++j) {
			sum += gggdata[i][j];
			//printf("sum = %f, data[%d] = %o\n", sum, i, gggdata[i][j]);
		}
	}

	// Restore parameters
	glCullFace(GL_BACK);

	glDisable(GL_STENCIL_TEST);
	glStencilMask(0xFF);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_LIGHTING);
}

GLfloat* object_class::Transform(GLfloat* vec, GLfloat* mat)
{
	GLfloat result[3];
	result[0] = vec[0] * mat[0] + vec[1] * mat[4] + vec[2] + mat[8];
	result[1] = vec[0] * mat[1] + vec[1] * mat[5] + vec[2] + mat[9];
	result[2] = vec[0] * mat[2] + vec[1] * mat[6] + vec[2] + mat[10];
	return result;
}

void object_class::Transform_Light(GLfloat* global_Light, GLfloat* mat)
{
	for (int i = 0; i < 3; i++) // 新的光點
		light_at_obj[i] = mat[i] * global_Light[0] + mat[4 + i] * global_Light[1] + mat[8 + i] * global_Light[2] + mat[12 + i];
}

void object_class::local_light(float* global_Light, const float* ObjectRotation, int DebugFlag = 0){
	
	// Calculate global_light in Object Coordinate and Store result in light_at_obj
	GLfloat ObjectRotation_M[16];
	glPushMatrix();
		glLoadIdentity();
		glRotatef(-ObjectRotation[2], 0, 0, 1);
		glRotatef(-ObjectRotation[1], 0, 1, 0);
		glRotatef(-ObjectRotation[0], 1, 0, 0);
		glGetFloatv(GL_MODELVIEW_MATRIX, ObjectRotation_M);
	glPopMatrix();

	Transform_Light(global_Light, ObjectRotation_M);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	glDisable(GL_LIGHTING);
	
	if(DebugFlag == 0)
	{
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}
	else
	{
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	}

	glCullFace(GL_BACK);
	
	glPushMatrix();

		
		for (int i = 0; i < plane_count; i++)
		//for (int i = 0; i < 1; i++)
		{
			double a = planeEq[i][0];
			double b = planeEq[i][1];
			double c = planeEq[i][2];
			double d = planeEq[i][4];

			double result = a * light_at_obj[0] + b * light_at_obj[1] + c * light_at_obj[2] + d;

			if (result <= 0) {
				continue;
			}
			
			if (DebugFlag == 2) {
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			}

			glDepthMask(GL_FALSE);
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

			if (DebugFlag == 2) {
				glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			}

			// Start Stencil Test
			glDepthMask(GL_FALSE);

			for (int count = 0; count < 2; ++count) 
			{
				if (count == 1) {
					//continue;
					glFrontFace(GL_CCW);
					
					glEnable(GL_STENCIL_TEST);
					glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
					glStencilFunc(GL_ALWAYS, 1, 0xFF);
					glStencilMask(0xFF);
					//glDepthFunc(GL_LEQUAL);
					glDepthMask(false);
				}
				else { // count == 1
					
					glFrontFace(GL_CW);

					glEnable(GL_STENCIL_TEST);
					glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
					glStencilFunc(GL_ALWAYS, 1, 0xFF);
					glStencilMask(0xFF);
					//glDepthFunc(GL_LEQUAL);
					glDepthMask(false);
				}

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

					// x + y + 0 * z + 0 = 40
					vec4 groundEq(0, 0, 1, 100);

					// Hmmm... I Should Consider as plane formula = planeEq shift long distance,
					// Or may can't deal with different direction of light

					double t1 = ((groundEq.x * light_at_obj[0] + groundEq.y * light_at_obj[2] + groundEq.z * light_at_obj[2] + groundEq.z + groundEq.w) * -1.0) / (lightVec1[0] + lightVec1[1] + lightVec1[2]);
					GLfloat intersectPoint1[3] = { light_at_obj[0] + (lightVec1[0] * t1), light_at_obj[1] + (lightVec1[1] * t1), light_at_obj[2] + (lightVec1[2] * t1) };

					double t2 = ((groundEq.x * light_at_obj[0] + groundEq.y * light_at_obj[2] + groundEq.z * light_at_obj[2] + groundEq.z + groundEq.w) * -1.0) / (lightVec2[0] + lightVec2[1] + lightVec2[2]);
					GLfloat intersectPoint2[3] = { light_at_obj[0] + (lightVec2[0] * t2), light_at_obj[1] + (lightVec2[1] * t2), light_at_obj[2] + (lightVec2[2] * t2) };

					#pragma region debugFunc
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
#pragma endregion

					// Draw Triangle of Shadow Volume
					/*glBegin(GL_TRIANGLES);
						glVertex3f(p1[0], p1[1], p1[2]);
						glVertex3f(p2[0], p2[1], p2[2]);
						glVertex3f(intersectPoint1[0], intersectPoint1[1], intersectPoint1[2]);
						glVertex3f(intersectPoint1[0], intersectPoint1[1], intersectPoint1[2]);
						glVertex3f(p2[0], p2[1], p2[2]);
						glVertex3f(intersectPoint2[0], intersectPoint2[1], intersectPoint2[2]);
					glEnd();*/

					glPushMatrix();
					//glLoadIdentity();
						
						glBegin(GL_QUADS);					
							glVertex3f(intersectPoint1[0], intersectPoint1[1], intersectPoint1[2]);
							glVertex3f(p1[0], p1[1], p1[2]);
							glVertex3f(p2[0], p2[1], p2[2]);						
							glVertex3f(intersectPoint2[0], intersectPoint2[1], intersectPoint2[2]);
						glEnd();
					glPopMatrix();



				} // End of j-Loop
			} // End of Count-Loop
		} // End of i-Loop

	glPopMatrix();
	glFrontFace(GL_CCW);
	
	glEnable(GL_LIGHTING);
	
	glDepthMask(true);
	glStencilMask(0x00);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_LIGHTING);
	
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}