#version 450

//shader input
layout (location = 0) in vec2 inUV;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 colour;

//output write
layout (location = 0) out vec4 outFragColor;

//texture to access
layout(set =0, binding = 0) uniform sampler2D displayTexture;

layout(set=1, binding=0) uniform  SceneData{   

	mat4 view;
	mat4 proj;
	mat4 viewproj;
	vec4 ambientColor;
	vec4 sunlightDirection; //w for sun power
	vec4 sunlightColor;
} sceneData;

void main() 
{
	//return red
	outFragColor = texture(displayTexture, inUV) * colour;
}