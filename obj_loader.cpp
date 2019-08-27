#include "obj_loader.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <map>

static bool compareObjIdxPtr(const ObjIdx* a, const ObjIdx* b);
static inline unsigned int findNextChar(unsigned int start, const char* str, unsigned int length, char token);
static inline unsigned int parseObjIdxVal(const std::string& token, unsigned int start, unsigned int end);
static inline float parseObjFloatVal(const std::string& token, unsigned int start, unsigned int end);
static inline std::vector<std::string> splitStr(const std::string &s, char delim);

ObjModel::ObjModel(const std::string& name) {
	hasUv = false;
	hasNorm = false;

	std::ifstream f;
	std::string l;
	f.open(name.c_str());
	if (f.is_open()) {
		while (f.good()) {
			getline(f, l);

			if (l.length() < 2) {
				continue;
			}

			const char* str = l.c_str();
			switch (str[0]) {
				case 'v':
					if (str[1] == 't') {
						this->uv.push_back(parseObjVec2(l));
					} else if (str[1] == 'n') {
						this->norm.push_back(parseObjVec3(l));
					} else if (str[1] == ' ' || str[1] == '\t') {
						this->vtc.push_back(parseObjVec3(l));
					}

					break;

				case 'f':
					createObjFace(l);

					break;

				default:
					break;
			};
		}
	} else {
		std::cerr << "Unable to load mesh: " << name << std::endl;
	}
}

void IdxedModel::calcNorm() {
	for (
		unsigned int i = 0;
		i < idc.size();
		i += 3
	) {
		int
			i0 = idc[i],
			i1 = idc[i + 1],
			i2 = idc[i + 2];

		glm::vec3 v1 = pos[i1] - pos[i0];
		glm::vec3 v2 = pos[i2] - pos[i0];

		glm::vec3 val = glm::normalize(glm::cross(v1, v2));

		this->norm[i0] += val;
		this->norm[i1] += val;
		this->norm[i2] += val;
	}

	for (
		unsigned int i = 0;
		i < pos.size();
		i++
	) {
		norm[i] = glm::normalize(norm[i]);
	}
}

IdxedModel ObjModel::toIdxedModel() {
	IdxedModel
		res,
		normalModel;

	unsigned int numIndices = objIdc.size();

	std::vector<ObjIdx*> indexLookup;

	for(unsigned int i = 0; i < numIndices; i++)
		indexLookup.push_back(&objIdc[i]);

	std::sort(indexLookup.begin(), indexLookup.end(), compareObjIdxPtr);

	std::map<ObjIdx, unsigned int> normalModelIndexMap;
	std::map<unsigned int, unsigned int> idxMap;

	for (unsigned int i = 0; i < numIndices; i++) {
		ObjIdx* currentIndex = &objIdc[i];

		glm::vec3 currPos = vtc[currentIndex->vtxIdx];
		glm::vec2 currTexCoord;
		glm::vec3 currNorm;

		if (hasUv) {
			currTexCoord = uv[currentIndex->uvIdx];
		} else {
			currTexCoord = glm::vec2(0,0);
		}

		if (hasNorm) {
			currNorm = norm[currentIndex->normIdx];
		} else {
			currNorm = glm::vec3(0,0,0);
		}

		unsigned int
			normModelIdx,
			resModelIdx;

		std::map<ObjIdx, unsigned int>::iterator it = normalModelIndexMap.find(*currentIndex);
		if (it == normalModelIndexMap.end()) {
			normModelIdx = normalModel.pos.size();

			normalModelIndexMap.insert(std::pair<ObjIdx, unsigned int>(*currentIndex, normModelIdx));
			normalModel.pos.push_back(currPos);
			normalModel.texCoords.push_back(currTexCoord);
			normalModel.norm.push_back(currNorm);
		} else {
			normModelIdx = it->second;
		}

		// create model which properly separates texture coordinates
		unsigned int previousVertexLocation = findLastVtxIdx(indexLookup, currentIndex, res);

		if (previousVertexLocation == (unsigned int) -1) {
			resModelIdx = res.pos.size();

			res.pos.push_back(currPos);
			res.texCoords.push_back(currTexCoord);
			res.norm.push_back(currNorm);
		} else {
			resModelIdx = previousVertexLocation;
		}

		normalModel.idc.push_back(normModelIdx);
		res.idc.push_back(resModelIdx);
		idxMap.insert(std::pair<unsigned int, unsigned int>(resModelIdx, normModelIdx));
	}

	if (!hasNorm) {
		normalModel.calcNorm();

		for (
			unsigned int i = 0;
			i < res.pos.size();
			i++
		) {
			res.norm[i] = normalModel.norm[idxMap[i]];
		}
	}

	return res;
};

unsigned int ObjModel::findLastVtxIdx(const std::vector<ObjIdx*>& indexLookup, const ObjIdx* currentIndex, const IdxedModel& result) {
	unsigned int
		start = 0,
		end = indexLookup.size(),
		current = (end - start) / 2 + start,
		previous = start;

	while (current != previous) {
		ObjIdx* testIndex = indexLookup[current];

		if (testIndex->vtxIdx == currentIndex->vtxIdx) {
			unsigned int countStart = current;

			for (unsigned int i = 0; i < current; i++) {
				ObjIdx* possibleIndex = indexLookup[current - i];

				if (possibleIndex == currentIndex) {
					continue;
				}

				if (possibleIndex->vtxIdx != currentIndex->vtxIdx) {
					break;
				}

				countStart--;
			}

			for (unsigned int i = countStart; i < indexLookup.size() - countStart; i++) {
				ObjIdx* possibleIndex = indexLookup[current + i];

				if (possibleIndex == currentIndex) {
					continue;
				}

				if (possibleIndex->vtxIdx != currentIndex->vtxIdx) {
					break;
				} else if((!hasUv || possibleIndex->uvIdx == currentIndex->uvIdx) && (!hasNorm || possibleIndex->normIdx == currentIndex->normIdx)) {
					glm::vec3 currPos = vtc[currentIndex->vtxIdx];
					glm::vec2 currTexCoord;
					glm::vec3 currNorm;

					if (hasUv) {
						currTexCoord = uv[currentIndex->uvIdx];
					} else {
						currTexCoord = glm::vec2(0,0);
					}

					if (hasNorm) {
						currNorm = norm[currentIndex->normIdx];
					} else {
						currNorm = glm::vec3(0,0,0);
					}

					for (
						unsigned int j = 0;
						j < result.pos.size();
						j++
					) {
						if (currPos == result.pos[j] && ((!hasUv || currTexCoord == result.texCoords[j]) && (!hasNorm || currNorm == result.norm[j]))) {
							return j;
						}
					}
				}
			}

			return -1;
		} else {
			if (testIndex->vtxIdx < currentIndex->vtxIdx) {
				start = current;
			} else {
				end = current;
			}
		}

		previous = current;
		current = (end - start) / 2 + start;
	}

	return -1;
}

void ObjModel::createObjFace(const std::string& line) {
	std::vector<std::string> tokens = splitStr(line, ' ');

	this->objIdc.push_back(parseObjIdx(tokens[1], &this->hasUv, &this->hasNorm));
	this->objIdc.push_back(parseObjIdx(tokens[2], &this->hasUv, &this->hasNorm));
	this->objIdc.push_back(parseObjIdx(tokens[3], &this->hasUv, &this->hasNorm));

	if((int)tokens.size() > 4) {
		this->objIdc.push_back(parseObjIdx(tokens[1], &this->hasUv, &this->hasNorm));
		this->objIdc.push_back(parseObjIdx(tokens[3], &this->hasUv, &this->hasNorm));
		this->objIdc.push_back(parseObjIdx(tokens[4], &this->hasUv, &this->hasNorm));
	}
}

ObjIdx ObjModel::parseObjIdx(const std::string& token, bool* hasUv, bool* hasNorm) {
	unsigned int tokenLength = token.length();
	const char* tokenString = token.c_str();

	unsigned int
		vertIndexStart = 0,
		vertIndexEnd = findNextChar(vertIndexStart, tokenString, tokenLength, '/');

	ObjIdx result;
	result.vtxIdx = parseObjIdxVal(token, vertIndexStart, vertIndexEnd);
	result.uvIdx = 0;
	result.normIdx = 0;

	if (vertIndexEnd >= tokenLength) {
		return result;
	}

	vertIndexStart = vertIndexEnd + 1;
	vertIndexEnd = findNextChar(vertIndexStart, tokenString, tokenLength, '/');

	result.uvIdx = parseObjIdxVal(token, vertIndexStart, vertIndexEnd);
	*hasUv = true;

	if (vertIndexEnd >= tokenLength) {
		return result;
	}

	vertIndexStart = vertIndexEnd + 1;
	vertIndexEnd = findNextChar(vertIndexStart, tokenString, tokenLength, '/');

	result.normIdx = parseObjIdxVal(token, vertIndexStart, vertIndexEnd);
	*hasNorm = true;

	return result;
}

glm::vec3 ObjModel::parseObjVec3(const std::string& line) {
	unsigned int tokenLength = line.length();
	const char* tokenString = line.c_str();

	unsigned int vertIndexStart = 2;

	while(vertIndexStart < tokenLength) {
		if(tokenString[vertIndexStart] != ' ') {
			break;
		}

		vertIndexStart++;
	}

	unsigned int vertIndexEnd = findNextChar(vertIndexStart, tokenString, tokenLength, ' ');

	float x = parseObjFloatVal(line, vertIndexStart, vertIndexEnd);

	vertIndexStart = vertIndexEnd + 1;
	vertIndexEnd = findNextChar(vertIndexStart, tokenString, tokenLength, ' ');

	float y = parseObjFloatVal(line, vertIndexStart, vertIndexEnd);

	vertIndexStart = vertIndexEnd + 1;
	vertIndexEnd = findNextChar(vertIndexStart, tokenString, tokenLength, ' ');

	float z = parseObjFloatVal(line, vertIndexStart, vertIndexEnd);

	return glm::vec3(x,y,z);
}

glm::vec2 ObjModel::parseObjVec2(const std::string& line) {
	unsigned int tokenLength = line.length();
	const char* tokenString = line.c_str();

	unsigned int vertIndexStart = 3;

	while(vertIndexStart < tokenLength) {
		if (tokenString[vertIndexStart] != ' ') {
			break;
		}

		vertIndexStart++;
	}

	unsigned int vertIndexEnd = findNextChar(vertIndexStart, tokenString, tokenLength, ' ');

	float x = parseObjFloatVal(line, vertIndexStart, vertIndexEnd);

	vertIndexStart = vertIndexEnd + 1;
	vertIndexEnd = findNextChar(vertIndexStart, tokenString, tokenLength, ' ');

	float y = parseObjFloatVal(line, vertIndexStart, vertIndexEnd);

	return glm::vec2(x,y);
}

static bool compareObjIdxPtr(const ObjIdx* a, const ObjIdx* b) {
	return a->vtxIdx < b->vtxIdx;
}

static inline unsigned int findNextChar(unsigned int start, const char* str, unsigned int length, char token) {
	unsigned int result = start;
	while(result < length) {
		result++;

		if(str[result] == token) {
			break;
		}
	}

	return result;
}

static inline unsigned int parseObjIdxVal(const std::string& token, unsigned int start, unsigned int end) {
	return atoi(token.substr(start, end - start).c_str()) - 1;
}

static inline float parseObjFloatVal(const std::string& token, unsigned int start, unsigned int end) {
	return atof(token.substr(start, end - start).c_str());
}

static inline std::vector<std::string> splitStr(const std::string &s, char delim) {
	std::vector<std::string> elems;

	const char* cstr = s.c_str();
	unsigned int
		strLength = s.length(),
		start = 0,
		end = 0;

	while(end <= strLength) {
		while(end <= strLength) {
			if(cstr[end] == delim) {
				break;
			}

			end++;
		}

		elems.push_back(s.substr(start, end - start));
		start = end + 1;
		end = start;
	}

	return elems;
}
