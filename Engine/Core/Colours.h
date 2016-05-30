#pragma once
#include "../Dependencies/glm/detail/type_vec4.hpp"

namespace Core
{
	namespace Colours_ARGB
	{
		vec4 HotPink = vec4(1.0f, 1.0f, 0.412f, 0.706f);
		vec4 Red = vec4(1.0f, 1.0f, 0.0f, 0.0f);
		vec4 Green = vec4(1.0f, 0.0f, 1.0f, 0.0f);
		vec4 Blue = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	}

	namespace Colours_RGBA
	{
		vec4 HotPink = vec4(1.0f, 0.412f, 0.706f, 1.0f);
		vec4 Red = vec4(1.0f, 0.0f, 0.0f, 1.0f);
		vec4 Green = vec4(0.0f, 1.0f, 0.0f, 1.0f);
		vec4 Blue = vec4(0.0f, 0.0f, 1.0f, 1.0f);
		vec4 Yellow = vec4(1.0f, 1.0f, 0.0f, 1.0f);
	}


	namespace Colours_RGB
	{
		vec3 HotPink(1.0f, 0.412f, 0.706f);
		vec3 Red(1.0f, 0.0f, 0.0f);
		vec3 Green(0.0f, 1.0f, 0.0f);
		vec3 Blue(0.0f, 0.0f, 1.0f);
		vec3 Yellow(1.0f, 1.0f, 0.0f);
	}
}
