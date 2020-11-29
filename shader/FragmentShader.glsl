#version 430 // change later



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

    float near_cp;
    float far_cp;
};

struct Material 
{
	vec4 ambient;
	vec4 diffus;
	vec4 specular;
	vec4 emissiv;
	float shininess;
};


struct FragmentInput
{
    vec3 normal;
	vec3 lightDir;
	vec3 viewDir;
    vec4 FragPos;

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
    sampler2D canvas;
    //sampler2DMS canvas; 
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
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec4 out_sc;
layout(location = 3) out vec4 out_depth;
layout(location = 4) out vec4 out_rgbd;
// =============================================================================================================



// Methods
// =============================================================================================================
float getZ_NDC(float contrast)
{
    return contrast * gl_FragCoord.z - (contrast - 1.f);
}

vec4 getDepth()
{
    return vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1.0f);
}

vec4 getDepthContrast(float contrast)
{
    return vec4(getZ_NDC(contrast));
}

vec4 getLinDepth(float contrast)
{
    float z_ndc = getZ_NDC(contrast);
    float linearDepth = (2.0 * light.near_cp * light.far_cp) / (light.far_cp + light.near_cp - z_ndc * (light.far_cp - light.near_cp));
    return vec4(linearDepth);
}



void main()
{
	fragmentprogram();
}
// =============================================================================================================




// Subroutine Implementation
// =============================================================================================================
//layout (index = 0) subroutine (FragmentProgram) void lambert()
//{
//    Normalized n;
//    n.viewDir = normalize(Input.viewDir);
//    n.lightDir = normalize(Input.lightDir);
//    n.normal = normalize(Input.normal);
//
//    float intensity = dot(n.lightDir, n.normal);
//
//
//    vec3 shadow_coordinates = Input.shadow_coordinates.xyz;
//    float shadowsample = clamp(texture(tex.shadowmap, shadow_coordinates), .35f, 1.f);
//
//
//    out_color = shadowsample * intensity * vec4(.7f);
//    out_color.w = 1.f;
//}


layout (index = 1) subroutine (FragmentProgram) void depthmap()
{
    out_color = vec4(gl_FragCoord.z); 
}

//
//layout (index = 2) subroutine (FragmentProgram) void phong()
//{
//  
//    Normalized n;
//    n.viewDir = normalize(Input.viewDir);
//    n.lightDir = normalize(Input.lightDir);
//    n.normal = normalize(Input.normal);
//
//    // Ambient light
//    vec4 ambient = light.ambient * material.ambient; 
//
//    // Diffuse light
//	float d = dot(n.normal, n.lightDir);
//	vec4 diffus = d * light.diffus * material.diffus;
//
//    // Specular
//	vec4 specular = vec4(0.f, 0.f, 0.f, 1.f);
//	if (d > 0.f) {
//		vec3 r = reflect(-n.lightDir, n.normal);
//		specular = pow(max(dot(normalize (r), n.normal), 0), material.shininess) * light.specular * material.spekular;
//	}
//	
//	out_color = ambient  + specular + diffus;
//}


layout (index = 3) subroutine (FragmentProgram) void phongWithLambert()
{
  
    Normalized n;
    n.viewDir = normalize(Input.viewDir); // view space
    n.lightDir = normalize(Input.lightDir); //world space
    n.normal = normalize(Input.normal); // view space

    // --------- Phong ---------

    // Ambient light
    vec4 ambient = light.ambient * material.ambient; 

    // Diffuse light
	float d = max(dot(n.normal, n.lightDir), 0.0f);
	vec4 diffus = d * light.diffus * material.diffus;

    // Specular
	vec4 specular = vec4(0.f, 0.f, 0.f, 1.f);
	if (d > 0)// -0.001f)
    {
		vec3 r = reflect(-n.lightDir, n.normal);
		specular = pow(max(dot(normalize(r), n.viewDir), 0.f), material.shininess) * light.specular * material.specular;
	}
	
    // --------- Shadows ---------
    vec3 shadow_coordinates = Input.shadow_coordinates.xyz / Input.shadow_coordinates.w;
    float min_shadow_darkness = 1.f, max_shadow_darkness = .0f; //.35f;

    float shadowsample = clamp(texture(tex.shadowmap, shadow_coordinates), max_shadow_darkness, min_shadow_darkness);


    // --------- Result --------- 

    out_color =(shadowsample * (specular + diffus) + ambient);
    //out_color =(shadowsample * + ambient);
    out_color.a = 1.f;

    //if (gl_FragCoord.z >0.9999 )
    //    out_color = vec4(1, 0, 0, 1);

    out_normal = n.normal * .5f + .5f;
    // Depth
    out_depth = getDepth();

    //out_lin_depth = getLinDepth(2.0);

    //out_depth_contrast = getDepthContrast(light.far_cp);
    //out_lin_depth_contrast = getLinDepth(light.far_cp);

    // RGBD
    out_rgbd = out_color;
    out_rgbd.w = out_depth.x;

    // Shadow Candidate
    if (shadowsample <= .8f)
        out_sc = vec4(1.f);
    else
        out_sc = vec4(0.f);
}


layout (index = 4) subroutine (FragmentProgram) void canvas_display()
{
    out_color = texture(tex.canvas, Input.TexCoords);

     //ivec2 st = ivec2(Input.TexCoords * vec2(1280, 720));
     //out_color = texelFetch(tex.canvas,st,gl_SampleID);


    //texelFetch requires a vec of ints for indexing (since we're indexing pixel locations)
	//texture coords is range [0, 1], we need range [0, viewport_dim].
	//texture coords are essentially a percentage, so we can multiply text coords by total size 

	//ivec2 vpCoords = ivec2(1280, 720);
	//vpCoords.x = int(vpCoords.x * Input.TexCoords.x); 
	//vpCoords.y = int(vpCoords.y * Input.TexCoords.y);
 //   //vpCoords = ivec2(Input.TexCoords);

	////do a simple average since this is just a demo
 //   vec4 sampleSum = vec4(0);
 //   for( int i = 0; i < 8; ++i) 
 //   {
 //       sampleSum += texelFetch(tex.canvas, vpCoords, i);
 //   }
	//out_color = sampleSum / 8.0f;
}

layout(index = 5) subroutine(FragmentProgram) void red()
{

    out_color = vec4(1, 0, 0, .0f);
}
// =============================================================================================================