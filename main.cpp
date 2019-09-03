#include <vector>
#include <iostream>

#include <SDL2/SDL_ttf.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "disp.h"
#include "shad.h"
#include "trans.h"
#include "piece.h"

void renderText(Disp& disp, std::string msg) {
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

  SDL_Surface* surf = TTF_RenderText_Blended(
		font,
		std::string("$ " + msg).c_str(),
		{95, 82, 134}
  );

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		surf->w,
		surf->h,
		0,
		GL_BGRA,
		GL_UNSIGNED_BYTE,
		surf->pixels
  );

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
	Cam cam(
		glm::vec3(0, 20, 7.5),
		70.0f,
		800.0 / 600.0,
		0.001f,
		100000.0f
	);

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

	glm::vec2 curs = glm::vec2(1, 0);
	Trans trans;
	glm::mat4 mvp = trans.getMvp(cam);
	bool t = false;

	bool turn = false;
	std::vector<Piece> coll;

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
			coll.push_back(Pawn(
				glm::vec3(
					(-4 * 2.0) + (x * 2.0),
					0.0,
					8.0 + ((-1.0 * team) * 16.0)
				),
				t
			));
		}

		for (
			unsigned int x = 0;
			x < 2;
			x++
		) {
			bool side = x % 2;

			coll.push_back(Rook(
				glm::vec3(
					8.0 + ((-1.0 * side) * (8.0 * 2.0)),
					0,
					8.0 + ((-1.0 * team) * 16.0) + 2.0
				),
				t
			));
		}

		for (
			unsigned int x = 0;
			x < 2;
			x++
		) {
			bool side = x % 2;

			coll.push_back(Knight(
				glm::vec3(
					-6.0 + ((-1.0 * side) * (-6.0 * 2.0)),
					0,
					8.0 + ((-1.0 * team) * 16.0) + 2.0
				),
				t
			));
		}

		for (
			unsigned int x = 0;
			x < 2;
			x++
		) {
			bool side = x % 2;

			coll.push_back(Bishop(
				glm::vec3(
					-4.0 + ((-1.0 * side) * (-4.0 * 2.0)),
					0,
					8.0 + ((-1.0 * team) * 16.0) + 2.0
				),
				t
			));
		}

		coll.push_back(King(
			glm::vec3(
				-2.0,
				0,
				8.0 + ((-1.0 * team) * 16.0) + 2.0
			),
			t
		));

		coll.push_back(Queen(
			glm::vec3(
				2.0,
				0,
				8.0 + ((-1.0 * team) * 16.0) + 2.0
			),
			t
		));
	}

	int p = 0;
	coll[p].active = true;

	SDL_Event e;
	bool down;
	int
		start[2],
		curr[2],
		delta;

	bool run = true;
	std::string buff;
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

					delta = start[0] - curr[0];

					trans.rot[1] = (delta / 400.0);
					mvp = trans.getMvp(cam);

					for (Piece& piece : coll) {
						piece.trans->rot[1] = (delta / 400.0);
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

					for (auto& piece : coll) {
						piece.rot[0] = cam.orientation * 90;
						piece.trans->setRot(piece.rot);
					}
				}

				if (code == SDL_SCANCODE_LEFT) {
					if (curs[0] > 0) {
						curs[0]--;
					}
				}

				if (code == SDL_SCANCODE_DOWN) {
					if (curs[1] > 0) {
						curs[1]--;
					}
				}

				if (code == SDL_SCANCODE_RIGHT) {
					if (curs[0] < 7) {
						curs[0]++;
					}
				}

				if (code == SDL_SCANCODE_UP) {
					if (curs[1] < 7) {
						curs[1]++;
					}
				}

				if (code == SDL_SCANCODE_TAB) {
					if (p < 15) {
						p++;

						for (auto& piece : coll) {
							piece.active = false;
						}
						coll[p + (turn * 16)].active = true;
					} else {
						p = 0;
					}
				}

				if (code == SDL_SCANCODE_RETURN) {
					glm::vec2 asdf = glm::vec2(
						curs[0] * 2,
						((float) (-4 * 2)) + ((turn * 8) * 2) + (curs[1] * 2)
					);

					/* coll[p + (turn * 16)].mv(cam, asdf); */

					turn = !turn;
					p = 0;

					for (auto& piece : coll) {
						piece.active = false;
					}
					coll[p + (turn * 16)].active = true;
				}
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
				glm::value_ptr(curs)
			);

			glBindVertexArray(vao);
			glDrawArrays(GL_QUADS, 0, 4);
			board.unUse();

			for (Piece& piece : coll) {
				piece.draw(cam);
			}

			renderText(
				disp,
				buff
			);

			disp.update();
		}
	}

	return 0;
}
