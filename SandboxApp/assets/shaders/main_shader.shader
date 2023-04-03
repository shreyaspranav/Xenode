#shadertype: fragment
#version 140

in vec4 color;
in vec2 TextureWorldCoords;

flat in float P1;
flat in float P2;
flat in float P3;
flat in float P4;
flat in float P5;
flat in float PrimitiveType;

out vec4 fragColor;

uniform sampler2D tex[8];

void main()
{
	//int tex_slot = int(TexSlot);
	fragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

#shadertype: vertex
#version 140

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
