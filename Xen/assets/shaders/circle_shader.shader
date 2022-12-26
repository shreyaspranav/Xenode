#shadertype: fragment
#version 140 core

in vec4 color;
in vec2 circleQuadWorldCoords;

out vec4 fragColor;

void main()
{
	float dist = 1.0 - distance(vec2(0.0, 0.0), circleQuadWorldCoords);

	dist = smoothstep(0.0, 0.01, dist);

	fragColor = vec4(1.0, 1.0, 1.0, dist) * color;
}

#shadertype: vertex
#version 140 core

in vec2 aCircleQuadWorldCoords;

in vec3 aCirclePosition;
in vec4 aCircleColor;

in float aCircleThickness;
in float aCircleOuterFade;
in float aCircleInnerFade;

out vec4 color;
out vec2 circleQuadWorldCoords;

uniform mat4 u_ViewProjectionMatrix;

void main()
{
	gl_Position = u_ViewProjectionMatrix * vec4(aCirclePosition, 1.0f);
	circleQuadWorldCoords = aCircleQuadWorldCoords;
	color = aCircleColor;
}