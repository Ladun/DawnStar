// Basic Texture Shader

#type vertex
#version 140
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_shading_language_420pack : require

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

layout (std140, binding = 0) uniform Camera
{
    mat4 u_View;
    mat4 u_Projection;
    mat4 u_ViewProjection;

    vec3 u_CameraPosition;
};

out vec4 v_Color;

void main()
{
	v_Color = a_Color;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 140
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_shading_language_420pack : require

layout(location = 0) out vec4 color;

in vec4 v_Color;

void main()
{
	color = v_Color;
}