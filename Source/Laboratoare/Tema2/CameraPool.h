#pragma once
#include <include/glm.h>
#include <include/math.h>

// TEACA BOGDAN

namespace CameraTema{
class CameraPool{
	public:
		CameraPool(){
			position = glm::vec3(0, 2, 5);
			forward = glm::vec3(0, 0, -1);
			up = glm::vec3(0, 1, 0);
			right = glm::vec3(1, 0, 0);
			distanceToTarget = 2;
		}

		CameraPool(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up){
			Set(position, center, up);
		}

		~CameraPool(){}

		void Set(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up){
			this->position = position;
			forward = glm::normalize(center - position);
			right = glm::cross(forward, up);
			this->up = glm::cross(right, forward);
		}

		void MoveForward(float distance){
			glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
		}

		void TranslateForward(float distance){
			this->position += glm::normalize(forward) * distance;
		}

		void TranslateUpword(float distance){
			this->position += glm::normalize(up) * distance;
		}

		void TranslateRight(float distance){
			this->position += glm::normalize(right) * distance;
		}

		void RotateFirstPerson_OX(float angle){
			forward = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, right) * glm::vec4(forward, 0)));
			up = glm::cross(right, forward);
		}

		void RotateFirstPerson_OY(float angle){
			forward = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, glm::vec3(0, 1, 0)) * glm::vec4(forward, 0)));
			right = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, glm::vec3(0, 1, 0)) * glm::vec4(right, 0)));
			up = glm::cross(right, forward);
		}

		void RotateFirstPerson_OZ(float angle){
			right = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, up) * glm::vec4(right, 0)));
			up = glm::cross(right, forward);
		}

		void RotateThirdPerson_OX(float angle){
			TranslateForward(distanceToTarget);
			RotateFirstPerson_OX(angle);
			TranslateForward(-distanceToTarget);
		}

		void RotateThirdPerson_OY(float angle){
			TranslateForward(distanceToTarget);
			RotateFirstPerson_OY(angle);
			TranslateForward(-distanceToTarget);
		}

		void RotateThirdPerson_OZ(float angle){
			TranslateForward(distanceToTarget);
			RotateFirstPerson_OZ(angle);
			TranslateForward(-distanceToTarget);
		}

		glm::mat4 GetViewMatrix(){
			return glm::lookAt(position, position + forward, up);
		}

		glm::vec3 GetTargetPosition(){
			return position + forward * distanceToTarget;
		}

	public:
		float distanceToTarget;
		glm::vec3 position;
		glm::vec3 forward;
		glm::vec3 right;
		glm::vec3 up;
	};
}