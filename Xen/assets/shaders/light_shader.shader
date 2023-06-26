#shadertype: fragment
#version 450 core

in vec4 color;
in vec2 worldCoords;

flat in float fallofA;
flat in float fallofB;
flat in float enttID;

layout(location = 0) out vec4 lightColor;

void main()
{
	float d = distance(vec2(0.0, 0.0), worldCoords * 5.0);

	float intensity = 1.0 / (1.0 + fallofA * d + fallofB * d * d);

	if(intensity < 0.01)
		intensity = 0.0;
	lightColor = vec4(1.0, 1.0, 1.0, intensity) * color;
}

#shadertype: vertex
#version 450 core

in vec3 aPosition;
in vec4 aColor;
in vec2 aTextureWorldCoords;

in float aP1;
in float aP2;
in float aP3;

out vec4 color;
out vec2 worldCoords;

flat out float fallofA;
flat out float fallofB;
flat out float enttID;

uniform mat4 u_ViewProjectionMatrix;

void main()
{
	gl_Position = u_ViewProjectionMatrix * vec4(aPosition, 1.0f);

	color = aColor;
	worldCoords = aTextureWorldCoords;

	fallofA = aP1;
	fallofB = aP2;
	enttID = aP3;
}
