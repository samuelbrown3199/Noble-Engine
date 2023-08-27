#version 330
#extension GL_ARB_separate_shader_objects : enable

uniform vec4 colour;
uniform sampler2D ourTexture;

uniform vec3 ambientColour;
uniform float ambientStrength;

struct PointLight
{
    vec3 position;

    vec3 diffuseLight;
    vec3 specularLight;

    float constant;
    float linear;
    float quadratic;
    float intensity;
};
uniform int numberOfLights;
uniform PointLight lights[20];

uniform vec3 viewPos;

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

vec3 CalculateAmbientLight()
{
	vec3 ambient = ambientColour * vec3(texture(ourTexture, texCoord)) * ambientStrength;
	return ambient;
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 diffuse  = light.diffuseLight  * diff * vec3(texture(ourTexture, texCoord));
    //vec3 specular = light.specularLight * spec * vec3(texture(material.specularTexture, texCoord));
    diffuse  *= attenuation;
    //specular *= attenuation;
    return (diffuse/* + specular*/);
}

void main()
{
    vec3 viewDir = normalize(viewPos - fragPos);
	vec3 result = CalculateAmbientLight();
	//result += CalculateDirLight(dirLight, normal, viewDir); for the future
	
	for(int i = 0; i < numberOfLights; i++)
	{	
		result += CalculatePointLight(lights[i], normal, fragPos, viewDir);
	}

	gl_FragColor = vec4(result, 1.0);
}