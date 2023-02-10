#include "camera.hpp"
#include "../helpers/utility.hpp"


using namespace IceRender;

Camera::Camera(float _fov, float _aspect, float _near, float _far)
{
	cParams[0] = _fov;
	cParams[1] = _aspect;
	cParams[2] = _near;
	cParams[3] = _far;

	transform = make_shared<Transform>();
}

Camera::~Camera() {}

shared_ptr<Transform> Camera::GetTransform() { return transform; }

void Camera::GetCameraDirection(glm::vec3& _viewDir, glm::vec3& _right, glm::vec3& _up) const
{
	// note: quaternion only works for a direction, not works for coordinate(x-y-z axis). We can not rotate x-y-z axis directly by using quaternion, 
	// because it may cause the x'-y'-z' not orthogonal. For example, if we rotate a vector (x+y+z) then get a new (x'+y'+z'), we still can not get the correct coordinate
	// x'-y'-z' from this new vector(x'+y'+z'). Because it has infinite combination(rolling around this new vector will have different axes).
	// In here, quaternion is created from euler angles, and the euler angles are created to rotate view direction. Therefore we can use quaternion * frontDir to get 
	// the current viewDir. However, we could not use quaternion to get Up, Right direction(as explained above).
	//float pitchAngle = transform->GetSubRotation(0);
	//float yawAngle = transform->GetSubRotation(1);
	//auto viewDir = glm::vec3(-cos(pitchAngle) * sin(yawAngle), sin(pitchAngle), -cos(pitchAngle) * cos(yawAngle));
	_viewDir = glm::normalize(transform->GetQuaternion() * Utility::frontV3); // Camera's rotation is based on euler angles and it only serves for view direction.

	// don't use 'glm::vec3 up = transform->GetQuaternion(); * Utility::upV3;', 
	// becasue it may get a wrong coordinate which is not orthogonal. Quaternion can only works for a direction.
	// As we don't roll camera, therefore the camera's up, Up direction and viewDir are on the same plane.
	_right = glm::normalize(glm::cross(_viewDir, Utility::upV3));
	_up = glm::normalize(glm::cross(_right, _viewDir)); // camera's up
}

glm::mat4 Camera::GetViewMatrix() const
{
	glm::vec3 viewDir, right, up;
	GetCameraDirection(viewDir, right, up);
	glm::vec3 pos = transform->GetPosition();
	return glm::lookAt(pos, pos + viewDir, up); // actually it will call lookAtRH(right hand)

	// CameraFrameMat = | R T |    and,  ViewMat = inverse(CameraFrameMat)
	//					| 0 1 |
	// and we know easily: R = |   .    .    .  |  [here it's right vector is column representation]
	//						   | right up -front|
	//						   |   .    .    .  |
	// and inverse(R)=R^-1=R_inv=transpose(R)=R^T=R_trans
	// then R_trnans = |   .  right  .  |  [here it's right vector is row representation]
	//				   |   .   up    .  |
	//				   |   . -front  .  | 
	// 
	// However we could not use 'glm::inverse(transform->ComputeTransformationMatrix())' as ViewMatrix
	// Because the Rotation part of CameraFrameMat is DIFFERENT from the Rotation from euler angles!!!
	// [We can konw euler angles of camera only works for view direction, not the whole camera's frame. 
	// That's why we could not just use the transformation to compute its viewMatrix]
}

glm::mat4 Camera::GetProjectionMatrix() const
{
	return glm::perspective(glm::radians(cParams[0]), cParams[1], cParams[2], cParams[3]);
}

float Camera::GetCameraParameter(CameraIndex _index) const
{
	return cParams[static_cast<int>(_index)];
}

void Camera::SetCameraParameter(CameraIndex _index, float _value)
{
	cParams[static_cast<int>(_index)] = _value;
}