// Includes																																														 Includes
//	===========================================================================================================================================================================================		#####
#include <Camera.h>
#include <iostream>
//	===========================================================================================================================================================================================		#####



// Usings																																														   Usings
//	===========================================================================================================================================================================================		#####
using glm::vec3;
using glm::mat3;
using glm::mat4;
//	===========================================================================================================================================================================================		#####



// Camera																																														   Camera
//	===========================================================================================================================================================================================		#####
cgbv::Camera::Camera()
{
	position = glm::vec3(0.f, 0.f, 0.f);
	target = glm::vec3(0.f, 0.f, -1.f);
	up = glm::vec3(0.f, 1.f, 0.f);
	upOrientation = 0.f;
}


cgbv::Camera::Camera(glm::vec3 position, glm::vec3 target)
{
	this->position = position;
	this->target = target;
	upOrientation = 0.f;
	adjustUp();
}


cgbv::Camera::~Camera()
{
	reset();
}


void cgbv::Camera::adjustUp()
{
	glm::vec3 left;
	glm::vec3 yVec = glm::vec3(0.f, 1.f, 0.f);

	glm::vec3 delta = target - position;
	delta = glm::normalize(delta);

	// Kreuzprodukt == (0, 0, 0) verhindern
	if(delta.x == yVec.x && delta.y == yVec.y && delta.z == yVec.z)
		up = glm::vec3(0.f, 0.f, 1.f);

	else if (delta.x == -yVec.x && delta.y == -yVec.y && delta.z == -yVec.z)
		up = -glm::vec3(0.f, 0.f, 1.f);

	else
	{
		left = glm::cross(yVec, delta);
		left = glm::normalize(left);

		up = glm::cross(delta, left);
		up = glm::normalize(up);
	}

	if(upOrientation != 0.f)
	{
		glm::vec3 axis = target - position;

		glm::mat3 rotation = glm::rotate(glm::mat4(1.f), upOrientation, axis);

		up = rotation * up;
	}
}


void cgbv::Camera::moveTo(glm::vec3 newPosition)
{
	position = newPosition;
}


void cgbv::Camera::moveTo(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}


void cgbv::Camera::moveForward(float delta)
{
	glm::vec3 forward = target - position;

	position += forward * delta;
	target += forward * delta;
}


void cgbv::Camera::moveRight(float delta)
{
	adjustUp();

	glm::vec3 forward = target - position;
	glm::vec3 right = glm::cross(forward, up);

	position += right * -delta;
	target += right * -delta;
}


void cgbv::Camera::setTarget(glm::vec3 newTarget)
{
	target = newTarget;
}


void cgbv::Camera::setTarget(float x, float y, float z)
{
	target.x = x;
	target.y = y;
	target.z = z;
}


void cgbv::Camera::setViewDirection(glm::vec3 viewDirection)
{
	target = position + viewDirection;
}


void cgbv::Camera::setViewDirection(float x, float y, float z)
{
	target = position + glm::vec3(x, y, z);
}


glm::mat4 cgbv::Camera::getViewMatrix()
{
	glm::vec3 delta = target - position;

	delta = glm::normalize(delta);

	adjustUp();

	glm::vec3 s = glm::cross(delta, up);
	s = glm::normalize(s);

	glm::vec3 u = glm::cross(s ,delta);

	glm::mat4 m;

	m[0][0] = s.x;				m[1][0] = s.y;				m[2][0] = s.z;				m[3][0] = 0.f;
	m[0][1] = u.x;				m[1][1] = u.y;				m[2][1] = u.z;				m[3][1] = 0.f;
	m[0][2] = -delta.x;			m[1][2] = -delta.y;			m[2][2] = -delta.z;			m[3][2] = 0.f;
	m[0][3] = 0.f;				m[1][3] = 0.f;				m[2][3] = 0.f;				m[3][3] = 1.f;

	m *= glm::translate(glm::mat4(1.f), -position);
	
	return m;
}

glm::vec3 cgbv::Camera::getUp()
{
	return up;
}

glm::vec3 cgbv::Camera::getForward()
{
	return glm::normalize(target - position); 
}

glm::vec3 cgbv::Camera::getRight()
{
	return glm::cross(getForward(), up);
}


glm::vec3 cgbv::Camera::getPosition()
{
	return position;
}


void cgbv::Camera::setUpOrientation(float orientation)
{
	upOrientation = orientation;
}


void cgbv::Camera::reset()
{
	position.x = 0.f;
	position.y = 0.f;
	position.z = 0.f;

	target.x = 0.f;
	target.y = 0.f;
	target.z = -1.f;

	up.x = 0.f;
	up.y = 1.f;
	up.z = 0.f;
}
//	===========================================================================================================================================================================================		#####
