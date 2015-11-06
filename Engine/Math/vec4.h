#pragma once
#include "vec3.h"

namespace Math
{
	class vec4
	{
	public:
		vec4();
		vec4(float f);
		vec4(float x, float y, float z, float w);
		vec4(const vec4& rhs);
		vec4(const vec3& rhs, float w);
		~vec4();

		vec4 &operator=(const vec4 &rhs);
		bool operator==(const vec4 &rhs);

		float Length() const;

		vec4 vec4::operator-() const;

		vec4 operator+=(const float s) const;
		vec4 operator-=(const float s) const;
		vec4 operator*=(const float s) const;
		vec4 operator*(const float s) const;
		vec4 operator/=(const float s) const;

		vec4 &operator+=(const float s);
		vec4 &operator-=(const float s);
		vec4 &operator*=(const float s);
		vec4 &operator/=(const float s);

		vec4 operator+(const vec4 rhs) const;
		vec4 operator+=(const vec4 rhs);
		vec4 operator-(const vec4 rhs) const;
		vec4 operator-=(const vec4 rhs);

		void Normalize();
		vec4 Cross(const vec4 rhs) const;
		float Dot(const vec4 rhs) const;

		static vec4 Zero;

	public:
		float x;
		float y;
		float z;
		float w;
	};
}
