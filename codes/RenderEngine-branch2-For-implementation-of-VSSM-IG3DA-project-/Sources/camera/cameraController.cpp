#define _USE_MATH_DEFINES

#include "cameraController.hpp"
#include "../globals.hpp"
#include "../helpers/utility.hpp"
#include <glm/gtx/euler_angles.hpp>
#include <math.h>

using namespace IceRender;

CameraController::CameraController()
{
	camera = nullptr;
	flag = UpdateFlags::None;
	moveDir = Utility::zeroV3;
	rotCache = Utility::zeroV3;
	pitchAngle = 0;
	yawAngle = 0;
	zoomCache = 0;
	moveSpeed = 0;
	rotSpeed = 0;
	zoomSpeed = 0;
	isLogCam = false;
}

CameraController::~CameraController() {}

void CameraController::Init()
{
	// camera setting
	float aspect = static_cast<float>(GLOBAL.WIN_WIDTH) / static_cast<float>(GLOBAL.WIN_HEIGHT);
	camera = make_shared<Camera>(GLOBAL.camFOV, aspect, GLOBAL.camNear, GLOBAL.camFar);

	Print("Camera initialized FOV: " + std::to_string(camera->GetCameraParameter(CameraIndex::FOV)));

	moveSpeed = GLOBAL.camMoveSpeed;
	rotSpeed = GLOBAL.camRotSpeed;
	zoomSpeed = GLOBAL.camZoomSpeed;
}

void CameraController::Update()
{
	if (flag != UpdateFlags::None)
	{
		if (static_cast<bool>(flag & UpdateFlags::Movement))
		{
			// update camera movement
			glm::vec3 viewDir, right, up;
			camera->GetCameraDirection(viewDir, right, up);
			glm::vec3 offset = Utility::zeroV3;
			if (moveDir.y != 0)
				offset += (moveDir.y * viewDir * moveSpeed);
			if (moveDir.x != 0)
				offset += (moveDir.x * right * moveSpeed);
			glm::vec3 oldPos = camera->GetTransform()->GetPosition();
			camera->GetTransform()->SetPosition(oldPos + offset);
			moveDir = Utility::zeroV3;
		}

		if (static_cast<bool>(flag & UpdateFlags::Rotation))
		{
			// update camera rotation
			// here rotation axis X,Y axis are actually right, up axis of camera.
			float angleX = rotCache.x * rotSpeed; // angle rotates around +X or -X axis, in radians. Using rotaCache.x because it is -1 or 1 or 0
			float angleY = rotCache.y * rotSpeed;
			pitchAngle = std::clamp(pitchAngle + angleX, static_cast<float>(-80 * M_PI / 180), static_cast<float>(80 * M_PI / 180)); // never reach at 90 degrees
			yawAngle += angleY;
			camera->GetTransform()->SetSubRotation(pitchAngle, 0);
			camera->GetTransform()->SetSubRotation(yawAngle, 1);
			rotCache = Utility::zeroV3;
		}

		if (static_cast<bool>(flag & UpdateFlags::Zoom))
		{
			float offset = zoomSpeed * zoomCache;
			float oldFOV = camera->GetCameraParameter(CameraIndex::FOV);
			camera->SetCameraParameter(CameraIndex::FOV, std::clamp(oldFOV + offset, 5.0f, 120.0f));
			zoomCache = 0;
			if (isLogCam)
				Print("Camera new FOV: " + std::to_string(camera->GetCameraParameter(CameraIndex::FOV)));
		}

		// reset things
		flag = UpdateFlags::None;

		// ------------------------------debug------------------------------ //
		if (isLogCam)
			Utility::PrintTransform(*camera->GetTransform(), "\nCamera's transformation\n");
		// ---------------------------------------------------------------- //
	}
}

void CameraController::MoveCamera(const glm::vec2& _dir)
{
	moveDir += _dir;
	if (moveDir != Utility::zeroV2)
		flag |= UpdateFlags::Movement;
	else
		flag = flag & ~UpdateFlags::Movement; // remove movement flag
}

void CameraController::RotateCamera(const glm::vec2& _cache)
{
	rotCache += _cache;
	if (rotCache != Utility::zeroV2)
		flag |= UpdateFlags::Rotation;
	else
		flag = flag & ~UpdateFlags::Rotation;
}

void CameraController::ZoomCamera(int _cache)
{
	zoomCache += _cache;
	if (zoomCache != 0)
		flag |= UpdateFlags::Zoom;
	else
		flag = flag & ~UpdateFlags::Zoom;
}

void CameraController::ResetCamera()
{
	pitchAngle = yawAngle = 0;
	moveDir = Utility::zeroV3;
	rotCache = Utility::zeroV3;
	zoomCache = 0;
	camera->SetCameraParameter(CameraIndex::FOV, GLOBAL.camFOV);
	camera->GetTransform()->SetPosition(Utility::zeroV3);
	camera->GetTransform()->SetRotation(Utility::zeroV3);
	camera->GetTransform()->SetScale(Utility::oneV3);
}

void CameraController::SetLogCamInfo(bool _isLog) { isLogCam = _isLog; }
shared_ptr<Camera> CameraController::GetActiveCamera() const { return camera; }
void CameraController::SetCameraPosition(const glm::vec3& _pos) { camera->GetTransform()->SetPosition(_pos); }
void CameraController::SetCameraRotation(const glm::vec3& _rot) { pitchAngle = _rot.x; yawAngle = _rot.y; camera->GetTransform()->SetRotation(_rot); }
