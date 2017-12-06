#version 330 core

uniform vec3 lightPos;
uniform vec3 CameraPos;
uniform vec3 worldPos;
uniform sampler2D mainTex;

in vec2 uv;
in vec4 outColor;
in vec3 worldNormal;

out vec4 FragColor;

void main()
{
	vec4 _LightColor0 = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 _Diffuse = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 _Specular = vec4(1.0, 1.0, 1.0, 1.0);
	float _Gloss = 25;
	
	// Note we did not move light so it's already world pos
	vec3 nWorldLight = normalize(lightPos);

	vec3 nWorldNormal = normalize(worldNormal);

	vec3 reflectDirection = normalize(reflect(-nWorldLight, nWorldNormal));

	vec3 viewDirection = normalize(CameraPos.xyz - worldPos.xyz);

	vec4 albedo = texture2D(mainTex, uv);

	vec3 diffuse = _LightColor0.rgb * _Diffuse.rgb * albedo.rgb * max(0, dot(nWorldNormal, nWorldLight));
				
	vec3 specular = _LightColor0.rgb * _Specular.rgb * pow(max(0, dot(viewDirection, reflectDirection)), _Gloss);

	vec3 color = diffuse + specular;

	FragColor = vec4(color, 1.0);
}