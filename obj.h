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

		bool t;

		Obj(std::string mesh, glm::vec3 loc, bool t) {
			this->mesh = new Mesh(mesh);
			this->prog = new Prog("purple");

			this->loc = loc;
			this->trans = new Trans;
			trans->setPos(loc);

			this->t = t;
		}

		void draw(const Cam& cam) {
			prog->use();

			// color
			glUniform1i(
				glGetUniformLocation(this->prog->id, "t"),
				this->t
			);

			// MVP
			this->mvp = trans->getMvp(cam);
			glUniformMatrix4fv(
				glGetUniformLocation(this->prog->id, "mvp"),
				1,
				GL_FALSE,
				glm::value_ptr(this->mvp)
			);

			// normal
			glm::mat4 norm = this->trans->getModel();
			glUniformMatrix4fv(
				glGetUniformLocation(this->prog->id, "norm"),
				1,
				GL_FALSE,
				glm::value_ptr(norm)
			);

			mesh->draw();
		}
};
