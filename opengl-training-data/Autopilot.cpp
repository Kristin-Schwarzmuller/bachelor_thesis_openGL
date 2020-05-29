#include "Autopilot.h"
#include <cg\CGRenderer.h>
#include <iostream>
#include <iomanip>
using namespace cgbv;

Autopilot::Autopilot(std::vector<int> modelMaxTurn, float lightDistance, float cameraDistance)
{
	// allocate the transfered variables 
	modelMaxTurn = modelMaxTurn;
	lightDistance = lightDistance;
	cameraDistance = cameraDistance;

	// Write the values into the vectors to iterate over it later 
	setupAzimuthCamera();
	setupVector(0, 95, 5, elevation); // 95 because 90 should be inclueded 
	setupVector(0, 360, 5, azimuthLight); // 360 should be excluded because 360 ~> 0

	// Send the column name to the file stream
	for (auto name : colname)
	{
		csvFile << name << u;
	}
	csvFile << n;

	// Disable vertical sync (faster image generation) 
	glfwSwapInterval(0);
}

Autopilot::~Autopilot()
{
	csvFile.close();
}

Autopilot::ReturnValues::ReturnValues(glm::vec3 cameraPos, glm::vec3 lightPos, unsigned int currentModel)
{
	cameraPos = cameraPos;
	lightPos = lightPos;
	currentModel = currentModel;
}

Autopilot::ReturnValues Autopilot::run()
{
	// iterate over all models
	//for (auto mod : cgbv::CGRenderer::modelfbx.modelPaths)
	for (currentModel = 0; currentModel < modelMaxTurn.size(); currentModel++)
	{
		// for (std::vector<int>::iterator it = elevation.begin(); it != elevation.end(); ++it)
		elevationLightPtr = elevation.begin();
		elevationCameraPtr = elevation.begin();
		azimuthLightPtr = azimuthLight.begin();
		azimuthCameraPtr = azimuthCamera.at(currentModel).begin();

		do
		{
			defImageName();
			writeDataCSV();
			counter++;
		} while (tick());


	}
	csvFile.close();
	return Autopilot::ReturnValues::ReturnValues(null, null, currentModel);
}

bool Autopilot::setupAzimuthCamera()
{
	// define the different turning ranges for the different models 
	std::vector<int> tmp;
	for (auto maxturn : modelMaxTurn)
	{
		for (int i = 0; i < maxturn; i=+5) {
			tmp.push_back(i);
		}
		tmp.push_back(-1); // add termination criterion in the end
		azimuthCamera.push_back(tmp);
		tmp.clear();
	}
	azimuthCamera.push_back(tmp);

	return true;
}

bool Autopilot::setupVector(int from, int to, int step_size, std::vector<int> vector)
{
	for (int i = from; i < to; i + step_size) {
		vector.push_back(i);
	}
	return true;
}

bool Autopilot::tick()
{
	// first try to move the light 
	if (tickLight()) {
		return true;
	}
	// if the vectors already reached their end, reset them and tick the camera  
	elevationLightPtr = elevation.begin();
	azimuthLightPtr = azimuthLight.begin();
	if (tickCamera()) {
		return true;
	}
	return false;
}

bool Autopilot::tickLight()
{
	if (++elevationLightPtr != elevation.end())
	{
		return true;
	}

	//else reached end of elevation vector = 90 degres on top 
	// --> reset elevation and tick azimuth light 
	elevationLightPtr = elevation.begin();
	if (++azimuthLightPtr != azimuthLight.end())
	{
		return true;
	}
	// both vectors ended --> all light directions from this camera position are done
	return false; 	
}

bool Autopilot::tickCamera()
{
	if (++elevationCameraPtr != elevation.end())
	{
		return true;
	}
	//else reached end of elevation vector = 90 degres on top 
	// --> reset elevation and tick azimuth camera 
	elevationCameraPtr = elevation.begin();
	if (++azimuthCameraPtr != azimuthCamera.at(currentModel).end())
	{
		return true;
	}
	return false;
}


std::string Autopilot::defImageName()
{

	return "";
}

bool Autopilot::writeDataCSV()
{
	//Filename Azimuth	Elevation S_x S_y C_A C_E
	csvFile << imageName << u;
	csvFile << *azimuthLightPtr << u;
	csvFile << *elevationLightPtr << u;
	csvFile << sx << u;
	csvFile << sy << u;
	csvFile << *azimuthCameraPtr << u;
	csvFile << *elevationCameraPtr << n;
	return true;
}

glm::vec3 Autopilot::calPos(std::vector<int>::const_iterator azimuthPtr, std::vector<int>::const_iterator elevationPtr, float distance)
{
	return glm::vec3();
}