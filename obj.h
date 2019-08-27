#pragma once

#include <string>

#include <glm/glm.hpp>

#include "shad.h"
#include "mesh.h"
#include "trans.h"

class Obj {
	public:
		Mesh* mesh;
		Prog* prog;
		Trans* trans;

		glm::vec3 loc;
		glm::mat4 mvp;

		Obj(std::string mesh, glm::vec3 loc) {
			this->mesh = new Mesh(mesh);
			this->prog = new Prog("purple");

			this->loc = loc;
			this->trans = new Trans;
			trans->setPos(loc);
		}

		void draw(const Cam& cam) {
			prog->use();

			// MVP
			this->mvp = trans->getMvp(cam);
			glUniformMatrix4fv(
				glGetUniformLocation(this->prog->id, "mvp"),
				1,
				GL_FALSE,
				glm::value_ptr(this->mvp)
			);

			mesh->draw();
		}
};
