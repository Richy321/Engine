#pragma once
#include <iostream>
#include "../Dependencies/glew/glew.h"
#include <memory>
#include "Networking/Address.h"
#include "../Dependencies/glm/glm.hpp"

#define Check_GLError() Utils::CheckGLError(__FILE__,__LINE__)
using namespace glm;

namespace Core
{
	const float EPSILON = 0.0001f;
	const float M_PI = 3.14159f;
	const float M_PI_2 = 1.5707963f;  // pi/2

	class Utils
	{
	public:
		
		static void CheckGLError(const char *file, int line)
		{
			GLenum err(glGetError());

			while (err != GL_NO_ERROR) {
				std::string error;

				switch (err) {
				case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
				case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
				case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
				case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
				case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
				}

				std::cerr << "GL_" << error.c_str() << " - " << file << ":" << line << std::endl;
				err = glGetError();
			}
		}

		struct GUIDComparer
		{
			bool operator()(const GUID & Left, const GUID & Right) const
			{
				// comparison logic goes here
				return memcmp(&Left, &Right, sizeof(Right)) < 0;
			}
		};

		struct SharedPtrAddressComparer
		{
			bool operator()(const std::shared_ptr<networking::Address>& left, const std::shared_ptr<networking::Address>& right) const
			{
				return left->toString() < right->toString();
			}
		};

		static float CrossVec2(const glm::vec2& a, const glm::vec2& b)
		{
			return  a.x * b.y - a.y * b.x;
		}

		static glm::vec2 CrossVec2(float a, const glm::vec2& v)
		{
			return glm::vec2(-a * v.y, a * v.x);
		}

		static glm::vec2 Cross(const glm::vec2& v, float a)
		{
			return glm::vec2(a * v.y, -a * v.x);
		}

		static float Len2Vec2(const glm::vec2& v)
		{
			return v.x * v.x + v.y * v.y;
		}

		static float DotVec2(const glm::vec2& a, const glm::vec2& b)
		{
			return a.x * b.x + a.y * b.y;
		}

		static glm::mat2 Transpose2D(const glm::mat2& m)
		{
			return glm::mat2(m[0][0], m[1][0], m[0][1], m[1][1]);
		}

		static glm::vec2 NormaliseVec2(glm::vec2& v)
		{
			float len = Len2Vec2(v);

			if (len > EPSILON)
			{
				float invLen = 1.0f / len;
				v.x *= invLen;
				v.y *= invLen;
			}
			return v;
		}

		static float DistSquared(const glm::vec2& a, const glm::vec2& b)
		{
			glm::vec2 c = a - b;
			return DotVec2(c, c);
		}

		static bool BiasGreaterThan(float a, float b)
		{
			const float k_biasRelative = 0.95f;
			const float k_biasAbsolute = 0.01f;
			return a >= b * k_biasRelative + a * k_biasAbsolute;
		}

		static bool EqualWithEpsilon(float a, float b)
		{
			return std::abs(a - b) <= EPSILON;
		}

		static glm::vec3 CalculateNormalMagnitude(glm::vec3& p1, glm::vec3& p2, glm::vec3& p3 )
		{
			glm::vec3 v1 = p2 - p1;
			glm::vec3 v2 = p3 - p1;

			return cross(v1, v2);
		}

		static glm::vec3 CalculateNormal(glm::vec3& p1, glm::vec3& p2, glm::vec3& p3)
		{
			return normalize(CalculateNormalMagnitude(p1, p2, p3));
		}
	};
}
