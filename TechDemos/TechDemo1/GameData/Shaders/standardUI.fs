#version 330

uniform sampler2D uiTexture;
in vec2 i_TexCoord;

void main()
{
	vec3 tex = vec3(texture(uiTexture, i_TexCoord));
	gl_FragColor = vec4(tex, 1.0);
}