#version 150

out vec4 col;

vec3 purple[2] = vec3[2](
	vec3(69 / 255.0, 59 / 255.0, 97 / 255.0),
	vec3(95 / 255.0, 82 / 255.0, 134 / 255.0)
);

void main() {
	col = vec4(purple[0], 1.0);
}
