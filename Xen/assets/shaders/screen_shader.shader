#shadertype: fragment
#version 450 core

in vec2 textureCoords;

layout(location = 0) out vec4 finalOutput;

uniform sampler2D u_UnlitSceneTexture;
uniform sampler2D u_SceneMaskTexture;
uniform sampler2D u_LightMapTexture;

uniform float u_AmbientLightIntensity;

void main()
{
	float light_intensity_on_free_space = 0.7;

	finalOutput = texture(u_UnlitSceneTexture, textureCoords) * texture(u_LightMapTexture, textureCoords);

	//if(texture(u_SceneMaskTexture, textureCoords) == vec4(1.0, 1.0, 1.0, 1.0))
	//	finalOutput = max(texture(u_UnlitSceneTexture, textureCoords) * texture(u_LightMapTexture, textureCoords), texture(u_UnlitSceneTexture, textureCoords) * u_AmbientLightIntensity);
	//else
	//	finalOutput = texture(u_UnlitSceneTexture, textureCoords) + texture(u_LightMapTexture, textureCoords) * light_intensity_on_free_space;


}

#shadertype: vertex
#version 450 core

out vec2 textureCoords;

const vec2 quad_verts[] = vec2[6](
	vec2( 1.0,  1.0),
	vec2(-1.0,  1.0),
	vec2(-1.0, -1.0),
	vec2( 1.0,  1.0),
	vec2(-1.0, -1.0),
	vec2( 1.0, -1.0)
);

const vec2 quad_tex_coords[] = vec2[6](
	vec2(1.0, 1.0),
	vec2(0.0, 1.0),
	vec2(0.0, 0.0),
	vec2(1.0, 1.0),
	vec2(0.0, 0.0),
	vec2(1.0, 0.0)
);

void main()
{
	gl_Position = vec4(quad_verts[gl_VertexID], 0.0, 1.0);
	textureCoords = quad_tex_coords[gl_VertexID];
}
