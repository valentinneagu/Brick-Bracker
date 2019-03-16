#include "ObjectBall.h"

#include <Core/Engine.h>

Mesh* ObjectBall::CreateBall(std::string name, glm::vec3 center, float radius, glm::vec3 color)
{
	int fragments = 100;
	const float PI = 3.1415926f;
	glm::vec3 corner = center;
	float angle;
	std::vector<VertexFormat> vertices;
	vertices.push_back(VertexFormat(corner, color));

	for (int i = 0; i < fragments; i ++) {
		angle = 2.0f * PI * (float)i / (float)fragments;
		vertices.push_back(VertexFormat(corner + glm::vec3(radius * cos(angle), radius * sin(angle), 0), color));
	}

	Mesh* circle = new Mesh(name);
	std::vector<unsigned short> indices;
	for (int i = 1; i < fragments; i++) {
		indices.push_back(0);
		indices.push_back(i);
		indices.push_back(i + 1);
	}

	//For final triangle
	indices.push_back(0);
	indices.push_back(fragments);
	indices.push_back(1);

	circle->InitFromData(vertices, indices);
	return circle;
}