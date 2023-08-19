#shadertype: fragment
#version 450 core

in vec4 color;
in vec2 TextureWorldCoords;

flat in float P1;
flat in float P2;
flat in float P3;
flat in float P4;
flat in float P5;
in flat uint PrimitiveType;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 maskColor;
layout(location = 2) out int entt_id;

uniform sampler2D tex[8];

void main()
{
	uint LINE			= 1 << 0;
	uint TRIANGLE		= 1 << 1;
	uint QUAD			= 1 << 2;
	uint POLYGON		= 1 << 3;
	uint CIRCLE		= 1 << 4;

	vec4 output_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);

	if (PrimitiveType == QUAD || PrimitiveType == TRIANGLE || PrimitiveType == POLYGON)
	{
		int tex_slot = int(P1);
		output_color = texture(tex[tex_slot], TextureWorldCoords) * color;
	}

	if (PrimitiveType == CIRCLE)
	{
		float circleThickness = P1;
		float circleInnerFade = P2;
		float circleOuterFade = P3;

		float dist = 1.0 - distance(vec2(0.0, 0.0), TextureWorldCoords);

		float outerFadeLayer = 0.0;
		float thicknessLayer = 1.0;
		float innerFadeLayer = 0.0;

		if (dist < 0.0)
			dist = 0.0;

		outerFadeLayer = smoothstep(0.0, circleOuterFade * circleThickness, dist);

		if (dist > circleThickness)
			thicknessLayer = 0.0;

		innerFadeLayer = smoothstep(circleThickness * (1.0 - circleInnerFade), circleThickness, dist);

		output_color = vec4(1.0, 1.0, 1.0, (1.0 - innerFadeLayer) * thicknessLayer * outerFadeLayer) * color;
	}

	fragColor = output_color;
	maskColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	entt_id = int(P5);
}

#shadertype: vertex
#version 450 core

layout(location = 0)in vec3 aPosition;
layout(location = 1)in vec4 aColor;
layout(location = 2)in vec2 aTextureWorldCoords;

layout(location = 3)in float aP1;
layout(location = 4)in float aP2;
layout(location = 5)in float aP3;
layout(location = 6)in float aP4;
layout(location = 7)in float aP5;
layout(location = 8)in uint aPrimitiveType;

out vec4 color;
out vec2 TextureWorldCoords;

flat out float P1;
flat out float P2;
flat out float P3;
flat out float P4;
flat out float P5;
out flat uint PrimitiveType;

uniform mat4 u_ViewProjectionMatrix;

void main()
{
	gl_Position = u_ViewProjectionMatrix * vec4(aPosition, 1.0f);

	color = aColor;
	TextureWorldCoords = aTextureWorldCoords;

	P1 = aP1;
	P2 = aP2;
	P3 = aP3;
	P4 = aP4;
	P5 = aP5;
	PrimitiveType = aPrimitiveType;
}
