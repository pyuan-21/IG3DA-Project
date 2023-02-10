#pragma once

#include <memory>
#include "../transform/transform.hpp"

namespace IceRender
{
	using namespace std;

	enum class CameraIndex
	{
		FOV = 0,
		ASPECT,
		NEAR,
		FAR,
		SIZE,
	};

	class Camera
	{
	private:
		shared_ptr<Transform> transform;

		// parameters for camera
		float cParams[static_cast<int>(CameraIndex::SIZE)]; // 0:fov(in degree), 1:aspect, 2:near, 3:far

	public:
		Camera(float _fov, float _aspect, float _near, float _far);
		~Camera();

		shared_ptr<Transform> GetTransform(); // allow any operation outside to change the transform directly, because we are using shared_ptr, we don't need to return &

		void GetCameraDirection(glm::vec3& _viewDir, glm::vec3& _right, glm::vec3& _up) const;
		glm::mat4 GetViewMatrix() const;
		glm::mat4 GetProjectionMatrix() const;

		float GetCameraParameter(CameraIndex _index) const;
		void SetCameraParameter(CameraIndex _index, float _value);
	};
}



