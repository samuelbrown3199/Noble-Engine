#include "../ECS/Component.h"

#include <glm/glm.hpp>

namespace NobleCore
{
	/**
	*Transform component, stores position rotation and scale.
	*/
	struct Transform : public ComponentData<Transform>
	{
	private:
		glm::mat4 modelMat(glm::mat4());

	public:
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;

		Transform()
		{
			position = glm::vec3(0, 0, 0);
			rotation = glm::vec3(0, 0, 0);
			scale = glm::vec3(1, 1, 1);
		}

		Transform(glm::vec3 _position, glm::vec3 _rotation, glm::vec3 _scale)
		{
			position = _position;
			rotation = _rotation;
			scale = _scale;
		}
	};
}