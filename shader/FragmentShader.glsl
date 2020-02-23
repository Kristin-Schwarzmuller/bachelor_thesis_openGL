#version 430 // change later



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


struct FragmentInput
{
    vec3 normal;

	vec3 lightDir;
	vec3 viewDir;
};

struct Normalized
{
    vec3 normal;
    vec3 lightDir;
    vec3 viewDir;
};
// =============================================================================================================




// Subroutines
// =============================================================================================================
subroutine void FragmentProgram();
// =============================================================================================================




// Variables
// =============================================================================================================
layout(location = 0) flat in FragmentInput Input;

uniform Matrices matrices;

uniform Light light;

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
layout (index = 0) subroutine (FragmentProgram) void toon()
{
    Normalized n;
    n.viewDir = normalize(Input.viewDir);
    n.lightDir = normalize(Input.lightDir);
    n.normal = normalize(Input.normal);

    float intensity = dot(n.lightDir, n.normal);

    vec4 toon = vec4(1.f);

    if(abs(intensity) > .9f)
        toon = vec4(1.f);

    else if(abs(intensity) > .8f)
        toon = vec4(.8f);

    else if(abs(intensity) > .7f)
        toon = vec4(.7f);

    else if(abs(intensity) > .6f)
        toon = vec4(.6f);

    else if(abs(intensity) > .5f)
        toon = vec4(.5f);

    else if(intensity > .4f)
        toon = vec4(.4f);

    else if(intensity > .3f)
        toon = vec4(.3f);

    else
        toon = vec4(.2f);

    toon.w = 1.f;


    out_color = toon * vec4((Input.normal * .5f) + .5f, 1);
}
// =============================================================================================================