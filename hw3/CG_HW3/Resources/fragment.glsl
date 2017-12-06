#version 330 core

uniform vec3 lightPos;
uniform vec3 CameraPos;
uniform vec3 worldPos;
uniform sampler2D mainTex;
uniform sampler2D bumpTex;

in mat4 Model;
in vec3 n;
in vec3 t;
in vec3 b;
in vec2 uv;
in vec4 outColor;
in vec3 worldNormal;

out vec4 FragColor;

mat3 toMat3(vec3 n, vec3 t, vec3 b)
{
	mat3 m;
	m[0][0] = t[0];
	m[0][1] = b[0];
	m[0][2] = n[0];

	m[1][0] = t[1];
	m[1][1] = b[1];
	m[1][2] = n[1];

	m[2][0] = t[2];
	m[2][1] = b[2];
	m[2][2] = n[2];

	return m;
}

void main()
{
	float _BumpScale = 1;
	vec4 _LightColor0 = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 _Diffuse = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 _Specular = vec4(1.0, 1.0, 1.0, 1.0);
	float _Gloss = 25;

	
	vec3 Normal = normalize(n);
	vec3 Tangent = normalize(t);
	//vec3 Binormal = normalize(cross(Normal, Tangent));
	vec3 Binormal = normalize(b);

	vec3 wNormal = normalize((Model * vec4(n, 1.0)).xyz);
	vec3 wTangent = normalize((Model * vec4(t, 1.0)).xyz);
	vec3 wBinormal = normalize(cross(wNormal, wTangent));


	//mat3 tangent2Model = toMat3((Model * vec4(Normal, 1.0)).xyz, (Model * vec4(Tangent, 1.0)).xyz, (Model * vec4(Binormal, 1.0)).xyz);
	mat3 tangent2Model = toMat3(wNormal, wTangent, wBinormal);

	//mat3 model2Tagent = transpose(tangent2Model);

	vec3 TtoW0 = vec3(tangent2Model[0][0], tangent2Model[0][1], tangent2Model[0][2]);
	vec3 TtoW1 = vec3(tangent2Model[1][0], tangent2Model[1][1], tangent2Model[1][2]);
	vec3 TtoW2 = vec3(tangent2Model[2][0], tangent2Model[2][1], tangent2Model[2][2]);
	
	vec3 tangentNormal = texture2D(bumpTex, uv).xyz;
	
	tangentNormal.xy *= _BumpScale;
	tangentNormal.z = sqrt(1.0 - max(0, dot(tangentNormal.xy, tangentNormal.xy)));

	tangentNormal = normalize(vec3(dot(TtoW0.xyz, tangentNormal), dot(TtoW1.xyz, tangentNormal), dot(TtoW2.xyz, tangentNormal)));

	// Note we did not move light so it's already world pos
	vec3 nWorldLight = normalize(lightPos);

	vec3 nWorldNormal = normalize(worldNormal);

	nWorldNormal = tangentNormal;

	vec3 reflectDirection = normalize(reflect(-nWorldLight, nWorldNormal));

	vec3 viewDirection = normalize(CameraPos.xyz - worldPos.xyz);

	vec4 albedo = texture2D(mainTex, uv);

	vec3 diffuse = _LightColor0.rgb * _Diffuse.rgb * albedo.rgb * max(0, dot(nWorldNormal, nWorldLight));
				
	vec3 specular = _LightColor0.rgb * _Specular.rgb * pow(max(0, dot(viewDirection, reflectDirection)), _Gloss);

	vec3 color = diffuse + specular;

	FragColor = vec4(color, 1.0);

}