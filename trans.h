#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "cam.h"

struct Trans {
	public:
		glm::vec3
			pos,
			rot,
			scale;

		float angle;

		Trans(
			const glm::vec3& pos = glm::vec3(),
			const glm::vec3& rot = glm::vec3(),
			const glm::vec3& scale = glm::vec3(
				1.0f,
				1.0f,
				1.0f
			)
		) {
			this->pos = pos;
			this->rot = rot;
			this->scale = scale;

			this->angle = 0.0f;
		}

		inline glm::mat4 getModel() const {
			glm::mat4 posMat = glm::translate(pos);
			glm::mat4 scaleMat = glm::scale(scale);

			glm::mat4 rotX = glm::rotate(rot.x, glm::vec3(1.0, 0.0, 0.0));
			glm::mat4 rotY = glm::rotate(rot.y, glm::vec3(0.0, 1.0, 0.0));
			glm::mat4 rotZ = glm::rotate(rot.z, glm::vec3(0.0, 0.0, 1.0));
			glm::mat4 rotMat = rotX * rotY * rotZ;

			return posMat * rotMat * scaleMat;
		}

		inline glm::mat4 getMvp(Cam cam) const {
			glm::mat4 vp = cam.getViewProj();
			glm::mat4 m = getModel();

			glm::mat4 orbit;
			glm::mat4 rotXWorld = glm::rotate(0.0f, glm::vec3(1.0, 0.0, 0.0));
			glm::mat4 rotYWorld = glm::rotate(glm::degrees(this->angle), glm::vec3(0.0, 1.0, 0.0));
			glm::mat4 rotZWorld = glm::rotate(0.0f, glm::vec3(0.0, 0.0, 1.0));
			orbit = rotXWorld * rotYWorld * rotZWorld;

			return (vp * orbit) * m;
		}

		inline glm::vec3* getPos() {
			return &pos;
		}
		inline glm::vec3* getRot() {
			return &rot;
		}
		inline glm::vec3* getScale() {
			return &scale;
		}

		inline void setPos(glm::vec3& pos) {
			this->pos = pos;
		}
		inline void setRot(glm::vec3& rot) {
			this->rot = rot;
		}
		inline void setScale(glm::vec3& scale) {
			this->scale = scale;
		}
};
