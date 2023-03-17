#shadertype: fragment
#version 140

in vec4 color;

out vec4 fragColor;

void main()
{
	fragColor = color;
}

#shadertype: vertex
#version 140

in vec3 aLinePosition;
in vec4 aLineColor;

out vec4 color;

uniform mat4 u_ViewProjectionMatrix;

void main()
{
	gl_Position = u_ViewProjectionMatrix * vec4(aLinePosition, 1.0);
	color = aLineColor;
}