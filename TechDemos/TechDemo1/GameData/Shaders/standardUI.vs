#version 330
layout(location = 0)in vec3 a_Position;
layout(location = 1)in vec2 a_TexCoords;

uniform mat4 u_UIPos;
uniform mat4 u_Ortho;

out vec2 i_TexCoord;

void main()
{
	i_TexCoord = a_TexCoords;
	gl_Position = u_Ortho * u_UIPos * vec4(a_Position, 1.0);
}