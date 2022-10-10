#version 460 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aColor;

out vec4 oColor;

void main()
{
	gl_Position = vec4(aPosition, 1.0f);
	oColor = aColor;
}