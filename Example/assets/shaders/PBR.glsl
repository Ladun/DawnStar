#type vertex
#version 140
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_separate_shader_objects : require
#extension GL_ARB_shading_language_420pack : require

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;

layout (std140, binding = 0) uniform Camera
{
    mat4 u_View;
    mat4 u_Projection;
    mat4 u_ViewProjection;

    vec3 u_CameraPosition;
};

uniform mat4 u_Model;

// struct VertexOutput
// {
// 	vec3 Normal;
// 	vec2 TexCoord;
// 	mat3 WorldNormals;
// 	mat3 WorldTransform;
// 	vec3 Binormal;
// };

// layout(location = 0) out VertexOutput Output;

out vec3 InNormal;
out vec2 InTexCoord;
out mat3 InWorldNormals;
out mat3 InWorldTransform;
out vec3 InBinormal;

void main()
{
    // Output.TexCoord = a_TexCoord;
    // Output.Normal = mat3(u_Model) * a_Normal;
	// Output.WorldNormals = mat3(u_Model) * mat3(a_Tangent, a_Bitangent, a_Normal);
	// Output.WorldTransform = mat3(u_Model);
    InTexCoord = a_TexCoord;
    InNormal = mat3(u_Model) * a_Normal;
	InWorldNormals = mat3(u_Model) * mat3(a_Tangent, a_Bitangent, a_Normal);
	InWorldTransform = mat3(u_Model);

	gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
}

#type fragment
#version 140
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_separate_shader_objects : require

const float PI = 3.141592653589793;
const float EPSILON = 0.000000000000001;

struct Properties
{
	vec4 AlbedoColor;
	float Roughness01;
	float Metalness01;
	vec3 EmissiveColor;
	float EmissiveIntensity;
	
	sampler2D AlbedoMap;
	sampler2D NormalMap;
	sampler2D MRAMap;
	sampler2D EmissiveMap;

	bool UseNormalMap;
};

uniform Properties u_Material;

// struct VertexOutput
// {
// 	vec3 Normal;
// 	vec2 TexCoord;
// 	mat3 WorldNormals;
// 	mat3 WorldTransform;
// 	vec3 Binormal;
// };
 
// layout(location = 0) in VertexOutput Input;
in vec3 InNormal;
in vec2 InTexCoord;
in mat3 InWorldNormals;
in mat3 InWorldTransform;
in vec3 InBinormal;

// =========================================
layout(location = 0) out vec4 o_Albedo;
layout(location = 1) out vec2 o_Normal;
layout(location = 2) out vec4 o_MetallicRoughnessAO;
layout(location = 3) out vec4 o_Emission;

vec3 GetNormalFromMap()
{
    vec3 tangentNormal = texture(u_Material.NormalMap, InTexCoord).rgb * 2.0 - 1.0;
    return normalize(InWorldNormals * tangentNormal);
}

//////////////////////////////////////////////////////////////////////////////////////////////
///// Octahedron-normal vectors //////////////////////////////////////////////////////////////
//// https://knarkowicz.wordpress.com/2014/04/16/octahedron-normal-vector-encoding/ //////////
//////////////////////////////////////////////////////////////////////////////////////////////
vec2 signNotZero(vec2 v)
{
	return vec2((v.x >= 0.0) ? +1.0 : -1.0, (v.y >= 0.0) ? +1.0 : -1.0);
}

vec2 Encode(in vec3 v)
{
	vec2 p = v.xy * (1.0 / (abs(v.x) + abs(v.y) + abs(v.z)));
	return (v.z <= 0.0) ? ((1.0 - abs(p.yx)) * signNotZero(p)) : p;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//// Entry Point /////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
    vec4 albedoWithAlpha = pow(texture(u_Material.AlbedoMap, InTexCoord) * u_Material.AlbedoColor, vec4(2.2, 2.2, 2.2, 1.0));
	if (albedoWithAlpha.a < 0.01)
		discard;

    vec3 outAlbedo	= albedoWithAlpha.rgb;
    vec3 outNormal = u_Material.UseNormalMap ? GetNormalFromMap() : normalize(InNormal);
    vec3 outEmission = texture(u_Material.EmissiveMap, InTexCoord).rgb * u_Material.EmissiveColor;

	float outRoughness = u_Material.Roughness01 * texture(u_Material.MRAMap, InTexCoord).g;
	outRoughness = max(outRoughness, 0.05); // Minimum roughness of 0.05 to keep specular highlight
	
	float outMetalness = u_Material.Metalness01 * texture(u_Material.MRAMap, InTexCoord).r;
	float outAO = texture(u_Material.MRAMap, InTexCoord).b;

    o_Albedo = vec4(outAlbedo, 1.0);
	o_Normal = Encode(outNormal);
	o_MetallicRoughnessAO = vec4(outMetalness, outRoughness, outAO, 1.0);
	o_Emission = vec4(outEmission, u_Material.EmissiveIntensity / 255);
}