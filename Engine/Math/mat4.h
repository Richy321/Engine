#pragma once
#include "vec4.h"

namespace Math
{
	class mat4
	{
	private:
		vec4 v[4];
	public:
		mat4();
		mat4(const vec4 &x, const vec4 &y, const vec4 &z, const vec4 &w);
		~mat4();

		mat4 &Scale(float x, float y, float z)
		{
			v[0] *= x;
			v[1] *= y;
			v[2] *= z;
			return *this;
		}

		mat4& Translate()
		{
			return *this;
		}

		///Multiply by vector on the left - for use with row vectors
		vec4 PreMultiply(const vec4 &lhs) const
		{
			return vec4(
				lhs.x * v[0].x +	lhs.y *v[1].x +		lhs.z * v[2].x +	lhs.w * v[3].x,
				lhs.x * v[0].y +	lhs.y *v[1].y +		lhs.z * v[2].y +	lhs.w * v[3].y,
				lhs.x * v[0].z +	lhs.y *v[1].z +		lhs.z * v[2].z +	lhs.w * v[3].z,
				lhs.x * v[0].w +	lhs.y *v[1].w +		lhs.z * v[2].w +	lhs.w * v[3].w
				);
		}

		///Multiply by vector on the right - for use with column vectors
		vec4 PostMultiply(const vec4 &rhs) const
		{
			return vec4(
				v[0].Dot(rhs),
				v[1].Dot(rhs),
				v[2].Dot(rhs),
				v[3].Dot(rhs));
		}	

	};
}


