#pragma once

#include <fstream>
#include <string>
#include <iostream>

std::string rd(std::string name) {
	std::ifstream in;
	in.open("./" + name);

	std::string cont;
	for (
		std::string l;
		std::getline(in, l);
	) {
		cont += l + "\n";
	}

	in.close();

	return cont;
}

class Shad {
	public:
		GLuint id;

		Shad(std::string name, GLenum type) {
			std::string ext;
			if (type == GL_VERTEX_SHADER) {
				ext = "vs";
			}
			if (type == GL_FRAGMENT_SHADER) {
				ext = "fs";
			}

			std::string str = rd(name + "." + ext);
			const char* src = str.c_str();

			GLuint shad = glCreateShader(type);
			this->id = shad;
			glShaderSource(this->id, 1, &src, NULL);
			glCompileShader(this->id);
		}

		~Shad() {
			glDeleteShader(this->id);
		}
};

class Prog {
	public:
		GLuint id;

		Prog(std::string name) {
			Shad
				vtx("shad/" + name, GL_VERTEX_SHADER),
				frag("shad/" + name, GL_FRAGMENT_SHADER);

			GLuint prog = glCreateProgram();
			this->id = prog;
			glAttachShader(this->id, frag.id);
			glAttachShader(this->id, vtx.id);

			// position
			GLint pos = glGetAttribLocation(prog, "a_pos");
			glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(pos);
		}

		~Prog() {
			glDeleteProgram(this->id);
		}

		void use() {
			glLinkProgram(this->id);
			glUseProgram(this->id);
		}
};
