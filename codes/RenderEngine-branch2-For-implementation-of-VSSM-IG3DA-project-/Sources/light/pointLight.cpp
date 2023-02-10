#define _USE_MATH_DEFINES

#include "pointLight.hpp"
#include "../globals.hpp"
#include "../helpers/utility.hpp"
#include <math.h>

using namespace IceRender;

PointLight::PointLight(const string& _name) :BaseLight(_name), range(std::numeric_limits<int>::max()), attenuation(0, 0) { type = LightType::POINT; }
PointLight::~PointLight() {}

void PointLight::SetRange(const int& _range)
{
	range = _range;
	if (range == std::numeric_limits<int>::max())
		attenuation = glm::vec2(0, 0); // not using attenuation
	else
	{
		// get attenuation coefficient from Map
		int rangeMax = 0;
		int boundMax = 0;
		for (auto it = GLOBAL.attenuationMap.begin(); it != GLOBAL.attenuationMap.end(); it++)
		{
			if (it->first > rangeMax)
				rangeMax = it->first;
			if (range <= it->first && it->first > boundMax)
				boundMax = it->first;
		}
		if (boundMax == 0)
			attenuation = GLOBAL.attenuationMap[rangeMax]; // if boundMax is 0 which means _range is greater than all these range. In this case just use the rangeMax.
		else
			attenuation = GLOBAL.attenuationMap[boundMax];
	}
}

glm::vec2 PointLight::GetAttenuation() const { return attenuation; }

glm::mat4 PointLight::GetLightSpaceMat(LightCamInfo& _lightCamInfo)
{
	// NOTE: A good way to test whether this LightMat is correct is to use these mat directly in some shader(just replace camera's matrix with them)
	auto sceneBox = GLOBAL.sceneMgr->GetBoundingBox();
	glm::vec3 center = sceneBox->GetCenter();
	glm::vec3 min = sceneBox->GetMin();
	glm::vec3 max = sceneBox->GetMax();
	glm::vec3 size = max - min;

	// consider light source is a camera now
	glm::vec3 right, up;
	_lightCamInfo.lightCamPos = transform->GetPosition();
	_lightCamInfo.lightViewDir = glm::normalize(center - _lightCamInfo.lightCamPos);
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

	// now to build light perspective projection matrix
	float fov, aspect; // here fov is in radians not degree
	if (!GLOBAL.shadowMgr->IsUseTightSpace() || sceneBox->IsContains(_lightCamInfo.lightCamPos))
	{
		// TODO: a way to handle inside bounding box is to divide the space. (Check paper later) -> see below
		// Acutally from what I see from lots of games, only one sun for rendering the shadow and it is definite outside of the bounding box of the whole scene
		// if enter the room something, the logic is similar, the light on the ceil for instance, will be ouside of the inner scene bounding box(the bounding box of the thing in the room)
		// Therefore, there is no need to concern about whether it is inside bounding box or not!
		// Print("WARNING: light source is inside the scene bounding box. Therefore, predefined matrix is used.");
		_lightCamInfo.near = GLOBAL.camNear;
		_lightCamInfo.far = std::min(GLOBAL.camFar, static_cast<float>(range));
		fov = glm::radians(GLOBAL.camFOV);
		aspect = static_cast<float>(GLOBAL.WIN_WIDTH) / static_cast<float>(GLOBAL.WIN_HEIGHT);
	}
	else
	{
		// The purpose for computing the tight light view frustum is to reduce the aliasing, peter panning, z-fighting and etc issues.
		// refer: https://learn.microsoft.com/en-us/windows/win32/dxtecharts/common-techniques-to-improve-shadow-depth-maps?redirectedfrom=MSDN#light-frustum-intersected-with-scene-to-calculate-near-and-far-planes
		// but in my experiment, it is not good for sovling the shadow acne when adding bias.
		// NOTE: below method only works when light source is outside the bounding box.
		// build perspective projection matrix of light
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

		_lightCamInfo.near = _lightCamInfo.far = glm::dot(p[0] - _lightCamInfo.lightCamPos, _lightCamInfo.lightViewDir); // pick the first one as initialized value
		fov = glm::dot(glm::normalize(p[0] - _lightCamInfo.lightCamPos), _lightCamInfo.lightViewDir);
		// find the minimal/maximal distance(near/far) along with view direction of light
		// find the maximal fov(in radians) between lp_i and lc, where l is light source position, p_i is p[i], c is center of bounding box
		for (int i = 0; i < 8; i++)
		{
			auto vec = p[i] - _lightCamInfo.lightCamPos;
			float value = glm::dot(vec, _lightCamInfo.lightViewDir);
			if (value < _lightCamInfo.near)
				_lightCamInfo.near = value;
			if (value > _lightCamInfo.far)
				_lightCamInfo.far = value;
			value = value / glm::length(vec);
			if (value > fov)
				fov = value;
		}

		shared_ptr<BasicShadowMapRender> basicShadowMapRender;
		if (GLOBAL.shadowMgr->IsNeedShadowRender())
			basicShadowMapRender = dynamic_pointer_cast<BasicShadowMapRender>(GLOBAL.shadowMgr->GetShadowRender());
		if (basicShadowMapRender == NULL)
			aspect = static_cast<float>(GLOBAL.WIN_WIDTH) / static_cast<float>(GLOBAL.WIN_HEIGHT);
		else
		{
			int w, h;
			basicShadowMapRender->GetResolution(w, h);
			aspect = static_cast<float>(w) / static_cast<float>(h);
		}

		_lightCamInfo.far = std::min(_lightCamInfo.far, static_cast<float>(range)); // TODO: maybe too small range will cause some issues.
	}

	glm::mat4 lightProjMat = glm::perspective(fov, aspect, _lightCamInfo.near, _lightCamInfo.far);

	return lightProjMat * lightViewMat;
}
