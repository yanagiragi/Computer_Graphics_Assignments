namespace
{
	char *obj_file_dir = "Resources/Ball.obj";
	char *main_tex_dir = "Resources/Tile.ppm";
	char *normal_map_dir = "Resources/NormalMap.ppm";

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
	
	
	float eyex = 0.0;
	float eyey = 0.0;
	float eyez = 3.0;
	
	GLfloat light_pos[] = { 1, 1, 1 };
	GLfloat ball_pos[] = { 0.0, 0.0, 0.0 };
	GLfloat ball_rot[] = { 0.0, 0.0, 0.0 };
	
	const float speed = 0.01;				//camera/light/ball moving speed
	const float rotation_speed = 0.05;		//ball rotation speed
	
	int frame = 0;
	float realFrame = 0;
	int startBouncing = 0;
	int isBump = 0;
	float bumpScale = 1.0;
	float _PixelSize = 0.01;
}

void motion(int x, int y) {
	if (mleft)
	{
		eyep -= (x - mousex)*0.1;
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
		if (state == GLUT_DOWN && !mright && !mmiddle)
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
	GLfloat dx = 0, dy = 0, dz = 0;
	if (left || right || forward || backward || up || down)
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
	if (lleft || lright || lforward || lbackward || lup || ldown)
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
		else if (ldown)
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
	if (bx || by || bz || brx || bry || brz)
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
			right = false;
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
		case '=':
		{
			bumpScale += 0.1f;
			break;
		}
		case '-':
		{
			bumpScale -= 0.1f;
			break;
		}
		case 'n':
		{
			startBouncing = (startBouncing + 1) % 4;
			std::cout << "mode = " << startBouncing << std::endl;
			realFrame = 0.0;
			break;
		}
		case '.':
		{
			_PixelSize += 0.001f;
			break;
		}
		case '/':
		{
			_PixelSize -= 0.001f;
			if (_PixelSize <= 0)
			{
				_PixelSize = 0.001f;
			}
			break;
		}
		default:
		{
			break;
		}
	}
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
			isBump++;
			isBump = (isBump) % 2;
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
			startBouncing = false;
			_PixelSize = 0.0001;
			bumpScale = 1.0;
			break;
		}
		default:
		{
			break;
		}
	}
}
