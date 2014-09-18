#include "Util.h"

void Util::barycentricCoordinate(const Vector3f &p1, const Vector3f &p2, const Vector3f &p3, const Vector3f &p, float &alpha, float &beta)
{
	Vector3f v = p - p1;
	Vector3f v1 = p2 - p1;
	Vector3f v2 = p3 - p1;

	Vector3f triangleArea = Vector3f::crossProduct(v1, v2);
	Vector3f triangleArea1 = Vector3f::crossProduct(v, v2);
	Vector3f triangleArea2 = Vector3f::crossProduct(v1, v);
	alpha = triangleArea1.length() / triangleArea.length();
	if (Vector3f::dotProduct(triangleArea, triangleArea1) < 0) alpha = -alpha;
	beta = triangleArea2.length() / triangleArea.length();
	if (Vector3f::dotProduct(triangleArea, triangleArea2) < 0) beta = -beta;
}

bool Util::intersect(const Vector3f &o, const Vector3f &d, const Vector3f &p1, const Vector3f &p2, const Vector3f &p3, float &t, float &alpha, float &beta)
{
	Vector3f e1 = p2 - p1;
	Vector3f e2 = p3 - p1;

	Vector3f h = Vector3f::crossProduct(d, e2);
	float a = Vector3f::dotProduct(e1, h);
	if (a > -0.00001 && a < 0.00001) return false;

	float f = 1.0f / a;
	Vector3f s = o - p1;
	alpha = f * Vector3f::dotProduct(s, h);
	if (alpha < 0.0 || alpha > 1.0) return false;

	Vector3f q = Vector3f::crossProduct(s, e1);
	beta = f * Vector3f::dotProduct(d, q);
	if (beta < 0.0 || alpha + beta > 1.0) return false;

	t = f * Vector3f::dotProduct(e2, q);
	if (t > 0.00001) return true;
	else return false;
}