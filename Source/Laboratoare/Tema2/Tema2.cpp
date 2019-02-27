#include "Tema2.h"
#include <vector>
#include <string>
#include <iostream>
#include <Core/Engine.h>

// TEACA BOGDAN

using namespace std;

Tema2::Tema2() {}

Tema2::~Tema2() {}

inline glm::mat4 RotateOZ(float rad) {
	return glm::mat4(
		cos(rad), -sin(rad), 0, 0,
		sin(rad), cos(rad), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);
}

inline glm::mat4 RotateOY(float rad) {
	return glm::mat4(
		cos(rad), 0, sin(rad), 0,
		0, 1, 0, 0,
		-sin(rad), 0, cos(rad), 0,
		0, 0, 0, 1
	);
}

inline glm::mat4 RotateOX(float rad) {
	return glm::mat4(
		1, 0, 0, 0,
		0, cos(rad), -sin(rad), 0,
		0, sin(rad), cos(rad), 0,
		0, 0, 0, 1
	);
}

void Tema2::Init() {
	camera = new CameraTema::CameraPool();
	camera->Set(glm::vec3(0, 2, board_height / 2), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

	projectionMatrix = glm::perspective(RADIANS(fov_angle), aspect_ratio, 0.01f, 200.0f);

	spot_on = 1;
	angle = 30;

	{
		Mesh * mesh = new Mesh("box");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh * mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	Shader * shader = new Shader("ShaderPrincipal"); // shaderul pentru iluminare Phong
	shader->AddShader("Source/Laboratoare/Tema2/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
	shader->AddShader("Source/Laboratoare/Tema2/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
	shader->CreateAndLink();
	shaders[shader->GetName()] = shader;

	Shader * shader_cue = new Shader("ShaderAnimation"); // shader pentru animatia tacului
	shader_cue->AddShader("Source/Laboratoare/Tema2/Shaders/CueShader.glsl", GL_VERTEX_SHADER);
	shader_cue->AddShader("Source/Laboratoare/Tema2/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
	shader_cue->CreateAndLink();
	shaders[shader_cue->GetName()] = shader_cue;

	//Light & material properties
	lightPosition = glm::vec3(0, 5, 0);
	lightDirection = glm::vec3(0, -1, 0);
	materialShininess = 30;
	materialKd = 0.5;
	materialKs = 0.5;

	// distanta dintre doua randuri de bile din configuratia triunghiulara initiala a bilelor
	float ball_padding = sqrt(3) / 2;

	// initializam culoarile si pozitiile bilelor din configuratia triunghiulara
	balls[0] = { red, -2 * ball_diameter, 0 * ball_diameter - board_height / 3 };
	balls[1] = { yellow, -1 * ball_diameter, 0 * ball_diameter - board_height / 3 };
	balls[2] = { yellow, 0 * ball_diameter, 0 * ball_diameter - board_height / 3 };
	balls[3] = { red, 1 * ball_diameter, 0 * ball_diameter - board_height / 3 };
	balls[4] = { red, 2 * ball_diameter, 0 * ball_diameter - board_height / 3 };

	balls[5] = { yellow, -1.5f * ball_diameter, ball_padding * ball_diameter - board_height / 3 };
	balls[6] = { red, -0.5f * ball_diameter, ball_padding * ball_diameter - board_height / 3 };
	balls[7] = { red, 0.5f * ball_diameter, ball_padding * ball_diameter - board_height / 3 };
	balls[8] = { yellow, 1.5f * ball_diameter, ball_padding * ball_diameter - board_height / 3 };

	balls[9] = { red, -1 * ball_diameter, 2 * ball_padding * ball_diameter - board_height / 3 };
	balls[10] = { black, 0 * ball_diameter, 2 * ball_padding * ball_diameter - board_height / 3 };
	balls[11] = { yellow, 1 * ball_diameter, 2 * ball_padding * ball_diameter - board_height / 3 };

	balls[12] = { yellow, -0.5f * ball_diameter, 3 * ball_padding * ball_diameter - board_height / 3 };
	balls[13] = { red, 0.5f * ball_diameter, 3 * ball_padding * ball_diameter - board_height / 3 };

	balls[14] = { red, 0 * ball_diameter, 4 * ball_padding * ball_diameter - board_height / 3 };

	balls[15] = { white, 0 * ball_diameter, ball_diameter + board_height / 3 };

	for (int i = 0; i < 16; i++) {
		// initial bilele nu se misca (au viteza zero) si sunt toate pe masa
		balls[i].vel_x = 0;
		balls[i].vel_y = 0;
		balls[i].alive = true;
	}

	holes[0] = { -board_width / 2 + wall_thickness * 3 / 2, -board_height / 2 + wall_thickness * 3 / 2 };
	holes[1] = { -board_width / 2 + wall_thickness * 3 / 2, 0 };
	holes[2] = { -board_width / 2 + wall_thickness * 3 / 2, board_height / 2 - wall_thickness * 3 / 2 };
	holes[3] = { board_width / 2 - wall_thickness * 3 / 2, -board_height / 2 + wall_thickness * 3 / 2 };
	holes[4] = { board_width / 2 - wall_thickness * 3 / 2, 0 };
	holes[5] = { board_width / 2 - wall_thickness * 3 / 2, board_height / 2 - wall_thickness * 3 / 2 };

	printf("Urmeaza jucatorul 1.\n");
}

void Tema2::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Tema2::UpdatePositions(Ball * ball) {
	ball->pos_x += ball->vel_x;
	ball->pos_y += ball->vel_y;
}

void Tema2::Update(float deltaTimeSeconds)
{
	total_time += deltaTimeSeconds;

	for (int i = 0; i < 16; i++) {
		if (balls[i].alive) {
			UpdatePositions(&balls[i]);
		}
	}

	if (game_state == three) {
		bool change_state = true;

		for (int i = 0; i < 16; i++) {
			if (balls[i].alive && (balls[i].vel_x * balls[i].vel_x > 0.00001f || balls[i].vel_y * balls[i].vel_y > 0.00001f)) {
				change_state = false;
				break;
			}
		}

		if (change_state == true) {
			if (player_ok == false && player_fault == false) {
				int temp = current_player;
				current_player = waiting_player;
				waiting_player = temp;

				printf("Urmeaza jucatorul %d.\n", current_player);
			}

			if (player_fault == true) {
				game_state = four;

				int temp = current_player;
				current_player = waiting_player;
				waiting_player = temp;

				printf("Jucatorul %d poate plasa bila alba oriunde.\n", current_player);
			}else {
				game_state = two;

				float rise = 0.4f;
				camera->position = glm::vec3(balls[15].pos_x, 1 + rise, balls[15].pos_y + 2);
				camera->forward = glm::vec3(0, -rise / 2, -1);
				camera->forward = glm::normalize(camera->forward);
				camera->up = RotateOX(RADIANS(-90)) * glm::vec4(camera->forward, 1);
				camera->up = glm::normalize(camera->up);
				camera->right = glm::vec3(1, 0, 0);
				camera->distanceToTarget = glm::distance(camera->position, glm::vec3(balls[15].pos_x, 1, balls[15].pos_y));
			}

			player_ok = false;
			player_fault = false;
			player_safe = false;

			for (int i = 0; i < 16; i++) {
				balls[i].vel_x = 0;
				balls[i].vel_y = 0;
			}
		}

		for (int i = 0; i < 16; i++) {
			if (balls[i].alive) {
				for (int j = 0; j < 6; j++) {
					if (HolesCollision(balls[i], holes[j], ball_diameter / 2, hole_diameter / 2)) {
						if (i == 15) {
							if (game_state == three) {
								if (player_fault == false) {
									player_fault = true;
									printf("Jucatorul %d a introdus in buzunar o bila alba.\n", current_player);
								}
							}
						}else if (i == 10) {
							printf("Jucatorul %d a introdus in buzunar o bila neagra.\n", current_player);

							if (players[current_player].score != 7) {
								printf("Jucatorul %d a castigat.\n", waiting_player);
								printf("Jucatorul %d a pierdut.\n", current_player);

								for (int i = 0; i < 16; i++) {
									balls[i].alive = false;
								}
							}else {
								printf("Jucatorul %d a castigat.\n", current_player);
								printf("Jucatorul %d a pierdut.\n", waiting_player);

								for (int i = 0; i < 16; i++) {
									balls[i].alive = false;
								}
							}
						}else {
							balls[i].alive = false;

							string ball_color;

							if (balls[i].color == yellow) {
								printf("Jucatorul %d a introdus in buzunar o bila galbena.\n", current_player);

								if (players[current_player].color == black) {
									players[current_player].color = yellow;
									players[waiting_player].color = red;

									printf("Jucatorul %d trebuie sa introduca in buzunar bilele galbene.\n", current_player);
									printf("Jucatorul %d trebuie sa introduca in buzunar bilele rosii.\n", waiting_player);
								}
							}else if (balls[i].color == red) {
								printf("Jucatorul %d a introdus in buzunar o bila rosie.\n", current_player);

								if (players[current_player].color == black) {
									players[current_player].color = red;
									players[waiting_player].color = yellow;

									printf("Jucatorul %d trebuie sa introduca in buzunar bilele rosii.\n", current_player);
									printf("Jucatorul %d trebuie sa introduca in buzunar bilele galbene.\n", waiting_player);
								}
							}

							if (players[current_player].color == balls[i].color) {
								players[current_player].score++;
								player_ok = true;
							}else {
								players[waiting_player].score++;
							}
						}

						balls[i].vel_x = 0;
						balls[i].vel_y = 0;
					}
				}
			}
		}

		for (int i = 0; i < 16; i++) {
			if (balls[i].alive) {
				if (balls[i].pos_x - ball_diameter / 2 < -board_width / 2 + wall_thickness || balls[i].pos_x + ball_diameter / 2 > board_width / 2 - wall_thickness) {
					balls[i].vel_x = -balls[i].vel_x;
				}

				if (balls[i].pos_y - ball_diameter / 2 < -board_height / 2 + wall_thickness || balls[i].pos_y + ball_diameter / 2 > board_height / 2 - wall_thickness) {
					balls[i].vel_y = -balls[i].vel_y;
				}
			}
		}

		float new_vel_x[16];
		float new_vel_y[16];

		for (int i = 0; i < 16; i++) {
			if (balls[i].alive) {
				new_vel_x[i] = balls[i].vel_x;
				new_vel_y[i] = balls[i].vel_y;
			}
		}

		for (int i = 0; i < 16; i++) {
			if (balls[i].alive) {
				for (int j = i + 1; j < 16; j++) {
					if (balls[j].alive) {
						if (BallsCollision(balls[i], balls[j], ball_diameter / 2, ball_diameter / 2)) {
							if (j == 15) {
								if (player_safe == false && player_fault == false) {
									if (balls[i].color == players[current_player].color && players[current_player].color != black) {
										player_safe = true;
										printf("Jucatorul %d a atins prima data o minge colarata corect.\n", current_player);
									}else if(balls[i].color != players[current_player].color && players[current_player].color != black){
										player_fault = true;
										printf("Jucatorul %d a atins prima data o minge colarata gresit. Fault!\n", current_player);
									}else if(players[current_player].color == black){
										player_safe = true;
									}
								}
							}

							// calculam directia in care se vor deplasa bilele (modificam componentele vitezelor bilelor)

							double dist = sqrt(pow(balls[i].pos_x - balls[j].pos_x, 2) + pow(balls[i].pos_y - balls[j].pos_y, 2));
							double t_x = (balls[j].pos_x - balls[i].pos_x) / dist;
							double t_y = (balls[j].pos_y - balls[i].pos_y) / dist;
							double momentum = balls[i].vel_x * t_x + balls[i].vel_y * t_y - balls[j].vel_x * t_x - balls[j].vel_y * t_y;

							new_vel_x[i] = balls[i].vel_x - momentum * t_x;
							new_vel_y[i] = balls[i].vel_y - momentum * t_y;
							new_vel_x[j] = balls[j].vel_x + momentum * t_x;
							new_vel_y[j] = balls[j].vel_y + momentum * t_y;

							// deplasam bilele astfel incat sa nu fie intrate una in alta

							float overlap = (dist - ball_diameter) / 2; // deplasam bilele cu distanta overlap

							balls[i].pos_x -= overlap * (balls[i].pos_x - balls[j].pos_x) / dist;
							balls[i].pos_y -= overlap * (balls[i].pos_y - balls[j].pos_y) / dist;

							balls[j].pos_x += overlap * (balls[i].pos_x - balls[j].pos_x) / dist;
							balls[j].pos_y += overlap * (balls[i].pos_y - balls[j].pos_y) / dist;
						}
					}
				}
			}
		}

		for (int i = 0; i < 16; i++) {
			if (balls[i].alive) {
				balls[i].vel_x = new_vel_x[i];
				balls[i].vel_y = new_vel_y[i];
			}
		}

		for (int i = 0; i < 16; i++) {
			if (balls[i].alive) {
				balls[i].vel_x *= friction;
				balls[i].vel_y *= friction;
			}
		}
	}

	for (int i = 0; i < 16; i++) {
		if (balls[i].alive) {
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(balls[i].pos_x, 1, balls[i].pos_y));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(ball_diameter));

			glm::vec3 color;
			if (balls[i].color == red) color = glm::vec3(1, 0, 0);
			if (balls[i].color == yellow) color = glm::vec3(1, 1, 0);
			if (balls[i].color == black) color = glm::vec3(0.1f, 0.1f, 0.1f);
			if (balls[i].color == white) color = glm::vec3(1, 1, 1);
			RenderSimpleMesh(meshes["sphere"], shaders["ShaderPrincipal"], modelMatrix, color);
		}
	}

	for (int i = 0; i < 6; i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(holes[i].pos_x, 0.8, holes[i].pos_y));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(hole_diameter));
		RenderSimpleMesh(meshes["sphere"], shaders["ShaderPrincipal"], modelMatrix, glm::vec3(0, 0, 0));
	}

	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1, board_height / 2 - wall_thickness / 2));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(board_width, wall_thickness, wall_thickness));
		RenderSimpleMesh(meshes["box"], shaders["ShaderPrincipal"], modelMatrix, glm::vec3(0, 0.5, 0));
	}

	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1, -board_height / 2 + wall_thickness / 2));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(board_width, wall_thickness, wall_thickness));
		RenderSimpleMesh(meshes["box"], shaders["ShaderPrincipal"], modelMatrix, glm::vec3(0, 0.5, 0));
	}

	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(board_width / 2 - wall_thickness / 2, 1, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(wall_thickness, wall_thickness, board_height));
		RenderSimpleMesh(meshes["box"], shaders["ShaderPrincipal"], modelMatrix, glm::vec3(0, 0.5, 0));
	}

	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-board_width / 2 + wall_thickness / 2, 1, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(wall_thickness, wall_thickness, board_height));
		RenderSimpleMesh(meshes["box"], shaders["ShaderPrincipal"], modelMatrix, glm::vec3(0, 0.5, 0));
	}

	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1 - wall_thickness, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(board_width, wall_thickness, board_height));
		RenderSimpleMesh(meshes["box"], shaders["ShaderPrincipal"], modelMatrix, glm::vec3(0, 1, 0));
	}

	{
		glm::mat4 modelMatrix = glm::mat4(1);
		
		modelMatrix = glm::translate(modelMatrix, glm::vec3(balls[15].pos_x, 1, balls[15].pos_y));

		glm::vec3 camera_forward = glm::vec3(camera->forward.x, 0, camera->forward.z);
		camera_forward = normalize(camera_forward);
		if (camera_forward.z < 0) {
			modelMatrix = glm::rotate(modelMatrix, RADIANS(90) + glm::acos(dot(camera_forward, glm::vec3(1, 0, 0))), glm::vec3(0, 1, 0));
		}else {
			modelMatrix = glm::rotate(modelMatrix, RADIANS(270) + glm::acos(dot(camera_forward, glm::vec3(-1, 0, 0))), glm::vec3(0, 1, 0));
		}

		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, cue_length * (1 - sin(total_time * 2)) / 2));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01f, 0.01f, cue_length * (1 - sin(total_time * 2))));

		RenderSimpleMesh(meshes["box"], shaders["ShaderPrincipal"], modelMatrix, glm::vec3(1, 1, 0));
	}
	
	if (game_state == two) {
		glm::mat4 modelMatrix = glm::mat4(1);

		RenderSimpleMesh(meshes["box"], shaders["ShaderAnimation"], modelMatrix, glm::vec3(0.9, 0.9, 0.9));
	}
}

void Tema2::FrameEnd()
{
	DrawCoordinatSystem(camera->GetViewMatrix(), projectionMatrix);
}

bool Tema2::BallsCollision(Ball ball1, Ball ball2, float ball1_radius, float ball2_radius) {
	float radius_sum = ball1_radius + ball2_radius;

	if (ball1.pos_x + radius_sum > ball2.pos_x && ball1.pos_x < ball2.pos_x + 2 * radius_sum && ball1.pos_y + 2 * radius_sum > ball2.pos_y && ball1.pos_y < ball2.pos_y + 2 * radius_sum) {
		float dist = sqrt((ball1.pos_x - ball2.pos_x) * (ball1.pos_x - ball2.pos_x) + (ball1.pos_y - ball2.pos_y) * (ball1.pos_y - ball2.pos_y));

		if (dist < radius_sum) {
			return true;
		}
	}

	return false;
}

bool Tema2::HolesCollision(Ball ball, Hole hole, float ball_radius, float hole_radius) {
	float radius_sum = ball_radius + hole_radius - ball_radius;

	if (ball.pos_x + radius_sum > hole.pos_x && ball.pos_x < hole.pos_x + 2 * radius_sum && ball.pos_y + 2 * radius_sum > hole.pos_y && ball.pos_y < hole.pos_y + 2 * radius_sum) {
		float dist = sqrt((ball.pos_x - hole.pos_x) * (ball.pos_x - hole.pos_x) + (ball.pos_y - hole.pos_y) * (ball.pos_y - hole.pos_y));

		if (dist < radius_sum) {
			return true;
		}
	}

	return false;
}

void Tema2::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, const glm::vec3 &color) {
	string str("ShaderAnimation");

	if (shader->GetName() == str) {
		if (!mesh || !shader || !shader->GetProgramID()) return;

		glUseProgram(shader->program); // render an object using the specified shader and the specified position

		glUniform1f(glGetUniformLocation(shader->program, "time"), total_time);
		glUniform1f(glGetUniformLocation(shader->program, "cue_length"), cue_length);
		glUniform1f(glGetUniformLocation(shader->program, "cue_thickness"), cue_thickness);
		glm::vec3 camForward = camera->forward;
		glUniform3f(glGetUniformLocation(shader->program, "cam_forward"), camForward.x, camForward.y, camForward.z);
		glUniform2f(glGetUniformLocation(shader->program, "ball"), balls[15].pos_x, balls[15].pos_y);

		// shader uniforms for model, view and projection matrices
		glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
		glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		// Draw the object
		glBindVertexArray(mesh->GetBuffers()->VAO);
		glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
	}else {
		if (!mesh || !shader || !shader->GetProgramID()) return;

		glUseProgram(shader->program); // render an object using the specified shader and the specified position

		// shader uniforms for light
		glUniform3f(glGetUniformLocation(shader->program, "light_pos"), lightPosition.x, lightPosition.y, lightPosition.z);
		glUniform3f(glGetUniformLocation(shader->program, "light_dir"), lightDirection.x, lightDirection.y, lightDirection.z);
		glUniform1f(glGetUniformLocation(shader->program, "spot_on"), spot_on);
		glUniform1f(glGetUniformLocation(shader->program, "angle"), angle);
		glm::vec3 eyePosition = camera->position;
		glUniform3f(glGetUniformLocation(shader->program, "eye_pos"), eyePosition.x, eyePosition.y, eyePosition.z);

		// shader uniforms for material
		glUniform1i(glGetUniformLocation(shader->program, "material_shininess"), materialShininess);
		glUniform1f(glGetUniformLocation(shader->program, "material_kd"), materialKd);
		glUniform1f(glGetUniformLocation(shader->program, "material_ks"), materialKs);
		glUniform3f(glGetUniformLocation(shader->program, "object_color"), color.r, color.g, color.b);

		// shader uniforms for model, view and projection matrices
		glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
		glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		// Draw the object
		glBindVertexArray(mesh->GetBuffers()->VAO);
		glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);

	}
}

void Tema2::OnInputUpdate(float deltaTime, int mods) {
	if (game_state == one || game_state == four) {
		if (balls[15].pos_x - ball_diameter / 2 > -board_width / 2 + hole_diameter - ball_diameter) {
			if (window->KeyHold(GLFW_KEY_A)) balls[15].pos_x -= deltaTime * move_speed;
		}

		if (balls[15].pos_x + ball_diameter / 2 < board_width / 2 - hole_diameter + ball_diameter) {
			if (window->KeyHold(GLFW_KEY_D)) balls[15].pos_x += deltaTime * move_speed;
		}

		if (game_state == four) {
			if (balls[15].pos_y - ball_diameter / 2 > -board_height / 2 + hole_diameter - ball_diameter) {
				if (window->KeyHold(GLFW_KEY_W)) balls[15].pos_y -= deltaTime * move_speed;
			}
		}else {
			if (balls[15].pos_y - ball_diameter / 2 > board_height / 2 - board_height / 3) {
				if (window->KeyHold(GLFW_KEY_W)) balls[15].pos_y -= deltaTime * move_speed;
			}
		}

		if (balls[15].pos_y + ball_diameter / 2 < board_height / 2 - hole_diameter + ball_diameter) {
			if (window->KeyHold(GLFW_KEY_S)) balls[15].pos_y += deltaTime * move_speed;
		}
	}

	lightDirection = RotateOX((float)glm::sin(total_time) * 0.3) * glm::vec4(0, -1, 0, 1);
}

void Tema2::OnKeyPress(int key, int mods) {
	if (key == GLFW_KEY_F) {
		spot_on++;
		spot_on %= 2;
	}

	if (key == GLFW_KEY_SPACE) {
		if (game_state == one || game_state == four) {
			bool no_collision = true;

			for (int i = 0; i < 15; i++) {
				if (BallsCollision(balls[i], balls[15], ball_diameter / 2, ball_diameter / 2)) {
					no_collision = false;
					break;
				}
			}

			if (no_collision == true) {
				game_state = two;

				player_fault = false;
				player_ok = false;
				player_safe = false;

				float rise = 0.4f;
				camera->position = glm::vec3(balls[15].pos_x, 1 + rise, balls[15].pos_y + 2);
				camera->forward = glm::vec3(0, -rise / 2, -1);
				camera->forward = glm::normalize(camera->forward);
				camera->up = RotateOX(RADIANS(-90)) * glm::vec4(camera->forward, 1);
				camera->up = glm::normalize(camera->up);
				camera->right = glm::vec3(1, 0, 0);
				camera->distanceToTarget = glm::distance(camera->position, glm::vec3(balls[15].pos_x, 1, balls[15].pos_y));
			}
		}
	}
}

void Tema2::OnKeyRelease(int key, int mods) {}

void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY){
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
		float sensivityOX = 0.004f;
		float sensivityOY = 0.004f;

		camera->RotateThirdPerson_OX(sensivityOX * -deltaY);
		camera->RotateThirdPerson_OY(sensivityOY * -deltaX);
	}

}

void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {}

void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {
	if (game_state == two && button == 1) {
		glm::vec3 camera_forward = glm::vec3(camera->forward.x, 0, camera->forward.z);
		camera_forward = normalize(camera_forward);
		glm::vec2 ball_vel = glm::vec2(glm::dot(camera_forward, glm::vec3(1, 0, 0)), glm::dot(camera_forward, glm::vec3(0, 0, 1)));
		ball_vel = glm::normalize(ball_vel);
		ball_vel *= 0.1 * (1 - sin(total_time * 2));
		balls[15].vel_x = ball_vel.x;
		balls[15].vel_y = ball_vel.y;

		camera->position = glm::vec3(0, 10, 0);
		camera->forward = glm::vec3(0, -1, 0);
		camera->up = RotateOX(RADIANS(-90)) * glm::vec4(camera->forward, 1);
		camera->up = glm::normalize(camera->up);
		camera->right = glm::vec3(1, 0, 0);
		camera->distanceToTarget = glm::distance(camera->position, glm::vec3(0, 1, 0));

		game_state = three;
	}
}

void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {}

void Tema2::OnWindowResize(int width, int height) {}
