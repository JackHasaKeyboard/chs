#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <string>

struct Cam {
	public:
		glm::mat4 proj;
		glm::vec3
			pos,
			fwd,
			up;

		bool orientation;

		Cam(
			const glm::vec3& pos,
			float fov,
			float aspect,
			float zNear,
			float zFar
		) {
			this->pos = pos;
			this->fwd = glm::vec3(0.0f, 0.0f, 1.0f);
			this->up = glm::vec3(0.0f, 1.0f, 0.0f);
			this->proj = glm::perspective(fov, aspect, zNear, zFar);

			this->orientation = false;
		}

		inline glm::mat4 getViewProj() {
			if (this->orientation) {
				this->pos = glm::vec3(0, 40, 0);

				return proj * glm::lookAt(
					pos,
					fwd,
					up
				);
			} else {
				this->pos = glm::vec3(-15, 40, -15);

				return proj * glm::lookAt(
					pos,
					fwd,
					up
				);
			}
		}

		void pitch(float angle) {
			glm::vec3 right = glm::normalize(glm::cross(up, fwd));

			fwd = glm::vec3(glm::normalize(glm::rotate(angle, right) * glm::vec4(fwd, 0.0)));
			up = glm::normalize(glm::cross(fwd, right));
		}
};
