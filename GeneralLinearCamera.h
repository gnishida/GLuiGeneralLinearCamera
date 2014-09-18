#pragma once

#include <vector>
#include "Vertex.h"
#include "Vector3f.h"

class GeneralLinearCamera
{
public:
	static enum { TYPE_PERSPECTIVE = 0, TYPE_ORTHOGONAL, TYPE_PUSHBROOM, TYPE_BILINEAR, TYPE_XSLIT };
public:
	GeneralLinearCamera();

	void setType(int type);
	Vector3f castRay(float s, float t, std::vector<Vertex> &vertices, Vector3f &bgColor);

private:
	float _u[3];
	float _v[3];
	float _s[3];
	float _t[3];
};

