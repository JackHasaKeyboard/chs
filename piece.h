#pragma once

#include <string>

#include <glm/glm.hpp>

#include "shad.h"
#include "mesh.h"
#include "trans.h"

class Piece {
	public:
		Mesh* mesh;
		Prog* prog;
		Trans* trans;

		glm::vec3 loc;
		glm::vec3 rot;
		glm::mat4 mvp;

		bool
			t,
			active;

		Piece(std::string mesh, glm::vec3 loc, bool t) {
			this->mesh = new Mesh(mesh);
			this->prog = new Prog("purple");

			this->loc = loc;
			this->trans = new Trans;
			trans->setPos(loc);

			this->t = t;
			this->active = false;
		}

		void draw(const Cam& cam) {
			prog->use();

			// color
			glUniform1i(
				glGetUniformLocation(this->prog->id, "t"),
				this->t
			);

			// active
			glUniform1i(
				glGetUniformLocation(this->prog->id, "active"),
				this->active
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

			prog->unUse();
		}
};

class Pawn : public Piece {
	public:
		Pawn(glm::vec3 pos, bool t) : Piece(
			"pawn",
			pos,
			t
		) {
		}

		void mv(Cam& cam, glm::vec2 curs) {
		}
};

class Rook : public Piece {
	public:
		Rook(glm::vec3 pos, bool t) : Piece(
			"rook",
			pos,
			t
		) {
		}

		void mv(Cam& cam, glm::vec2 curs) {
		}
};

class Knight : public Piece {
	public:
		Knight(glm::vec3 pos, bool t) : Piece(
			"knight",
			pos,
			t
		) {
		}

		void mv(Cam& cam, glm::vec2 curs) {
		}
};

class Bishop : public Piece {
	public:
		Bishop(glm::vec3 pos, bool t) : Piece(
			"bishop",
			pos,
			t
		) {
		}

		void mv(Cam& cam, glm::vec2 curs) {
		}
};

class King : public Piece {
	public:
		King(glm::vec3 pos, bool t) : Piece(
			"king",
			pos,
			t
		) {
		}

		void mv(Cam& cam, glm::vec2 curs) {
		}
};

class Queen : public Piece {
	public:
		Queen(glm::vec3 pos, bool t) : Piece(
			"queen",
			pos,
			t
		) {
		}

		void mv(Cam& cam, glm::vec2 curs) {
		}
};
