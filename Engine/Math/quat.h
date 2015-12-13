#pragma once
#include "vec4.h"

namespace Math
{
	class quat
	{
		float v[4];
	public:

		quat(float x, float y, float z, float w)
		{
			v[0] = x;
			v[1] = y;
			v[2] = z;
			v[3] = w;
		}
		quat(const vec4 &vector)
		{
			v[0] = vector.x;
			v[1] = vector.y;
			v[2] = vector.z;
			v[3] = vector.w;
		}

		~quat()
		{
		}

		quat operator*(const quat &r) const { return mul(r); }


		quat mul(const quat &q) const
		{
			
		}

	};
}
