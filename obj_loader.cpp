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
		normMod;

	unsigned int numIdc = objIdc.size();

	std::vector<ObjIdx*> idxLookup;

	for (
		unsigned int i = 0;
		i < numIdc;
		i++
	) {
		idxLookup.push_back(&objIdc[i]);
	}

	std::sort(idxLookup.begin(), idxLookup.end(), compareObjIdxPtr);

	std::map<ObjIdx, unsigned int> normModIdxMap;
	std::map<unsigned int, unsigned int> idxMap;

	for (
		unsigned int i = 0;
		i < numIdc;
		i++
	) {
		ObjIdx* currIdx = &objIdc[i];

		glm::vec3 currPos = vtc[currIdx->vtxIdx];
		glm::vec2 currTexCoord;
		glm::vec3 currNorm;

		if (hasUv) {
			currTexCoord = uv[currIdx->uvIdx];
		} else {
			currTexCoord = glm::vec2(0,0);
		}

		if (hasNorm) {
			currNorm = norm[currIdx->normIdx];
		} else {
			currNorm = glm::vec3(0,0,0);
		}

		unsigned int
			normModelIdx,
			resModelIdx;

		std::map<ObjIdx, unsigned int>::iterator it = normModIdxMap.find(*currIdx);
		if (it == normModIdxMap.end()) {
			normModelIdx = normMod.pos.size();

			normModIdxMap.insert(std::pair<ObjIdx, unsigned int>(*currIdx, normModelIdx));
			normMod.pos.push_back(currPos);
			normMod.texCoords.push_back(currTexCoord);
			normMod.norm.push_back(currNorm);
		} else {
			normModelIdx = it->second;
		}

		// create model which properly separates texture coordinates
		unsigned int prevVtxLoc = findLastVtxIdx(idxLookup, currIdx, res);

		if (prevVtxLoc == (unsigned int) -1) {
			resModelIdx = res.pos.size();

			res.pos.push_back(currPos);
			res.texCoords.push_back(currTexCoord);
			res.norm.push_back(currNorm);
		} else {
			resModelIdx = prevVtxLoc;
		}

		normMod.idc.push_back(normModelIdx);
		res.idc.push_back(resModelIdx);
		idxMap.insert(std::pair<unsigned int, unsigned int>(resModelIdx, normModelIdx));
	}

	if (!hasNorm) {
		normMod.calcNorm();

		for (
			unsigned int i = 0;
			i < res.pos.size();
			i++
		) {
			res.norm[i] = normMod.norm[idxMap[i]];
		}
	}

	return res;
};

unsigned int ObjModel::findLastVtxIdx(const std::vector<ObjIdx*>& idxLookup, const ObjIdx* currIdx, const IdxedModel& result) {
	unsigned int
		start = 0,
		end = idxLookup.size(),
		curr = (end - start) / 2 + start,
		prev = start;

	while (curr != prev) {
		ObjIdx* testIdx = idxLookup[curr];

		if (testIdx->vtxIdx == currIdx->vtxIdx) {
			unsigned int cntStart = curr;

			for (
				unsigned int i = 0;
				i < curr;
				i++
			) {
				ObjIdx* possIdx = idxLookup[curr - i];

				if (possIdx == currIdx) {
					continue;
				}

				if (possIdx->vtxIdx != currIdx->vtxIdx) {
					break;
				}

				cntStart--;
			}

			for (
				unsigned int i = cntStart;
				i < idxLookup.size() - cntStart;
				i++
			) {
				ObjIdx* possIdx = idxLookup[curr + i];

				if (possIdx == currIdx) {
					continue;
				}

				if (possIdx->vtxIdx != currIdx->vtxIdx) {
					break;
				} else if((!hasUv || possIdx->uvIdx == currIdx->uvIdx) && (!hasNorm || possIdx->normIdx == currIdx->normIdx)) {
					glm::vec3 currPos = vtc[currIdx->vtxIdx];
					glm::vec2 currTexCoord;
					glm::vec3 currNorm;

					if (hasUv) {
						currTexCoord = uv[currIdx->uvIdx];
					} else {
						currTexCoord = glm::vec2(0,0);
					}

					if (hasNorm) {
						currNorm = norm[currIdx->normIdx];
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
			if (testIdx->vtxIdx < currIdx->vtxIdx) {
				start = curr;
			} else {
				end = curr;
			}
		}

		prev = curr;
		curr = (end - start) / 2 + start;
	}

	return -1;
}

void ObjModel::createObjFace(const std::string& line) {
	std::vector<std::string> tok = splitStr(line, ' ');

	this->objIdc.push_back(parseObjIdx(tok[1], &this->hasUv, &this->hasNorm));
	this->objIdc.push_back(parseObjIdx(tok[2], &this->hasUv, &this->hasNorm));
	this->objIdc.push_back(parseObjIdx(tok[3], &this->hasUv, &this->hasNorm));

	if(tok.size() > 4) {
		this->objIdc.push_back(parseObjIdx(tok[1], &this->hasUv, &this->hasNorm));
		this->objIdc.push_back(parseObjIdx(tok[3], &this->hasUv, &this->hasNorm));
		this->objIdc.push_back(parseObjIdx(tok[4], &this->hasUv, &this->hasNorm));
	}
}

ObjIdx ObjModel::parseObjIdx(const std::string& token, bool* hasUv, bool* hasNorm) {
	unsigned int tokLn = token.length();
	const char* str = token.c_str();

	unsigned int
		vertIdxStart = 0,
		vertIdxEnd = findNextChar(vertIdxStart, str, tokLn, '/');

	ObjIdx result;
	result.vtxIdx = parseObjIdxVal(token, vertIdxStart, vertIdxEnd);
	result.uvIdx = 0;
	result.normIdx = 0;

	if (vertIdxEnd >= tokLn) {
		return result;
	}

	vertIdxStart = vertIdxEnd + 1;
	vertIdxEnd = findNextChar(vertIdxStart, str, tokLn, '/');

	result.uvIdx = parseObjIdxVal(token, vertIdxStart, vertIdxEnd);
	*hasUv = true;

	if (vertIdxEnd >= tokLn) {
		return result;
	}

	vertIdxStart = vertIdxEnd + 1;
	vertIdxEnd = findNextChar(vertIdxStart, str, tokLn, '/');

	result.normIdx = parseObjIdxVal(token, vertIdxStart, vertIdxEnd);
	*hasNorm = true;

	return result;
}

glm::vec3 ObjModel::parseObjVec3(const std::string& l) {
	unsigned int tokLn = l.length();
	const char* str = l.c_str();

	unsigned int vertIdxStart = 2;

	while(vertIdxStart < tokLn) {
		if (str[vertIdxStart] != ' ') {
			break;
		}

		vertIdxStart++;
	}

	unsigned int vertIdxEnd = findNextChar(vertIdxStart, str, tokLn, ' ');

	float x = parseObjFloatVal(l, vertIdxStart, vertIdxEnd);

	vertIdxStart = vertIdxEnd + 1;
	vertIdxEnd = findNextChar(vertIdxStart, str, tokLn, ' ');

	float y = parseObjFloatVal(l, vertIdxStart, vertIdxEnd);

	vertIdxStart = vertIdxEnd + 1;
	vertIdxEnd = findNextChar(vertIdxStart, str, tokLn, ' ');

	float z = parseObjFloatVal(l, vertIdxStart, vertIdxEnd);

	return glm::vec3(x, y, z);
}

glm::vec2 ObjModel::parseObjVec2(const std::string& l) {
	unsigned int tokLn = l.length();
	const char* str = l.c_str();

	unsigned int vertIdxStart = 3;

	while (vertIdxStart < tokLn) {
		if (str[vertIdxStart] != ' ') {
			break;
		}

		vertIdxStart++;
	}

	unsigned int vertIdxEnd = findNextChar(vertIdxStart, str, tokLn, ' ');

	float x = parseObjFloatVal(l, vertIdxStart, vertIdxEnd);

	vertIdxStart = vertIdxEnd + 1;
	vertIdxEnd = findNextChar(vertIdxStart, str, tokLn, ' ');

	float y = parseObjFloatVal(l, vertIdxStart, vertIdxEnd);

	return glm::vec2(x,y);
}

static bool compareObjIdxPtr(const ObjIdx* a, const ObjIdx* b) {
	return a->vtxIdx < b->vtxIdx;
}

static inline unsigned int findNextChar(unsigned int start, const char* str, unsigned int length, char tok) {
	unsigned int res = start;
	while(res < length) {
		res++;

		if (str[res] == tok) {
			break;
		}
	}

	return res;
}

static inline unsigned int parseObjIdxVal(const std::string& tok, unsigned int start, unsigned int end) {
	return atoi(tok.substr(start, end - start).c_str()) - 1;
}

static inline float parseObjFloatVal(const std::string& tok, unsigned int start, unsigned int end) {
	return atof(tok.substr(start, end - start).c_str());
}

static inline std::vector<std::string> splitStr(const std::string &s, char delim) {
	std::vector<std::string> elem;

	const char* str = s.c_str();
	unsigned int
		strLn = s.length(),
		start = 0,
		end = 0;

	while (end <= strLn) {
		while (end <= strLn) {
			if (str[end] == delim) {
				break;
			}

			end++;
		}

		elem.push_back(s.substr(start, end - start));
		start = end + 1;
		end = start;
	}

	return elem;
}
