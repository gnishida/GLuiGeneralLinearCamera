#include "OBJLoader.h"
#include <fstream>
#include <iostream>
#include <sstream>

/**
 * Load vertices data from a OBJ file.
 */
void OBJLoader::load(const char* filename, std::vector<Vertex>& vertices)
{
	std::ifstream file(filename);
	if (!file.is_open()) {
		return;
	}
 
	std::vector<float> raw_vertices;
	std::vector<float> raw_normals;
	std::vector<unsigned int> v_elements;
	std::vector<unsigned int> n_elements;

	std::string line;
	while (getline(file, line)) {
		if (line.substr(0, 2) == "v ") {
			int index1 = indexOfNumberLetter(line, 2);
			int index2 = lastIndexOfNumberLetter(line);
			std::vector<std::string> values = split(line.substr(index1, index2 - index1 + 1), ' ');
			raw_vertices.push_back(atof(values[0].data()));
			raw_vertices.push_back(atof(values[1].data()));
			raw_vertices.push_back(atof(values[2].data()));
		} else if (line.substr(0, 3) == "vn ") {
			int index1 = indexOfNumberLetter(line, 2);
			int index2 = lastIndexOfNumberLetter(line);
			std::vector<std::string> values = split(line.substr(index1, index2 - index1 + 1), ' ');
			raw_normals.push_back(atof(values[0].data()));
			raw_normals.push_back(atof(values[1].data()));
			raw_normals.push_back(atof(values[2].data()));
		} else if (line.substr(0, 2) == "f ") {
			int index1 = indexOfNumberLetter(line, 2);
			int index2 = lastIndexOfNumberLetter(line);
			std::vector<std::string> values = split(line.substr(index1, index2 - index1 + 1), ' ');
			for (int i = 0; i < values.size() - 2; ++i) {
				unsigned int a,b,c;
				std::vector<std::string> v1 = split(values[0], '/');
				a = atoi(v1[0].data()) - 1;
				std::vector<std::string> v2= split(values[i+1], '/');
				b = atoi(v2[0].data()) - 1;
				std::vector<std::string> v3 = split(values[i+2], '/');
				c = atoi(v3[0].data()) - 1;
				v_elements.push_back(a); v_elements.push_back(b); v_elements.push_back(c);

				if (v1.size() >= 3 && v1[2].length() > 0) {
					a = atoi(v1[2].data()) - 1;
					b = atoi(v2[2].data()) - 1;
					c = atoi(v3[2].data()) - 1;
					n_elements.push_back(a); n_elements.push_back(b); n_elements.push_back(c);
				}
			}
		} else if (line.size() == 0) {
			/* ignore empty line */
		} else if (line.substr(0, 1) == "#") {
			/* ignore comment line */
		} else {
			/* ignore this line */
		}
	}
	file.close();

	vertices.resize(v_elements.size());
	for (int i = 0; i < v_elements.size(); i+=3) {
		unsigned int ia = v_elements[i];
		unsigned int ib = v_elements[i+1];
		unsigned int ic = v_elements[i+2];

		for (int j = 0; j < 3; ++j) {
			vertices[i].position[j] = raw_vertices[ia*3 + j];
			vertices[i+1].position[j] = raw_vertices[ib*3 + j];
			vertices[i+2].position[j] = raw_vertices[ic*3 + j];
		}

		if (n_elements.size() > 0) {
			for (int j = 0; j < 3; ++j) {
				vertices[i].normal[j] = raw_normals[n_elements[i]*3 + j];
				vertices[i+1].normal[j] = raw_normals[n_elements[i+1]*3 + j];
				vertices[i+2].normal[j] = raw_normals[n_elements[i+2]*3 + j];
			}
		} else {
			Vector3f normal = Vector3f::crossProduct(
				Vector3f(raw_vertices[ib*3], raw_vertices[ib*3+1], raw_vertices[ib*3+2]) - Vector3f(raw_vertices[ia*3], raw_vertices[ia*3+1], raw_vertices[ia*3+2]),
				Vector3f(raw_vertices[ic*3], raw_vertices[ic*3+1], raw_vertices[ic*3+2]) - Vector3f(raw_vertices[ia*3], raw_vertices[ia*3+1], raw_vertices[ia*3+2]));
			normal.normalize();

			for (int j = 0; j < 3; ++j) {
				vertices[i].normal[j] = j == 0 ? normal.x() : (j == 1 ? normal.y() : normal.z());
				vertices[i+1].normal[j] = j == 0 ? normal.x() : (j == 1 ? normal.y() : normal.z());
				vertices[i+2].normal[j] = j == 0 ? normal.x() : (j == 1 ? normal.y() : normal.z());
			}
		}

		// assign some colors and texture coordinates
		for (int j = 0; j < 3; ++j) {
			vertices[i].color[j] = 1.0f;
			vertices[i+1].color[j] = 1.0f;
			vertices[i+2].color[j] = 1.0f;
			vertices[i].texCoord[j] = 0.0f;
			vertices[i+1].texCoord[j] = j == 0 ? 1.0f : 0.0f;
			vertices[i+2].texCoord[j] = j == 0 ? 0.0f : 1.0f;
		}
	}
}

int OBJLoader::indexOfNumberLetter(std::string& str, int offset) {
	for (int i = offset; i < str.length(); ++i) {
		if ((str[i] >= '0' && str[i] <= '9') || str[i] == '-' || str[i] == '.') return i;
	}
	return str.length();
}

int OBJLoader::lastIndexOfNumberLetter(std::string& str) {
	for (int i = str.length() - 1; i >= 0; --i) {
		if ((str[i] >= '0' && str[i] <= '9') || str[i] == '-' || str[i] == '.') return i;
	}
	return 0;
}

std::vector<std::string> OBJLoader::split(const std::string &s, char delim) {
	std::vector<std::string> elems;

	std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }

    return elems;
}