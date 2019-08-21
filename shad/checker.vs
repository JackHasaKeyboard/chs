#version 150

uniform mat4 matr;

in vec2 pos;

void main() {
	gl_Position = matr * vec4(pos, 0.0, 1.0);
}
