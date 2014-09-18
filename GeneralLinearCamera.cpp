#include "GeneralLinearCamera.h"
#include "Util.h"

GeneralLinearCamera::GeneralLinearCamera()
{
	setType(TYPE_PERSPECTIVE);
}

void GeneralLinearCamera::setType(int type)
{
	if (type == TYPE_PERSPECTIVE) {
		float fov = 60.0f;
		float width = 250.0f;
		float focal_length = width * 0.5f / tanf(fov * 3.1415926 / 360.0f * 0.5f);
		float ratio = (focal_length - 1.0f) / focal_length;

		_u[0] = 0.0f; _v[0] = 0.0f;
		_u[1] = 1.0f; _v[1] = 0.0f;
		_u[2] = 0.0f; _v[2] = 1.0f;

		_s[0] = 0.0f; _t[0] = 0.0f;
		_s[1] = ratio; _t[1] = 0.0f;
		_s[2] = 0.0f; _t[2] = ratio;
	} else if (type == TYPE_ORTHOGONAL) {
		_u[0] = 0.0f; _v[0] = 0.0f;
		_u[1] = 1.0f; _v[1] = 0.0f;
		_u[2] = 0.0f; _v[2] = 1.0f;

		_s[0] = 0.0f; _t[0] = 0.0f;
		_s[1] = 1.0f; _t[1] = 0.0f;
		_s[2] = 0.0f; _t[2] = 1.0f;
	} else if (type == TYPE_PUSHBROOM) {
		_u[0] = 0.0f; _v[0] = 0.0f;
		_u[1] = 1.0f; _v[1] = 0.0f;
		_u[2] = 0.0f; _v[2] = 1.0f;

		_s[0] = 0.0f; _t[0] = 0.0f;
		_s[1] = 1.0f; _t[1] = 0.0f;
		_s[2] = 0.0f; _t[2] = 1.001f;
	} else if (type == TYPE_BILINEAR) {
		_u[0] = 0.0f; _v[0] = 0.0f;
		_u[1] = 1.0f; _v[1] = 0.0f;
		_u[2] = 0.0f; _v[2] = 1.0f;

		_s[0] = 0.0f; _t[0] = 0.0f;
		_s[1] = 1.0f; _t[1] = 0.0003f;
		_s[2] = 0.0006f; _t[2] = 1.0f;
	} else if (type == TYPE_XSLIT) {
		_u[0] = 0.0f; _v[0] = 0.0f;
		_u[1] = 1.0f; _v[1] = 0.0f;
		_u[2] = 0.0f; _v[2] = 1.0f;

		_s[0] = 0.0f; _t[0] = 0.0f;
		_s[1] = 1.00001f; _t[1] = 0.0f;
		_s[2] = 0.0f; _t[2] = 1.001f;
	}
}

Vector3f GeneralLinearCamera::castRay(float u, float v, std::vector<Vertex> &vertices, Vector3f &bgColor)
{
	float den = (_u[1] - _u[2]) * (_v[0] - _v[2]) - (_v[1] - _v[2]) * (_u[0] - _u[2]);
	float alpha = ((u - _u[2]) * (_v[1] - _v[2]) - (v - _v[2]) * (_u[1] - _u[2])) / -den;
	float beta = ((u - _u[2]) * (_v[0] - _v[2]) - (v - _v[2]) * (_u[0] - _u[2])) / den;

	float s = alpha * _s[0] + beta * _s[1] + (1.0f - alpha - beta) * _s[2];
	float t = alpha * _t[0] + beta * _t[1] + (1.0f - alpha - beta) * _t[2];

	Vector3f o(u, v, 0.0f);
	Vector3f dir(u - s, v - t, -1.0f);
	dir.normalize();

	float min_dist = std::numeric_limits<float>::max();
	float min_k, min_l;
	Vector3f colors[3];
	for (int i = 0; i < vertices.size(); i+=3) {
		Vector3f p1(vertices[i].position[0], vertices[i].position[1], vertices[i].position[2]);
		Vector3f p2(vertices[i+1].position[0], vertices[i+1].position[1], vertices[i+1].position[2]);
		Vector3f p3(vertices[i+2].position[0], vertices[i+2].position[1], vertices[i+2].position[2]);

		float dist, k, l;
		if (Util::intersect(o, dir, p1, p2, p3, dist, k, l)) {
			if (dist < min_dist) {
				min_dist = dist;
				min_k = k;
				min_l = l;
				colors[0] = Vector3f(vertices[i].color[0], vertices[i].color[1], vertices[i].color[2]);
				colors[1] = Vector3f(vertices[i+1].color[0], vertices[i+1].color[1], vertices[i+1].color[2]);
				colors[2] = Vector3f(vertices[i+2].color[0], vertices[i+2].color[1], vertices[i+2].color[2]);
			}
		}
	}

	if (min_dist < std::numeric_limits<float>::max()) {
		return colors[0] * min_k + colors[1] * min_l + colors[2] * (1.0f - min_k - min_l);
	} else {
		return bgColor;
	}
}
