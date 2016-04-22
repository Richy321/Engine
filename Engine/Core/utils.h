#pragma once
#include <iostream>
#include "../Dependencies/glew/glew.h"
#include <memory>
#include "Networking/Address.h"
#include "../Dependencies/glm/vec2.hpp"
#define Check_GLError() Utils::CheckGLError(__FILE__,__LINE__)

namespace Core
{
	const float EPSILON = 0.0001f;
	const float M_PI = 3.14159f;

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

		static float Len2Vec2(const glm::vec2& v)
		{
			return v.x * v.x + v.y * v.y;
		}

		static float DotVec2(const glm::vec2& a, const glm::vec2& b)
		{
			return a.x * b.x + a.y * b.y;
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
	};
}
