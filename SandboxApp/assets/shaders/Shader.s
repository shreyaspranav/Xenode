#shadertype: fragment
#version 460 core
in vec4 color;

out vec4 fragColor;

void main()
{
	fragColor = color;
}


#shadertype: vertex
#version 460 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTextureCoords;
layout (location = 3) in float a;

out vec4 color;

uniform mat4 u_ViewProjectionMatrix;

void main()
{
	gl_Position = u_ViewProjectionMatrix * vec4(aPosition, 1.0f);
	color = aColor;
}