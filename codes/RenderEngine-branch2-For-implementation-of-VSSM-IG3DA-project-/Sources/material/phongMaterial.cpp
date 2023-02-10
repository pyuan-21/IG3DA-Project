#include "phongMaterial.hpp"
#include "../helpers/utility.hpp"


using namespace IceRender;

PhongMaterial::PhongMaterial() { ka = kd = ks = Utility::zeroV3;  shiness = 0; }
PhongMaterial::PhongMaterial(const glm::vec3& _ka, const glm::vec3& _kd, const glm::vec3& _ks, const float& _shiness) : ka(_ka), kd(_kd), ks(_ks), shiness(_shiness) {}

glm::vec3 PhongMaterial::GetAmbientCoef() const { return ka; }
glm::vec3 PhongMaterial::GetDiffuseCoef() const { return kd; }
glm::vec3 PhongMaterial::GetSpecularCoef() const { return ks; }
float PhongMaterial::GetShiness() const { return shiness; }

void PhongMaterial::SetAmbientCoef(const glm::vec3& _ka)
{
	ka.x = std::clamp(_ka.x, 0.0f, 1.0f);
	ka.y = std::clamp(_ka.y, 0.0f, 1.0f);
	ka.z = std::clamp(_ka.z, 0.0f, 1.0f);
}
void PhongMaterial::SetDiffuseCoef(const glm::vec3& _kd)
{
	kd.x = std::clamp(_kd.x, 0.0f, 1.0f);
	kd.y = std::clamp(_kd.y, 0.0f, 1.0f);
	kd.z = std::clamp(_kd.z, 0.0f, 1.0f);
}
void PhongMaterial::SetSpecularCoef(const glm::vec3& _ks)
{
	ks.x = std::clamp(_ks.x, 0.0f, 1.0f);
	ks.y = std::clamp(_ks.y, 0.0f, 1.0f);
	ks.z = std::clamp(_ks.z, 0.0f, 1.0f);
}
void PhongMaterial::SetShiness(const float& _shiness) { shiness = _shiness; if (shiness < 0)shiness = 0; }