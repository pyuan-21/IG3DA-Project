#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>


namespace IceRender
{
	class AABB
	{
	private:
		glm::vec3 bounds[2]; // min, max points

	public:
		AABB();
		AABB(const glm::vec3& _min, const glm::vec3& _max);

		void Recompute(const std::vector<glm::vec3>& _points);
		void Extend(const glm::vec3& _point);
		void Extend(const std::shared_ptr<AABB> _other);
		glm::vec3 GetCenter() const;
		const glm::vec3 GetMin() const;
		const glm::vec3 GetMax() const;
		bool IsValid() const; // check whether AABB has been intialized correctly
		bool IsContains(const glm::vec3& _point); // check whether AABB contains this point
	};
}
