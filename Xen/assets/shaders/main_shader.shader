#shadertype: fragment
#version 450 core

in vec4 color;
in vec2 TextureWorldCoords;

flat in float P1;
flat in float P2;
flat in float P3;
flat in float P4;
flat in float P5;
flat in float PrimitiveType;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out int entt_id;

uniform sampler2D tex[8];

void main()
{
	const int LINE			= 1 << 0;
	const int TRIANGLE		= 1 << 1;
	const int QUAD			= 1 << 2;
	const int POLYGON		= 1 << 3;
	const int CIRCLE		= 1 << 4;

	int primitive_type = int(PrimitiveType);

	vec4 output_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);

	if (primitive_type == QUAD || primitive_type == TRIANGLE || primitive_type == POLYGON)
	{
		int tex_slot = int(P1);
		output_color = texture(tex[tex_slot], TextureWorldCoords) * color;
	}

	if (primitive_type == CIRCLE)
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
	entt_id = int(P5);
}

#shadertype: vertex
#version 450 core

in vec3 aPosition;
in vec4 aColor;
in vec2 aTextureWorldCoords;

in float aP1;
in float aP2;
in float aP3;
in float aP4;
in float aP5;
in float aPrimitiveType;

out vec4 color;
out vec2 TextureWorldCoords;

flat out float P1;
flat out float P2;
flat out float P3;
flat out float P4;
flat out float P5;
flat out float PrimitiveType;

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
