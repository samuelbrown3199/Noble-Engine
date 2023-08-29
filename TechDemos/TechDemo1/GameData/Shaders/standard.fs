#version 330

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
uniform int numberOfPointLights;
uniform PointLight pointLights[20];

struct SpotLight
{
    vec3  position;
    vec3  direction;
    float cutOff;
	float outerCutOff;
	
	float constant;
    float linear;
    float quadratic;
	
	vec3 diffuseLight;
    vec3 specularLight;
};
uniform int numberOfSpotLights;
uniform SpotLight spotLights[20];

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

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 diffuse = light.diffuseLight * diff * vec3(texture(ourTexture, texCoord));
    //vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    diffuse *= attenuation * intensity;
    //specular *= attenuation * intensity;
    return (diffuse);
}

void main()
{
    vec3 viewDir = normalize(viewPos - fragPos);
	vec3 result = CalculateAmbientLight();
	//result += CalculateDirLight(dirLight, normal, viewDir); for the future
	
	for(int i = 0; i < numberOfPointLights; i++)	
		result += CalculatePointLight(pointLights[i], normal, fragPos, viewDir);
	
	for(int i = 0; i < numberOfSpotLights; i++)
		result += CalculateSpotLight(spotLights[i], normal, fragPos, viewDir);

	gl_FragColor = vec4(result, 1.0);
}