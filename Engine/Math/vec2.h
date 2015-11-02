#pragma once
#include <cmath>
namespace Math
{
	class vec2
	{
	public:
		vec2();
		vec2(float x, float y);
		vec2(vec2& rhs);
		~vec2();

		vec2 &operator=(const vec2 &rhs);
		bool operator==(const vec2 &rhs);

		float Length() const;

		vec2 Math::vec2::operator-() const;

		vec2 operator+=(const float s) const;
		vec2 operator-=(const float s) const;
		vec2 operator*=(const float s) const;
		vec2 operator/=(const float s) const;

		vec2 &operator+=(const float s);
		vec2 &operator-=(const float s);
		vec2 &operator*=(const float s);
		vec2 &operator/=(const float s);

		vec2 operator+(const vec2 rhs) const;
		vec2 operator-(const vec2 rhs) const;

		void normalize();
		float dot(const vec2 rhs) const;

		static vec2 Zero;
	public:
		float x;
		float y;
	};
}
