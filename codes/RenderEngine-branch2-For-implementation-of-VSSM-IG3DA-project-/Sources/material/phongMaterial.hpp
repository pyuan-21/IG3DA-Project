#pragma once

#include "material.hpp"

namespace IceRender
{
	class PhongMaterial : public Material
	{
	private:
		glm::vec3 ka; // coefficient for ambient
		glm::vec3 kd; // coefficient for diffuse
		glm::vec3 ks; // coefficient for specular
		float shiness;

	public:
		PhongMaterial();
		PhongMaterial(const glm::vec3& _ka, const glm::vec3& _kd, const glm::vec3& _ks, const float& _shiness);

		glm::vec3 GetAmbientCoef() const;
		glm::vec3 GetDiffuseCoef() const;
		glm::vec3 GetSpecularCoef() const;
		float GetShiness() const;

		void SetAmbientCoef(const glm::vec3& _ka);
		void SetDiffuseCoef(const glm::vec3& _kd);
		void SetSpecularCoef(const glm::vec3& _ks);
		void SetShiness(const float& _shiness);
	};
}
