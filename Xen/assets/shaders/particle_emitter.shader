#shadertype: geometry
#version 450 core

layout(points) in;
layout(points) out;
layout(max_vertices = 40) out;

// Input from vertex shader
layout(location = 0) in vec3          position[];
layout(location = 1) in vec4        startColor[];
layout(location = 2) in float  currentLifetime[];
layout(location = 3) in float    totallifetime[];
layout(location = 4) in vec2          velocity[];
layout(location = 5) in uint      particleType[];

layout(std140, binding = 2) uniform PerFrameData
{
	float frameTime;
	vec3 randomSeed;

	uint numToGenerate;
};

layout(std140, binding = 3) uniform ParticleEmitterSettings
{
	vec4 startColor;
	vec4 endColor;

	vec2 lifeRange;
	vec2 sizeRange;
}

layout(location = 0) out vec3  outPosition;
layout(location = 1) out vec4  outColor;
layout(location = 2) out float outCurrentLifetime;
layout(location = 3) out float outTotalLifetime;
layout(location = 4) out vec2  outVelocity;
layout(location = 5) out uint  outParticleType;
 

vec3 localRandomSeed;

// Temp:
const float velocityDamping = 0.01;
const float particleLifetime = 4.0;

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
	localRandomSeed = randomSeed;

	// particleType can be either 1 or 0
	// 0 means generator particle
	// 1 means normal particle

	outPosition = position[0];
	outVelocity = velocity[0];

	if (particleType[0])
	{
		outPosition += velocity * frameTime;
		outVelocity -= velocity * velocityDamping * frameTime;
	}

	outColor = mix(startColor, endColor, lifeTime[0] / totalLifeTime);
	outLifeTime -= frameTime;
	outParticleType = particleType[0];

	if (outParticleType == 0)
	{
		EmitVertex();
		EndPrimitive();

		for (int i = 0; i < numToGenerate; i++)
		{
			outVelocity = vec2(rand(), rand());
			outColor = startColor;
			outLifetime = mix(lifeRange.x, lifeRange.y, rand());

			EmitVertex();
			EndPrimitive();
		}
	}
	else if (outLifeTime > 0.0)
	{
		EmitVertex();
		EndPrimitive();
	}
}

#shadertype: vertex
#version 450 core

layout(location = 0) in vec3  aPosition;
layout(location = 1) in vec4  aColor;
layout(location = 2) in float aCurrentLifetime;
layout(location = 3) in float aTotalLifetime;
layout(location = 4) in vec2 aVelocity;
layout(location = 5) in uint  aParticleType;


layout(location = 0) out vec3  position;
layout(location = 1) out vec4  color;
layout(location = 2) out float currentLifetime;
layout(location = 3) out float totalLifetime;
layout(location = 4) out vec2  velocity;
layout(location = 5) out uint  particleType;

void main()
{
	position = aPosition;
	color = aColor;
	currentLifetime = aCurrentLifetime;
	totalLifetime = aTotalLifetime;
	velocity = aVelocity;
	particleType = aParticleType;
}