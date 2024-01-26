#include "Camera.h"

Camera::Camera(glm::vec3 InPosition, glm::vec3 InUp, float InYaw, float InPitch): Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
	Position = InPosition;
	WorldUp = InUp;
	Yaw = InYaw;
	Pitch = InPitch;
	UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(const Camera_Movement Direction, const float DeltaTime)
{
	const float Velocity = MovementSpeed * DeltaTime;
	if (Direction == FORWARD)
	{
		Position += Front * Velocity;
	}
	if (Direction == BACKWARD)
	{
		Position -= Front * Velocity;
	}
	if (Direction == LEFT)
	{
		Position -= Right * Velocity;
	}
	if (Direction == RIGHT)
	{
		Position += Right * Velocity;
	}
}

void Camera::ProcessMouseMovement(float Xoffset, float Yoffset, const bool ConstrainPitch)
{
	Xoffset *= MouseSensitivity;
	Yoffset *= MouseSensitivity;

	Yaw   += Xoffset;
	Pitch += Yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (ConstrainPitch)
	{
		if (Pitch > 89.0f)
		{
			Pitch = 89.0f;
		}
		if (Pitch < -89.0f)
		{
			Pitch = -89.0f;
		}
	}

	// update Front, Right and Up Vectors using the updated Euler angles
	UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(const float Yoffset)
{
	Zoom -= (float)Yoffset;
	if (Zoom < 1.0f)
	{
		Zoom = 1.0f;
	}
	if (Zoom > 45.0f)
	{
		Zoom = 45.0f;
	}
}

void Camera::UpdateCameraVectors()
{
	// calculate the new Front vector
	glm::vec3 LocalFront;
	LocalFront.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	LocalFront.y = sin(glm::radians(Pitch));
	LocalFront.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(LocalFront);
	// also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	Up    = glm::normalize(glm::cross(Right, Front));
}
