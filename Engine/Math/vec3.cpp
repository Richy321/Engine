#include "vec3.h"
#include <cmath>

using namespace Math;

vec3 vec3::Zero(0, 0, 0);

#pragma region Construction/Destruction
vec3::vec3()
{
}

vec3::vec3(float nx, float ny, float nz) : x(nx), y(ny), z(nz)
{
}

vec3::vec3(vec3 &rhs) :x(rhs.x), y(rhs.y), z(rhs.z)
{
}

vec3::~vec3()
{
}
#pragma endregion

vec3& vec3::operator=(const vec3 &rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;

	return *this;
}

bool vec3::operator==(const vec3 & rhs)
{
	return x == rhs.x && y == rhs.y && z == rhs.z;
}

float vec3::Length() const
{
	return sqrt(x*x + y*y + z*z);
}

#pragma region Unary operations
//Unary minus returns negative of the vector
vec3 vec3::operator-() const
{
	return vec3(-x, -y, -z);
}

#pragma endregion

#pragma region scalar operations
vec3 vec3::operator+=(const float s) const
{
	return vec3(x+s, y+s, z+s);
}

vec3 vec3::operator-=(const float s) const
{
	return vec3(x-s, y-s, z-s);
}

vec3 vec3::operator*=(const float s) const
{
	return vec3(x*s, y*s, z*s);
}

vec3 vec3::operator/=(const float s) const
{
	float recip = 1.0f / s; // one div by s and multiplying by recip*3 is faster than div by s
	return vec3(x*recip, y*recip, z*recip);
}

vec3 &vec3::operator+=(const float s)
{
	x += s;
	y += s;
	z += s;
	return *this;
}

vec3 &vec3::operator-=(const float s)
{
	x -= s;
	y -= s;
	z -= s;
	return *this;
}

vec3 & vec3::operator/=(const float s)
{
	float recip = 1.0f / s;
	x /= recip;
	y /= recip;
	z /= recip;
	return *this;
}

vec3 & vec3::operator*=(const float s)
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
}
#pragma endregion

#pragma region vector operations
vec3 vec3::operator+(const vec3 rhs) const
{
	return vec3(x + rhs.x, y + rhs.y, z + rhs.z);
}

vec3 vec3::operator-(const vec3 rhs) const
{
	return vec3(x - rhs.x, y - rhs.y, z - rhs.z);
}

void vec3::normalize()
{
	float magnitudeSquared = x*x + y*y + z*z;
	if (magnitudeSquared > 0)
	{
		float magRecip = 1.0f / magnitudeSquared;
		x *= magRecip;
		y *= magRecip;
		z *= magRecip;
	}
}
vec3 vec3::cross(const vec3 rhs) const
{
	return vec3(y * rhs.z - z * rhs.y,
				z * rhs.x - x * rhs.z,
				x * rhs.y - y * rhs.x);
}
float vec3::dot(const vec3 rhs) const
{
	return x*rhs.x + y*rhs.y + z*rhs.z;
}
#pragma endregion