#version 460 core
in vec4 oColor;

out vec4 fragColor;
uniform vec4 u_QuadColor;

void main()
{
	fragColor = vec4(u_QuadColor);
}