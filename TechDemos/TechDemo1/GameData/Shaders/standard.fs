#version 330

uniform vec4 colour;
uniform sampler2D ourTexture;

uniform vec3 ambientColour;
uniform float ambientStrength;

in vec2 texCoord;

vec4 CalculateAmbientLight()
{
	vec4 ambient = ambientStrength * vec4(ambientColour, 1.0f);
	return ambient;
}

void main()
{
	vec4 objectTexture = (texture(ourTexture, texCoord) * colour);
	gl_FragColor = objectTexture * CalculateAmbientLight();
}