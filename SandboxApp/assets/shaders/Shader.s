#shadertype: fragment
#version 460 core
in vec4 oColor;

out vec4 fragColor;
uniform vec4 u_QuadColor;

void main()
{
	fragColor = vec4(u_QuadColor);
}


#shadertype: vertex
#version 460 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aColor;

out vec4 oColor;

uniform mat4 u_ViewProjectionMatrix;

void main()
{
	gl_Position = u_ViewProjectionMatrix * vec4(aPosition, 1.0f);
	oColor = aColor;
}
