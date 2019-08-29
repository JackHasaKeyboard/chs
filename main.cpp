#include <vector>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "disp.h"
#include "shad.h"
#include "trans.h"
#include "obj.h"

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
		-8.0, 0.0, -8.0,
		8.0, 0.0, -8.0,
		8.0, 0.0, 8.0,
		-8.0, 0.0, 8.0
	};

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof vtc, vtc, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(0);

	glm::vec2 mv = glm::vec2(6, 6);
	Trans trans;
	glm::mat4 mvp = trans.getMvp(cam);
	bool t = false;

	bool team = false;
	std::vector<Obj> coll;
	for (
		unsigned int t = 0;
		t < 2;
		t++
	) {
		for (
			signed int x = -4;
			x < 4;
			x++
		) {
			for (
				unsigned int z = 0;
				z < 2;
				z++
			) {
				coll.push_back(Obj(
					"pawn",
					glm::vec3(
						(x * 2),
						0,
						((float) (-4 * 2)) + ((t * 8) * 2) + (z * 2)
					),
					t
				));
			}
		}
	}

	int p = 0;
	coll[p].active = true;

	SDL_Event e;
	bool down;
	int
		start[2],
		curr[2],
		delta;

	while (true) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_MOUSEBUTTONDOWN) {
				down = true;

				SDL_GetMouseState(&start[0], &start[1]);
			}

			if (e.type == SDL_MOUSEMOTION) {
				if (down) {
					SDL_GetMouseState(&curr[0], &curr[1]);

					delta = start[0] - curr[0];

					cam.pitch(delta / 1000.0);
				}
			}

			if (e.type == SDL_MOUSEBUTTONUP) {
				down = false;
			}

			if (e.type == SDL_KEYDOWN) {
				GLchar k = e.key.keysym.sym;

				if (k == 62 /* F5 */) {
					cam.orientation = !cam.orientation;
				}

				if (k == SDLK_a) {
					if (mv[0] > 0) {
						mv[0]--;
					}
				}

				if (k == SDLK_d) {
					if (mv[0] < 8) {
						mv[0]++;
					}
				}

				if (k == SDLK_s) {
					if (mv[1] > 0) {
						mv[1]--;
					}
				}

				if (k == SDLK_w) {
					if (mv[1] < 8) {
						mv[1]++;
					}
				}

				if (k == SDLK_TAB) {
					if (p < 15) {
						p++;

						for (auto& piece : coll) {
							piece.active = false;
						}
						coll[p + (team * 16)].active = true;
					} else {
						p = 0;
					}
				}

				if (k == SDLK_RETURN) {
					team = !team;
					p = 0;

					for (auto& piece : coll) {
						piece.active = false;
					}
					coll[p + (team * 16)].active = true;
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

			// position
			glUniform2fv(
				glGetUniformLocation(board.id, "mv"),
				1,
				glm::value_ptr(mv)
			);

			glBindVertexArray(vao);
			glDrawArrays(GL_QUADS, 0, 4);
			board.unUse();

			for (Obj& piece : coll) {
				piece.draw(cam);
			}

			disp.update();
		}
	}

	return 0;
}
