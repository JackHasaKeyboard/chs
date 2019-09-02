#version 150

in vec2 coord;
out vec4 col;

uniform int t;
uniform vec2 curs;

vec3 grey[2] = vec3[2](
	vec3(34 / 255.0, 34 / 255.0, 34 / 255.0),
	vec3(102 / 255.0, 102 / 255.0, 102 / 255.0)
);

vec3 purple[2] = vec3[2](
	vec3(69 / 255.0, 59 / 255.0, 97 / 255.0),
	vec3(95 / 255.0, 82 / 255.0, 134 / 255.0)
);

void main() {
	if ((mod(coord.x, 4) < 2 && mod(coord.y, 4) < 2) || (mod(coord.x, 4) > 2 && mod(coord.y, 4) > 2)) {
		col = (vec4(grey[0], 1.0));
	} else {
		col = vec4(grey[1], 1.0);
	}

	if (
		coord.x > ((curs.x - 4) * 2) &&
		coord.x < (((curs.x - 4) * 2) + (1 * 2)) &&
		coord.y > (((curs.y - 4) * 2)) &&
		coord.y < (((curs.y - 4) * 2) + (1 * 2))
	) {
		col = vec4(purple[t], 1.0);
	}

	if (
		(coord.x < -8) ||
		(coord.y < -8) ||
		(coord.x > 8) ||
		(coord.y > 8)
	) {
		col = vec4(grey[0], 1.0);
	}
}
