#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

uniform mat4 MVP;
uniform mat4 AlterM;
uniform float Time;
out vec4 ourColor;

vec4 q;
mat4 _AlterM;
mat4 _MVP;

vec4 mul(vec4 lhs, vec4 rhs)
{
	vec3 vThis = vec3(lhs.x, lhs.y, lhs.z);
	vec3 vRhs = vec3(rhs.x, rhs.y, rhs.z);
	vec3 product = lhs.w * vRhs + rhs.w * vThis + cross(vThis, vRhs);

	vec4 q;

	q.w = lhs.w * rhs.w - dot(vThis, vRhs);
	q.x = product.x;
	q.y = product.y;
	q.z = product.z;

	return q;
}

vec4 rotate(float angle, vec3 rhs)
{
	vec4 q;

	float sin = sin(angle * 0.5f);
	float cos = cos(angle * 0.5f);

	q.w = cos;
	q.x = rhs.x * sin;
	q.y = rhs.y * sin;
	q.z = rhs.z * sin;

	return q;
}

mat4 toMat4(vec4 q)
{
	mat4 res;
	
	res[0][0] = 1.0 - 2.0 * q.y * q.y - 2.0 * q.z * q.z;
	res[0][1] = 2 * q.x * q.y - 2 * q.z * q.w;
	res[0][2] = 2 * q.x * q.z + 2 * q.y * q.w;
	res[0][3] = 0;

	res[1][0] = 2 * q.x * q.y + 2 * q.z * q.w;
	res[1][1] = 1 - 2 * q.x * q.x - 2 * q.z * q.z;
	res[1][2] = 2 * q.y * q.z - 2 * q.x * q.w;
	res[1][3] = 0;

	res[2][0] = 2 * q.x * q.z - 2 * q.y * q.w;
	res[2][1] = 2 * q.y * q.z + 2 * q.x * q.w;
	res[2][2] = 1 - 2 * q.x * q.x - 2 * q.y * q.y;
	res[2][3] = 0;
	
	res[3][0] = 0;
	res[3][1] = 0;
	res[3][2] = 0;
	res[3][3] = 1;	

	return res;
}

void main()
{
	// init
	q = vec4(1.0, 0.0, 0.0, 0.0);
	_AlterM = AlterM;
	_MVP = MVP;

	// 自轉
	q = rotate(Time, vec3(0.0, 1.0, 0.0));	
	
	// 公轉
	vec4 tmp = rotate(Time, vec3(1.0, 0.0, 0.0));
	_AlterM = toMat4(tmp) * _AlterM;

	_MVP = _MVP * _AlterM * toMat4(q);
	
	gl_Position = _MVP * vec4(pos.x, pos.y, pos.z, 1.0);
	//ourColor = vec4(normal.x, normal.y, normal.z, 1.0) * sin(Time);
	
	ourColor = vec4(normal.x, normal.y, normal.z, 1.0);
}