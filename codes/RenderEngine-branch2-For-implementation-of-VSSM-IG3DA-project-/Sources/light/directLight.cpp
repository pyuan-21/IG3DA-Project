#define _USE_MATH_DEFINES

#include "directLight.hpp"
#include "../globals.hpp"
#include <limits>
#include "../helpers/utility.hpp"
#include <math.h>

using namespace IceRender;

DirectLight::DirectLight(const string& _name) : BaseLight(_name), direction(glm::normalize(glm::vec3(-1))) { type = LightType::DIRECT; }
DirectLight::~DirectLight() {}

void DirectLight::SetDirection(const glm::vec3 _dir) { direction = glm::normalize(_dir); }
glm::vec3 DirectLight::GetDirection() const { return direction; }

glm::mat4 DirectLight::GetLightSpaceMat(LightCamInfo& _lightCamInfo)
{
	// NOTE: A good way to test whether this LightMat is correct is to use these mat directly in some shader(just replace camera's matrix with them)
	auto sceneBox = GLOBAL.sceneMgr->GetBoundingBox();
	glm::vec3 center = sceneBox->GetCenter();
	glm::vec3 min = sceneBox->GetMin();
	glm::vec3 max = sceneBox->GetMax();
	glm::vec3 size = max - min;

	// then use this radisu to compute the virtual light-camera position
	float radius = glm::length(size);
	// consider light source is a camera now
	// Note: the position of direct light is meaningless! Only the light direction is meaningful.
	glm::vec3 right, up;
	_lightCamInfo.lightViewDir = direction;
	_lightCamInfo.lightCamPos = center - _lightCamInfo.lightViewDir * radius;
	// check whether _lightViewDir is almost parallel with up direction
	float angle = acosf(abs(glm::dot(_lightCamInfo.lightViewDir, Utility::upV3))) * 180.0f / M_PI;
	if (angle <= Utility::zeroFlag)
	{
		up = glm::vec3(0, 0, -1); // when view direction is parallel with the (0,1,0), it doesn't matter which the up direction we choose. Hence, we can just pick (0,0,-1)
		right = glm::normalize(glm::cross(_lightCamInfo.lightViewDir, up));
	}
	else
	{
		right = glm::normalize(glm::cross(_lightCamInfo.lightViewDir, Utility::upV3));
		up = glm::normalize(glm::cross(right, _lightCamInfo.lightViewDir));
	}
	glm::mat4 lightViewMat = glm::lookAt(_lightCamInfo.lightCamPos, center, up);

	// now to build light orthogonal projection matrix
	float halfWidth, halfHeight;
	// first to find near/far
	glm::vec3 p[8]; // 8 corners of scene bounding box
	p[0] = min;
	p[1] = min + glm::vec3(size.x, 0, 0);
	p[2] = min + glm::vec3(0, size.y, 0);
	p[3] = min + glm::vec3(size.x, size.y, 0);

	p[4] = max;
	p[5] = max - glm::vec3(size.x, 0, 0);
	p[6] = max - glm::vec3(0, size.y, 0);
	p[7] = max - glm::vec3(size.x, size.y, 0);

	_lightCamInfo.near = std::numeric_limits<float>::max();
	_lightCamInfo.far = halfWidth = halfHeight = 0;
	for (int i = 0; i < 8; i++)
	{
		glm::vec4 p_homo = glm::vec4(p[i], 1); // homogeneous point
		p_homo = lightViewMat * p_homo;
		p[i] = glm::vec3(p_homo.x, p_homo.y, p_homo.z); // get position in light view space 
		if (abs(p[i].x) > halfWidth)
			halfWidth = abs(p[i].x);
		if (abs(p[i].y) > halfHeight)
			halfHeight = abs(p[i].y);
		if (p[i].z < 0)
		{
			if (abs(p[i].z) < _lightCamInfo.near)
				_lightCamInfo.near = abs(p[i].z);
			if (abs(p[i].z) > _lightCamInfo.far)
				_lightCamInfo.far = abs(p[i].z);
		}
	}
	glm::mat4 lightProjMat = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, _lightCamInfo.near, _lightCamInfo.far);

	return lightProjMat * lightViewMat;
}
