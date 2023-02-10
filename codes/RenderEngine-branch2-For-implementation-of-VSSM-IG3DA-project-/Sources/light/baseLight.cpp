#include "baseLight.hpp"
#include "../helpers//utility.hpp"

using namespace IceRender;

BaseLight::BaseLight(const string& _name) :name(_name), type(LightType::NONE), transform(make_shared<Transform>()), color(Utility::oneV3), intensity(1.0f), renderShadow(false) {}
BaseLight::~BaseLight() { transform = nullptr; }

const string BaseLight::GetName() const { return name; }
shared_ptr<Transform> BaseLight::GetTransform() { return transform; }
LightType BaseLight::GetType() const { return type; }
glm::vec3 BaseLight::GetColor() const { return color; }
void BaseLight::SetIntensity(const float& _intensity) { intensity = _intensity; }
float BaseLight::GetIntensity() const { return intensity; }
bool BaseLight::IsRenderShadow() const { return renderShadow; }
void BaseLight::SetRenderShadow(const bool& _val) { renderShadow = _val; }