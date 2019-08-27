#version 150

out vec4 col;

vec3 grey[2] = vec3[2](
	vec3(34 / 255.0, 34 / 255.0, 34 / 255.0),
	vec3(102 / 255.0, 102 / 255.0, 102 / 255.0)
);

void main() {
	if ((mod(gl_FragCoord.x, 100.0) < 50.0 && mod(gl_FragCoord.y, 100.0) < 50.0) || (mod(gl_FragCoord.x, 100.0) > 50.0 && mod(gl_FragCoord.y, 100.0) > 50.0)) {
		col = (vec4(grey[0], 1.0));
	} else {
		col = vec4(grey[1], 1.0);
	}
}
