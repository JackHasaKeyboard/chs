#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>

struct ObjIdx {
	unsigned int
		vtxIdx,
		uvIdx,
		normIdx;

	bool operator < (const ObjIdx& r) const {
		return vtxIdx < r.vtxIdx;
	}
};

class IdxedModel {
	public:
		std::vector<glm::vec3> pos;
		std::vector<glm::vec2> texCoords;
		std::vector<glm::vec3> norm;

		std::vector<unsigned int> indices;

		void calcNorm();
};

class ObjModel {
	private:
		unsigned int findLastVtxIdx(const std::vector<ObjIdx*>& idxLookup, const ObjIdx* currIdx, const IdxedModel& res);
		void createObjFace(const std::string& l);

		glm::vec2 parseObjVec2(const std::string& l);
		glm::vec3 parseObjVec3(const std::string& l);
		ObjIdx parseObjIdx(const std::string& tok, bool* hasUv, bool* hasNorm);

	public:
		std::vector<ObjIdx> objIdc;
		std::vector<glm::vec3> vtc;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> norm;

		bool
			hasUv,
			hasNorm;

		ObjModel(const std::string& name);

		IdxedModel toIdxedModel();
};
