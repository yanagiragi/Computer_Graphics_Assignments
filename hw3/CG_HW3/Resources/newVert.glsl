#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textcoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

uniform vec3 lightPos;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec2 uv;
out vec3 MV_Position;
out vec3 MV_Normal;
out vec3 MV_LightPos;
out vec3 n;
out vec3 t;
out vec3 b;
out mat3 tbn;

void main()
{
	uv = textcoord;

	//vec3 newbitangent = normalize(cross(normal, tangent));

	tbn = mat3(tangent, bitangent, normal);
	
	MV_Normal = normalize(mat3(transpose(inverse(V * M))) * normal);
	MV_LightPos = (V * vec4(lightPos, 1.0)).xyz;
	MV_Position = (V * M * vec4(position, 1.0)).xyz;
	
	n = normal;
	t = tangent;
	b = bitangent;
	
	//gl_Position = P * V * M * (vec4(position.x, position.y, position.z, 1.0));
	gl_Position = P * V * M * (vec4(position.xyz + normal.xyz * 0.1,  1.0));
}