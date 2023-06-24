#version 330

layout(location = 0)in vec3 a_Position;
layout(location = 1)in vec2 a_TexCoord;

uniform mat4 vpMat;
uniform mat4 transMat;

out vec2 texCoord;

void main()
{
	texCoord = a_TexCoord;
	gl_Position = vpMat * transMat * vec4(a_Position.x, a_Position.y, a_Position.z, 1.0);
}
