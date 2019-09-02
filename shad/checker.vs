#version 150

uniform mat4 mvp;

in vec3 pos;
out vec2 coord;

void main() {
	gl_Position = mvp * vec4(pos, 1.0);
	
	coord = pos.xz;
}
