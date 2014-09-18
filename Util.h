#pragma once

#include <vector>
#include "Vector3f.h"

class Util
{
protected:
	Util() {}

public:
	static void barycentricCoordinate(const Vector3f &p1, const Vector3f &p2, const Vector3f &p3, const Vector3f &p, float &alpha, float &beta);
	static bool intersect(const Vector3f &o, const Vector3f &v, const Vector3f &p1, const Vector3f &p2, const Vector3f &p3, float &t, float &alpha, float &beta);
};

