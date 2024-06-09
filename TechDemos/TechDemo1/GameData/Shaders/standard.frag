#version 450

//shader input
layout (location = 0) in vec2 inUV;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 colour;
layout (location = 3) in vec3 fragPos;

//output write
layout (location = 0) out vec4 outFragColor;

//texture to access
layout(set =0, binding = 0) uniform sampler2D displayTexture;

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

struct DirectionalLight
{
	vec3 direction;
	float intensity;
	
	vec3 diffuseLight;
	vec3 specularLight;
};

layout(set=1, binding=0, std140) uniform  SceneData{   

	mat4 view;
	mat4 proj;
	mat4 viewproj;
	vec4 ambientColour;
		
	vec3 viewPos;
	
	int numberOfPointLights;
	PointLight pointLights[64];
	
	int numberOfDirLights;
	DirectionalLight directionalLights[4];
	
} sceneData;

vec3 CalculateAmbientLight()
{
    vec3 ambient = vec3(sceneData.ambientColour) * vec3(texture(displayTexture, inUV)) * sceneData.ambientColour.w;
	return ambient;
}

vec3 CalculatePointLight(PointLight light, vec3 viewDir)
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
    vec3 diffuse  = light.diffuseLight  * diff * vec3(texture(displayTexture, inUV));
    //vec3 specular = light.specularLight * spec * vec3(texture(material.specularTexture, texCoord));
    diffuse  *= attenuation;
    //specular *= attenuation;
    return (diffuse/* + specular*/);
}

vec3 CalculateDirLight(DirectionalLight light, vec3 viewDir)
{
    vec3 lightDir = normalize(vec3(light.direction) - fragPos);
    //diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    //specular shading, NYI
    //vec3 reflectDir = reflect(-lightDir, normal);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    //combine results

    vec3 diffuse = vec3(light.diffuseLight) * diff * vec3(texture(displayTexture, inUV)) * light.intensity;
    //vec3 specular = light.specularLight * spec * vec3(texture(material.specularTexture, i_TexCoord));

    return diffuse;
}

void main() 
{
    vec3 viewDir = normalize(sceneData.viewPos - fragPos);
	vec3 result = CalculateAmbientLight();
	
	for(int i = 0; i < sceneData.numberOfDirLights; i++)	
		result += CalculateDirLight(sceneData.directionalLights[i], viewDir);
	
	for(int i = 0; i < sceneData.numberOfPointLights; i++)	
		result += CalculatePointLight(sceneData.pointLights[i], viewDir);

	outFragColor = vec4(result, 1.0f) * colour;
}