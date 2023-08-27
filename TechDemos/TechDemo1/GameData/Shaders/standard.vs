#version 330

layout(location = 0)in vec3 a_Position;
layout(location = 1)in vec3 a_Normals;
layout(location = 2)in vec2 a_TexCoord;

uniform mat4 vpMat;
uniform mat4 transMat;

out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;

void main()
{
	texCoord = a_TexCoord;
	normal = mat3(transMat) * a_Normals;
	fragPos = vec3(transMat * vec4(a_Position, 1.0));
	
	gl_Position = vpMat * transMat * vec4(a_Position.x, a_Position.y, a_Position.z, 1.0);
}
