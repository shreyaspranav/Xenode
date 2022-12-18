#shadertype: fragment
#version 460 core

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
#version 460 core

layout(location = 5) in vec2 aCircleQuadWorldCoords;

layout(location = 6) in vec3 aPosition;
layout(location = 7) in vec4 aColor;

layout(location = 8)  in float aThickness;
layout(location = 9)  in float aOuterFade;
layout(location = 10) in float aInnerFade;

out vec4 color;
out vec2 circleQuadWorldCoords;

uniform mat4 u_ViewProjectionMatrix;

void main()
{
	gl_Position = u_ViewProjectionMatrix * vec4(aPosition, 1.0f);
	circleQuadWorldCoords = aCircleQuadWorldCoords;
	color = aColor;
}