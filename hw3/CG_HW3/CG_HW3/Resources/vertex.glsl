#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textcoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

uniform sampler2D bumpTex;
uniform float frame;
uniform int bouncing;

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
	
	gl_Position = P * V * M * (vec4(position.x, position.y, position.z, 1.0));

	if (bouncing == 1)
	{
		float time = frame;

		vec4 bump = (texture2D(bumpTex, uv) * 2.0 - 1.0);
		bump = vec4(tbn * bump.xyz, bump.w);
		vec3 MV_bump = mat3(transpose(inverse(V * M))) * bump.xyz;

		vec4 Position = vec4(inverse(P * V * M) * vec4(gl_Position.xyz, 1.0));
		Position = vec4(Position.xyz + MV_bump.xyz, 1.0);
		Position = P * V * M * Position;

		gl_Position = ((sin(time) + 1.0) / 2.0) * Position + (1 - ((sin(time) + 1.0) / 2.0)) * gl_Position;
	}
}