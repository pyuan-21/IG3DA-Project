#include "transform.hpp"
#include "../helpers/utility.hpp"
#include <glm/gtx/euler_angles.hpp>


using namespace IceRender;

Transform::Transform()
{
	position = Utility::zeroV3;
	rotation = Utility::zeroV3;
	scale = Utility::oneV3;
}

Transform::Transform(const glm::vec3& _pos, const glm::vec3& _rot, const glm::vec3& _scale)
{
	position = _pos;
	rotation = _rot;
	scale = _scale;
}

void Transform::SetPosition(const glm::vec3& _pos) { position = _pos; }
void Transform::SetSubPosition(const float& _value, const int& _idx) { position[_idx] = _value; }
void Transform::SetRotation(const glm::vec3& _rot) { rotation = _rot; }
void Transform::SetSubRotation(const float& _value, const int& _idx) { rotation[_idx] = _value; }
void Transform::SetScale(const glm::vec3& _scale) { scale = _scale; }
void Transform::SetSubScale(const float& _value, const int& _idx) { scale[_idx] = _value; }

glm::vec3 Transform::GetPosition() const { return position; }
float Transform::GetSubPosition(const int& _idx) const { return position[_idx]; }
glm::vec3 Transform::GetRotation() const { return rotation; }
float Transform::GetSubRotation(const int& _idx) const { return rotation[_idx]; }
glm::vec3 Transform::GetScale() const { return scale; }
float Transform::GetSubScale(const int& _idx) const { return scale[_idx]; }
glm::quat Transform::GetQuaternion() const { return glm::normalize(glm::quat(rotation)); }
glm::mat4 Transform::GetRotationMat4() const 
{ 
	// same method to get Rotation Matrix
	//glm::mat4 rotMat = glm::rotate(glm::identity<glm::mat4>(), rotation[0], Utility::rightV3);
	//rotMat = glm::rotate(rotMat2, rotation[1], Utility::upV3);
	//rotMat = glm::rotate(rotMat2, rotation[2], Utility::backV3);
	// rotation order: x-y-z
	return glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z); 
}

glm::mat4 Transform::ComputeTransformationMatrix() const
{
	// transformation matrix build order: scaling -> rotation -> translation

	// if it's applied to Mesh(points set), the centroid of mesh will be at origin, in this case, scaling first makes sense, then rotation.
	glm::mat4 scaleMat = glm::scale(glm::identity<glm::mat4>(), scale);

	glm::mat4 rotMat = GetRotationMat4();

	glm::mat4 transMat = glm::translate(glm::identity<glm::mat4>(), position); // translation matrix

	return transMat * rotMat * scaleMat;
}

