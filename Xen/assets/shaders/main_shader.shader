#shadertype: fragment
#version 450 core

layout(location = 0) in vec4 color;
layout(location = 1) in vec2 TextureWorldCoords;
layout(location = 2) flat in float P1;
layout(location = 3) flat in float P2;
layout(location = 4) flat in float P3;
layout(location = 5) flat in float P4;
layout(location = 6) flat in float P5;
layout(location = 7) flat in uint PrimitiveType;
layout(location = 8) flat in int vertexID;


layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 maskColor;
layout(location = 2) out int entt_id;

uniform sampler2D tex[8];

void main()
{
	const int LINE				= 1;
	const int TRIANGLE			= 2;
	const int QUAD				= 4;
	const int POLYGON			= 8;
	const int CIRCLE			= 16;

	vec4 output_color = vec4(1.0f, 0.0f, 1.0f, 1.0f);

	if (PrimitiveType == QUAD || PrimitiveType == POLYGON)
	{
		int tex_slot = int(P1);
		output_color = texture(tex[tex_slot], TextureWorldCoords) * color;
	}
	
    if (PrimitiveType == TRIANGLE)
    {
        if ((TextureWorldCoords.y < TextureWorldCoords.x * 2.0) && TextureWorldCoords.y < 2.0 - TextureWorldCoords.x * 2.0)
			output_color = color;
		else
            output_color = vec4(0.0, 0.0, 0.0, 0.0);
    }

	if (PrimitiveType == CIRCLE)
	{
		float circleThickness = P1;
		float circleInnerFade = P2;
		float circleOuterFade = P3;

		float dist = 1.0 - distance(vec2(0.0, 0.0), TextureWorldCoords);

		float outerFadeLayer = 0.0;
		float thicknessLayer = 1.0;
		float innerFadeLayer = 0.0;
		
		if (dist < 0.0)
			dist = 0.0;

		outerFadeLayer = smoothstep(0.0, circleOuterFade * circleThickness, dist);

		if (dist > circleThickness)
			thicknessLayer = 0.0;

		innerFadeLayer = smoothstep(circleThickness * (1.0 - circleInnerFade), circleThickness, dist);

		output_color = vec4(1.0, 1.0, 1.0, (1.0 - innerFadeLayer) * thicknessLayer * outerFadeLayer) * color;
	}

	fragColor = output_color;
	maskColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	entt_id = vertexID;
}

#shadertype: vertex
#version 450 core

// Using arrays of floats instead of vector types because too lazy to calculate padding according to std430 layout
struct Vertex {
	float pos[3];
	float col[4];
	float texWorldCoord[2];
	
	uint primitiveT;
	
	float p1, p2, p3, p4, p5;
	
	int _verID;
};

int defalult_indices[6] = { 0, 1, 2, 0, 2, 3 };

layout(std430, binding = 1) readonly buffer VertexData		{ Vertex vertices[]; };
layout(std140, binding = 1) uniform PerFrameData			{ mat4 ViewProjectionMatrix; };

vec3 getPosition(int i) {
	return vec3(vertices[i].pos[0], 
		vertices[i].pos[1], 
		vertices[i].pos[2]);
}

vec4 getColor(int i) {
	return vec4(vertices[i].col[0], 
		vertices[i].col[1], 
		vertices[i].col[2], 
		vertices[i].col[3]);
}

vec2 getTextureWorldCoord(int i) {
	return vec2(vertices[i].texWorldCoord[0], 
		vertices[i].texWorldCoord[1]);
}

int getVertexIndex(int index) {
    return defalult_indices[index % 6] + ((index / 6) * 4);
}


layout(location = 0) out vec4 color;
layout(location = 1) out vec2 TextureWorldCoords;
layout(location = 2) flat out float P1;
layout(location = 3) flat out float P2;
layout(location = 4) flat out float P3;
layout(location = 5) flat out float P4;
layout(location = 6) flat out float P5;
layout(location = 7) flat out uint PrimitiveType;
layout(location = 8) flat out int vertexID;

void main()
{
    int vertexIndex = getVertexIndex(gl_VertexID);
	
    vec3 pos = getPosition(vertexIndex);
	gl_Position = ViewProjectionMatrix * vec4(pos, 1.0f);

    color = getColor(vertexIndex);
    TextureWorldCoords = getTextureWorldCoord(vertexIndex);

	P1 = vertices[vertexIndex].p1;
	P2 = vertices[vertexIndex].p2;
	P3 = vertices[vertexIndex].p3;
	P4 = vertices[vertexIndex].p4;
    P5 = vertices[vertexIndex].p5;
    PrimitiveType = vertices[vertexIndex].primitiveT;
    vertexID = vertices[vertexIndex]._verID;
}
