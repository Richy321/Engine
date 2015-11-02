#pragma once
namespace Math
{
	class vec3
	{
	public:
		vec3();
		vec3(float x, float y, float z);
		vec3(vec3& rhs);
		~vec3();

		vec3 &operator=(const vec3 &rhs);
		bool operator==(const vec3 &rhs);

		float Length() const;

		vec3 vec3::operator-() const;

		vec3 operator+=(const float s) const;
		vec3 operator-=(const float s) const;
		vec3 operator*=(const float s) const;
		vec3 operator/=(const float s) const;

		vec3 &operator+=(const float s);
		vec3 &operator-=(const float s);
		vec3 &operator*=(const float s);
		vec3 &operator/=(const float s);
		
		vec3 operator+(const vec3 rhs) const;
		vec3 operator-(const vec3 rhs) const;

		void normalize();
		vec3 cross(const vec3 rhs) const;
		float dot(const vec3 rhs) const;

		static vec3 Zero;

	public:
		float x;
		float y;
		float z;
	};
}
