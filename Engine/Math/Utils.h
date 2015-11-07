#pragma once
namespace Math
{
	static const float PI = 3.14159265f;
	class Utils
	{
	public:
		static float DegToRad(float angle) { return angle * (PI / 180); }
		static float RadToDeg(float angle) { return angle * (180 / PI); }
	};
}

