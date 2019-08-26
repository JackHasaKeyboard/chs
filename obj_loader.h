#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

struct OBJIndex {
	unsigned int vertexIndex;
	unsigned int uvIndex;
	unsigned int normalIndex;

	bool operator < (const OBJIndex& r) const {
		return vertexIndex < r.vertexIndex;
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

		void CalcNormals();
};

class OBJModel {
	private:
		unsigned int FindLastVertexIndex(const std::vector<OBJIndex*>& indexLookup, const OBJIndex* currentIndex, const IdxedModel& result);
		void CreateOBJFace(const std::string& line);

		glm::vec2 ParseOBJVec2(const std::string& line);
		glm::vec3 ParseOBJVec3(const std::string& line);
		OBJIndex ParseOBJIndex(const std::string& token, bool* hasUVs, bool* hasNormals);

	public:
		std::vector<OBJIndex> OBJIndices;
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;

		bool
			hasUVs,
			hasNormals;

		OBJModel(const std::string& fileName);

		IdxedModel toIdxedModel();
};
