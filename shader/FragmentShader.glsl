#version 430 // change later



// Structures
// =============================================================================================================
struct Matrices
{
	mat4 mvp;
	mat4 mv;
	mat3 normal;
    mat4 bmvp;
};


struct Light
{
    vec3 lightPos;
};


struct FragmentInput
{
    vec3 normal;
    vec3 surfaceNormal;

	vec3 lightDir;
	vec3 viewDir;

    vec4 shadow_coordinates;
};

struct Normalized
{
    vec3 normal;
    vec3 lightDir;
    vec3 viewDir;
};

struct Textures
{
    sampler2DShadow shadowmap;
};
// =============================================================================================================




// Subroutines
// =============================================================================================================
subroutine void FragmentProgram();
// =============================================================================================================




// Variables
// =============================================================================================================
layout(location = 0) in FragmentInput Input;

uniform Matrices matrices;

uniform Light light;

uniform Textures tex;

subroutine uniform FragmentProgram fragmentprogram;

layout(location = 0) out vec4 out_color;
// =============================================================================================================




// Methods
// =============================================================================================================
void main()
{
	fragmentprogram();
}
// =============================================================================================================




// Subroutine Implementation
// =============================================================================================================
layout (index = 0) subroutine (FragmentProgram) void lambert()
{
    Normalized n;
    n.viewDir = normalize(Input.viewDir);
    n.lightDir = normalize(Input.lightDir);
    n.normal = normalize(Input.normal);

    float intensity = dot(n.lightDir, n.normal);


    vec3 shadow_coordinates = Input.shadow_coordinates.xyz;
//    shadow_coordinates.z -= (.0001f + (1.f - intensity) * .005f); // Needs to be adjusted

    float shadowsample = clamp(texture(tex.shadowmap, shadow_coordinates) + 0.25f, 0.f, 1.f);


    out_color = shadowsample * intensity * vec4(.7f);
    out_color.w = 1.f;
}


subroutine (FragmentProgram) void depthmap()
{
    out_color = vec4(gl_FragCoord.z);
}
// =============================================================================================================