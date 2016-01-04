#version 430 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

uniform mat4 gWorld;
uniform mat4 gView;
uniform mat4 gProjection;
uniform mat4 gWP;

out vec3 normal;
out vec2 uv;

void main(void)
{
	//propagate normal and uv
	normal = in_normal;
	uv = in_uv;

	mat4 wvp = gProjection * gView * gWorld;
	gl_Position = wvp * vec4(in_position, 1.0);
}