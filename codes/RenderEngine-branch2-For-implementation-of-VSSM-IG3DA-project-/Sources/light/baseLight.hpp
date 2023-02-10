#pragma once

#include <string>
#include <memory>
#include "../transform/transform.hpp"

using namespace std;

namespace IceRender
{
	enum class LightType
	{
		NONE = -1,
		POINT = 0,
		DIRECT
	};

	class LightCamInfo
	{
	public:
		float near;
		float far;
		glm::vec3 lightCamPos;
		glm::vec3 lightViewDir;
	};

	class BaseLight
	{
	protected:
		string name;
		LightType type;
		shared_ptr<Transform> transform;
		glm::vec3 color;
		float intensity;
		bool renderShadow;

	public:
		BaseLight(const string& _name);
		~BaseLight();

		const string GetName() const;
		shared_ptr<Transform> GetTransform(); // allow any operation outside to change the transform directly
		LightType GetType() const;
		glm::vec3 GetColor() const;
		void SetIntensity(const float& _intensity);
		float GetIntensity() const;
		bool IsRenderShadow() const;
		void SetRenderShadow(const bool& _val);

		virtual glm::mat4 GetLightSpaceMat(LightCamInfo& _lightCamInfo) = 0; // it equals to projMat*viewMat of light, and it also returns lightCamPos, lightViewDir
	};
}
