#shadertype: fragment
#version 460 core
in vec4 color;
flat in float TexSlot;
in vec2 TextureCoords;

out vec4 fragColor;

uniform sampler2D tex[32];

void main()
{
	int tex_slot = int(TexSlot);
	fragColor = texture(tex[tex_slot], TextureCoords) * color;
}


#shadertype: vertex
#version 460 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTextureCoords;
layout (location = 3) in float aTexSlot;

out vec4 color;
out vec2 TextureCoords;
flat out float TexSlot; // 'flat' To avoid Texture Flickering

uniform mat4 u_ViewProjectionMatrix;

void main()
{
	gl_Position = u_ViewProjectionMatrix * vec4(aPosition, 1.0f);
	TexSlot = aTexSlot;
	TextureCoords = aTextureCoords;
	color = aColor;
}