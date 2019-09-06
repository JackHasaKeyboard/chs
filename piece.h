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
			glm::vec3 dist;
			dist[0] = (float) (((-4 * 2) + (loc[0] * 2)) + 1);
			dist[1] = (float) loc[1];
			dist[2] = (float) (((-4 * 2) + (loc[2] * 2)) + 1);
			trans->setPos(dist);

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

		void mv(Cam& cam, glm::vec3 loc) {
			this->loc[0] = (float) ((-8 + (loc[0] * 2)) + 1);
			this->loc[2] = (float) ((-8 + (loc[2] * 2)) + 1);
			this->trans->setPos(this->loc);
		}
};

class Pawn : public Piece {
	public:
		Pawn(glm::vec3 pos, bool t) : Piece(
			"pawn",
			pos,
			t
		) {
			signed int side = t ? 1 : -1;

			this->legal.push_back(
				glm::vec2(
					-4 + this->loc.x,
					-4 + this->loc.z - side
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
				int x = -8;
				x < 8;
				x++
			) {
				this->legal.push_back(
					glm::vec2(
						-4 + this->loc.x + x,
						-4 + this->loc.z
					)
				);
			}

			for (
				int z = -8;
				z < 8;
				z++
			) {
				this->legal.push_back(
					glm::vec2(
						-4 + this->loc.x,
						-4 + this->loc.z + z
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
								loc.x + ((-1 * x) * 3) + (-1 * i),
								loc.z + ((-1 * z) * 3) + (1 * i)
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
				int p = -8;
				p < 8;
				p++
			) {
				this->legal.push_back(
					glm::vec2(
						-4 + loc.x + p,
						-4 + loc.z + p
					)
				);
			}

			for (
				int p = -8;
				p < 8;
				p++
			) {
				this->legal.push_back(
					glm::vec2(
						-4 + loc.x + -p,
						-4 + loc.z + p
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
				signed int side = x ? 1 : -1;

				this->legal.push_back(
					glm::vec2(
						-4 + this->loc.x + side,
						-4 + this->loc.z
					)
				);
			}

			for (
				int x = 0;
				x < 2;
				x++
			) {
				signed int side = x ? 1 : -1;

				this->legal.push_back(
					glm::vec2(
						-4 + this->loc.x,
						-4 + this->loc.z + side
					)
				);
			}

			for (
				int x = 0;
				x < 2;
				x++
			) {
				signed int side = x ? 1 : -1;

				this->legal.push_back(
					glm::vec2(
						-4 + this->loc.x + side,
						-4 + this->loc.z + side
					)
				);
			}

			for (
				int x = 0;
				x < 2;
				x++
			) {
				signed int side = x ? 1 : -1;

				this->legal.push_back(
					glm::vec2(
						-4 + this->loc.x + side,
						-4 + this->loc.z - side
					)
				);
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
				int x = -8;
				x < 8;
				x++
			) {
				this->legal.push_back(
					glm::vec2(
						-4 + this->loc.x + x,
						this->loc.z + -4
					)
				);
			}

			for (
				int z = -8;
				z < 8;
				z++
			) {
				this->legal.push_back(
					glm::vec2(
						-4 + this->loc.x + 0,
						this->loc.z + -4 + z
					)
				);
			}

			for (
				int d0 = -8;
				d0 < 8;
				d0++
			) {
				this->legal.push_back(
					glm::vec2(
						-4 + this->loc.x + d0,
						this->loc.z + -4 + d0
					)
				);
			}

			for (
				int d1 = -8;
				d1 < 8;
				d1++
			) {
				this->legal.push_back(
					glm::vec2(
						-4 + this->loc.x + -d1,
						this->loc.z + -4 + d1
					)
				);
			}
		}
};
