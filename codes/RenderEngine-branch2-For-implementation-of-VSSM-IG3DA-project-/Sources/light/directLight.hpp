#pragma once

#include "baseLight.hpp"

namespace IceRender
{
	class DirectLight : public BaseLight
	{
	private:
		glm::vec3 direction;

	public:
		DirectLight(const string& _name);
		~DirectLight();

		void SetDirection(const glm::vec3 _dir);
		glm::vec3 GetDirection() const;

		glm::mat4 GetLightSpaceMat(LightCamInfo& _lightCamInfo) override;
	};
}
