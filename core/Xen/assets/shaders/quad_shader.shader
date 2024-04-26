#shadertype: fragment
#version 140
in vec4 color;
flat in float TexSlot;
in vec2 TextureCoords;

out vec4 fragColor;

uniform sampler2D tex[8];

void main()
{
	int tex_slot = int(TexSlot);
	fragColor = texture(tex[tex_slot], TextureCoords) * color;

	//vec4 texColor = color;
	//
	//switch(int(TexSlot))
	//{
	//	case  0: texColor *= texture(tex[0], TextureCoords); break;
	//	case  1: texColor *= texture(tex[1], TextureCoords); break;
	//	case  2: texColor *= texture(tex[2], TextureCoords); break;
	//	case  3: texColor *= texture(tex[3], TextureCoords); break;
	//	case  4: texColor *= texture(tex[4], TextureCoords); break;
	//	case  5: texColor *= texture(tex[5], TextureCoords); break;
	//	case  6: texColor *= texture(tex[6], TextureCoords); break;
	//	case  7: texColor *= texture(tex[7], TextureCoords); break;
	//}
	//fragColor = texColor;
}

#shadertype: vertex
#version 140
in vec3 aQuadPosition;
in vec4 aQuadColor;
in vec2 aQuadTextureCoords;
in float aQuadTexSlot;

out vec4 color;
out vec2 TextureCoords;
flat out float TexSlot; // 'flat' To avoid Texture Flickering

uniform mat4 u_ViewProjectionMatrix;

out mat4 view_projection;

void main()
{
	gl_Position = u_ViewProjectionMatrix * vec4(aQuadPosition, 1.0f);
	TexSlot = aQuadTexSlot;
	TextureCoords = aQuadTextureCoords;
	color = aQuadColor;

	view_projection = u_ViewProjectionMatrix;
}