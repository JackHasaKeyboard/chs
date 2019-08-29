#version 400

uniform mat4 mvp;
uniform mat4 norm;

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_norm;

flat out vec3 normCalc;

void main() {
	gl_Position = mvp * vec4(a_pos, 1.0);

	normCalc = (norm * vec4(a_norm, 0.0)).xyz;
}
