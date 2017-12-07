#version 330 core

uniform sampler2D mainTex;
uniform sampler2D bumpTex;
uniform mat4 M;
uniform mat4 V;
uniform bool isBump;
uniform float _BumpScale;

in vec2 uv;
in vec3 MV_Position;
in vec3 MV_Normal;
in vec3 MV_LightPos;
in vec3 n;
in vec3 t;
in vec3 b;
in mat3 tbn;

out vec4 FragColor;

void main()
{
	//float _BumpScale = 5;
	vec4 _LightColor0 = vec4(1.0, 1.0, 1.0, 1.0);
	vec3 _Ambient = vec3(0.1, 0.1, 0.1) * 2;
	vec3 _Diffuse = vec3(0.8, 0.8, 0.8);
	vec3 _Specular = vec3(0.5, 0.5, 0.5);
	float _Gloss = 4;

	vec4 bump = (texture2D(bumpTex, uv) * 2.0 - 1.0);

	bump = vec4(tbn * bump.xyz, bump.w);

	vec3 MV_bump = mat3(transpose(inverse(V * M))) * bump.xyz;

	MV_bump *= _BumpScale;
	MV_bump.z = sqrt(1 - dot(MV_bump.xy, MV_bump.xy));
	
	vec3 n_MV_Normal = normalize(MV_Normal);

	if (isBump) 
	{
		n_MV_Normal = MV_bump.xyz;
	}

	vec3 MV_LightDir = normalize(MV_LightPos - MV_Position);

	vec3 reflectDirection = normalize(reflect(-MV_LightDir, n_MV_Normal));

	vec4 albedo = texture2D(mainTex, uv);

	vec3 ambient = _Ambient * albedo.xyz;

	vec3 diffuse = _LightColor0.rgb * _Diffuse.rgb * albedo.rgb * max(0, dot(n_MV_Normal, MV_LightDir));
	
	vec3 viewDirection = normalize(-MV_Position);

	vec3 specular = _LightColor0.rgb * _Specular.rgb * pow(max(0, dot(viewDirection, reflectDirection)), _Gloss);

	vec3 color = ambient + diffuse + specular;

	FragColor = vec4(color, 1.0);

}