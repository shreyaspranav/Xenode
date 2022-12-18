#shadertype: fragment
#version 460 core
in vec4 color;
flat in float TexSlot;
in vec2 TextureCoords;

out vec4 fragColor;

uniform sampler2D tex[32];

void main()
{
	//int tex_slot = int(TexSlot);
	//fragColor = texture(tex[tex_slot], TextureCoords) * color;

	vec4 texColor = color;

	vec2 circle_frag_coords = vec2(0.0, 0.0);
	
	switch(int(TexSlot))
	{
		case -1: circle_frag_coords = TextureCoords; break;
		case  0: texColor *= texture(tex[ 0], TextureCoords); break;
		case  1: texColor *= texture(tex[ 1], TextureCoords); break;
		case  2: texColor *= texture(tex[ 2], TextureCoords); break;
		case  3: texColor *= texture(tex[ 3], TextureCoords); break;
		case  4: texColor *= texture(tex[ 4], TextureCoords); break;
		case  5: texColor *= texture(tex[ 5], TextureCoords); break;
		case  6: texColor *= texture(tex[ 6], TextureCoords); break;
		case  7: texColor *= texture(tex[ 7], TextureCoords); break;
		case  8: texColor *= texture(tex[ 8], TextureCoords); break;
		case  9: texColor *= texture(tex[ 9], TextureCoords); break;
		case 10: texColor *= texture(tex[10], TextureCoords); break;
		case 11: texColor *= texture(tex[11], TextureCoords); break;
		case 12: texColor *= texture(tex[12], TextureCoords); break;
		case 13: texColor *= texture(tex[13], TextureCoords); break;
		case 14: texColor *= texture(tex[14], TextureCoords); break;
		case 15: texColor *= texture(tex[15], TextureCoords); break;
		case 16: texColor *= texture(tex[16], TextureCoords); break;
		case 17: texColor *= texture(tex[17], TextureCoords); break;
		case 18: texColor *= texture(tex[18], TextureCoords); break;
		case 19: texColor *= texture(tex[19], TextureCoords); break;
		case 20: texColor *= texture(tex[20], TextureCoords); break;
		case 21: texColor *= texture(tex[21], TextureCoords); break;
		case 22: texColor *= texture(tex[22], TextureCoords); break;
		case 23: texColor *= texture(tex[23], TextureCoords); break;
		case 24: texColor *= texture(tex[24], TextureCoords); break;
		case 25: texColor *= texture(tex[25], TextureCoords); break;
		case 26: texColor *= texture(tex[26], TextureCoords); break;
		case 27: texColor *= texture(tex[27], TextureCoords); break;
		case 28: texColor *= texture(tex[28], TextureCoords); break;
		case 29: texColor *= texture(tex[29], TextureCoords); break;
		case 30: texColor *= texture(tex[30], TextureCoords); break;
		case 31: texColor *= texture(tex[31], TextureCoords); break; 
	}
	fragColor = texColor;
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

out mat4 view_projection;

void main()
{
	gl_Position = u_ViewProjectionMatrix * vec4(aPosition, 1.0f);
	TexSlot = aTexSlot;
	TextureCoords = aTextureCoords;
	color = aColor;

	view_projection = u_ViewProjectionMatrix;
}