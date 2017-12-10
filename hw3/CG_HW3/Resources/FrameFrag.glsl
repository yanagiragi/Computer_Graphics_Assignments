#version 330 core

in vec2 UV;

out vec3 color;

uniform sampler2D renderedTexture;
uniform float _PixelSize;

void main(){	

    vec2 newUV = vec2(int(UV.x / _PixelSize), int(UV.y / _PixelSize)) * _PixelSize;
	
	color = texture2D(renderedTexture, newUV).xyz;
	
}