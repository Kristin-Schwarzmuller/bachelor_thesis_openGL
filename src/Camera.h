#pragma once


// Includes																																														 Includes
//	===========================================================================================================================================================================================		#####
//#include <Datatypes.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
//	===========================================================================================================================================================================================		#####



namespace cgbv
{
	// Camera																																													   Camera
	//	=========================================================================================================================================================================================	#####
	class Camera
	{
		glm::vec3 position;
		glm::vec3 target;
		glm::vec3 up;
		float upOrientation;

		void adjustUp();

	public:
		Camera(void);
		Camera(glm::vec3 position, glm::vec3 target);

		~Camera(void);

		void moveTo(glm::vec3 newPosition);
		void moveTo(float x, float y, float z);

		void moveForward(float delta);
		void moveRight(float delta);

		void setTarget(glm::vec3 newTarget);
		void setTarget(float x, float y, float z);

		void setViewDirection(glm::vec3 viewDirection);
		void setViewDirection(float x, float y, float z);

		void setUpOrientation(float orientation);

		glm::mat4 getViewMatrix();
		glm::vec3 getUp();
		glm::vec3 getForward();
		glm::vec3 getRight();

		void reset();
	};
	//	=========================================================================================================================================================================================	#####
}

