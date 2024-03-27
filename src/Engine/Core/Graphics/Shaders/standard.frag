#version 450

//shader input
layout (location = 0) in vec2 inUV;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 colour;

//output write
layout (location = 0) out vec4 outFragColor;

//texture to access
layout(set =0, binding = 0) uniform sampler2D displayTexture;

void main() 
{
	//return red
	outFragColor = texture(displayTexture, inUV) * colour;
}