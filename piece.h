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

		std::vector<glm::vec2> legal;

		Piece(
			std::string mesh,
			glm::vec3 loc,
			bool t,
			std::vector<glm::vec2> legal
		) {
			this->mesh = new Mesh(mesh);
			this->prog = new Prog("purple");

			this->loc = loc;
			this->trans = new Trans;
			glm::vec3 dist;
			dist[0] = (float) (((-4 * 2) + (loc[0] * 2)) + 1);
			dist[1] = (float) loc[1];
			dist[2] = (float) (((-4 * 2) + (loc[2] * 2)) + 1);
			trans->setPos(dist);

			this->t = t;
			this->active = false;

			this->legal = legal;
			this->legal.push_back(
				glm::vec2(
					-4 + this->loc.x,
					-4 + this->loc.z
				)
			);
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

		void mv(Cam& cam, glm::vec3 loc) {
			this->loc[0] = (float) ((loc[0] * 2)) + 1;
			this->loc[2] = (float) ((loc[2] * 2)) + 1;
			this->trans->setPos(this->loc);
		}
};

class Pawn : public Piece {
	public:
		Pawn(
			glm::vec3 pos,
			bool t,
			std::vector<glm::vec2> legal
		) : Piece(
			"pawn",
			pos,
			t,
			legal
		) {
		}
};

class Rook : public Piece {
	public:
		Rook(
			glm::vec3 pos,
			bool t,
			std::vector<glm::vec2> legal
		) : Piece(
			"rook",
			pos,
			t,
			legal
		) {
		}
};

class Knight : public Piece {
	public:
		Knight(
			glm::vec3 pos,
			bool t,
			std::vector<glm::vec2> legal
		) : Piece(
			"knight",
			pos,
			t,
			legal
		) {
		}
};

class Bishop : public Piece {
	public:
		Bishop(
			glm::vec3 pos,
			bool t,
			std::vector<glm::vec2> legal
		) : Piece(
			"bishop",
			pos,
			t,
			legal
		) {
		}
};

class King : public Piece {
	public:
		King(
			glm::vec3 pos,
			bool t,
			std::vector<glm::vec2> legal
		) : Piece(
			"king",
			pos,
			t,
			legal
		) {
		}
};

class Queen : public Piece {
	public:
		Queen(
			glm::vec3 pos,
			bool t,
			std::vector<glm::vec2> legal
		) : Piece(
			"queen",
			pos,
			t,
			legal
		) {
		}
};
