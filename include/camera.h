#pragma once

#include <stdio.h>
#include <glm/glm.hpp>

struct FTCamera
{
	enum Camera_Movement
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;

	glm::vec3 WorldUp;

	float Yaw;
	float Pitch;

	const float MovementSpeed = 0.5f;
	float MouseSensitivity = 0.1f;
};

void InitCamera(FTCamera& in)
{
	in.Front = glm::vec3(0.0f, 0.0f, 0.0f);
	in.Position = glm::vec3(1.1f, 0.0f, -3.5f);
	in.Up = glm::vec3(0.0f, 1.0f, 0.0f);
	in.WorldUp = in.Up;
	in.Right = glm::normalize(glm::cross(in.Front, in.WorldUp));

	in.Yaw = 90.f;
	in.Pitch = 0.f;
}

void MoveAndOrientCamera(FTCamera& in, float xoffset, float yoffset, int move, float dt)
{
	in.Yaw -= xoffset * in.MovementSpeed;
	in.Pitch -= yoffset * in.MovementSpeed;

	if (in.Pitch > 89.0f)
		in.Pitch = 89.0f;
	if (in.Pitch < -89.0f)
		in.Pitch = -89.0f;

	in.Front.x = cos(glm::radians(in.Yaw)) * cos(glm::radians(in.Pitch));
	in.Front.y = sin(glm::radians(in.Pitch));
	in.Front.z = sin(glm::radians(in.Yaw)) * cos(glm::radians(in.Pitch));
	in.Front = glm::normalize(in.Front);

	in.Right = glm::normalize(glm::cross(in.Front, in.WorldUp));
	in.Up = glm::normalize(glm::cross(in.Right, in.Front));

	if (move == in.FORWARD)
	{
		in.Position += in.Front * in.MovementSpeed * dt;
	}
	else if (move == in.BACKWARD)
	{
		in.Position -= in.Front * in.MovementSpeed * dt;
	}
	else if (move == in.RIGHT)
	{
		in.Position += in.Right * in.MovementSpeed * dt;
	}
	else if (move == in.LEFT)
	{
		in.Position -= in.Right * in.MovementSpeed * dt;
	}
}