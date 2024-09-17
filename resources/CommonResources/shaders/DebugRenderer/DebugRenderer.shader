#shadertype: fragment
#version 450 core

// Primitives: As defined on the enum ------------------------------------
const uint QUAD		= 0;
const uint CIRCLE	= 1;

const uint TEXT		= 4;
// -----------------------------------------------------------------------

const float thicknessScale = 0.04;

layout(location = 0) in vec4 color;
layout(location = 1) in vec2 worldCoords;
layout(location = 2) flat in uint primitiveType;
layout(location = 3) flat in float P1;
layout(location = 4) flat in float P2;
layout(location = 5) flat in float P3;
layout(location = 6) flat in uint renderFromDebugCamera;

layout(binding = 1) uniform sampler2D fontAtlasTexture;

layout(location = 0) out vec4 fragColor;

void main()
{
	vec4 outColor = vec4(0.0, 0.0, 0.0, 0.0);

	if(primitiveType == QUAD)
	{
		outColor = color;

		// P1 -> thickness of the border
		// P2 -> Scale X
		// P3 -> Scale Y

		float thickness = P1 * thicknessScale;
		float scaleX = P2;
		float scaleY = P3;
		
		float x1 = scaleX - thickness, x2 = thickness - scaleX;
		float y1 = scaleY - thickness, y2 = thickness - scaleY;

		if (worldCoords.x < x1 && worldCoords.x > x2 && worldCoords.y < y1 && worldCoords.y > y2)
			outColor = vec4(0.0);
	}
	else if(primitiveType == CIRCLE)
	{
		// Technically this is implemented as an ellipse.
		
		// P1 -> thickness of the border
		// P2 -> Scale X
		// P3 -> Scale Y
	
		float thickness = P1 * thicknessScale;
		float scaleX = P2;
		float scaleY = P3;

		// lhs = (x^2 / a^2) + (y^2 / b^2)
		// rhs = 1
		// a = Scale X
		// b = Scale Y
		// 
		// Rearrange the equation to get these:
		// As the equation is rearranged, rhs = a^2 * b^2
		float lhs = (pow(scaleY, 2) * pow(worldCoords.x, 2)) + (pow(scaleX, 2) * pow(worldCoords.y, 2));
		float rhs = (pow(scaleX, 2) * pow(scaleY, 2));

		// Calcuate the 'a' and 'b' for the inner ellipse which will be cut out
		float aLimit = scaleX - thickness;
		float bLimit = scaleY - thickness;

		// Calculate the inner ellipse using the equation again.
		float lhsLimit = (pow(bLimit, 2) * pow(worldCoords.x, 2)) + (pow(aLimit, 2) * pow(worldCoords.y, 2));
		float rhsLimit = (pow(aLimit, 2) * pow(bLimit, 2));

		// Finally fill only outside the inner part but inside the outer part.
		if(lhs < rhs && lhsLimit > rhsLimit)
			outColor = color;
	}
	else if(primitiveType == TEXT)
	{
		// TODO: use parameters P1, P2, P3 for charecter spacing or something.

		// fontAtlasTexture is a single red channel texture(GL_RED), 
		// Convert it to greyscale image.
		vec4 greyScaleFontAtlasTexture = vec4(texture(fontAtlasTexture, worldCoords).r);

		// texture coords = worldCoords
		outColor = greyScaleFontAtlasTexture * color;
	}

	fragColor = outColor;
}

#shadertype: vertex
#version 450 core

// Struct that represents a vertex. 
// Using arrays instead of vector types becacuse I am too lazy to calculate the 
// paddings and offsets according to the std430 layout.
struct Vertex
{
	float position[3];
	float color[4];
	float worldCoords[2];
	
	uint primitiveType;

	float P1, P2, P3;
	bool renderFromDebugCamera;
};

int defaultIndices[] = { 0, 1, 2, 0, 2, 3 };

layout(std430, binding = 2) readonly buffer DebugVertexData		{ Vertex vertices[]; };

layout(std140, binding = 1) uniform PerFrameData				{ mat4 viewProjectionMatrix; };
layout(std140, binding = 4) uniform DebugPerFrameData			{ mat4 debugViewProjectionMatrix; };

vec3 GetPosition(int i)
{
	return vec3(
		vertices[i].position[0],
		vertices[i].position[1],
		vertices[i].position[2]
	);
}

vec4 GetColor(int i)
{
	return vec4(
		vertices[i].color[0],
		vertices[i].color[1],
		vertices[i].color[2],
		vertices[i].color[3]
	);
}

vec2 GetWorldCoords(int i)
{
	return vec2(
		vertices[i].worldCoords[0],
		vertices[i].worldCoords[1]
	);
}

int GetVertexIndex(int i)
{
	// If the indices are   0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10; ...
	// the output should be 0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 7, ...

	return defaultIndices[i % 6] + (i / 6) * 4;
}

layout(location = 0) out vec4  color;
layout(location = 1) out vec2  worldCoords;
layout(location = 2) flat out uint  primitiveType;
layout(location = 3) flat out float P1;
layout(location = 4) flat out float P2;
layout(location = 5) flat out float P3;
layout(location = 6) flat out uint  renderFromDebugCamera;

void main()
{
	int vertexIndex = GetVertexIndex(gl_VertexID);

	if(vertices[vertexIndex].renderFromDebugCamera)
		gl_Position = debugViewProjectionMatrix * vec4(GetPosition(vertexIndex), 1.0);
	else
		gl_Position = viewProjectionMatrix * vec4(GetPosition(vertexIndex), 1.0);
	

	color = GetColor(vertexIndex);
	worldCoords = GetWorldCoords(vertexIndex);
	primitiveType = vertices[vertexIndex].primitiveType;

	P1 = vertices[vertexIndex].P1;
	P2 = vertices[vertexIndex].P2;
	P3 = vertices[vertexIndex].P3;

	renderFromDebugCamera = uint(vertices[vertexIndex].renderFromDebugCamera);
}

