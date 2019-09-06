#version 150

in vec2 coord;
out vec4 col;

uniform int t;
uniform vec2 curs;
uniform vec2[8 * 8] legal;

vec3 grey[2] = vec3[2](
	vec3(34 / 255.0, 34 / 255.0, 34 / 255.0),
	vec3(102 / 255.0, 102 / 255.0, 102 / 255.0)
);

vec3 purple[2] = vec3[2](
	vec3(69 / 255.0, 59 / 255.0, 97 / 255.0),
	vec3(95 / 255.0, 82 / 255.0, 134 / 255.0)
);

void main() {
	bool b = (mod(coord.x, 4) < 2 && mod(coord.y, 4) < 2) || (mod(coord.x, 4) > 2 && mod(coord.y, 4) > 2);

	col = (vec4(grey[int(b)], 1.0));

	if ((coord.x < -8) ||(coord.y < -8) ||(coord.x > 8) ||(coord.y > 8)) {
		col = vec4(grey[0], 1.0);
	}

	if (coord.x > ((curs.x - 4) * 2) &&coord.x < (((curs.x - 4) * 2) + (1 * 2)) &&coord.y > (((curs.y - 4) * 2)) &&coord.y < (((curs.y - 4) * 2) + (1 * 2))) {
		col = vec4(purple[t], 1.0);
	}

	for (
		int i = 0;
		i < 8 * 8;
		i++
	) {
		if (
			coord.x > (legal[i][0] * 2) &&
			coord.y > (legal[i][1] * 2) &&
			coord.x < (legal[i][0] * 2) + 2 &&
			coord.y < (legal[i][1] * 2) + 2
		) {
			col = vec4(mix(grey[int(b)], purple[t], 0.5), 1.0);
			/* col = vec4(1.0, 0.0, 0.0, 1.0); */
		}
	}
}
