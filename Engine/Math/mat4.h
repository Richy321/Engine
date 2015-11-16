#pragma once
#include "vec4.h"
#include <assert.h>
#include "Utils.h"

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
		mat4::mat4(const float i)
		{
			v[0] = i;
			v[1] = i;
			v[2] = i;
			v[3] = i;
		}

		vec4 GetRow(int i) const
		{
			assert(i >= 0 && i < 4);
			return v[i];
		}
		
		vec4 GetColumn(int i) const
		{
			assert(i >= 0 && i < 4);
			return vec4(v[0][i], v[1][i], v[2][i], v[3][i]);
		}

		void SetRow(int i, vec4 newValues)
		{
			assert(i >= 0 && i < 4);
			v[i] = newValues;
		}

		void SetColumn(int i, vec4 newValues)
		{
			assert(i >= 0 && i < 4);

			v[0][i] = newValues[0];
			v[1][i] = newValues[1];
			v[2][i] = newValues[2];
			v[3][i] = newValues[3];
		}
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

		mat4 operator*(const float f) const
		{
			return mat4(v[0] * f,
						v[1] * f,
						v[2] * f,
						v[3] * f
						);
		}

		mat4 operator*(const mat4 &rhs) const
		{
			return RowMajorMultiply(rhs);
		}
		
		vec4 operator*(const vec4 &rhs) const
		{
			return PostMultiply(rhs);
		}

		vec4 &operator[](const int i)
		{
			return v[i];
		}

		mat4 &Scale(float x, float y, float z)
		{
			v[0] *= x;
			v[1] *= y;
			v[2] *= z;
			return *this;
		}

		mat4 &Scale(float s)
		{
			v[0] *= s;
			v[1] *= s;
			v[2] *= s;
			return *this;
		}

		mat4 &Translate(float x, float y, float z)
		{
			v[3] += vec4(x, y, z, 0);
			
			return *this;
		}

		mat4 &Translate(const vec3 val)
		{
			v[3] += vec4(val, 0);

			return *this;
		}

		mat4 &Rotate(float angle, float x, float y, float z)
		{
			float c = cosf(Utils::DegToRad(angle));
			float s = sinf(Utils::DegToRad(angle));
			mat4 rotation
			{
				vec4(x*x*(1 - c) + c,		x*y*(1 - c) + z*s,	x*z*(1 - c) - y*s,	0.0f),
				vec4(x*y*(1 - c) - z*s,		y*y*(1 - c) + c,	y*z*(1 - c) + x*s,	0.0f),
				vec4(x*z*(1 - c) + y*s,		y*z*(1 - c) - x*s,	z*z*(1 - c) + c,	0.0f),
				vec4(      0.0f,			      0.0f,				 0.0f,			1.0f)
			};

			*this = rotation * *this;	
			return *this;
		}

		mat4 &Rotate(float angle, vec3 &arbitaryVector)
		{
			mat4 rotation = Rotate(angle, arbitaryVector.x, arbitaryVector.y, arbitaryVector.z);

			*this = rotation * *this;
			return *this;
		}

		mat4 &RotateX(float angle)
		{
			float c = cosf(Utils::DegToRad(angle));
			float s = sinf(Utils::DegToRad(angle));

			mat4 rotation
			{
				vec4(1.0f, 0.0f, 0.0f, 0.0f),
				vec4(0.0f,  c,	  s,   0.0f),
				vec4(0.0f, -s,	  c,   0.0f),
				vec4(0.0f, 0.0f, 0.0f, 1.0f)
			};

			*this = rotation * *this;
			return *this;
		}

		mat4 &RotateY(float angle)
		{
			float c = cosf(Utils::DegToRad(angle));
			float s = sinf(Utils::DegToRad(angle));

			mat4 rotation
			{
				vec4(  c,  0.0f, -s,   0.0f),
				vec4(0.0f, 1.0f, 0.0f, 0.0f),
				vec4(  s,  0.0f,  c,   0.0f),
				vec4(0.0f, 0.0f, 0.0f, 1.0f)
			};

			*this = rotation * *this;
			return *this;
		}

		mat4 &RotateZ(float angle)
		{
			float c = cosf(Utils::DegToRad(angle));
			float s = sinf(Utils::DegToRad(angle));

			mat4 rotation
			{
				vec4(  c,   s,   0.0f, 0.0f),
				vec4( -s,   c,   0.0f, 0.0f),
				vec4(0.0f, 0.0f, 1.0f, 0.0f),
				vec4(0.0f, 0.0f, 0.0f, 1.0f)
			};

			mat4 combined = *this * rotation;
			*this = combined;
			return *this;
		}

		///Multiply by vector on the left - for use with row vectors
		vec4 PreMultiply(const vec4 &lhs) const
		{
			return vec4(
				lhs.x() * v[0].x() +	lhs.y() *v[1].x() +		lhs.z() * v[2].x() +	lhs.w() * v[3].x(),
				lhs.x() * v[0].y() +	lhs.y() *v[1].y() +		lhs.z() * v[2].y() +	lhs.w() * v[3].y(),
				lhs.x() * v[0].z() +	lhs.y() *v[1].z() +		lhs.z() * v[2].z() +	lhs.w() * v[3].z(),
				lhs.x() * v[0].w() +	lhs.y() *v[1].w() +		lhs.z() * v[2].w() +	lhs.w() * v[3].w()
				);
		}

		///Multiply by vector on the right - for use with column vectors
		vec4 PostMultiply(const vec4 &rhs) const
		{
			return vec4(
				v[0].Dot(rhs),
				v[1].Dot(rhs),
				v[2].Dot(rhs),
				v[3].Dot(rhs)
				);
		}

		mat4 ColumnMajorMultiply(const mat4 &rhs) const
		{
			mat4 result;

			for (size_t i = 0; i < 4; i++)
			{
				result.SetColumn(i, vec4(
					(GetColumn(i) * (rhs.GetRow(0))).Sum(),
					(GetColumn(i) * (rhs.GetRow(1))).Sum(),
					(GetColumn(i) * (rhs.GetRow(2))).Sum(),
					(GetColumn(i) * (rhs.GetRow(3))).Sum()
					));
			}
			return result;
		}

		mat4 RowMajorMultiply(const mat4 &rhs) const
		{
			mat4 result;

			for (size_t i = 0; i < 4; i++)
			{
				result.SetRow(i, vec4(
					(v[i] * (rhs.GetColumn(0))).Sum(),
					(v[i] * (rhs.GetColumn(1))).Sum(),
					(v[i] * (rhs.GetColumn(2))).Sum(),
					(v[i] * (rhs.GetColumn(3))).Sum()
					));
			}
			return result;
		}

		mat4 RowMajorMultiply_Andy(const mat4 &rhs) const
		{
			mat4 result;

			for (size_t i = 0; i < 4; i++)
			{
				result.SetRow(i,
					rhs.GetRow(0) * vec4(v[i].x()) +
					rhs.GetRow(1) * vec4(v[i].y()) +
					rhs.GetRow(2) * vec4(v[i].z()) +
					rhs.GetRow(3) * vec4(v[i].w()));
			}
			return result;
		}

		float* GetMatrixFloatValues()
		{
			return &v[0][0];
		}

		const float* GetMatrixFloatValues() const
		{
			return &v[0][0];
		}
	};
}


