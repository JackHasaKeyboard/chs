#include <vector>
#include <iostream>
#include <algorithm>

#include <SDL2/SDL_ttf.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "disp.h"
#include "shad.h"
#include "trans.h"
#include "piece.h"

void print(Disp& disp, std::string msg) {
	TTF_Init();

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  gluOrtho2D(0, 800, 0, 600);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);

  TTF_Font* font = TTF_OpenFont("terminus.bdf", 12);

  SDL_Surface* surf = TTF_RenderText_Blended(font, std::string("$ " + msg).c_str(), {95, 82, 134});

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surf->w, surf->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surf->pixels);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex2f(0, 0);
	glTexCoord2f(1, 1); glVertex2f(surf->w, 0);
	glTexCoord2f(1, 0); glVertex2f(surf->w, surf->h);
	glTexCoord2f(0, 0); glVertex2f(0, surf->h);
	glEnd();

  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);

  TTF_CloseFont(font);
  SDL_FreeSurface(surf);
  glDeleteTextures(1, &tex);
}

int main() {
	Disp disp("chs", 800, 600);
	Cam cam(glm::vec3(0, 20, 7.5), 70.0f, 800.0 / 600.0, 0.001f, 100000.0f);

	Prog board("checker");

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	const GLfloat vtc[] = {
		-8.32, 0.32, -8.32,
		8.32, 0.32, -8.32,
		8.32, 0.32, 8.32,
		-8.32, 0.32, 8.32
	};

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof vtc, vtc, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(0);

	Trans trans;
	glm::mat4 mvp = trans.getMvp(cam);
	bool t = false;

	bool turn = false;
	std::vector<Piece> coll;
	std::vector<Piece> graveYard;

	std::vector<glm::vec2> legal;

	for (
		unsigned int team = 0;
		team < 2;
		team++
	) {
		for (
			unsigned int x = 0;
			x < 8;
			x++
		) {
			signed int dir = team ? 1 : -1;

			Pawn pawn(
				glm::vec3(
					x,
					0,
					(7 * team) - dir
				),
				(7 * team),
				legal
			);

			legal.clear();
			legal.push_back(
				glm::vec2(
					-4 + pawn.loc.x,
					-4 + pawn.loc.z - dir
				)
			);
			pawn.legal = legal;

			coll.push_back(pawn);
		}

		for (
			unsigned int x = 0;
			x < 2;
			x++
		) {
			signed int side = x ? 1 : -1;

			Rook rook(
				glm::vec3(
					(x * 7),
					0,
					(7 * team)
				),
				t,
				legal
			);

			legal.clear();
			for (
				int x = -8;
				x < 8;
				x++
			) {
				legal.push_back(
					glm::vec2(
						-4 + x,
						-4 + rook.loc.z
					)
				);
			}
			for (
				int z = -8;
				z < 8;
				z++
			) {
				legal.push_back(
					glm::vec2(
						-4 + rook.loc.x,
						-4 + z
					)
				);
			}

			rook.legal = legal;

			coll.push_back(rook);
		}

		for (
			unsigned int x = 0;
			x < 2;
			x++
		) {
			signed int side = x ? 1 : -1;

			Knight knight(
				glm::vec3(
					(x * 7) - (1 * side),
					0, (7 * team)
				),
				t,
				legal
			);

			legal.clear();
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
						legal.push_back(
							glm::vec2(
								knight.loc.x + ((-1 * x) * 3) + (-1 * i),
								knight.loc.z + ((-1 * z) * 3) + (1 * i)
							)
						);
					}
				}
			}

			knight.legal = legal;

			coll.push_back(knight);
		}

		for (
			unsigned int x = 0;
			x < 2;
			x++
		) {
			signed int side = x ? 1 : -1;

			Bishop bishop(
				glm::vec3(
					(x * 7) - (2 * side),
					0,
					(7 * team)
				),
				t,
				legal
			);

			for (
				int p = -8;
				p < 8;
				p++
			) {
				legal.push_back(
					glm::vec2(
						-4 + bishop.loc.x + p,
						-4 + bishop.loc.z + p
					)
				);
			}

			for (
				int p = -8;
				p < 8;
				p++
			) {
				legal.push_back(
					glm::vec2(
						-4 + bishop.loc.x + -p,
						-4 + bishop.loc.z + p
					)
				);
			}

			bishop.legal = legal;

			coll.push_back(bishop);
		}

		unsigned int x = 0;
		signed int side;
		{
			side = x ? 1 : -1;

			King king(
				glm::vec3(
					3 + ((-1 * side) * team),
					0,
					(7 * team)
				),
				t,
				legal
			);

			legal.clear();
			for (
				int x = 0;
				x < 2;
				x++
			) {
				signed int side = x ? 1 : -1;

				legal.push_back(
					glm::vec2(
						-4 + king.loc.x + side,
						-4 + king.loc.z
					)
				);
			}
			for (
				int x = 0;
				x < 2;
				x++
			) {
				signed int side = x ? 1 : -1;

				legal.push_back(
					glm::vec2(
						-4 + king.loc.x,
						-4 + king.loc.z + side
					)
				);
			}
			for (
				int x = 0;
				x < 2;
				x++
			) {
				signed int side = x ? 1 : -1;

				legal.push_back(
					glm::vec2(
						-4 + king.loc.x + side,
						-4 + king.loc.z + side
					)
				);
			}
			for (
				int x = 0;
				x < 2;
				x++
			) {
				signed int side = x ? 1 : -1;

				legal.push_back(
					glm::vec2(
						-4 + king.loc.x + side,
						-4 + king.loc.z - side
					)
				);
			}

			king.legal = legal;

			coll.push_back(king);

			x++;
		}

		{
			side = x ? 1 : -1;

			Queen queen(
				glm::vec3(
					4 + ((-1 * side) * team),
					0,
					(7 * team)
				),
				t,
				legal
			);

			legal.clear();
			for (
				int x = -8;
				x < 8;
				x++
			) {
				legal.push_back(
					glm::vec2(
						-4 + queen.loc.x + x,
						queen.loc.z + -4
					)
				);
			}
			for (
				int z = -8;
				z < 8;
				z++
			) {
				legal.push_back(
					glm::vec2(
						-4 + queen.loc.x + 0,
						queen.loc.z + -4 + z
					)
				);
			}
			for (
				int d0 = -8;
				d0 < 8;
				d0++
			) {
				legal.push_back(
					glm::vec2(
						-4 + queen.loc.x + d0,
						queen.loc.z + -4 + d0
					)
				);
			}
			for (
				int d1 = -8;
				d1 < 8;
				d1++
			) {
				legal.push_back(
					glm::vec2(
						-4 + queen.loc.x + -d1,
						queen.loc.z + -4 + d1
					)
				);
			}

			queen.legal = legal;

			coll.push_back(queen);

			x++;
		}
	}

	int p = 0;
	coll[p].active = true;

	SDL_Event e;
	bool down = false;
	int
		start[2],
		curr[2];

	float delta;

	bool run = true;
	std::string buff;

	unsigned int c = 0;
	glm::vec2 curs = coll[p + (turn * 16)].legal[c];

	while (run) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				run = false;
			}

			if (e.type == SDL_MOUSEBUTTONDOWN) {
				down = true;

				SDL_GetMouseState(&start[0], &start[1]);
			}

			if (e.type == SDL_MOUSEMOTION) {
				if (down) {
					SDL_GetMouseState(&curr[0], &curr[1]);

					delta = (float) ((float) start[0] - (float) curr[0]);

					trans.angle = delta / 10000.0f;
					mvp = trans.getMvp(cam);

					for (Piece& piece : coll) {
						piece.trans->angle = delta / 10000.0f;
						piece.mvp = trans.getMvp(cam);
					}
				}
			}

			if (e.type == SDL_MOUSEBUTTONUP) {
				down = false;
			}

			if (e.type == SDL_KEYDOWN) {
				GLchar
					code = e.key.keysym.scancode,
					k = e.key.keysym.sym;

				if (k > ' ' && k < '~') {
					buff.push_back(k);
				}

				if (k == SDLK_BACKSPACE) {
					if (!buff.empty()) {
						buff.pop_back();
					}
				}

				if (k == SDLK_RETURN) {
					buff.clear();
				}

				if (code == SDL_SCANCODE_F5) {
					cam.orientation = !cam.orientation;

					for (Piece& piece : coll) {
						piece.rot[0] = cam.orientation * 90;
						piece.trans->setRot(piece.rot);
					}
				}

				if (code == SDL_SCANCODE_DOWN) {
					if (c > 0) {
						c--;
					}

					curs = coll[p + (turn * 16)].legal[c];
				}

				if (code == SDL_SCANCODE_UP) {
					if (c < coll[p + (turn * 16)].legal.size() - 1) {
						c++;
					}

					curs = coll[p + (turn * 16)].legal[c];
				}

				if (code == SDL_SCANCODE_TAB) {
					if (p < 15) {
						p++;
					} else {
						p = 0;
					}

					for (Piece& piece : coll) {
						piece.active = false;
					}
					coll[p + (turn * 16)].active = true;

					c = 0;
					curs = coll[p + (turn * 16)].legal[c];
				}

				if (code == SDL_SCANCODE_RETURN) {
					graveYard.push_back(coll[p + (turn * 16)]);
					int inc = 0;
					for (Piece& piece : graveYard) {
						piece.mv(
							cam,
							glm::vec3(
								(float) (-4 + inc),
								0,
								(float) (-4 + -2)
							)
						);

						inc++;
					}

					coll.erase(
						std::remove_if(
							coll.begin(),
							coll.end(),
							[&] (Piece piece) {
								return
									piece.loc.x == 4 + curs[0] &&
									piece.loc.z == 4 + curs[1];
							}
						),
						coll.end()
					);

					coll[p + (turn * 16)].mv(
						cam,
						glm::vec3(
							curs[0],
							0,
							curs[1]
						)
					);

					c = 0;
					turn = !turn;
					p = 0;

					for (Piece& piece : coll) {
						piece.active = false;
					}
					coll[p + (turn * 16)].active = true;
				}

				curs = coll[p + (turn * 16)].legal[c];
			}

			disp.clear(0, 0, 0, 1);

			board.use();

			// MVP
			glUniformMatrix4fv(
				glGetUniformLocation(board.id, "mvp"),
				1,
				GL_FALSE,
				glm::value_ptr(mvp)
			);

			// team
			glUniform1i(
				glGetUniformLocation(board.id, "t"),
				t
			);

			// cursor
			glUniform2fv(
				glGetUniformLocation(board.id, "curs"),
				1,
				glm::value_ptr(coll[p + (turn * 16)].legal[c])
			);

			// legal
			std::vector<glm::vec2> legal = coll[p + (turn * 16)].legal;

			for (
				int i = 0;
				i < legal.size();
				i++
			) {
				glUniform2f(
					glGetUniformLocation(board.id, std::string("legal[" + std::to_string(i) + "]").c_str()),
					legal[i][0],
					legal[i][1]
				);
			}

			glBindVertexArray(vao);
			glDrawArrays(GL_QUADS, 0, 4);
			board.unUse();

			for (Piece& piece : coll) {
				piece.draw(cam);
			}

			for (Piece& piece : graveYard) {
				piece.draw(cam);
			}

			print(disp, buff);

			disp.update();
		}
	}

	return 0;
}
