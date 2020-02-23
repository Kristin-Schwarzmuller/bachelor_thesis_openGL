#version 410



// Structures
// =============================================================================================================
struct Matrices
{
	mat4 mvp;
	mat4 mv;
	mat3 normal;
};


struct Light
{
    vec3 lightPos;
};


struct VertexOutput
{
    vec3 normal;

	vec3 lightDir;
	vec3 viewDir;
};
// =============================================================================================================




// Subroutines
// =============================================================================================================
subroutine void VertexProgram();
// =============================================================================================================




// Variables
// =============================================================================================================
layout(location = 0) in vec4 vertex;
layout(location = 1) in vec3 normal;

uniform Matrices matrices;

uniform Light light;

subroutine uniform VertexProgram vertexprogram;

layout(location = 0) flat out VertexOutput Output;
// =============================================================================================================




// Methods
// =============================================================================================================
void main()
{
	vertexprogram();
}
// =============================================================================================================




// Subroutine Implementation
// =============================================================================================================
subroutine (VertexProgram) void verts_and_normals()
{
    gl_Position = matrices.mvp * vertex;

    Output.normal = matrices.normal * normal;

    vec4 h = matrices.mv * vertex;
    vec3 mvPos = h.xyz / h.w;

    Output.lightDir = light.lightPos - mvPos;
    Output.viewDir = -mvPos;
}
// =============================================================================================================