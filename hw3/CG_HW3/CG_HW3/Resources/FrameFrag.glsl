#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D renderedTexture;
uniform sampler2D _NoiseTex;
uniform sampler2D _LineTex;
uniform sampler2D _VignetteTex;
uniform float _PixelSize;
uniform int _Time;
uniform int mode;

vec2 LenDistortion(vec2 uv)
{
	float _DistortionSrength = 5;

	// map uv to [-1,1]
	vec2 center = (uv - 0.5) / 2.0;
	float r2 = dot(center, center);
	float ratio = 1 + r2 * (_DistortionSrength + sqrt(r2));

	return ratio * center * 2.0 + 0.5;
}

void main(){	

	if(mode == 0 || mode == 1)
	{
		color = texture2D(renderedTexture, UV);
	}
	else if(mode == 2)
	{
		vec2 newUV = vec2(int(UV.x / _PixelSize), int(UV.y / _PixelSize)) * _PixelSize;
	
		color = texture2D(renderedTexture, newUV);
	}
	else if(mode == 3)
	{
		float _NoiseXSpeed = 100.0;
		float _NoiseYSpeed = 100.0;
		float _NoiseCutoff = 0;
		vec4 _LineColor = vec4(1,1,1,1);
		float _LineScrollSpeed = 0.05;

		vec2 distortionUV = LenDistortion(UV);
		
		if(distortionUV.x > 1 || distortionUV.y > 1 || distortionUV.x < 0 || distortionUV.y < 0)
			color = vec4(0.0, 0.0, 0.0, 1.0);
		else
			color = texture2D(renderedTexture, distortionUV);
			
		vec2 noiseUV = distortionUV + vec2(_NoiseXSpeed, _NoiseYSpeed) * sin(_Time);
		vec4 noiseColor = texture2D(_NoiseTex, noiseUV);
		if(noiseColor.r < _NoiseCutoff){
			noiseColor = vec4(1,1,1,1);
		}

		vec2 lineUV = distortionUV;
		lineUV.y = lineUV.y + (_Time % 10) * _LineScrollSpeed;
		vec4 lineColor = texture2D(_LineTex, lineUV) * _LineColor;

		color *= noiseColor;

		color += lineColor * noiseColor;

		vec4 vignetteColor = texture2D(_VignetteTex, UV);
				
		color *= vignetteColor;
	}
}