#pragma once

#include "baseLight.hpp"


namespace IceRender
{
	class PointLight : public BaseLight
	{
	private:
		int range; // lighting range, which will affect its attenuation. Setting range to int_max means not using attenuation.
		glm::vec2 attenuation; // x/y are the linear/quadratic attenuation coefficient; keep constant equal to 1.

	public:
		PointLight(const string& _name);
		~PointLight();

		void SetRange(const int& _range);

		glm::vec2 GetAttenuation() const;
		
		/*
		TODO: check the survey of soft shader because some paper have already studied it well. My idea is like:
		case 1: light source is inside the bounding box of scene. In this case,
				divide the bounding box into two sub-bounding boxes, at least one of them is not containing the light source,
				and repeat it until all sub-bounding boxes are not containing the light source, then it becomes case 2.
				(Also consider fov in this case, if fove is too big, we need to divide it as well)
		case 2: light source is outside the bounding box of scene. (Not sure whether it's good idea or not)Connecting the center of bounding box and light source.
				This is the view direction. Similar to build the view matrix of camera, once we know the view direction,
				we can simply project 8 corners of bounding box into this view direction in order to get near, far plane.
				It can also derive the fov, ratio of height and width. Hence, the light space matrix is built tightly.
				Additionally, building and maintaining the bounding box of scene is simple thing, we can store min/max point of each mesh,
				then build the scene bounding box easily. Even add/remove the mesh, it is still simple to recompute the bounding box because we just need one iteration of all mesh.
		*/
		glm::mat4 GetLightSpaceMat(LightCamInfo& _lightCamInfo) override;
	};
}
