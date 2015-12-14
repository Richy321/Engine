#pragma once
#include "vec3.h"
#include "assert.h"

namespace Math
{
	class vec4
	{
		float v[4];
	public:
		vec4();
		vec4(float f);
		vec4(float x, float y, float z, float w);
		vec4(const vec4& rhs);
		vec4(const vec3& rhs, float w);
		~vec4();

		vec4 &operator=(const vec4 &rhs);
		bool operator==(const vec4 &rhs) const;

		float Length() const;

		vec4 vec4::operator-() const;

		#pragma region Scalar operations
		vec4 operator+(const float s) const
		{
			return *this + vec4(s);
		}
		vec4 operator-(const float s) const
		{
			return *this - vec4(s);
		}
		vec4 operator*(const float s) const
		{
			return *this * vec4(s);
		}
		vec4 operator/(const float s) const
		{
			return *this * vec4(1.0f / s);
		}
		#pragma endregion

		#pragma region In-place scalar operations
		vec4 &operator+=(const float s);
		vec4 &operator-=(const float s);
		vec4 &operator*=(const float s);
		vec4 &operator/=(const float s);
		#pragma endregion 

		#pragma region Vector operations
		vec4 operator+(const vec4 &rhs) const;
		vec4 operator-(const vec4 &rhs) const;
		vec4 operator*(const vec4 &rhs) const;
		#pragma endregion

		#pragma region In-place vector operations
		vec4 &operator+=(const vec4 &rhs);
		vec4 &operator-=(const vec4 &rhs);
		vec4 &operator*=(const vec4 &rhs);
		#pragma endregion

		void Normalize();
		vec4 Cross(const vec4 rhs) const;
		float Dot(const vec4 rhs) const;

		float Sum() const
		{
			return v[0] + v[1] + v[2] + v[3];
		}
		static vec4 Zero;

		float &operator[](int i)
		{
			assert(i >= 0 && i < 4);
			return v[i];
		}
		
		const float &operator[] (int i) const
		{
			assert(i >= 0 && i < 4);
			return v[i];
		}

		float &x() { return v[0]; }
		float &y() { return v[1]; }
		float &z() { return v[2]; }
		float &w() { return v[3]; }

		const float &x() const { return v[0]; }
		const float &y() const { return v[1]; }
		const float &z() const { return v[2]; }
		const float &w() const { return v[3]; }


		vec3 xyz() { return vec3(this->x(), this->y(), this->z()); }
	};
}
