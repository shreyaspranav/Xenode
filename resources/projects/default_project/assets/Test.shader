#shadertype: vertex
#version 450

layout(location = 0) in vec3 inPosition; // Input vertex position
layout(location = 1) in vec3 inColor;   // Input vertex color

layout(location = 0) out vec3 fragColor; // Output color to fragment shader

layout(set = 0, binding = 0) uniform MVP {
    mat4 modelViewProjection;  // Model-View-Projection matrix
} mvp;

void main() {
    gl_Position = mvp.modelViewProjection * vec4(inPosition, 1.0);
    fragColor = inColor; // Pass color to fragment shader
}

#shadertype: fragment

#version 450

layout(location = 0) in vec3 fragColor; // Color passed from vertex shader

layout(location = 0) out vec4 outColor; // Output color

void main() {
    outColor = vec4(fragColor, 1.0); // Set the final color
}

