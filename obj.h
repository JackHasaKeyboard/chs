#pragma once

#include <string>

#include "shad.h"
#include "mesh.h"
#include "trans.h"

class Obj {
	public:
		Mesh* mesh;
		Prog* prog;
		Trans* trans;

		glm::mat4 mvp;

		Obj(std::string mesh) {
			this->mesh = new Mesh(mesh);
			this->prog = new Prog("purple");
			this->trans = new Trans;
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
