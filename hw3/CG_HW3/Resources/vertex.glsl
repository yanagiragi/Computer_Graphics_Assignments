#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textcoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

uniform mat4 MVP;
out vec4 outColor;

void main()
{
	gl_Position = MVP * vec4(position.x, position.y, position.z, 1.0);
		
	outColor = vec4(normal.x, normal.y, normal.z, 1.0);
}