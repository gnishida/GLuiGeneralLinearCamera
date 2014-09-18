#pragma once

#include "Vector3f.h"

/**
 * This structure defines a vertex data.
 */
struct Vertex {
	float position[3];
	float color[3];
	float normal[3];
	float texCoord[3];

	Vertex() {}

	Vertex(Vector3f pos, Vector3f c, Vector3f n, Vector3f tex) {
		position[0]=pos.x();
		position[1]=pos.y();
		position[2]=pos.z();

		color[0]=c.x();
		color[1]=c.y();
		color[2]=c.z();

		normal[0]=n.x();
		normal[1]=n.y();
		normal[2]=n.z();

		texCoord[0]=tex.x();
		texCoord[1]=tex.y();
		texCoord[2]=tex.z();
	}
};
