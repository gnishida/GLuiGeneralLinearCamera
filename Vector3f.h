#pragma once

struct Vector3f {
	float _x;
	float _y;
	float _z;

public:
	Vector3f() : _x(0), _y(0), _z(0) {}
	Vector3f(float x, float y, float z) : _x(x), _y(y), _z(z) {}

	void normalize();
	float length() const;

	void setX(float x) { _x = x; }
	void setY(float y) { _y = y; }
	void setZ(float z) { _z = z; }
	float x() const { return _x; }
	float y() const { return _y; }
	float z() const { return _z; }

	static float dotProduct(const Vector3f& a, const Vector3f& b);
	static Vector3f crossProduct(const Vector3f& a, const Vector3f& b);
};

const Vector3f	operator+(const Vector3f& v1, const Vector3f& v2);
const Vector3f	operator-(const Vector3f& v1, const Vector3f& v2);
const Vector3f	operator*(const Vector3f& v1, float s);