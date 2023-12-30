#shadertype: geometry
#version 450 core

layout(points) in;
layout(points) out;
layout(max_vertices = 40) out;

struct SourceParticleVertexOut {
	vec3 position;
	vec4 startColor;
	vec4 endColor;
	float lifetime;
	uint particleCount;
};

//layout(location = 0) in SourceParticleVertexOut sParticleVertexIn[];

layout(location = 0) in vec3   position[];
layout(location = 1) in vec4 startColor[];
layout(location = 2) in vec4   endColor[];
layout(location = 3) in float  lifetime[];
layout(location = 4) in uint particleCount[];

layout(std140, binding = 2) uniform PerFrameData 
{
	float frameTime;
	vec3 randomSeed;
};

vec3 localRandomSeed;

layout(location = 0) out vec3  outPosition;
layout(location = 1) out vec4  outStartColor;
layout(location = 2) out vec4  outEndColor;
layout(location = 3) out float outLifetime;
layout(location = 4) out uint  outParticleCount;


float rand()
{
    uint n = floatBitsToUint(localRandomSeed.y * 214013.0 + localRandomSeed.x * 2531011.0 + localRandomSeed.z * 141251.0);
    n = n * (n * n * 15731u + 789221u);
    n = (n >> 9u) | 0x3F800000u;
 
    float fRes =  2.0 - uintBitsToFloat(n);
	
    localRandomSeed = vec3(localRandomSeed.x + 147158.0 * fRes, 
						   localRandomSeed.y * fRes  + 415161.0 * fRes, 
						   localRandomSeed.z + 324154.0 * fRes);
    return fRes;
}

void main()
{
	gl_Position.xyz = position[0] + vec3(1.0, 1.0, 0.0);
	outPosition = position[0] + vec3(1.0, 1.0, 0.0);
	EmitVertex();
	EndPrimitive();
	
	gl_Position.xyz = position[0] + vec3(-1.0, 1.0, 0.0);
	outPosition = position[0] + vec3(-1.0, 1.0, 0.0);
	EmitVertex();
	EndPrimitive();
	
	gl_Position.xyz = position[0] + vec3(1.0, -1.0, 0.0);
	outPosition = position[0] + vec3(1.0, -1.0, 0.0);
	EmitVertex();
	EndPrimitive();
	
	outStartColor = startColor[0];
}

//#shadertype: fragment
//#version 450 core
//
//layout(location = 1) in vec4 startColor;
//
//layout(location = 0) out vec4 fragColor;
//layout(location = 1) out vec4 maskColor;
//layout(location = 2) out int entt_id;
//
//void main() {
//	  fragColor = startColor;
//}



#shadertype: vertex
#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aStartColor;
layout(location = 2) in vec4 aEndColor;
layout(location = 3) in float aLifetime;
layout(location = 4) in uint aParticleCount;


layout(location = 0) out vec3 position;
layout(location = 1) out vec4 startColor;
layout(location = 2) out vec4 endColor;
layout(location = 3) out float lifetime;
layout(location = 4) out uint particleCount;

void main()
{
	position = aPosition;
	startColor = aStartColor;
	endColor = aEndColor;
	lifetime = aLifetime;
	particleCount = aParticleCount;
	
	//gl_PointSize = 10.0;
}