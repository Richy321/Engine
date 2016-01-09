#version 430 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

layout(location = 0) out vec3 out_normal;
layout(location = 1) out vec2 out_uv;
layout(location = 2) out vec3 out_worldPos;

uniform mat4 gWorld;
uniform mat4 gView;
uniform mat4 gProjection;

void main(void)
{
	mat4 wvp = gProjection * gView * gWorld;

	out_normal = (gWorld * vec4(in_normal, 0.0)).xyz;
	out_uv = in_uv;
	out_worldPos = (gWorld * vec4(in_position, 1.0)).xyz;
	gl_Position = wvp * vec4(in_position, 1.0);
}