#version 330 core

out vec4 fragColor;

uniform vec4 squareColor;

void main()
{
	fragColor = squareColor;
}