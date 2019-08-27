#version 150

out vec4 col;

uniform int t;
uniform vec2 mv;

vec3 grey[2] = vec3[2](
	vec3(34 / 255.0, 34 / 255.0, 34 / 255.0),
	vec3(102 / 255.0, 102 / 255.0, 102 / 255.0)
);

vec3 purple[2] = vec3[2](
	vec3(69 / 255.0, 59 / 255.0, 97 / 255.0),
	vec3(95 / 255.0, 82 / 255.0, 134 / 255.0)
);

void main() {
	if ((mod(gl_FragCoord.x, 100.0) < 50.0 && mod(gl_FragCoord.y, 100.0) < 50.0) || (mod(gl_FragCoord.x, 100.0) > 50.0 && mod(gl_FragCoord.y, 100.0) > 50.0)) {
		col = (vec4(grey[0], 1.0));
	} else {
		col = vec4(grey[1], 1.0);
	}

	if (
		gl_FragCoord.x > (mv.x * 50) &&
		gl_FragCoord.x < ((mv.x * 50) + (1 * 50)) &&
		gl_FragCoord.y > ((mv.y * 50)) &&
		gl_FragCoord.y < ((mv.y * 50) + (1 * 50))
	) {
		col = vec4(purple[t], 1.0);
	}
}
