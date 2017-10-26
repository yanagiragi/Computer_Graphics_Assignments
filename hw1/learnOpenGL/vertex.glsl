#version 330 core

layout (location = 0) in vec3 aPos;
uniform mat4 matrixM;
uniform mat4 matrixV;
uniform mat4 matrixP;
out vec4 c;

void main()
{
    vec4 pos = vec4(aPos.x, aPos.y, aPos.z, 1.0);

	c =  matrixM * pos;
	gl_Position = c;
	c = matrixM * matrixP * pos;
};