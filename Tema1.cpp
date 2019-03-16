#include "Tema1.h"

#include <vector>
#include <iostream>

#include "Transform.h"
#include <Core/Engine.h>
#include "ObjectRectangle.h"
#include "ObjectBall.h"
#include <time.h>

using namespace std;

enum Position {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

typedef std::tuple<bool, Position, glm::vec2> Collision;


Tema1::Tema1()
{
}

Tema1::~Tema1()
{
}

void Tema1::Init()
{
	glm::ivec2 resolution = window->GetResolution();
	auto camera = GetSceneCamera();
	camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	srand(time(NULL));
	//Init
	glm::vec3 corner = glm::vec3(0, 0, 0);
	glm::vec3 center = glm::vec3(0, 0, 0);

	Mesh* rectangleSide = ObjectRectangle::CreateRectangle("rectangleSide", corner, 
		  rectangleHeigth, rectangleWidth, glm::vec3(1, 0, 0));
	AddMeshToList(rectangleSide);

	Mesh* rectangleMiddle = ObjectRectangle::CreateRectangle("rectangleMiddle", corner,
		  rectangleWidth, rectangleHeigth, glm::vec3(1, 0, 0));
	AddMeshToList(rectangleMiddle);

	Mesh* brick = ObjectRectangle::CreateRectangle("brick", corner,
		brickWidth, brickHeigth, glm::vec3(1, 0.27f, 0));
	AddMeshToList(brick);

	Mesh* sideBar = ObjectRectangle::CreateRectangle("sideBar", corner,
		sideBarHeight, sideBarLength, glm::vec3(0, 1, 0));
	AddMeshToList(sideBar);

	Mesh* circle = ObjectBall::CreateBall("circle", center,
		10, glm::vec3(1, 1, 1));
	AddMeshToList(circle);

	Mesh* square = ObjectRectangle::CreateRectangle("square", corner,
		squareSide, squareSide, glm::vec3(0, 0, 1));
	AddMeshToList(square);

	//Initializare date
	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 10; j++) {
			destructionArray[i][j] = false;
			scaleX[i][j] = 1;
			scaleY[i][j] = 1;
			randomUps[i][j] = rand() % 2;
			angularSteps[i][j] = 0;
			bonusPosition[i][j].x = brickPosX + i * brickDistanceX + brickHeigth / 2;
			bonusPosition[i][j].y = brickPosY + j * brickDistanceY + brickWidth / 2;
		}
	}

	Change.x = 1;
	Change.y = 1;
}

void Tema1::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Tema1::Update(float deltaTimeSeconds)
{
	//Manage lifes when ball gets out of field
	CheckLifes();

	glm::ivec2 resolution = window->GetResolution();
	glm::ivec2 cursorPosition = window->GetCursorPosition();
	//Map
	CreateMap();
	//Bricks
	CreateBricks(deltaTimeSeconds);
	//Bar
	CreateBar();
	//Ball
	modelMatrix = glm::mat3(1);
	if (ballStart == true) {
		if (platformHit == true) {
			ballPosX += Change.x * deltaTimeSeconds * 300;
		}
		ballPosY += Change.y * deltaTimeSeconds * 300;
	}
	else {
		ballPosX = cursorPosition.x;
	}
	modelMatrix *= Transform::Translate(ballPosX, ballPosY);
	RenderMesh2D(meshes["circle"], shaders["VertexColor"], modelMatrix);

	//Lifes
	DisplayLifes();
	//Collisions
	VerifyCollisions();
	CreateBonus(deltaTimeSeconds);
	VerifyCollisionsBonus();
}

Position GetPosition(glm::vec2 target)
{
	glm::vec2 compass[] = {
		glm::vec2(0.0f, 1.0f),	// up
		glm::vec2(1.0f, 0.0f),	// right
		glm::vec2(0.0f, -1.0f),	// down
		glm::vec2(-1.0f, 0.0f)	// left
	};
	float max = 0.0f;
	int result = -1;
	for (int i = 0; i < 4; i++)
	{
		float aux = glm::dot(glm::normalize(target), compass[i]);
		if (aux > max)
		{
			max = aux;
			result = i;
		}
	}
	return (Position)result;
}

float clamp(float value, float min, float max) {
	return std::max(min, std::min(max, value));
}

Collision checkCollision(float ballPosX, float ballPosY,
	float rectanglePosX, float rectanglePosY, float height, float width)
{
	float radius = 10;
	glm::vec2 center(ballPosX, ballPosY);
	glm::vec2 halfFigure(height / 2, width / 2);
	glm::vec2 figureCenter(
		rectanglePosX + halfFigure.x,
		rectanglePosY + halfFigure.y
	);
	glm::vec2 difference = center - figureCenter;
	glm::vec2 clamped = glm::clamp(difference, -halfFigure, halfFigure);
	glm::vec2 closest = figureCenter + clamped;
	difference = closest - center;
	if (glm::length(difference) <= radius)
		return std::make_tuple(true, GetPosition(difference), difference);
	else
		return std::make_tuple(false, UP, glm::vec2(0, 0));
}

void Tema1::VerifyCollisions()
{
	for (int i = 0; i < brickCollumns; i++) {
		for (int j = 0; j < brickRows; j++) {
			if (destructionArray[i][j] == false) {
				Collision check = checkCollision(ballPosX, ballPosY,
					brickPosX + i * brickDistanceX, brickPosY + j * brickDistanceY,
					brickHeigth, brickWidth);
				if (std::get<0>(check))
				{
					destructionArray[i][j] = true;
					Position position = std::get<1>(check);
					glm::vec2 difference = std::get<2>(check);
					if (position == LEFT || position == RIGHT)
					{
						Change.x = -Change.x;
						float adjust = radius - std::abs(difference.x);
						if (position == LEFT) {
							ballPosX += adjust;
						}
						else {
							ballPosX -= adjust;
						}
					}
					else {
						Change.y = -Change.y;
						float adjust = radius - std::abs(difference.y);
						if (position == UP) {
							ballPosY -= adjust;
						}
						else {
							ballPosY += adjust;
						}
					}
				}
			}
		}
	}

	//WALL COLLISION
	if (ballPosX - radius < rectangleWidth && ballPosY > rectanglePosY) {
		Change.x = -Change.x;
	}

	Collision checkTop = checkCollision(ballHit, ballPosY, 0, rezY - rectangleWidth, 
		rectangleWidth, rectangleHeigth);
	if (std::get<0>(checkTop)) {
		Change.y = -Change.y;
	}

	if (ballPosX + radius > rezX - rectangleWidth && ballPosY > rectanglePosY) {
		Change.x = -Change.x;
	}

	updateAfterBarCollision();
}


void Tema1::updateAfterBarCollision()
{
	glm::ivec2 cursorPosition = window->GetCursorPosition();
	Collision collisionBar = checkCollision(ballPosX, ballPosY,
		cursorPosition.x - sideBarLength / 2, sideBarPosY,
		sideBarLength, sideBarHeight);
	if (std::get<0>(collisionBar) == true) {
		platformHit = true;
		ballHit = ballPosX - cursorPosition.x;
		if (ballHit >= sideBarLength / 2) {
			ballHit = ballHit - sideBarLength / 2;
		}
		ballHit = ballHit / (sideBarLength / 2);
		glm::vec2 backUp = Change;
		Change.x = ballHit * 2.0f;
		Change.y = -Change.y;
		Change = glm::normalize(Change) * glm::length(backUp);
	}
}

void Tema1::CreateMap()
{
	//Left rectangle
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform::Translate(0, rectanglePosY);
	RenderMesh2D(meshes["rectangleSide"], shaders["VertexColor"], modelMatrix);

	//Middle rectangle
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform::Translate(0, rezY - rectangleWidth);
	RenderMesh2D(meshes["rectangleMiddle"], shaders["VertexColor"], modelMatrix);

	//Right rectangle
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform::Translate(rezX - rectangleWidth, rectanglePosY);
	RenderMesh2D(meshes["rectangleSide"], shaders["VertexColor"], modelMatrix);
}

void Tema1::CreateBar()
{
	glm::ivec2 cursorPosition = window->GetCursorPosition();
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform::Translate(cursorPosition.x - sideBarLength / 2, sideBarPosY);
	RenderMesh2D(meshes["sideBar"], shaders["VertexColor"], modelMatrix);
}

void Tema1::CreateBricks(float deltaTimeSeconds)
{
	//Bricks
	for (int i = 0; i < brickCollumns; i++) {
		for (int j = 0; j < brickRows; j++) {

			//Daca nu a fost atinsa de bila ramane la locul ei
			if (destructionArray[i][j] == false) {
				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform::Translate(brickPosX + i * brickDistanceX,
					brickPosY + j * brickDistanceY);
				RenderMesh2D(meshes["brick"], shaders["VertexColor"], modelMatrix);
			}
			//Daca a fost atinsa de bila ii dam scale pana dispare
			else {
				if (scaleX[i][j] != 0) {
					if (scaleX[i][j] > 0) {
						scaleX[i][j] -= deltaTimeSeconds * 3;
						//Ca sa nu se reformeze in sens opus
						if (scaleX[i][j] < 0) {
							scaleX[i][j] = 0;
						}
					}
					if (scaleY[i][j] > 0) {
						scaleY[i][j] -= deltaTimeSeconds * 3;
						//Ca sa nu se reformeze in sens opus
						if (scaleY[i][j] < 0) {
							scaleY[i][j] = 0;
						}
					}
					modelMatrix = glm::mat3(1);
					modelMatrix *= Transform::Translate(brickPosX + i * brickDistanceX + brickHeigth / 2,
						brickPosY + j * brickDistanceY + brickWidth / 2);
					modelMatrix *= Transform::Scale(scaleX[i][j], scaleY[i][j]);
					modelMatrix *= Transform::Translate(-brickHeigth / 2, -brickWidth / 2);
					RenderMesh2D(meshes["brick"], shaders["VertexColor"], modelMatrix);
				}
			}
		}
	}
}

bool CheckCollisionBonus(glm::vec2 bonusPosition, float barX, float barY,
	float barLength, float barHeight, float squareSide)
{
	bool collisionX = barX + barLength >= bonusPosition.x &&
		bonusPosition.x + squareSide >= barX;
	bool collisionY = barY + barHeight >= bonusPosition.y - 13 &&
		bonusPosition.y + squareSide >= barY;
	return collisionX && collisionY;
}

void Tema1::VerifyCollisionsBonus() {

	glm::ivec2 cursorPosition = window->GetCursorPosition();
	for (int i = 0; i < brickCollumns; i++) {
		for (int j = 0; j < brickRows; j++) {
			if (randomUps[i][j] == 1) {
				bool check = CheckCollisionBonus(bonusPosition[i][j],
					cursorPosition.x - sideBarLength / 2, sideBarPosY,
					sideBarLength, sideBarHeight, squareSide);
				if (check == true) {
					displayBonus[i][j] = check;
					if (numberOfLifes < 5) {
						numberOfLifes++;
					}
					bonusPosition[i][j].x = 0;
					bonusPosition[i][j].y = 0;
				}
			}
		}
	}
}

void Tema1::CreateBonus(float deltaTimeSeconds)
{
	for (int i = 0; i < brickCollumns; i++) {
		for (int j = 0; j < brickRows; j++) {
			if (destructionArray[i][j] == true && randomUps[i][j] == 1 && displayBonus[i][j] == false) {
				bonusPosition[i][j].y -= deltaTimeSeconds * 100;
				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform::Translate(bonusPosition[i][j].x,
					bonusPosition[i][j].y);
				angularSteps[i][j] += deltaTimeSeconds * 3.14;
				modelMatrix *= Transform::Rotate(angularSteps[i][j]);
				modelMatrix *= Transform::Translate(-squareSide / 2, -squareSide / 2);
				RenderMesh2D(meshes["square"], shaders["VertexColor"], modelMatrix);
			}
		}
	}
}

void Tema1::DisplayLifes()
{
	for (int i = 0; i < numberOfLifes; i++) {
		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform::Translate(lifePos + i * lifePos, lifePos);
		RenderMesh2D(meshes["circle"], shaders["VertexColor"], modelMatrix);
	}
}

void Tema1::CheckLifes() {
	if (ballPosY < 0 || ballPosX < 0 || ballPosX > rezX) {
		ballStart = false;
		numberOfLifes--;
		ballPosY = 95;
		Change.x = 1;
		Change.y = 1;
		platformHit = false;
	}

	if (numberOfLifes < 1) {
		for (int i = 0; i < 12; i++) {
			for (int j = 0; j < 10; j++) {
				destructionArray[i][j] = false;
				scaleX[i][j] = 1;
				scaleY[i][j] = 1;
			}
		}
		numberOfLifes = 3;
		Change.x = 1;
		Change.y = 1;
	}
}

void Tema1::FrameEnd()
{

}

void Tema1::OnInputUpdate(float deltaTime, int mods)
{
	
}

void Tema1::OnKeyPress(int key, int mods)
{
	// add key press event
}

void Tema1::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
}

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
	if (window->MouseHold(GLFW_MOUSE_BUTTON_LEFT)) {
		ballStart = true;
	}
}

void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema1::OnWindowResize(int width, int height)
{
}