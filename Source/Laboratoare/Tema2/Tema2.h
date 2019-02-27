#pragma once
#include <Component/SimpleScene.h>
#include <Component/Transform/Transform.h>
#include <Core/GPU/Mesh.h>
#include "CameraPool.h"

// TEACA BOGDAN

class Tema2 : public SimpleScene
{
public:
	Tema2();
	~Tema2();

	void Init() override;

	// culorile posibile pentru bile
	enum BallColor { white, red, yellow, black };

	// structura pentru o bila
	typedef struct Ball {
		BallColor color;
		float pos_x;
		float pos_y;
		float vel_x;
		float vel_y;
		bool alive;
	} Ball;

	// structura pentru un buzunar
	typedef struct Hole {
		float pos_x;
		float pos_y;
	} Hole;

	// structura pentru un jucator
	typedef struct Player {
		float color;
		float score;
	} Player;

	// timpul care a trecut de la inceputul jocului
	float total_time = 0;

	// diametrul unei bile
	float ball_diameter = 0.2;

	// dimensiunile mesei de biliard
	float board_width = 5;
	float board_height = 7.5;
	float wall_thickness = 0.2;

	// coeficient de frecare: bilele isi micsoreaza viteza in timp
	float friction = 0.99;

	// initializez proprietatile jucatorilor: black inseamna ca nu
	// are nicio culoare selectata, si am initializat 3 jucatori
	// dar voi folosi numai ultimii doi pentru a usura lucrul cu
	// indecsii jucatorilor (jucatorul 1 are indexul 1, iar jucatorul
	// 2 are indexul 2)
	Player players[3] = { {black, 0}, {black, 0}, {black, 0} };

	int current_player = 1; // indexul jucatorului curent
	int waiting_player = 2; // indexul jucatorului care asteapta
	bool player_fault = false; // daca jucatorul curent are un Fault la lovitura curenta
	bool player_ok = false; // daca jucatorul curent a bagat o bila in buzunar de culoarea corecta
	bool player_safe = false; // daca jucatorul curent a atins prima data o bila de culoarea corecta

	int spot_on; // boolean pentru lumina spot
	float angle; // unghiul conului spot-ului de lumina
	float move_speed = 2; // viteza cu care jucatorul misca o minge cu WASD

	float cue_length = 2.8; // lungimea tacului
	float cue_thickness = 0.05; // grosimea tacului

	enum GameState { one, two, three, four }; // starile jocului
	// one = starea initiala de la pornirea jocului in care primul jucatorul muta bila alba cu WASD
	// two = starea in care jucatorul trebuie sa loveasca bila alba cu tacul
	// three = starea in care bilele se misca si se ciocnesc
	// four = starea in care jucatorul misca bila alba cu WASD din cauza unul Fault

	GameState game_state = one;

	Ball balls[16]; // array cu bilele din joc
	Hole holes[6]; // array cu buzunarele de pe masa

	float hole_diameter = 0.4; // diametrul unui buzunar

private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void Tema2::UpdatePositions(Ball * ball);
	bool Tema2::BallsCollision(Ball ball1, Ball ball2, float ball1_radius, float ball2_radius);
	bool Tema2::HolesCollision(Ball ball, Hole hole, float ball_radius, float hole_radius);
	void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, const glm::vec3 &color = glm::vec3(1));

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;

	glm::vec3 lightPosition;
	glm::vec3 lightDirection;
	unsigned int materialShininess;
	float materialKd;
	float materialKs;

	CameraTema::CameraPool * camera;
	glm::mat4 projectionMatrix;
	float fov_angle = 60.0f;
	float aspect_ratio = 2.0f;
};
