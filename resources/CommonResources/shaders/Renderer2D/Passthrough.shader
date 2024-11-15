#shadertype: fragment
#version 450 core

layout(location = 0) in vec2 texCoord;
layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D textureSampler;

void main()
{
	outColor = texture(textureSampler, texCoord);
}

#shadertype: vertex
#version 450 core

layout(location = 0) out vec2 texCoord;

vec4 vertices[4] = {
	vec4( 1.0,  1.0, 0.0, 1.0),
	vec4(-1.0,  1.0, 0.0, 1.0),
	vec4(-1.0, -1.0, 0.0, 1.0),
	vec4( 1.0, -1.0, 0.0, 1.0)
};

vec2 texCoords[4] = {
	vec2(1.0, 1.0),
	vec2(0.0, 1.0),
	vec2(0.0, 0.0),
	vec2(1.0, 0.0)
};

int indices[] = { 0, 1, 2, 0, 2, 3 };

void main()
{
	gl_Position = vertices[indices[gl_VertexID]];
	texCoord = texCoords[indices[gl_VertexID]];
}

