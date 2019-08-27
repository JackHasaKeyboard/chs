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

	/* Prog board("checker"); */
	/* board.use(); */

	/* GLuint vao; */
	/* glGenVertexArrays(1, &vao); */
	/* glBindVertexArray(vao); */

	/* const GLfloat vtc[] = { */
	/* 	-1.0, -1.0, */
	/* 	1.0, -1.0, */
	/* 	1.0, 1.0, */
	/* 	-1.0, 1.0 */
	/* }; */

	/* GLuint vbo; */
	/* glGenBuffers(1, &vbo); */
	/* glBindBuffer(GL_ARRAY_BUFFER, vbo); */
	/* glBufferData(GL_ARRAY_BUFFER, sizeof vtc, vtc, GL_STATIC_DRAW); */

	/* glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0); */
	/* glEnableVertexAttribArray(0); */

	/* // MVP */
	/* Trans trans; */
	/* trans.getRot()->x = 90; */

	/* glm::mat4 mvp = trans.getMvp(cam); */
	/* glUniformMatrix4fv(glGetUniformLocation(board.id, "matr"), 1, GL_FALSE, glm::value_ptr(mvp)); */

	Obj pawn("pawn");

	while (true) {
		disp.clear(0, 0, 0, 1);

		/* glDrawArrays(GL_QUADS, 0, 4); */
		pawn.draw(cam);

		disp.update();
	}

	return 0;
}
