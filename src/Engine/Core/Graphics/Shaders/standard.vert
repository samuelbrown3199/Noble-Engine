#version 450
#extension GL_EXT_buffer_reference : require

struct Vertex {
	vec3 position;
	float uv_x;
	vec3 normal;
	float uv_y;
}; 

layout(buffer_reference, std430) readonly buffer VertexBuffer{ 
	Vertex vertices[];
};

//push constants block
layout( push_constant ) uniform constants
{	
	mat4 render_matrix;
	vec4 colour;
	VertexBuffer vertexBuffer;
} PushConstants;

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec4 colour;
layout (location = 3) out vec3 fragPos;

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

layout(set=1, binding=0, std140) uniform  SceneData{   

	mat4 view;
	mat4 proj;
	mat4 viewproj;
	vec4 ambientColor;
	vec4 sunlightDirection; //w for sun power
	vec4 sunlightColor;
		
	vec3 viewPos;
	
	int numberOfPointLights;
	PointLight pointLights[64];
} sceneData;

void main()
{
	Vertex v = PushConstants.vertexBuffer.vertices[gl_VertexIndex];
	
	gl_Position = sceneData.viewproj * PushConstants.render_matrix * vec4(v.position, 1.0f);
	outUV.x = v.uv_x;
	outUV.y = v.uv_y;
	normal = v.normal;
	colour = PushConstants.colour;
	fragPos = vec3(PushConstants.render_matrix * vec4(v.position, 1.0));
}