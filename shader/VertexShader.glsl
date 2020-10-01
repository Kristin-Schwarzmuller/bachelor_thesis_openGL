#version 410



// Structures
// =============================================================================================================
struct Matrices
{
	mat4 mvp;
	mat4 mv;
    mat4 mod;
	mat3 normal;
    mat4 bmvp;
};


struct Light
{
    vec3 lightPos;
	vec4 ambient;
	vec4 diffus;
	vec4 specular;
    float brightnessFactor;
};


struct VertexOutput
{
    vec3 normal;
    vec3 surfaceNormal;

	vec3 lightDir;
	vec3 viewDir;
    vec4 FragPos;

    vec4 shadow_coordinates;

    vec2 TexCoords;
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

layout(location = 0) out VertexOutput Output;
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
    Output.surfaceNormal = normal;

    vec4 h = matrices.mv * vertex;
    vec3 mvPos = h.xyz / h.w;
    // Here define if light is directional or point 
    Output.lightDir = normalize(light.lightPos - mvPos); 
    Output.FragPos = vec4(gl_Position.xyz / gl_Position.w, 1.0f);   
    //Output.FragPosNormalized = normalize((light.lightprojection_z_max - light.lightprojection_z_min) * (((gl_Position.xyz / gl_Position.w) - light.lightprojection_z_min) / (light.lightprojection_z_max - light.lightprojection_z_min)) + light.lightprojection_z_min);
    //Output.lightDir = light.lightPos;
    Output.viewDir = -mvPos;

    Output.shadow_coordinates = matrices.bmvp * vertex;
}

subroutine (VertexProgram) void simple_placement()
{
    gl_Position = matrices.mvp * vertex;
    gl_Position = vec4(gl_Position.xyz / gl_Position.w, 1.f);
}

subroutine (VertexProgram) void canvas_placement() 
{
    gl_Position = vec4(vertex.x, vertex.y, 0.0, 1.0); 
    Output.TexCoords = vertex.xy / 2 + 0.5;
}
// =============================================================================================================