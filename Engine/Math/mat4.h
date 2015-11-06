#pragma once
#include "vec4.h"

namespace Math
{
	class mat4
	{
	private:
		vec4 v[4];
	public:
		mat4::mat4()
		{
			//identity matix
			v[0] = vec4(1, 0, 0, 0);
			v[1] = vec4(0, 1, 0, 0);
			v[2] = vec4(0, 0, 1, 0);
			v[3] = vec4(0, 0, 0, 1);
		}
		~mat4() { }

		mat4::mat4(const vec4 &x, const vec4 &y, const vec4 &z, const vec4 &w)
		{
			v[0] = x;
			v[1] = y;
			v[2] = z;
			v[3] = w;
		}

		vec4 row(int i) const
		{
			return v[i];
		}
		/*
		vec4 column(int i) const
		{
			return vec4(v[0][i], v[1][i], v[2][i], v[3][i]);
		}*/

		mat4 operator+(const mat4 &rhs) const
		{
			return mat4(
				v[0] + rhs.v[0],
				v[1] + rhs.v[1],
				v[2] + rhs.v[2],
				v[3] + rhs.v[3]
				);
		}

		mat4 &operator+=(const mat4 &rhs)
		{
			v[0] += rhs.v[0];
			v[1] += rhs.v[1];
			v[2] += rhs.v[2];
			v[3] += rhs.v[3];
			return *this;
		}

		mat4 operator*(const mat4 &rhs) const
		{
			mat4 result;
			//v[0] = PreMultiply(

			return result;
		}
		
		mat4 operator*(const float f) const
		{
			return mat4(v[0] * f,
				v[1] * f,
				v[2] * f,
				v[3] * f
				);
		}
		mat4 &Scale(float x, float y, float z)
		{
			v[0] *= x;
			v[1] *= y;
			v[2] *= z;
			return *this;
		}

		mat4 &Translate(float x, float y, float z)
		{
			v[3].x += x;
			v[3].y += y;
			v[3].z += z;
			
			return *this;
		}

		mat4 &Translate(const vec3 val)
		{
			v[3].x += val.x;
			v[3].y += val.y;
			v[3].z += val.z;

			return *this;
		}

		mat4 &Rotate(float angle, float x, float y, float z)
		{
			mat4 rotation;

			*this = rotation * *this;
		}

		mat4 &RotateX(float angle)
		{


			return 


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


