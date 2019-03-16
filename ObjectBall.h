#pragma once

#include <string>

#include <include/glm.h>
#include <Core/GPU/Mesh.h>

namespace ObjectBall
{
	//Create rectangle
	Mesh* CreateBall(std::string name, glm::vec3 center, float radius, glm::vec3 color);
}