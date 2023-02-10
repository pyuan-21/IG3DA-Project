#include "material.hpp"
#include "../helpers/utility.hpp"


using namespace IceRender;

Material::Material() : color(Utility::oneV3), albedo(0) {  }
Material::~Material()
{
	if (glIsTexture(albedo))
		glDeleteTextures(1, &albedo);
	albedo = 0;
}

void Material::SetColor(const glm::vec3& _color) { color = _color; }
void Material::SetAlbedo(const GLuint& _albedo) { albedo = _albedo; }
void Material::SetUV(const vector<glm::vec2>& _uv) { uv = _uv; }

glm::vec3 Material::GetColor()const { return color; }
size_t Material::GetUVDataSize() const { return sizeof(glm::vec2) * uv.size(); }
const void* Material::GetUVData() const { return uv.data(); }
GLuint Material::GetAlbedo() const { return albedo; }