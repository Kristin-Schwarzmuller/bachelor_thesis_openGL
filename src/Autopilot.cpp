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

	Autopilot::ReturnValues::ReturnValues(glm::vec3 lightPos, glm::vec3 cameraPos, float modelRotation, unsigned int modelID, std::string imageName)
	{
		this->lightPos = lightPos;
		this->cameraPos = cameraPos;
		this->modelRotation = (float)modelRotation;
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

	float Autopilot::ReturnValues::getModelRotation()
	{
		return modelRotation;
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

	bool Autopilot::init(const std::vector<float> modelMaxTurn, const std::vector<std::string> modelNames, float distanceLight, float distanceCamera)
	{
		// allocate the transfered variables 
		this->modelMaxTurn = modelMaxTurn;
		this->modelNames = modelNames;
		this->distanceLight = distanceLight;
		this->distanceCamera = distanceCamera;

		// Write the values into the vectors to iterate over it later 
		setupAzimuthCamera();
		elevationLight = setupVector(0, 90, stepSizeElevationLight, false);
		elevationCamera = setupVector(0, 90, stepSizeElevationCamera, true);
		azimuthLight = setupVector(0, 355, stepSizeAzimuthLight, false);

		// Initialize the iterators  
		elevationLightPtr = elevationLight.begin();
		elevationCameraPtr = elevationCamera.begin();
		azimuthLightPtr = azimuthLight.begin();
		azimuthCameraPtr = azimuthCamera.at(currentModel).begin();

		csvFile.open(csvName, std::ofstream::out | std::ofstream::trunc);


		std::ofstream test;
		test.open("test.txt", std::ofstream::out | std::ofstream::trunc);
		test.flush();
		test.close();
		// Send the column name to the file stream
		if (!csvFile) {
			std::cout << "Error while creating csv" << std::endl;
		}
		for (auto name : colname)
		{
			csvFile << name << u;
		}
		csvFile << n;
		csvFile.flush();
		defImageName();
		return true;
	}

	Autopilot::ReturnValues Autopilot::getValues()
	{
		return Autopilot::ReturnValues::ReturnValues(
			calPos(*azimuthLightPtr, *elevationLightPtr, distanceLight),
			calPos(0.f, *elevationCameraPtr, distanceCamera),
			-(*azimuthCameraPtr),
			currentModel,
			currentImageName);
	}

	void Autopilot::step()
	{
		writeDataCSV();
		if (tick())
		{
			defImageName();
		}
	}

	bool Autopilot::setupAzimuthCamera()
	{
		//todo: nur ein vector pro maxturn value 
		//define the different turning ranges for the different models 
		std::vector<float> tmp;
		for (auto maxturn : modelMaxTurn)
		{
			for (float i = 0; i < maxturn; i += stepSizeAzimuthCamera) {
				tmp.push_back(i);
			}
			azimuthCamera.push_back(tmp);
			tmp.clear();
		}
		return true;
	}

	std::vector<float> Autopilot::setupVector(float from, float to, float step_size, bool firstNotZero)
	{
		std::vector<float> vec;
		for (float i = from; i <= to; i += step_size) {
			vec.push_back(i);
		}
		if (firstNotZero)
		{
			vec.at(0) = 1;
		}
		return vec;
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
			elevationLightPtr = elevationLight.begin();
			azimuthLightPtr = azimuthLight.begin();
			return true;
		}
		// else: 
		if (++currentModel < modelMaxTurn.size())
		{
			elevationLightPtr = elevationLight.begin();
			elevationCameraPtr = elevationCamera.begin();
			azimuthLightPtr = azimuthLight.begin();
			azimuthCameraPtr = azimuthCamera.at(currentModel).begin();
			return true;
		}
		// everything is done
		csvFile.close();
		return false;
	}

	bool Autopilot::tickLight()
	{
		if (++elevationLightPtr != elevationLight.end())
		{
			return true;
		}

		//else reached end of elevation vector = 90 degres on top 
		// --> reset elevation and tick azimuth light 

		if (++azimuthLightPtr != azimuthLight.end())
		{
			elevationLightPtr = elevationLight.begin();
			return true;
		}
		// both vectors ended --> all light directions from this camera position are done
		return false;
	}

	bool Autopilot::tickCamera()
	{
		if (++elevationCameraPtr != elevationCamera.end())
		{
			return true;
		}
		//else reached end of elevation vector = 90 degres on top 
		// --> reset elevation and tick azimuth camera 
		// just for the labeling in the csv --> needs to be 
		if (++azimuthCameraPtr != azimuthCamera.at(currentModel).end())
		{
			elevationCameraPtr = elevationCamera.begin();
			return true;
		}
		return false;
	}


	void Autopilot::defImageName()
	{
		sprintf_s(nameBuffer, "%08d", counter++);
		// todo here exeption bei current Model = 7 weil es das nicht mehr gibt 
		currentImageName = std::string(nameBuffer) + modelNames.at(currentModel);
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
		csvFile.flush();
		return true;
	}

	glm::vec3 Autopilot::calPos(float azimuthPtr, float elevationPtr, float distance)
	{
		// https://en.wikipedia.org/wiki/Spherical_coordinate_system#Cartesian_coordinates
		x = distance * cosf(glm::radians(elevationPtr)) * cosf(glm::radians(azimuthPtr));
		y = distance * cosf(glm::radians(elevationPtr)) * sinf(glm::radians(azimuthPtr));
		z = distance * sinf(glm::radians(elevationPtr));

		//return glm::vec3(y, z, x);
		return glm::vec3(x, y, z);
	}
}