#shadertype: fragment
#version 450 core

in vec4 color;
in vec2 worldCoords;
in float lightIntensity;

in float fallofA;
in float fallofB;

flat in float primitiveType;

layout(location = 0) out vec4 lightColor;

void main()
{

	if(primitiveType == float(1 << 5))
	{
		float d = distance(vec2(0.0, 0.0), worldCoords * 50.0);

		float i = 1.0 / (1.0 + fallofA * d + fallofB * d * d);

		if(i < 0.01)
			i = 0.0;

		float final_light_intensity = i * lightIntensity;
		lightColor = vec4(vec3(final_light_intensity), 1.0) * color;
	}
	else if(primitiveType == float(1 << 6))
	{
		lightColor = vec4(0.0, 0.0, 0.0, 0.0); 
	}
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
in float aPrimitiveType;

out vec4 color;
out vec2 worldCoords;

out float fallofA;
out float fallofB;
out float lightIntensity;
flat out float primitiveType;

uniform mat4 u_ViewProjectionMatrix;

void main()
{
	gl_Position = u_ViewProjectionMatrix * vec4(aPosition, aP4);

	color = aColor;
	worldCoords = aTextureWorldCoords;

	fallofA = aP1;
	fallofB = aP2;
	lightIntensity = aP3;
	primitiveType = aPrimitiveType;
}
