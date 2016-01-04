#version 430 core

layout(location = 1) in vec2 in_uv;
layout(location = 0) in vec3 in_normal;

layout(location = 0) out vec4 out_color;

void main()
{
	out_color = vec4(0,1,0,1);
}