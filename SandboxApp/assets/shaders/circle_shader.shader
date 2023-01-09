#shadertype: fragment
#version 140

in vec4 color;
in vec2 circleQuadWorldCoords;

in float circleThickness;
in float circleOuterFade;
in float circleInnerFade;

out vec4 fragColor;

void main()
{
	// Calculate distance and fill circle with white

	float dist = 1.0 - distance(vec2(0.0, 0.0), circleQuadWorldCoords);

	float outerFadeLayer = 0.0;
	float thicknessLayer = 1.0;
	float innerFadeLayer = 0.0;

	if (dist < 0.0)
		dist = 0.0;

	outerFadeLayer = smoothstep(0.0, circleOuterFade * circleThickness, dist);

	if (dist > circleThickness)
		thicknessLayer = 0.0;

	innerFadeLayer = smoothstep(circleThickness * (1.0 - circleInnerFade), circleThickness, dist);

	fragColor = vec4(1.0, 1.0, 1.0, (1.0 - innerFadeLayer) * thicknessLayer * outerFadeLayer) * color;
}

#shadertype: vertex
#version 140

in vec2 aCircleQuadWorldCoords;

in vec3 aCirclePosition;
in vec4 aCircleColor;

in float aCircleThickness;
in float aCircleOuterFade;
in float aCircleInnerFade;

out vec4 color;
out vec2 circleQuadWorldCoords;

out float circleThickness;
out float circleOuterFade;
out float circleInnerFade;

uniform mat4 u_ViewProjectionMatrix;

void main()
{
	gl_Position = u_ViewProjectionMatrix * vec4(aCirclePosition, 1.0f);
	circleQuadWorldCoords = aCircleQuadWorldCoords;
	color = aCircleColor;

	circleThickness = aCircleThickness;
	circleOuterFade	= aCircleOuterFade;
	circleInnerFade	= aCircleInnerFade;
}