#pragma once

#include <string>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "obj.h"

struct Vertex {
	private:
		glm::vec3 pos;
		glm::vec2 texCoord;
		glm::vec3 norm;

	public:
		Vertex(const glm::vec3& pos, const glm::vec2& texCoord, const glm::vec3& norm) {
			this->pos = pos;
			this->texCoord = texCoord;
			this->norm = norm;
		}

		glm::vec3* getPos() {
			return &pos;
		}
		glm::vec2* getTexCoord() {
			return &texCoord;
		}
		glm::vec3* getNorm() {
			return &norm;
		}
};

enum MeshBufferPositions {
	POSITION_VB,
	TEXCOORD_VB,
	NORMAL_VB,
	INDEX_VB
};

class Mesh {
	private:
		static const unsigned int noBuff = 4;

		void initMesh(const IdxedModel& model);

		GLuint vao;
		GLuint vbo[noBuff];
		unsigned int noIdc;

	public:
		Mesh(const std::string& fName);
		Mesh(Vertex* vtc, unsigned int noVtc, unsigned int* idc, unsigned int noIdc);

		void draw();

		virtual ~Mesh();
};
