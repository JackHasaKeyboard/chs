#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>

struct objIdx {
	unsigned int vtxIdx;
	unsigned int uvIdx;
	unsigned int normIdx;

	bool operator < (const objIdx& r) const {
		return vtxIdx < r.vtxIdx;
	}
};

class IdxedModel {
	public:
		std::vector<glm::vec3> pos;
		std::vector<glm::vec2> texCoords;
		std::vector<glm::vec3> normals;

		std::vector<
			unsigned int
		> indices;

		void calcNorm();
};

class objModel {
	private:
		unsigned int FindLastVertexIndex(const std::vector<objIdx*>& indexLookup, const objIdx* currentIndex, const IdxedModel& result);
		void CreateOBJFace(const std::string& line);

		glm::vec2 parseObjVec2(const std::string& line);
		glm::vec3 ParseOBJVec3(const std::string& line);
		objIdx ParseobjIdx(const std::string& token, bool* hasUVs, bool* hasNormals);

	public:
		std::vector<objIdx> OBJIndices;
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;

		bool
			hasUv,
			hasNorm;

		objModel(const std::string& fileName);

		IdxedModel toIdxedModel();
};
