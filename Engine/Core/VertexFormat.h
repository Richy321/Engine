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


struct VertexPositionNormalTextured
{
	vec3 position;
	vec3 normal;
	vec2 uv;

	VertexPositionNormalTextured(const vec3 &pos, const vec3 &norm, const vec2 texCoords)
	{
		position = pos;
		normal = norm;
		uv = texCoords;
	}
};
