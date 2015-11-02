#include "vec4.h"
#include <cmath>

using namespace Math;

vec4 vec4::Zero(0, 0, 0, 0);

#pragma region Construction/Destruction
vec4::vec4()
{
}

vec4::vec4(float nx, float ny, float nz, float nw) : x(nx), y(ny), z(nz), w(nw)
{
}

vec4::vec4(const vec4 &rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w)
{
}

vec4::vec4(const vec3& xyz, float w)
{
	x = xyz.x;
	y = xyz.y;
	z = xyz.z;
	this->w = w;
}

vec4::~vec4()
{
}
#pragma endregion

vec4& vec4::operator=(const vec4 &rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	w = rhs.w;

	return *this;
}
bool vec4::operator==(const vec4 & rhs)
{
	return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
}

float vec4::Length() const
{
	return sqrt(x*x + y*y + z*z + w*w);
}

#pragma region Unary operations
//Unary minus returns negative of the vector
vec4 Math::vec4::operator-() const
{
	return vec4(-x, -y, -z, -w);
}

#pragma endregion

#pragma region scalar operations
vec4 Math::vec4::operator+=(const float s) const
{
	return vec4(x + s, y + s, z + s, w + s);
}

vec4 Math::vec4::operator-=(const float s) const
{
	return vec4(x - s, y - s, z - s, w - s);
}

vec4 Math::vec4::operator*=(const float s) const
{
	return vec4(x*s, y*s, z*s, w*s);
}

vec4 Math::vec4::operator/=(const float s) const
{
	float recip = 1.0f / s; // one div by s and multiplying by recip*4 is faster than div by s
	return vec4(x*recip, y*recip, z*recip, w*recip);
}

vec4 &Math::vec4::operator+=(const float s)
{
	x += s;
	y += s;
	z += s;
	w += s;
	return *this;
}

vec4 &Math::vec4::operator-=(const float s)
{
	x -= s;
	y -= s;
	z -= s;
	w -= s;
	return *this;
}

vec4 & Math::vec4::operator/=(const float s)
{
	float recip = 1.0f / s;
	x /= recip;
	y /= recip;
	z /= recip;
	w /= recip;
	return *this;
}

vec4 & Math::vec4::operator*=(const float s)
{
	x *= s;
	y *= s;
	z *= s;
	w *= s;
	return *this;
}
#pragma endregion

#pragma region vector operations
vec4 Math::vec4::operator+(const vec4 rhs) const
{
	return vec4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
}

vec4 Math::vec4::operator-(const vec4 rhs) const
{
	return vec4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
}

void Math::vec4::Normalize()
{
	float magnitudeSquared = x*x + y*y + z*z + w*w;
	if (magnitudeSquared > 0)
	{
		float magRecip = 1.0f / magnitudeSquared;
		x *= magRecip;
		y *= magRecip;
		z *= magRecip;
		w *= magRecip;
	}
}
/*
vec4 Math::vec4::cross(const vec4 rhs) const
{
	return vec4(y * rhs.z - z * rhs.y,
				z * rhs.w - w * rhs.z,
				w * rhs.x - x * rhs.w,
				x * rhs.y - y * rhs.x);
}
*/
vec4 vec4::Cross(const vec4 rhs) const
{
	return vec4(y * rhs.z - z * rhs.y,
		z * rhs.x - x * rhs.z,
		x * rhs.y - y * rhs.x,
		0.0f);
}
float Math::vec4::Dot(const vec4 rhs) const
{
	return x*rhs.x + y*rhs.y + z*rhs.z + w*rhs.w;
}
#pragma endregion