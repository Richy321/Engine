#include "vec2.h"

using namespace Math;

vec2 vec2::Zero(0, 0);

#pragma region Construction/Destruction
vec2::vec2()
{
}

vec2::vec2(float nx, float ny) : x(nx), y(ny)
{
}

Math::vec2::vec2(vec2 &rhs) : x(rhs.x), y(rhs.y)
{
}

vec2::~vec2()
{
}
#pragma endregion

vec2& Math::vec2::operator=(const vec2 &rhs)
{
	x = rhs.x;
	y = rhs.y;

	return *this;
}
bool Math::vec2::operator==(const vec2 & rhs)
{
	return x == rhs.x && y == rhs.y;
}

float vec2::Length() const
{
	return sqrt(x*x + y*y);
}

#pragma region Unary operations
//Unary minus returns negative of the vector
vec2 Math::vec2::operator-() const
{
	return vec2(-x, -y);
}

#pragma endregion

#pragma region scalar operations
vec2 Math::vec2::operator+=(const float s) const
{
	return vec2(x + s, y + s);
}

vec2 Math::vec2::operator-=(const float s) const
{
	return vec2(x - s, y - s);
}

vec2 Math::vec2::operator*=(const float s) const
{
	return vec2(x*s, y*s);
}

vec2 Math::vec2::operator/=(const float s) const
{
	float recip = 1.0f / s; // one div by s and multiplying by recip*3 is faster than div by s
	return vec2(x*recip, y*recip);
}

vec2 &Math::vec2::operator+=(const float s)
{
	x += s;
	y += s;
	return *this;
}

vec2 &Math::vec2::operator-=(const float s)
{
	x -= s;
	y -= s;
	return *this;
}

vec2 & Math::vec2::operator/=(const float s)
{
	float recip = 1.0f / s;
	x /= recip;
	y /= recip;
	return *this;
}

vec2 & Math::vec2::operator*=(const float s)
{
	x *= s;
	y *= s;
	return *this;
}
#pragma endregion

#pragma region vector operations
vec2 Math::vec2::operator+(const vec2 rhs) const
{
	return vec2(x + rhs.x, y + rhs.y);
}

vec2 Math::vec2::operator-(const vec2 rhs) const
{
	return vec2(x - rhs.x, y - rhs.y);
}

void Math::vec2::normalize()
{
	float magnitudeSquared = x*x + y*y;
	if (magnitudeSquared > 0)
	{
		float magRecip = 1.0f / magnitudeSquared;
		x *= magRecip;
		y *= magRecip;
	}
}

float Math::vec2::dot(const vec2 rhs) const
{
	return x*rhs.x + y*rhs.y;
}
#pragma endregion