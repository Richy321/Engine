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
		quat operator*(float r) const { return quat(v[0] * r, v[1]*r, v[2]*r, v[3]*r); }
		quat &operator*=(const quat &r) { *this = mul(r); return *this; }
		quat mul(const quat &r) const
		{
			return quat(
				v[0] * r.v[3] + v[3] * r.v[0] + v[1] * r.v[2] - v[2] * r.v[1],
				v[1] * r.v[3] + v[3] * r.v[1] + v[2] * r.v[0] - v[0] * r.v[2],
				v[2] * r.v[3] + v[3] * r.v[2] + v[0] * r.v[1] - v[1] * r.v[0],
				v[3] * r.v[3] - v[0] * r.v[0] - v[1] * r.v[1] - v[2] * r.v[2]
				);
		}

		quat conjugate() const
		{
			return *this * quat(-1, -1, -1, 1);
		}
		
		vec4 rotate(const vec4 &r) const 
		{
			quat q = (*this * r) * conjugate();
			return vec4(q.v[0], q.v[1], q.v[2], q.v[3]);
		}

	};
}
