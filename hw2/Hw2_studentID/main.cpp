/* 
[CG hw2 stencil buffer]
You can focus on the part "You may need to do something here", and also "object_class.h"
If needed, please feel free to modify the code structure and all the source code we provided.
*/
#include <stdio.h>
#include <stdlib.h>
#include "../GL/glut.h"
#include "object_class.h"
#include "image_class.h"

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>

#define obj_total 3

using namespace glm;

namespace{
	//const double CamaraPos[]={1.5, 0.5 ,18.0};
	double Camera_Pos_[]={2.0, 1.0 ,18.0};
	const double BarrierPos[]={-2.0,-2.0,-0.5};

	double CameraDistance = sqrtf(Camera_Pos_[0] * Camera_Pos_[0] + Camera_Pos_[1] * Camera_Pos_[1] + Camera_Pos_[2] * Camera_Pos_[2]);
	double CameraPitch = glm::degrees(glm::asin(Camera_Pos_[2] / CameraDistance));
	double CameraYaw = glm::degrees(glm::acos(Camera_Pos_[1] / (CameraDistance * glm::sin((CameraPitch)))));;
	
	int width;
	int height;

	float LightPos[] = { 2.4f, 2.2f, 2.9f };//global position
	float LightAmb[] = { 0.1f, 0.1f, 0.3f, 1.0f };			// Ambient Light Values
	float LightDif[] = { 0.8f, 0.8f, 1.0f, 1.0f };			// Diffuse Light Values
	float LightSpc[] = { 0.5f, 0.5f, 0.5f, 1.0f };			// Specular Light Values

	float MatAmb[] = { 1.0f, 1.0f, 1.0f, 1.0f };			// Material - Ambient Values
	float MatDif[] = { 1.0f, 1.0f, 1.0f, 1.0f };			// Material - Diffuse Values
	float MatSpc[] = { 0.0f, 0.0f, 0.0f, 1.0f };			// Material - Specular Values
	float MatShn[] = { 0.1f };								// Material - Shininess
	GLUquadric* q;

	int obj_counter=0;
	object_class* obj_ptr[obj_total];
	object_class obj1("../Data/Object.txt");
	object_class obj2("../Data/Object1.txt");
	object_class obj3("../Data/Object2.txt");

	image_class* image;

	bool shadow_mode = true;
	int debug_mode = 0;
}

void DrawGLRoom()										// Draw The Room (Box)
{
	
	glDisable(GL_LIGHTING);
	glColor3d(1.0,1.0,1.0);
	glEnable(GL_TEXTURE_2D);
	image[0].BuildTexture();
	glBegin(GL_QUADS);									// Begin Drawing Quads
	// Floor
	glNormal3f(0.0f, 1.0f, 0.0f);					// Normal Pointing Up
	glTexCoord2d(0.0,1.0);
	glVertex3f(-8.0f, -8.0f, -20.0f);				// Back Left
	glTexCoord2d(0.0,0.0);
	glVertex3f(-8.0f, -8.0f, 20.0f);				// Front Left
	glTexCoord2d(1.0,0.0);
	glVertex3f(8.0f, -8.0f, 20.0f);					// Front Right
	glTexCoord2d(1.0,1.0);
	glVertex3f(8.0f, -8.0f, -20.0f);				// Back Right
	glEnd();
	image[1].BuildTexture();
	glBegin(GL_QUADS);
	// Ceiling
	glNormal3f(0.0f, -1.0f, 0.0f);					// Normal Point Down
	glTexCoord2d(0.0,1.0);
	glVertex3f(-8.0f, 8.0f, 20.0f);					// Front Left
	glTexCoord2d(0.0,0.0);
	glVertex3f(-8.0f, 8.0f, -20.0f);				// Back Left
	glTexCoord2d(1.0,0.0);
	glVertex3f(8.0f, 8.0f, -20.0f);					// Back Right
	glTexCoord2d(1.0,1.0);
	glVertex3f(8.0f, 8.0f, 20.0f);					// Front Right
	glEnd();
	image[2].BuildTexture();
	glBegin(GL_QUADS);
	// Front Wall
	glNormal3f(0.0f, 0.0f, 1.0f);					// Normal Pointing Away From Viewer
	glTexCoord2d(0.0,1.0);
	glVertex3f(-8.0f, 8.0f, -20.0f);				// Top Left
	glTexCoord2d(0.0,0.0);
	glVertex3f(-8.0f, -8.0f, -20.0f);				// Bottom Left
	glTexCoord2d(1.0,0.0);
	glVertex3f(8.0f, -8.0f, -20.0f);				// Bottom Right
	glTexCoord2d(1.0,1.0);
	glVertex3f(8.0f, 8.0f, -20.0f);					// Top Right
	glEnd();
	image[3].BuildTexture();
	glBegin(GL_QUADS);
	// Back Wall
	glNormal3f(0.0f, 0.0f, -1.0f);					// Normal Pointing Towards Viewer
	glTexCoord2d(0.0,1.0);
	glVertex3f(8.0f, 8.0f, 20.0f);					// Top Right
	glTexCoord2d(0.0,0.0);
	glVertex3f(8.0f, -8.0f, 20.0f);					// Bottom Right
	glTexCoord2d(1.0,0.0);
	glVertex3f(-8.0f, -8.0f, 20.0f);				// Bottom Left
	glTexCoord2d(1.0,1.0);
	glVertex3f(-8.0f, 8.0f, 20.0f);					// Top Left
	glEnd();
	image[4].BuildTexture();
	glBegin(GL_QUADS);
	// Left Wall
	glNormal3f(1.0f, 0.0f, 0.0f);					// Normal Pointing Right
	glTexCoord2d(0.0,1.0);
	glVertex3f(-8.0f, 8.0f, 20.0f);					// Top Front
	glTexCoord2d(0.0,0.0);
	glVertex3f(-8.0f, -8.0f, 20.0f);				// Bottom Front
	glTexCoord2d(1.0,0.0);
	glVertex3f(-8.0f, -8.0f, -20.0f);				// Bottom Back
	glTexCoord2d(1.0,1.0);
	glVertex3f(-8.0f, 8.0f, -20.0f);				// Top Back
	glEnd();
	image[5].BuildTexture();
	glBegin(GL_QUADS);
	// Right Wall
	glNormal3f(-1.0f, 0.0f, 0.0f);					// Normal Pointing Left
	glTexCoord2d(0.0,1.0);
	glVertex3f(8.0f, 8.0f, -20.0f);					// Top Back
	glTexCoord2d(0.0,0.0);
	glVertex3f(8.0f, -8.0f, -20.0f);				// Bottom Back
	glTexCoord2d(1.0,0.0);
	glVertex3f(8.0f, -8.0f, 20.0f);					// Bottom Front
	glTexCoord2d(1.0,1.0);
	glVertex3f(8.0f, 8.0f, 20.0f);					// Top Front
	glEnd();										// Done Drawing Quads
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

void init()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);				//���w�ϥΤ��z���¦�M�I��
	
	CameraPitch = 89.9187;
	CameraYaw = 449.655;

	// ReCalculate Camera
	Camera_Pos_[0] = CameraDistance * glm::sin(radians(CameraPitch)) * glm::cos(radians(CameraYaw));
	Camera_Pos_[2] = CameraDistance * glm::sin(radians(CameraPitch)) * glm::sin(radians(CameraYaw));
	Camera_Pos_[1] = CameraDistance * glm::cos(radians(CameraPitch));

	glShadeModel(GL_SMOOTH);
	q = gluNewQuadric();
	gluQuadricNormals(q, GL_SMOOTH);					// Enable Smooth Normal Generation
	//gluQuadricDrawStyle(q, GLU_FILL);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_LIGHT1);// Enable Light1
	glEnable(GL_LIGHTING);// Enable Lighting
	glLightfv(GL_LIGHT1, GL_POSITION, LightPos);// Set Light1 Position
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmb);// Set Light1 Ambience
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDif);// Set Light1 Diffuse
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpc);// Set Light1 Specular

	glMaterialfv(GL_FRONT, GL_AMBIENT, MatAmb);			// Set Material Ambience
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MatDif);			// Set Material Diffuse
	glMaterialfv(GL_FRONT, GL_SPECULAR, MatSpc);		// Set Material Specular
	glMaterialfv(GL_FRONT, GL_SHININESS, MatShn);		// Set Material Shininess

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	
	obj_ptr[0] = &obj1;
	obj_ptr[0]->scaling(0.5f, 0.5f, 0.5f);
	obj_ptr[1] = &obj2;
	obj_ptr[1]->scaling(0.7f, 0.7f, 0.7f);
	obj_ptr[1]->position[0] = -1.5;
	obj_ptr[1]->position[2] = 1.5;
	obj_ptr[2] = &obj3;
	obj_ptr[2]->scaling(0.9f, 0.9f, 0.9f);
	obj_ptr[2]->position[0] = 1.5;
	obj_ptr[2]->position[2] = -1.5;
	obj_counter=0;

	image=(image_class*) operator new(6*sizeof(image_class));
	new (image+0) image_class("../Data/bottom.bmp");
	new (image+1) image_class("../Data/ceiling.bmp");
	new (image+2) image_class("../Data/front.bmp");
	new (image+3) image_class("../Data/back.bmp");
	new (image+4) image_class("../Data/left.bmp");
	new (image+5) image_class("../Data/right.bmp");
	
}

void idle(){
	//printf("idle\n");
	glutPostRedisplay();
}
//---------�t�d������ø�Ϥ��e�����---------
void WindowSize(int w, int h)
{
	printf("�ثe�����j�p��%d X %d\n", w, h);
	
	width = w;
	height = h;

	glViewport(0, 0, w, h);							//��������e���ܮɡA�e���]�����
	glMatrixMode(GL_PROJECTION);					//��ܧ�v�x�}�Ҧ�
	glLoadIdentity();
	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f, (GLfloat)w / (GLfloat)h, 0.001f, 100.0f);
	gluLookAt(Camera_Pos_[0], Camera_Pos_[1], Camera_Pos_[2], 0, 0, 0, 0, 1, 0);				//�ù����Y���y�ФΤ�V
	glMatrixMode(GL_MODELVIEW);						//model�Ҧ��̱`�ΡA�S�ƴN���^�o�ӼҦ�
	glLoadIdentity();
}

//---------�����L��J---------
void Keyboard(unsigned char key, int x, int y)
{
	printf("�A�ҫ����䪺�X�O%x\t���ɵ��������ƹ��y�ЬO(%d,%d)\n", key, x, y);
	
	if(key==0x62) shadow_mode=!shadow_mode;//b �}�����v
	
	if(key==0x76)//v ��ܪ���
	{
		obj_counter=(obj_counter+1)%3;
		printf("Selecting Object: %d\n", obj_counter+1);
	}

	if(key==0x61) LightPos[0]-=0.1f;//a
	if(key==0x64) LightPos[0]+=0.1f;//d
	if(key==0x77) LightPos[1]+=0.1f;//w
	if(key==0x73) LightPos[1]-=0.1f;//s
	if(key==0x71) LightPos[2]-=0.1f;//q
	if(key==0x65) LightPos[2]+=0.1f;//e

	if(key==0x6a) obj_ptr[obj_counter]->rotate_speed[0]-=0.2f;//j
	if(key==0x6c) obj_ptr[obj_counter]->rotate_speed[0]+=0.2f;//l
	if(key==0x69) obj_ptr[obj_counter]->rotate_speed[1]+=0.2f;//i
	if(key==0x6b) obj_ptr[obj_counter]->rotate_speed[1]-=0.2f;//k
	if(key==0x75) obj_ptr[obj_counter]->rotate_speed[2]-=0.2f;//u
	if(key==0x6f) obj_ptr[obj_counter]->rotate_speed[2]+=0.2f;//o

	if(key==0x34) {obj_ptr[obj_counter]->position[0]-=0.2f;}//numpad4
	if(key==0x36) {obj_ptr[obj_counter]->position[0]+=0.2f;}//numpad6
	if(key==0x35) {obj_ptr[obj_counter]->position[1]-=0.2f;}//numpad5
	if(key==0x38) {obj_ptr[obj_counter]->position[1]+=0.2f;}//numpad8
	if(key==0x37) {obj_ptr[obj_counter]->position[2]-=0.2f;}//numpad7
	if(key==0x39) {obj_ptr[obj_counter]->position[2]+=0.2f;}//numpad9

	/*if (key == 0x66) { CamaraPos[0] -= 0.3f; } // f
	if (key == 0x74) { CamaraPos[2] += 0.3f; } // t
	if (key == 0x67) { CamaraPos[2] -= 0.3f; } // g
	if (key == 0x68) { CamaraPos[0] += 0.3f; } // h*/

	double CameraSpeed = 3.0f;

	if (key == 0x66) { CameraYaw += CameraSpeed; } // f
	if (key == 0x68) { CameraYaw -= CameraSpeed; } // h
	if (key == 0x74) { CameraDistance -= CameraSpeed; } // t
	if (key == 0x67) { CameraDistance += CameraSpeed; } // g
	if (key == 0x72) { CameraPitch += CameraSpeed; } // r
	if (key == 0x79) { CameraPitch -= CameraSpeed; } // y

	Camera_Pos_[0] = CameraDistance * glm::sin(radians(CameraPitch)) * glm::cos(radians(CameraYaw));
	Camera_Pos_[2] = CameraDistance * glm::sin(radians(CameraPitch)) * glm::sin(radians(CameraYaw));
	Camera_Pos_[1] = CameraDistance * glm::cos(radians(CameraPitch));

	cout << "Distance = ," << CameraDistance;
	cout << "Yaw = ," << CameraYaw;
	cout << "Pitch = " << CameraPitch << endl;


	if(key==0x20) std::cout<<"light pos:"<<LightPos[0]<<'\t'<<LightPos[1]<<'\t'<<LightPos[2]<<'\n'; //space

	if (key == 0x7a) {
		++debug_mode;
		if (debug_mode == 3) {
			debug_mode = 0;
		}
	}

}

void DrawObjects(int count)
{
	if (count > 3) count = 3;

	for (int i = 0; i < count; i++) {
		
		glPushMatrix();
		glScalef(obj_ptr[i]->scale[0], obj_ptr[i]->scale[1], obj_ptr[i]->scale[2]);
		glTranslatef(obj_ptr[i]->position[0], obj_ptr[i]->position[1], obj_ptr[i]->position[2]);
		glRotatef(obj_ptr[i]->rotation[0], 1, 0, 0);
		glRotatef(obj_ptr[i]->rotation[1], 0, 1, 0);
		glRotatef(obj_ptr[i]->rotation[2], 0, 0, 1);
		obj_ptr[i]->draw(); //draw the objects
		glPopMatrix();
	}
}

void DrawObjectsStencil(int count)
{
	if (count > 3) count = 3;

	GLfloat ObjectRotation[3];

	for (int i = 0; i < count; i++) {
		glLoadIdentity();

		// Setup ObjectRotation
		ObjectRotation[0] = obj_ptr[i]->rotation[0];
		ObjectRotation[1] = obj_ptr[i]->rotation[1];
		ObjectRotation[2] = obj_ptr[i]->rotation[2];

		glScalef(obj_ptr[i]->scale[0], obj_ptr[i]->scale[1], obj_ptr[i]->scale[2]);
		glTranslatef(obj_ptr[i]->position[0], obj_ptr[i]->position[1], obj_ptr[i]->position[2]);
		glRotatef(obj_ptr[i]->rotation[0], 1, 0, 0);
		glRotatef(obj_ptr[i]->rotation[1], 0, 1, 0);
		glRotatef(obj_ptr[i]->rotation[2], 0, 0, 1);

		glPushMatrix();
			//glTranslatef(-obj_ptr[i]->position[0], -obj_ptr[i]->position[1], -obj_ptr[i]->position[2]);
			obj_ptr[i]->local_light(LightPos, ObjectRotation, debug_mode); //draw the objects
		glPopMatrix();
		
	}
}

void DrawAll()
{
	glPushMatrix();//���]�����x�}�A�e�ж�
	DrawGLRoom();
	glPopMatrix();

	glPushMatrix();//���]�����x�}�A�e�p�y
	glTranslated(BarrierPos[0], BarrierPos[1], BarrierPos[2]);
	glutSolidSphere(1.0, 16, 16);//�p�y�A�u�O�ӻ�ê��
	glPopMatrix();

	glPushMatrix();
		DrawObjects(3);
	glPopMatrix();
}

void DrawShadow(double *CameraPos)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glCullFace(GL_FRONT);

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_STENCIL_TEST);
	glStencilMask(0x00);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

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

	glEnable(GL_LIGHTING);
}

//---------�yø�e��---------
void Display(void)
{
	glEnable(GL_STENCIL_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);	//�M�z�w�İ�
	glDisable(GL_STENCIL_BUFFER_BIT);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPos);// Set Light1 Position
	
	glMatrixMode(GL_PROJECTION);					//��ܧ�v�x�}�Ҧ�
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.001f, 100.0f);	
	gluLookAt(Camera_Pos_[0], Camera_Pos_[1], Camera_Pos_[2], 0, 0, 0, 0, 1, 0);
	
	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();
	
	/* Draw All Objects Without Stencil Test*/
	glPushMatrix();
		DrawAll();
	glPopMatrix();

	/* Stencil Test */
	glPushMatrix();
		DrawObjectsStencil(3);
	glPopMatrix();

	if(shadow_mode){
		/* Draw Shadow */
		glPushMatrix();
			DrawShadow(Camera_Pos_);
		glPopMatrix();
	}
		
	//������ϥΪ̪��D�����b��
	glLoadIdentity();//���m�A�e��
	glTranslatef(LightPos[0], LightPos[1], LightPos[2]);
	glColor4f(0.0f, 0.6f, 1.0f, 1.0f);
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);
	gluSphere(q, 0.1f, 16, 8);//��
	glEnable(GL_LIGHTING);
	glDepthMask(GL_TRUE);	

	for(int i=0;i<3;i++){
		obj_ptr[i]->rotation[0]+=obj_ptr[i]->rotate_speed[0];
		obj_ptr[i]->rotation[1]+=obj_ptr[i]->rotate_speed[1];
		obj_ptr[i]->rotation[2]+=obj_ptr[i]->rotate_speed[2];
	}

	glutSwapBuffers();								//��s�e��
}

int main()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);		//�yø�Ҧ��ϥ����w�İϥH�ά�����Alpha�C�ⶶ��
	glutInitWindowSize(600, 600);					//�������e
	glutInitWindowPosition(300, 200);				//�������W������m
	glutCreateWindow("HW2_shadow_0556652");		//�إߵ����å��W���D
	init();
													//�U���T�ӻPCallback�禡����
	glutReshapeFunc(WindowSize);					//��������ܤj�p�ɷ|����s���������e
	glutKeyboardFunc(Keyboard);						//�����L��J�T��
	//glutSpecialFunc(SpecialInput);				//�i�H�Ψӱo��W�U���k���S����A���L�Τ���
	glutDisplayFunc(Display);						//�t�d�yø
	glutIdleFunc(idle);
	glutMainLoop();									//�i�J�D�j��

	for(int i=0;i<6;i++) image[i].~image_class();	//�s���g�a�n�M��
	operator delete(image);

	return 0;
}