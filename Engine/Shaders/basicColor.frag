#version 430 core

//layout(location = 0) out vec4 out_color;

in vec4 color;
out vec4 out_color;

void main()
{
	out_color = color;
}