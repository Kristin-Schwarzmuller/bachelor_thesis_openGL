#include "Autopilot.h"
#include <cg\CGRenderer.h>
#include <iostream>
#include <iomanip>
#include <cmath>


namespace cgbv
{
	//===========================================================================================
	// class ReturnValues
	Autopilot::ReturnValues::ReturnValues()
	{
	}

	Autopilot::ReturnValues::ReturnValues(glm::vec3 lightPos, glm::vec3 cameraPos, unsigned int modelID, std::string imageName)
	{
		this->lightPos = lightPos;
		this->cameraPos = cameraPos;
		this->modelID = modelID;
		this->imageName = imageName;
	}

	glm::vec3 Autopilot::ReturnValues::getLightPos()
	{
		return lightPos;
	}

	glm::vec3 Autopilot::ReturnValues::getCameraPos()
	{
		return cameraPos;
	}

	unsigned int Autopilot::ReturnValues::getModelID()
	{
		return modelID;
	}
	std::string Autopilot::ReturnValues::getImageName()
	{
		return imageName;
	}
	// ===========================================================================================
	// class Autopilot 

	Autopilot::Autopilot()
	{
	}

	Autopilot::~Autopilot()
	{
		csvFile.close();
	}

	bool Autopilot::init(const std::vector<int> modelMaxTurn, float distanceLight, float distanceCamera)
	{
		// allocate the transfered variables 
		this->modelMaxTurn = modelMaxTurn;
		this->distanceLight = distanceLight;
		this->distanceCamera = distanceCamera;

		// Write the values into the vectors to iterate over it later 
		setupAzimuthCamera();
		setupVector(0, 95, 5, elevation); // 95 because 90 should be inclueded 
		setupVector(0, 360, 5, azimuthLight); // 360 should be excluded because 360 ~> 0

		// Initialize the iterators  
		elevationLightPtr = elevation.begin();
		elevationCameraPtr = elevation.begin();
		azimuthLightPtr = azimuthLight.begin();
		azimuthCameraPtr = azimuthCamera.at(currentModel).begin();

		// Send the column name to the file stream
		for (auto name : colname)
		{
			csvFile << name << u;
		}
		csvFile << n;
		return true;
	}

	Autopilot::ReturnValues Autopilot::getValues()
	{
		return Autopilot::ReturnValues::ReturnValues(
			calPos(*azimuthLightPtr, *elevationLightPtr, distanceLight),
			calPos(*azimuthCameraPtr, *elevationCameraPtr, distanceCamera),
			currentModel,
			currentImageName);
	}

	void Autopilot::step()
	{
		currentImageName = defImageName();
		writeDataCSV();
		counter++;
		tick();
	}

	bool Autopilot::setupAzimuthCamera()
	{
		// todo: nur ein vector pro maxturn value 
		// define the different turning ranges for the different models 
		std::vector<int> tmp;
		for (auto maxturn : modelMaxTurn)
		{
			for (int i = 0; i < maxturn; i+=45) {
				tmp.push_back(i);
			}
			azimuthCamera.push_back(tmp);
			tmp.clear();
		}
		azimuthCamera.push_back(tmp);

		return true;
	}

	bool Autopilot::setupVector(int from, int to, int step_size, std::vector<int> vector)
	{
		for (int i = from; i < to; i+=step_size) {
			vector.push_back(i);
		}
		return true;
	}

	bool Autopilot::tick()
	{
		// first try to move the light 
		if (tickLight()) 
		{
			return true;
		}
		// if the vectors already reached their end, reset them and tick the camera  
		if (tickCamera()) 
		{
			elevationLightPtr = elevation.begin();
			azimuthLightPtr = azimuthLight.begin();
			return true;
		}
		// else: 
		if (++currentModel < modelMaxTurn.size())
		{
			elevationLightPtr = elevation.begin();
			elevationCameraPtr = elevation.begin();
			azimuthLightPtr = azimuthLight.begin();
			azimuthCameraPtr = azimuthCamera.at(currentModel).begin();
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
		csvFile << currentImageName << u;
		csvFile << *azimuthLightPtr << u;
		csvFile << *elevationLightPtr << u;
		//csvFile << sx << u;
		//csvFile << sy << u;
		csvFile << *azimuthCameraPtr << u;
		csvFile << *elevationCameraPtr << n;
		return true;
	}

	glm::vec3 Autopilot::calPos(int azimuthPtr, int elevationPtr, float distance)
	{
		// https://en.wikipedia.org/wiki/Spherical_coordinate_system#Cartesian_coordinates
		x = distance * sinf(static_cast<float>(elevationPtr)) * cosf(static_cast<float>(azimuthPtr));
		y = distance * sinf(static_cast<float>(elevationPtr)) * sinf(static_cast<float>(azimuthPtr));
		z = distance * cosf(static_cast<float>(elevationPtr));

		return glm::vec3(x, y, z);
	}
}