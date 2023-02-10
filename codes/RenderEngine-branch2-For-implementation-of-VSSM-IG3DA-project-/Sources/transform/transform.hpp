#pragma once

#include <glm/glm.hpp>
#include "glm/gtx/quaternion.hpp"


namespace IceRender
{
	class Transform
	{
	private:
		glm::vec3 scale;
		glm::vec3 position;
		glm::vec3 rotation; // euler angles in radians. [rotation order: x-y-z~pitch-yaw-roll]

	public:
		Transform();
		Transform(const glm::vec3& _pos, const glm::vec3& _rot, const glm::vec3& _scale);

		void SetPosition(const glm::vec3& _pos);
		void SetSubPosition(const float& _value, const int& _idx);
		void SetRotation(const glm::vec3& _rot);
		void SetSubRotation(const float& _value, const int& _idx);
		void SetScale(const glm::vec3& _scale);
		void SetSubScale(const float& _value, const int& _idx);

		glm::vec3 GetPosition() const;
		float GetSubPosition(const int& _idx) const;
		glm::vec3 GetRotation() const;
		float GetSubRotation(const int& _idx) const;
		glm::vec3 GetScale() const;
		float GetSubScale(const int& _idx) const;
		glm::quat GetQuaternion() const;
		glm::mat4 Transform::GetRotationMat4() const;

		glm::mat4 ComputeTransformationMatrix() const;
	};
}