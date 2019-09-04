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
			this->legal.push_back(
				glm::vec2(
					0.0,
					(-4.0) + this->loc.z
				)
			);
		}
};

class Rook : public Piece {
	public:
		Rook(glm::vec3 pos, bool t) : Piece(
			"rook",
			pos,
			t
		) {
			for (
				int x = -4;
				x < 4;
				x++
			) {
				this->legal.push_back(
					glm::vec2(
						(float) (this->loc.x + x),
						0.0
					)
				);
			}

			for (
				int z = -4;
				z < 4;
				z++
			) {
				this->legal.push_back(
					glm::vec2(
						0.0,
						(float) (this->loc.z + z)
					)
				);
			}
		}
};

class Knight : public Piece {
	public:
		Knight(glm::vec3 pos, bool t) : Piece(
			"knight",
			pos,
			t
		) {
			for (
				int i = 0;
				i < 2;
				i++
			) {
				for (
					int x = 0;
					x < 2;
					x++
				) {
					for (
						int z = 0;
						z < 2;
						z++
					) {
						this->legal.push_back(
							glm::vec2(
								(float) (loc.x + ((-1 * x) * 3) + (-1 * i)),
								(float) (loc.z + ((-1 * z) * 3) + (1 * i))
							)
						);
					}
				}
			}
		}
};

class Bishop : public Piece {
	public:
		Bishop(glm::vec3 pos, bool t) : Piece(
			"bishop",
			pos,
			t
		) {
			for (
				int p = -4;
				p < 4;
				p++
			) {
				this->legal.push_back(
					glm::vec2(
						(float) (loc.x + p),
						(float) (loc.z + p)
					)
				);
			}

			for (
				int p = -4;
				p < 4;
				p++
			) {
				this->legal.push_back(
					glm::vec2(
						(float) (loc.x + -p),
						(float) (loc.z + p)
					)
				);
			}
		}
};

class King : public Piece {
	public:
		King(glm::vec3 pos, bool t) : Piece(
			"king",
			pos,
			t
		) {
			for (
				int x = 0;
				x < 2;
				x++
			) {
				for (
					int z = 0;
					z < 2;
					z++
				) {
					this->legal.push_back(
						glm::vec2(
							(float) this->loc.x + ((-1 * x) * 1),
							(float) this->loc.z + ((-1 * z) * 1)
						)
					);
				}
			}
		}
};

class Queen : public Piece {
	public:
		Queen(glm::vec3 pos, bool t) : Piece(
			"queen",
			pos,
			t
		) {
			for (
				int x = -4;
				x < 4;
				x++
			) {
				this->legal.push_back(
					glm::vec2(
						(float) x,
						0.0
					)
				);
			}

			for (
				int z = -4;
				z < 4;
				z++
			) {
				this->legal.push_back(
					glm::vec2(
						0.0,
						(float) z
					)
				);
			}

			for (
				int p = -4;
				p < 4;
				p++
			) {
				this->legal.push_back(
					glm::vec2(
						(float) p,
						(float) p
					)
				);
			}

			for (
				int p = -4;
				p < 4;
				p++
			) {
				this->legal.push_back(
					glm::vec2(
						(float) -p,
						(float) p
					)
				);
			}
		}
};
