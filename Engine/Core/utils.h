#pragma once
#include <iostream>
#include "../Dependencies/glew/glew.h"
#include <memory>
#include "Networking/Address.h"

#define Check_GLError() Utils::CheckGLError(__FILE__,__LINE__)

namespace Core
{
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
	};
}
