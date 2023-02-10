#pragma once

#include "camera.hpp"

namespace IceRender
{
	namespace CameraDefinition
	{
		enum class UpdateFlags
		{
			None = 0,
			Movement = 1 << 0,
			Rotation = 1 << 1,
			Zoom = 1 << 2
		};

		inline UpdateFlags operator|(UpdateFlags lhs, UpdateFlags rhs)
		{
			return static_cast<UpdateFlags>(static_cast<int>(lhs) | static_cast<int>(rhs));
		}

		inline UpdateFlags operator|=(UpdateFlags& lhs, UpdateFlags rhs)
		{
			return lhs = lhs | rhs;
		}

		inline UpdateFlags operator &(UpdateFlags lhs, UpdateFlags rhs)
		{
			return static_cast<UpdateFlags>(static_cast<int>(lhs) & static_cast<int>(rhs));
		}

		inline UpdateFlags operator &=(UpdateFlags& lhs, UpdateFlags rhs)
		{
			return lhs = lhs & rhs;
		}

		inline UpdateFlags operator ~(UpdateFlags val)
		{
			return static_cast<UpdateFlags>(~static_cast<unsigned int>(val));
		}
	}
	
	using namespace CameraDefinition;

	class CameraController
	{
	private:
		shared_ptr<Camera> camera;

		// update flag
		UpdateFlags flag;

		// movement
		glm::vec2 moveDir; // this dir is based on original camera's position[which camera's pos = (0, 0, 0), rot = (0, 0, 0)]
		float moveSpeed;

		// rotation
		glm::vec2 rotCache; // cache rotate axis
		float pitchAngle; // angle rotates around X-axis
		float yawAngle; // angle rotates around Y-axis
		float rotSpeed;

		// zoom in/out
		int zoomCache;
		float zoomSpeed;

		// for debug
		bool isLogCam;

	public:
		CameraController();
		~CameraController();

		void Init();
		void Update();

		// per frame, input will create a move direction(it's possible to having multiple key pressed on the same time), cache it and use it later
		void MoveCamera(const glm::vec2& _dir);
		// rotate camera only consider two rotated axis now, the pitch and yaw. (pitch is x axis of camera's local space, while yaw is y axis)
		void RotateCamera(const glm::vec2& _cache);
		void ZoomCamera(int _cache);
		void ResetCamera();

		// for debug
		void SetLogCamInfo(bool _isLog);

		shared_ptr<Camera> GetActiveCamera() const;

		void SetCameraPosition(const glm::vec3& _pos);
		void SetCameraRotation(const glm::vec3& _rot);
	};
}

