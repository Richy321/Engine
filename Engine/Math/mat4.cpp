#include "mat4.h"

namespace Math
{

	mat4::mat4()
	{
		//identity matix
		v[0] = vec4(1, 0, 0, 0);
		v[1] = vec4(0, 1, 0, 0);
		v[2] = vec4(0, 0, 1, 0);
		v[3] = vec4(0, 0, 0, 1);
	}

	mat4::mat4(const vec4 &x, const vec4 &y, const vec4 &z, const vec4 &w)
	{
		v[0] = x;
		v[1] = y;
		v[2] = z;
		v[3] = w;
	}

	mat4::~mat4()
	{
	}


}
