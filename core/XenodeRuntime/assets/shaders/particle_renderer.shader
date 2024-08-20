#shadertype: fragment
#version 450 core

layout(location = 0) in vec4 color;
layout(location = 1) smooth in vec2 textureCoords;

layout(location = 0) out vec4 outColor;

void main()
{
	outColor = color;
}


#shadertype: geometry
#version 450 core

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 4) out;

layout(std140, binding = 1) uniform PerFrameData { mat4 viewProjectionMat; }; 

// Input from vertex shader
layout(location = 0) in vec3               position[];
layout(location = 1) in vec3    originalGenPosition[];
layout(location = 2) in vec2               velocity[];
layout(location = 3) in vec2                   size[];
layout(location = 4) in vec4                  color[];
layout(location = 5) in float       currentLifetime[];
layout(location = 6) in float         totalLifetime[];
layout(location = 7) in uint           particleType[];

layout(location = 0)		out vec4 outColor;
layout(location = 1) smooth out vec2 outTextureCoords;


void main()
{
	vec3 pos = position[0];
	vec3 particleScale = vec3(size[0], 0.0);
	
	vec3 vert1 = pos + vec3( 0.5,  0.5, 0.0) * particleScale;
	vec3 vert2 = pos + vec3(-0.5,  0.5, 0.0) * particleScale;
	vec3 vert3 = pos + vec3(-0.5, -0.5, 0.0) * particleScale;
	vec3 vert4 = pos + vec3( 0.5, -0.5, 0.0) * particleScale;
	
	gl_Position = viewProjectionMat * vec4(vert1, 1.0);
	outTextureCoords = vec2(1.0, 1.0);
	outColor = color[0];
	EmitVertex();
	
	gl_Position = viewProjectionMat * vec4(vert2, 1.0);
	outTextureCoords = vec2(0.0, 1.0);
	outColor = color[0];
	EmitVertex();
	
	gl_Position = viewProjectionMat * vec4(vert4, 1.0);
	outTextureCoords = vec2(1.0, 0.0);
	outColor = color[0];
	EmitVertex();

	gl_Position = viewProjectionMat * vec4(vert3, 1.0);
	outTextureCoords = vec2(0.0, 0.0);
	outColor = color[0];
	EmitVertex();
	
	EndPrimitive();
}

#shadertype: vertex
#version 450 core

layout(location = 0) in vec3  aPosition;
layout(location = 1) in vec3  aOriginalGenPosition;
layout(location = 2) in vec2  aVelocity;
layout(location = 3) in vec2  aSize;
layout(location = 4) in vec4  aColor;
layout(location = 5) in float aCurrentLifetime;
layout(location = 6) in float aTotalLifetime;
layout(location = 7) in uint  aParticleType;


layout(location = 0) out vec3  position;
layout(location = 1) out vec3  originalGenPosition;
layout(location = 2) out vec2  velocity;
layout(location = 3) out vec2  size;
layout(location = 4) out vec4  color;
layout(location = 5) out float currentLifetime;
layout(location = 6) out float totalLifetime;
layout(location = 7) out uint  particleType;

void main()
{
	position = aPosition;
	originalGenPosition = aOriginalGenPosition;
	velocity = aVelocity;
	size = aSize;
	color = aColor;
	currentLifetime = aCurrentLifetime;
	totalLifetime = aTotalLifetime;
	particleType = aParticleType;
}