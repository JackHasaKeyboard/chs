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
		0.1f,
		100.0f
	);

	Prog board("checker");
	board.use();

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	const GLfloat vtc[] = {
		-8.0, -8.0,
		8.0, -8.0,
		8.0, 8.0,
		-8.0, 8.0
	};

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof vtc, vtc, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(0);

	// MVP
	Trans trans;
	trans.getRot()->x = 90;

	glm::mat4 mvp = trans.getMvp(cam);
	glUniformMatrix4fv(
		glGetUniformLocation(board.id, "mvp"),
		1,
		GL_FALSE,
		glm::value_ptr(mvp)
	);
	
	// team
	bool t = false;
	glUniform1i(
		glGetUniformLocation(board.id, "t"),
		t
	);

	// position
	glm::vec2 mv = glm::vec2(6, 6);
	glUniform2fv(
		glGetUniformLocation(board.id, "mv"),
		1,
		glm::value_ptr(mv)
	);

	std::vector<Obj> coll;
	for (
		unsigned int t = 0;
		t < 2;
		t++
	) {
			for (
			unsigned int x = 0;
			x < 8;
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
						(z * 2) + ((t * 8) * 2)
					),
					t
				));
			}
		}
	}

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

				if (k == SDLK_F5) {
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

				// position
				glUniform2fv(
					glGetUniformLocation(board.id, "mv"),
					1,
					glm::value_ptr(mv)
				);
			}

			disp.clear(0, 0, 0, 1);

			glBindVertexArray(vao);
			glDrawArrays(GL_QUADS, 0, 4);
			for (Obj& piece : coll) {
				piece.draw(cam);
			}

			disp.update();
		}
	}

	return 0;
}
