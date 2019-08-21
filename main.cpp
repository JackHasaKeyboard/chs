#include "disp.h"

int main() {
	Disp disp("chs", 800, 600);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	const GLfloat vtc[] = {
		-1.0, -1.0,
		1.0, -1.0,
		1.0, 1.0,
		-1.0, 1.0
	};

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof vtc, vtc, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(0);

	while (true) {
		disp.clear(0, 0, 0, 1);

		glDrawArrays(GL_QUADS, 0, 4);

		disp.update();
	}

	return 0;
}
