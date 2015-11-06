#pragma once
namespace Math
{
	class Utils
	{
	public:
		static const float PI;
		static float DegToRad(float angle) { return angle * (PI / 180); }
		static float RadToDeg(float angle) { return angle * (180 / PI); }
	};
	const float Utils::PI = 3.14159265f;
}

