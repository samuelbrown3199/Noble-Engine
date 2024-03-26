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
	VertexBuffer vertexBuffer;
} PushConstants;

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec3 normal;

void main()
{
	
	Vertex v = PushConstants.vertexBuffer.vertices[gl_VertexIndex];
	
	gl_Position = PushConstants.render_matrix *vec4(v.position, 1.0f);
	outUV.x = v.uv_x;
	outUV.y = v.uv_y;
	normal = v.normal;
}