#version 330 core

out vec4 FragColor;
in vec4 ourColor;

void main()
{
	vec4 Red = vec4(1.0, 0.0, 0.0, 1.0);
	FragColor = ourColor;
}