#version 330

layout(location = 0)in vec3 a_Position;
layout(location = 1)in vec2 a_TexCoords;
layout(location = 2)in vec3 a_Normal;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

out vec2 i_TexCoord;
out vec3 i_Normal;
out vec3 i_FragPos;
out mat4 i_ViewMat;

void main()
{
	i_TexCoord = a_TexCoords;
	i_Normal = mat3(u_Model) * a_Normal;
	i_FragPos = vec3(u_Model * vec4(a_Position, 1.0));
	i_ViewMat = u_View;

	gl_Position = u_View * u_Model * vec4(a_Position, 1.0);
}
