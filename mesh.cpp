#include <map>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdlib.h>

#include "mesh.h"

Mesh::Mesh(const std::string& fName) {
	initMesh(objModel("./obj/" + fName + ".obj").toIdxedModel());
}

void Mesh::initMesh(const IdxedModel& model) {
	noIdc = model.indices.size();
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(noBuff, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[POSITION_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof model.pos[0] * model.pos.size(), &model.pos[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof model.texCoords[0] * model.texCoords.size(), &model.texCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof model.norm[0] * model.norm.size(), &model.norm[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[INDEX_VB]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(model.indices[0]) * model.indices.size(), &model.indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}

Mesh::Mesh(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices) {
	IdxedModel model;

	for (
		unsigned int i = 0;
		i < numVertices;
		i++
	) {
		model.pos.push_back(*vertices[i].getPos());
		model.texCoords.push_back(*vertices[i].getTexCoord());
		model.norm.push_back(*vertices[i].getNorm());
	}
	
	for (
		unsigned int i = 0;
		i < numIndices;
		i++
	) {
		model.indices.push_back(indices[i]);
	}

	initMesh(model);
}

Mesh::~Mesh() {
	glDeleteBuffers(noBuff, vbo);
	glDeleteVertexArrays(1, &vao);
}

void Mesh::draw() {
	glBindVertexArray(vao);

	glDrawElementsBaseVertex(GL_TRIANGLES, noIdc, GL_UNSIGNED_INT, 0, 0);

	glBindVertexArray(0);
}
