#version 430 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;

uniform mat4 gWorld;
uniform mat4 gView;
uniform mat4 gProjection;

out vec4 color;

void main(void)
{
	color = in_color;

	mat4 wvp = gProjection * gView * gWorld;
	gl_Position = wvp * vec4(in_position, 1.0);
}