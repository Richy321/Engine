#pragma once

struct VertexFormat
{
	vec3 position;
	vec4 color;

	VertexFormat(const vec3 &pos, const vec4 &col)
	{
		position = pos;
		color = col;
	}
};
