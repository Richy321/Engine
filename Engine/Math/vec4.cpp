#include "vec4.h"
#include <cmath>

using namespace Math;

vec4 vec4::Zero(0, 0, 0, 0);

#pragma region Construction/Destruction
vec4::vec4() : v {0,0,0,0}
{
}

Math::vec4::vec4(float f) : v{f,f,f,f}
{
}

vec4::vec4(float nx, float ny, float nz, float nw) : v{ nx, ny, nz,nw }
{
}

vec4::vec4(const vec4 &rhs)
{
	v[0] = rhs.v[0];
	v[1] = rhs.v[1];
	v[2] = rhs.v[2];
	v[3] = rhs.v[3];
}

vec4::vec4(const vec3& xyz, float w)
{
	v[0] = xyz.x;
	v[1] = xyz.y;
	v[2] = xyz.z;
	v[3] = w;
}

vec4::~vec4()
{
}
#pragma endregion

vec4& vec4::operator=(const vec4 &rhs)
{
	v[0] = rhs.x();
	v[1] = rhs.y();
	v[2] = rhs.z();
	v[3] = rhs.w();

	return *this;
}
bool vec4::operator==(const vec4 & rhs)
{
	return v[0] == rhs.x() && v[1] == rhs.y() && v[2] == rhs.z() && v[3] == rhs.w();
}

float vec4::Length() const
{
	return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3]);
}

#pragma region Unary operations
//Unary minus returns negative of the vector
vec4 Math::vec4::operator-() const
{
	return vec4(-v[0], -v[1], -v[2], -v[3]);
}

#pragma endregion

#pragma region scalar operations
vec4 Math::vec4::operator+=(const float s) const
{
	return vec4(v[0] + s, v[1] + s, v[2] + s, v[3] + s);
}

vec4 Math::vec4::operator-=(const float s) const
{
	return vec4(v[0] - s, v[1] - s, v[2] - s, v[3] - s);
}

vec4 Math::vec4::operator*=(const float s) const
{
	return vec4(v[0]*s, v[1]*s, v[2]*s, v[3]*s);
}

vec4 vec4::operator*(float s) const
{
	return vec4(v[0] * s,
				v[1] * s,
				v[2] * s,
				v[3] * s);
}

vec4 Math::vec4::operator/=(const float s) const
{
	float recip = 1.0f / s; // one div by s and multiplying by recip*4 is faster than div by s
	return vec4(v[0]*recip, v[1]*recip, v[2]*recip, v[3]*recip);
}

vec4 &Math::vec4::operator+=(const float s)
{
	v[0] += s;
	v[1] += s;
	v[2] += s;
	v[3] += s;
	return *this;
}

vec4 &Math::vec4::operator-=(const float s)
{
	v[0] -= s;
	v[1] -= s;
	v[2] -= s;
	v[3] -= s;
	return *this;
}

vec4 &Math::vec4::operator/=(const float s)
{
	float recip = 1.0f / s;
	v[0] /= recip;
	v[1] /= recip;
	v[2] /= recip;
	v[3] /= recip;
	return *this;
}

vec4 &Math::vec4::operator*=(const float s)
{
	v[0] *= s;
	v[1] *= s;
	v[2] *= s;
	v[3] *= s;
	return *this;
}
#pragma endregion

#pragma region vector operations
vec4 Math::vec4::operator+(const vec4 rhs) const
{
	return vec4(v[0] + rhs.x(), v[1] + rhs.y(), v[2] + rhs.z(), v[3] + rhs.w());
}

vec4 Math::vec4::operator+=(const vec4 rhs)
{
	v[0] += rhs.x();
	v[1] += rhs.y();
	v[2] += rhs.z();
	v[3] += rhs.w();
	return *this;
}

vec4 Math::vec4::operator-(const vec4 rhs) const
{
	return vec4(v[0] - rhs.x(), v[1] - rhs.y(), v[2] - rhs.z(), v[3] - rhs.w());
}

vec4 Math::vec4::operator-=(const vec4 rhs)
{
	v[0] -= rhs.x();
	v[1] -= rhs.y();
	v[2] -= rhs.z();
	v[3] -= rhs.w();
	return *this;
}

void Math::vec4::Normalize()
{
	float magnitudeSquared = v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3];
	if (magnitudeSquared > 0)
	{
		float magRecip = 1.0f / magnitudeSquared;
		v[0] *= magRecip;
		v[1] *= magRecip;
		v[2] *= magRecip;
		v[3] *= magRecip;
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
	return vec4(v[1] * rhs.z() - v[2] * rhs.y(),
		v[2] * rhs.x() - v[0] * rhs.z(),
		v[0] * rhs.y() - v[1] * rhs.x(),
		0.0f);
}

float Math::vec4::Dot(const vec4 rhs) const
{
	return v[0]*rhs.x() + v[1]*rhs.y() + v[2]*rhs.z() + v[3]*rhs.w();
}

//Component-wise multiplication of vectors
vec4 vec4::Multiply(const vec4& rhs) const
{
	return vec4(v[0] * rhs.x(), v[1] * rhs.y(), v[2] * rhs.z(), v[3] * rhs.w());
}

vec4& vec4::Multiply(const vec4& rhs)
{
	*this = this->Multiply(rhs);
	return *this;
}
#pragma endregion