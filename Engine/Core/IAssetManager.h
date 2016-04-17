#pragma once
#include "Dependencies/glew/glew.h"

namespace Core
{
	class Mesh;
	class IAssetManager
	{
	public:

		IAssetManager()
		{
		}

		virtual ~IAssetManager()
		{
		}
		virtual GLuint GetOGLTextureID(const std::string texID) = 0;

		virtual bool BindTexture(const std::string texID, GLenum TextureUnit) = 0;
		virtual bool UnloadTexture(const std::string texID) = 0;
		virtual void UnloadAllTextures() = 0;

		virtual std::shared_ptr<Mesh> CreateTrianglePrimitive() = 0;
		virtual std::shared_ptr<Mesh> CreateQuadPrimitive(float width, float depth) const = 0;
		virtual std::shared_ptr<Mesh> CreateCubePrimitive(float size) = 0;
	};
}
