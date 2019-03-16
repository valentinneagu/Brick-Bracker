#pragma once

#include <string>

#include <include/glm.h>
#include <Core/GPU/Mesh.h>

namespace ObjectRectangle
{
	//Create rectangle
	Mesh* CreateRectangle(std::string name, glm::vec3 leftTopCorner, float heigth,
		float width, glm::vec3 color);
}