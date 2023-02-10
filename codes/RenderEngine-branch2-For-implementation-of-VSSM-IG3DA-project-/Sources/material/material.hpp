#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>


using namespace std;

namespace IceRender
{
	class Material
	{
	protected:
		glm::vec3 color; // if not using albedo texture, then we use this color,
		GLuint albedo; // using texture. texture=0 is not valid value.(it reserves for default texture.)
		vector<glm::vec2> uv;

	public:
		Material();
		~Material();

		void SetColor(const glm::vec3& _color);
		void SetAlbedo(const GLuint& _albedo);
		void SetUV(const vector<glm::vec2>& _uv);

		glm::vec3 GetColor() const;
		size_t GetUVDataSize() const;
		const void* GetUVData() const;
		GLuint GetAlbedo() const;
	};
}
