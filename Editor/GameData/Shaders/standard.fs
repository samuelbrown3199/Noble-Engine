#version 330

uniform vec4 colour;
uniform sampler2D ourTexture;

uniform vec3 ambientColour;
uniform float ambientStrength;

in vec2 texCoord;

vec4 CalculateAmbientLight()
{
	vec3 ambient = ambientStrength * ambientColour;
	vec4 returnAmbient = vec4(ambient, 1.0f);
	return returnAmbient;
}

void main()
{
	vec4 objectTexture = (texture(ourTexture, texCoord) * colour);
	gl_FragColor = objectTexture * CalculateAmbientLight();
}