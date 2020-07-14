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
	vec4 ambient;
	vec4 diffus;
	vec4 specular;
    float brightnessFactor;
};

struct Material 
{
	vec4 ambient;
	vec4 diffus;
	vec4 spekular;
	vec4 emissiv;
	float shininess;
};


struct FragmentInput
{
    vec3 normal;
    vec3 surfaceNormal;

	vec3 lightDir;
	vec3 viewDir;

    vec4 shadow_coordinates;

    vec2 TexCoords;
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
    //sampler2D canvas;
    sampler2DMS canvas; 
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
uniform Material material;

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

    float shadowsample = clamp(texture(tex.shadowmap, shadow_coordinates), .35f, 1.f);


    //out_color = vec4(1); // just white
    out_color = shadowsample * intensity * vec4(.7f);
    out_color.w = 1.f;
}


layout (index = 1) subroutine (FragmentProgram) void depthmap()
{
    out_color = vec4(gl_FragCoord.z);
}

layout (index = 2) subroutine (FragmentProgram) void phong()
{
  
    Normalized n;
    n.viewDir = normalize(Input.viewDir);
    n.lightDir = normalize(Input.lightDir);
    n.normal = normalize(Input.normal);

    // Ambient light
    vec4 ambient = light.ambient * material.ambient; 

    // Diffuse light
	float d = dot(n.normal, n.lightDir);
	vec4 diffus = d * light.diffus * material.diffus;

    // Specular
	vec4 specular = vec4(0.f, 0.f, 0.f, 1.f);
	if (d > 0.f) {
		vec3 r = reflect(-n.lightDir, n.normal);
		specular = pow(max(dot(normalize (r), n.normal), 0), material.shininess) * light.specular * material.spekular;
	}
	
	out_color = ambient  + specular + diffus;
}

layout (index = 3) subroutine (FragmentProgram) void phongWithLambert()
{
  
    Normalized n;
    n.viewDir = normalize(Input.viewDir);
    n.lightDir = normalize(Input.lightDir);
    n.normal = normalize(Input.normal);

    // --------- Phong ---------

    // Ambient light
    vec4 ambient = light.ambient * material.ambient; 

    // Diffuse light
	float d = dot(n.normal, n.lightDir);
	vec4 diffus = d * light.diffus * material.diffus;

    // Specular
	vec4 specular = vec4(0.f, 0.f, 0.f, 1.f);
	if (d > 0.f) {
		vec3 r = reflect(-n.lightDir, n.normal);
		specular = pow(max(dot(normalize (r), n.normal), 0), material.shininess) * light.specular * material.spekular;
	}
	
	//vec4 out_phong = ambient  + specular + diffus;

    // --------- Shadows ---------
    
    float intensity = dot(n.lightDir, n.normal);

    vec3 shadow_coordinates = Input.shadow_coordinates.xyz;
//    shadow_coordinates.z -= (.0001f + (1.f - intensity) * .005f); // Needs to be adjusted

    float shadowsample = clamp(texture(tex.shadowmap, shadow_coordinates), .35f, 1.f);
    
    //vec4 out_shadow = shadowsample * intensity * vec4(.7f);
    //out_shadow.w = 1.f;

    // --------- Result --------- 

    out_color = light.brightnessFactor * (shadowsample * intensity * 1.3f * (specular + diffus) + ambient);
    out_color.w = 1.f;
}

layout (index = 4) subroutine (FragmentProgram) void canvas_display()
{
      //out_color = texture(tex.canvas, Input.TexCoords);

      //ivec2 st = ivec2(Input.TexCoords * vec2(1280, 720));
     //out_color = texelFetch(tex.canvas,st,gl_SampleID);


    //texelFetch requires a vec of ints for indexing (since we're indexing pixel locations)
	//texture coords is range [0, 1], we need range [0, viewport_dim].
	//texture coords are essentially a percentage, so we can multiply text coords by total size 
	ivec2 vpCoords = ivec2(1280, 720);
	vpCoords.x = int(vpCoords.x * Input.TexCoords.x); 
	vpCoords.y = int(vpCoords.y * Input.TexCoords.y);
    //vpCoords = ivec2(Input.TexCoords);

	//do a simple average since this is just a demo
	vec4 sample1 = texelFetch(tex.canvas, vpCoords, 0);
	vec4 sample2 = texelFetch(tex.canvas, vpCoords, 1);
	vec4 sample3 = texelFetch(tex.canvas, vpCoords, 2);
	vec4 sample4 = texelFetch(tex.canvas, vpCoords, 3);
	out_color = (sample1);//+ sample2 + sample3 + sample4) / 4.0f;
}

// =============================================================================================================