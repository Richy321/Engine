#pragma once
#include <vector>
#include "IRenderObject.h"
namespace Rendering
{
	class Mesh : public IRenderObject
	{
	public:
		Mesh();
		virtual ~Mesh();

		virtual void Draw() override;
		virtual void Update() override;
		virtual void SetProgram(GLuint shaderName) override;
		virtual void Destroy() override;

		virtual GLuint GetVao() const override;
		virtual const std::vector<GLuint>& GetVbos() const override;

	protected:
		GLuint vao;
		GLuint program;
		std::vector<GLuint> vbos;
	};
}