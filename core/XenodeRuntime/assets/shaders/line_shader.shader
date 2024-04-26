#shadertype: fragment
#version 450 core

layout(location = 0)in vec4 color;

layout(location = 0) out vec4 fragColor;

void main()
{
	fragColor = color;
}

#shadertype: vertex
#version 450 core

layout(location = 0)in vec3 aLinePosition;
layout(location = 1)in vec4 aLineColor;

layout(location = 0)out vec4 color;

// uniform mat4 u_ViewProjectionMatrix;

layout(std140, binding = 1) uniform CameraData
{
	mat4 ViewProjectionMatrix;
};

void main()
{
    gl_Position = ViewProjectionMatrix * vec4(aLinePosition, 1.0);
	color = aLineColor;
}