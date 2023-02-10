#include "AABB.hpp"

using namespace IceRender;

AABB::AABB() { bounds[0] = glm::vec3(+INFINITY); bounds[1] = glm::vec3(-INFINITY); }
AABB::AABB(const glm::vec3& _min, const glm::vec3& _max) { bounds[0] = _min; bounds[1] = _max; }

void AABB::Recompute(const std::vector<glm::vec3>& _points)
{
	float x_min = +INFINITY, x_max = -INFINITY,
		y_min = +INFINITY, y_max = -INFINITY,
		z_min = +INFINITY, z_max = -INFINITY;
	for (auto point : _points) {
		x_min = std::min(x_min, point.x);
		x_max = std::max(x_max, point.x);
		y_min = std::min(y_min, point.y);
		y_max = std::max(y_max, point.y);
		z_min = std::min(z_min, point.z);
		z_max = std::max(z_max, point.z);
	}
	bounds[0] = glm::vec3(x_min, y_min, z_min);
	bounds[1] = glm::vec3(x_max, y_max, z_max);
}

void AABB::Extend(const glm::vec3& _point)
{
	for (int i = 0; i < 3; ++i)
	{
		if (_point[i] < bounds[0][i])
			bounds[0][i] = _point[i];
		if (_point[i] > bounds[1][i])
			bounds[1][i] = _point[i];
	}
}

void AABB::Extend(const std::shared_ptr<AABB> _other)
{
	this->Extend(_other->GetMin());
	this->Extend(_other->GetMax());
}

glm::vec3 AABB::GetCenter() const { return (bounds[0] + bounds[1]) / 2.0f; }
const glm::vec3 AABB::GetMin() const { return bounds[0]; }
const glm::vec3 AABB::GetMax() const { return bounds[1]; }

bool AABB::IsValid() const
{
	for (int i = 0; i < 3; i++)
	{
		if (bounds[0][i] > bounds[1][i])
			return false;
	}
	return true;
}

bool AABB::IsContains(const glm::vec3& _point)
{
	for (int i = 0; i < 3; i++)
		if (_point[i]<bounds[0][i] || _point[i]>bounds[1][i])
			return false;
	return true;
}
