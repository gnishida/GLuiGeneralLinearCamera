#pragma once

#include "Vector3f.h"
#include <vector>
#include "Vertex.h"

class OBJLoader
{
protected:
	OBJLoader() {}

public:
	static void load(const char* filename, std::vector<Vertex>& vertices);
	static int indexOfNumberLetter(std::string& str, int offset);
	static int lastIndexOfNumberLetter(std::string& str);
	static std::vector<std::string> split(const std::string &s, char delim);
};

