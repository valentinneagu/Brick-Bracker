#pragma once
#include <Component/SimpleScene.h>

class Tema1 : public SimpleScene
{
public:
	Tema1();
	~Tema1();

	void Init() override;

private:

	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;
	void updateAfterBarCollision();
	void CreateBricks(float deltaTimeSeconds);
	void CreateMap();
	void CreateBar();
	void DisplayLifes();
	void VerifyCollisions();
	void CheckLifes();
	void CreateBonus(float deltaTimeSeconds);
	void VerifyCollisionsBonus();

protected:
	glm::mat3 modelMatrix;
	//GENERAL
	float rezX = 1280;
	float rezY = 720;
	//RECTANGLE
	float rectangleHeigth = 1280;
	float rectangleHeigthLateral = 620;
	float rectangleWidth = 24;
	float rectanglePosY = 100;
	//SIDEBAR
	float sideBarLength = 210;
	float sideBarHeight = 10;
	float sideBarPosY = 55;
	//BRICKS
		//One brick
	float squareSide = 20;
	float brickWidth = 30;
	float brickHeigth = 50;
		//Brick distance
	float brickDistanceX = 70;
	float brickDistanceY = 40;
	int brickRows = 10;
	int brickCollumns = 12;
		//Position of first brick
	float brickPosX = 230;
	float brickPosY = 240;
	//BALLS & LIFES
	int numberOfLifes = 3;
	float lifePos = 30;
	bool ballStart = false;
	float ballPosX = rezX / 2;
	float ballPosY = 95;
	//@destructionArray folosit pentru a vedea daca o caramida
	//a fost distrusa
	bool destructionArray[12][10];
	float radius = 10;
	//@scaleX si @scaleY sunt folositi pentru a salva nivelul
	//de scalare pentru fiecare caramida
	float scaleX[12][10];
	float scaleY[12][10];
	//@platformHit verifica daca bila a atins bara
	bool platformHit = false;
	//@ballHit utilizat pentru a calcula punctul in care
	//bila loveste bara
	float ballHit = 0;
	//@Change este valoarea care se modifica in urma unei coliziuni
	//si care este folosita pentru a inmulti pozitia bilei cu scopul
	// de a-i da directia dorita
	glm::vec2 Change;
	//@randomUps este o matrice in care fiecare element este un numar
	//aleator (0 sau 1). Folosit pentru a genera patratele de la bonus
	int randomUps[12][10];
	float angularSteps[12][10];
	//@bonusPosition retinerea pozitiei patratelor bonus la orice pas
	glm::vec2 bonusPosition[12][10];
	//@displayBonus reprezinta disponibiitatea patratelor, adica daca
	//au fost culese sau daca au iesit din scena
	bool displayBonus[12][10];
};
