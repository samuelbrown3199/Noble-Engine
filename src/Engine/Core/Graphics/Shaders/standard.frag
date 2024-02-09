/*#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 normal;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(texture(texSampler, fragTexCoord).rgb, 1.0);
}
*/
#version 450

//shader input
layout (location = 0) in vec3 inColor;
//layout (location = 2) in vec3 normal;

//output write
layout (location = 0) out vec4 outFragColor;

void main() 
{
	//return red
	outFragColor = vec4(inColor, 1.0f);
}