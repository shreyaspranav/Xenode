#shadertype: geometry
#version 450 core

layout(points) in;
layout(points, max_vertices = 40) out;

// Input from vertex shader
layout(location = 0) in vec3               position[];
layout(location = 1) in vec3    originalGenPosition[];
layout(location = 2) in vec2               velocity[];
layout(location = 3) in vec2                   size[];
layout(location = 4) in vec4                  color[];
layout(location = 5) in float       currentLifetime[];
layout(location = 6) in float         totalLifetime[];
layout(location = 7) in uint           particleType[];

struct ColorSlider { vec4 color; float sliderValue; };
struct AlphaSlider { float alpha; float sliderValue; };

struct ColorGradient 
{ 
	ColorSlider colors[8]; 
	AlphaSlider alphas[8]; 

	uint colorSliderSize, alphaSliderSize; 
};

layout(std140, binding = 2) uniform PerFrameData
{
	float frameTime;
	vec3 randomSeed;

	uint numToGenerate;
};

layout(std140, binding = 3) uniform ParticleEmitterSettings
{
	vec3 genPosition;

	ColorGradient colorGradient;
	uint affectColorBy;
	float colorDistanceFactor;

	vec4 startColor;
	vec4 endColor;

	vec2 lifeRange, sizeRange, velocityRange;
	float velocityDamping;

};

layout(location = 0) out vec3  outPosition;
layout(location = 1) out vec3  outOriginalGenPosition;
layout(location = 2) out vec2  outVelocity;
layout(location = 3) out vec2  outSize;
layout(location = 4) out vec4  outColor;
layout(location = 5) out float outCurrentLifetime;
layout(location = 6) out float outTotalLifetime;
layout(location = 7) out uint  outParticleType;
 

vec3 localRandomSeed;

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

vec4 getFinalColor(ColorGradient gradient, float sliderValue)
{
	vec3 finalColor;
	float finalAlpha;

	// Color calculation: 
	if(sliderValue <= gradient.colors[0].sliderValue)
		finalColor = gradient.colors[0].color.xyz;
		
	else if(sliderValue >= gradient.colors[gradient.colorSliderSize - 1].sliderValue)
		finalColor = gradient.colors[gradient.colorSliderSize - 1].color.xyz;

	else 
	{
		for(int i = 0; i < gradient.colorSliderSize - 1; i++)
		{
			float leftSliderValue  = gradient.colors[i].sliderValue;
			float rightSliderValue = gradient.colors[i + 1].sliderValue;

			vec3 leftColor  = gradient.colors[i].color.xyz;
			vec3 rightColor = gradient.colors[i + 1].color.xyz;

			if(sliderValue >= leftSliderValue && sliderValue <= rightSliderValue)
			{
				float transformedPosition = (sliderValue - leftSliderValue) / (rightSliderValue - leftSliderValue);

				finalColor = leftColor * (1.0 - transformedPosition) + rightColor * transformedPosition;

				break;
			}
		}
	}

	// Alpha calculation: 
	if(sliderValue < gradient.alphas[0].sliderValue)
		finalAlpha = gradient.alphas[0].alpha;
		
	else if(sliderValue > gradient.colors[gradient.alphaSliderSize - 1].sliderValue)
		finalAlpha = gradient.alphas[gradient.alphaSliderSize - 1].alpha;

	else 
	{
		for(int i = 0; i < gradient.alphaSliderSize - 1; i++)
		{
			float leftSliderValue  = gradient.alphas[i].sliderValue;
			float rightSliderValue = gradient.alphas[i + 1].sliderValue;

			if(sliderValue > leftSliderValue && sliderValue <= rightSliderValue)
			{
				float leftAlpha  = gradient.alphas[i].alpha;
				float rightAlpha = gradient.alphas[i + 1].alpha;

				float transformedPosition = (sliderValue - leftSliderValue) / (rightSliderValue - leftSliderValue);

				finalAlpha = leftAlpha * (1.0 - transformedPosition) + rightAlpha * transformedPosition;

				break;
			}
		}
	}

	return vec4(finalColor, finalAlpha);
}

void main()
{
	localRandomSeed = randomSeed;
	
	// particleType can be either 1 or 0
	// 0 means generator particle
	// 1 means normal particle
	
	outPosition = position[0];
	outVelocity = velocity[0];
	outSize = size[0];
	outOriginalGenPosition = originalGenPosition[0];
	
	if (particleType[0] == 1)
	{
		outPosition += vec3(velocity[0] * frameTime * 0.01, genPosition.z);
		outVelocity -= velocity[0] * velocityDamping * frameTime * 0.001;
	}
	
	outCurrentLifetime = currentLifetime[0] - frameTime * 0.001;
	outTotalLifetime = totalLifetime[0];
	outParticleType = particleType[0];

	//outColor = mix(endColor, startColor, outCurrentLifetime / outTotalLifetime);

	float colorGradientSlider = 1.0f;

	switch(affectColorBy)
	{
		case 0:
			outColor = getFinalColor(colorGradient, distance(outOriginalGenPosition, position[0]) / colorDistanceFactor);
			break;
		case 1:
			outColor = getFinalColor(colorGradient,  1.0 - outCurrentLifetime / outTotalLifetime);
			break;
		case 2:
			outColor = color[0];
			break;
	}

	
	if (outParticleType == 0)
	{
		outColor = vec4(0.0, 0.0, 0.0, 0.0);
		
		EmitVertex();
		EndPrimitive();
	
		for (int i = 0; i < numToGenerate; i++)
		{
			outPosition = genPosition;
			outOriginalGenPosition = genPosition;

			float randNumSize = rand();
			outSize = vec2(mix(sizeRange.x, sizeRange.y, randNumSize));

			vec2 velocityDirection = vec2((rand() * 2.0) - 1.0, (rand() * 2.0) - 1.0);
			float velocityMag = mix(velocityRange.x, velocityRange.y, rand());

			outVelocity = velocityDirection * velocityMag;

			if(affectColorBy != 2)
				outColor = startColor;
			else 
				outColor = getFinalColor(colorGradient, rand());
			outTotalLifetime = mix(lifeRange.x, lifeRange.y, rand());
			outCurrentLifetime = outTotalLifetime;
			outParticleType = 1;
			
			EmitVertex();
			EndPrimitive();
		}
	}
	else if (outCurrentLifetime > 0.0)
	{
		EmitVertex();
		EndPrimitive();
	}
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