#pragma once

#include <string>

#include "shad.h"
#include "mesh.h"

class Obj {
	public:
		Mesh* mesh;
		Prog* prog;

		Obj(std::string mesh) {
			this->mesh = new Mesh(mesh);
			this->prog = new Prog("purple");
		}

		void draw() {
			prog->use();
			mesh->draw();
		}
};
