#version 400

uniform int t;
uniform int active;

flat in vec3 normCalc;
out vec4 col;

vec3 red = vec3(1.0, 0.0, 0.0);

vec3 purple[2] = vec3[2](
	vec3(69 / 255.0, 59 / 255.0, 97 / 255.0),
	vec3(95 / 255.0, 82 / 255.0, 134 / 255.0)
);

void main() {
	vec3 lightDir = vec3(0.0, 0.0, 1.0);

	vec4 ambient = vec4(vec3(0.1), 1.0);

	if (active == 1) {
		col = vec4(purple[t], 1.0);
	} else {
		col = ambient + vec4(purple[t], 255 / 255.0) * clamp(dot(-lightDir, normCalc), 0.0, 1.0);
	}
}
