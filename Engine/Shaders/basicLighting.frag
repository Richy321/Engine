#version 430 core

layout(location = 0) in vec3 in_normal;
layout(location = 1) in vec2 in_uv;

layout(location = 0) out vec4 out_color;


uniform sampler2D gColorMap; 

void main()
{
	vec3 normal = normalize(in_normal);
	vec4 totalLight = vec4(1.0, 1.0, 1.0, 1.0);

	out_color = texture(gColorMap, in_uv.xy);// * totalLight;
}