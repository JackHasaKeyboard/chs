#include "obj_loader.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <map>

static bool compareObjIdxPtr(const objIdx* a, const objIdx* b);
static inline unsigned int findNextChar(unsigned int start, const char* str, unsigned int length, char token);
static inline unsigned int parseObjIdxVal(const std::string& token, unsigned int start, unsigned int end);
static inline float parseObjFloatVal(const std::string& token, unsigned int start, unsigned int end);
static inline std::vector<std::string> splitStr(const std::string &s, char delim);

objModel::objModel(const std::string& fName) {
	hasUv = false;
	hasNorm = false;

	std::ifstream f;
	std::string l;
	f.open(fName.c_str());
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
						this->norm.push_back(ParseOBJVec3(l));
					} else if (str[1] == ' ' || str[1] == '\t') {
						this->vtc.push_back(ParseOBJVec3(l));
					}

					break;

				case 'f':
					CreateOBJFace(l);

					break;

				default:
					break;
			};
		}
	} else {
		std::cerr << "Unable to load mesh: " << fName << std::endl;
	}
}

void IdxedModel::calcNorm() {
	for (unsigned int i = 0; i < indices.size(); i += 3) {
		int
			i0 = indices[i],
			i1 = indices[i + 1],
			i2 = indices[i + 2];

		glm::vec3 v1 = pos[i1] - pos[i0];
		glm::vec3 v2 = pos[i2] - pos[i0];

		glm::vec3 normal = glm::normalize(glm::cross(v1, v2));

		norm[i0] += normal;
		norm[i1] += normal;
		norm[i2] += normal;
	}

	for (unsigned int i = 0; i < pos.size(); i++) {
		norm[i] = glm::normalize(norm[i]);
	}
}

IdxedModel objModel::toIdxedModel() {
	IdxedModel
		result,
		normalModel;

	unsigned int numIndices = objIdc.size();

	std::vector<objIdx*> indexLookup;

	for(unsigned int i = 0; i < numIndices; i++)
		indexLookup.push_back(&objIdc[i]);

	std::sort(indexLookup.begin(), indexLookup.end(), compareObjIdxPtr);

	std::map<objIdx, unsigned int> normalModelIndexMap;
	std::map<unsigned int, unsigned int> indexMap;

	for (unsigned int i = 0; i < numIndices; i++) {
		objIdx* currentIndex = &objIdc[i];

		glm::vec3 currentPosition = vtc[currentIndex->vtxIdx];
		glm::vec2 currentTexCoord;
		glm::vec3 currentNormal;

		if (hasUv) {
			currentTexCoord = uv[currentIndex->uvIdx];
		} else {
			currentTexCoord = glm::vec2(0,0);
		}

		if (hasNorm) {
			currentNormal = norm[currentIndex->normIdx];
		} else {
			currentNormal = glm::vec3(0,0,0);
		}

		unsigned int
			normalModelIndex,
			resultModelIndex;

		std::map<objIdx, unsigned int>::iterator it = normalModelIndexMap.find(*currentIndex);
		if (it == normalModelIndexMap.end()) {
			normalModelIndex = normalModel.pos.size();

			normalModelIndexMap.insert(std::pair<objIdx, unsigned int>(*currentIndex, normalModelIndex));
			normalModel.pos.push_back(currentPosition);
			normalModel.texCoords.push_back(currentTexCoord);
			normalModel.norm.push_back(currentNormal);
		} else {
			normalModelIndex = it->second;
		}

		// create model which properly separates texture coordinates
		unsigned int previousVertexLocation = FindLastVertexIndex(indexLookup, currentIndex, result);

		if (previousVertexLocation == (unsigned int) -1) {
			resultModelIndex = result.pos.size();

			result.pos.push_back(currentPosition);
			result.texCoords.push_back(currentTexCoord);
			result.norm.push_back(currentNormal);
		} else {
			resultModelIndex = previousVertexLocation;
		}

		normalModel.indices.push_back(normalModelIndex);
		result.indices.push_back(resultModelIndex);
		indexMap.insert(std::pair<unsigned int, unsigned int>(resultModelIndex, normalModelIndex));
	}

	if (!hasNorm) {
		normalModel.calcNorm();

		for(unsigned int i = 0; i < result.pos.size(); i++)
			result.norm[i] = normalModel.norm[indexMap[i]];
	}

	return result;
};

unsigned int objModel::FindLastVertexIndex(const std::vector<objIdx*>& indexLookup, const objIdx* currentIndex, const IdxedModel& result) {
	unsigned int
		start = 0,
		end = indexLookup.size(),
		current = (end - start) / 2 + start,
		previous = start;

	while (current != previous) {
		objIdx* testIndex = indexLookup[current];

		if (testIndex->vtxIdx == currentIndex->vtxIdx) {
			unsigned int countStart = current;

			for (unsigned int i = 0; i < current; i++) {
				objIdx* possibleIndex = indexLookup[current - i];

				if (possibleIndex == currentIndex) {
					continue;
				}

				if (possibleIndex->vtxIdx != currentIndex->vtxIdx) {
					break;
				}

				countStart--;
			}

			for (unsigned int i = countStart; i < indexLookup.size() - countStart; i++) {
				objIdx* possibleIndex = indexLookup[current + i];

				if (possibleIndex == currentIndex) {
					continue;
				}

				if (possibleIndex->vtxIdx != currentIndex->vtxIdx) {
					break;
				} else if((!hasUv || possibleIndex->uvIdx == currentIndex->uvIdx) && (!hasNorm || possibleIndex->normIdx == currentIndex->normIdx)) {
					glm::vec3 currentPosition = vtc[currentIndex->vtxIdx];
					glm::vec2 currentTexCoord;
					glm::vec3 currentNormal;

					if (hasUv) {
						currentTexCoord = uv[currentIndex->uvIdx];
					} else {
						currentTexCoord = glm::vec2(0,0);
					}

					if (hasNorm) {
						currentNormal = norm[currentIndex->normIdx];
					} else {
						currentNormal = glm::vec3(0,0,0);
					}

					for (unsigned int j = 0; j < result.pos.size(); j++) {
						if(currentPosition == result.pos[j] && ((!hasUv || currentTexCoord == result.texCoords[j]) && (!hasNorm || currentNormal == result.norm[j]))) {
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

void objModel::CreateOBJFace(const std::string& line) {
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

objIdx objModel::parseObjIdx(const std::string& token, bool* hasUv, bool* hasNorm) {
	unsigned int tokenLength = token.length();
	const char* tokenString = token.c_str();

	unsigned int
		vertIndexStart = 0,
		vertIndexEnd = findNextChar(vertIndexStart, tokenString, tokenLength, '/');

	objIdx result;
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

glm::vec3 objModel::ParseOBJVec3(const std::string& line) {
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

glm::vec2 objModel::parseObjVec2(const std::string& line) {
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

static bool compareObjIdxPtr(const objIdx* a, const objIdx* b) {
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
