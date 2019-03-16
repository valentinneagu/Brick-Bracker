#include "ObjectRectangle.h"

#include <Core/Engine.h>

Mesh* ObjectRectangle::CreateRectangle(std::string name, glm::vec3 leftBottomCorner, float height,
	float width, glm::vec3 color)
{
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(width, 0, 0), color),
		VertexFormat(corner + glm::vec3(width, height, 0), color),
		VertexFormat(corner + glm::vec3(0, height, 0), color)
	};

	Mesh* rectangle = new Mesh(name);
	std::vector<unsigned short> indices = { 0, 1, 2, 3 };
	indices.push_back(0);
	indices.push_back(2);
	rectangle->InitFromData(vertices, indices);
	return rectangle;
}