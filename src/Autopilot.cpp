#include <Autopilot.h>
#include <cg\CGRenderer.h>
#include <windows.h>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <filesystem>



namespace cgbv
{
	//===========================================================================================
	// class ReturnValues
	Autopilot::ReturnValues::ReturnValues()
	{
	}

	Autopilot::ReturnValues::ReturnValues(glm::vec3 lightPos, glm::vec3 cameraPos, float modelRotation, unsigned int modelID, std::vector<std::string> imageNames)
	{
		this->lightPos = lightPos;
		this->cameraPos = cameraPos;
		this->modelRotation = (float)modelRotation;
		this->modelID = modelID;
		this->imageNames = imageNames;
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
	std::vector<std::string> Autopilot::ReturnValues::getImageNames()
	{
		return imageNames;
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
		elevationLight = setupVector(0, 90, stepSizeElevationLight, true);
		elevationCamera = setupVector(0, 90, stepSizeElevationCamera, true);
		azimuthLight = setupVector(0, 355, stepSizeAzimuthLight, false);

		// Initialize the iterators  
		elevationLightPtr = elevationLight.begin();
		elevationCameraPtr = elevationCamera.begin();
		azimuthLightPtr = azimuthLight.begin();
		azimuthCameraPtr = azimuthCamera.at(currentModel).begin();
			
		createFolders();
		csvFile.open((dateFolder + "\\" + csvName), std::ofstream::out | std::ofstream::trunc);

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
		defImageNames();
		return true;
	}

	Autopilot::ReturnValues Autopilot::getValues()
	{
		return Autopilot::ReturnValues::ReturnValues(
			calPos(*azimuthLightPtr, *elevationLightPtr, distanceLight),
			calPos(0.f, *elevationCameraPtr, distanceCamera),
			-(*azimuthCameraPtr),
			currentModel,
			currentImagePaths);
	}

	void Autopilot::step()
	{
		writeDataCSV();
		if (tick())
		{
			defImageNames();
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


	void Autopilot::defImageNames()
	{
		sprintf_s(nameBuffer, "%08d", counter++);

		currentImageNames.clear();
		// Camera Azimuth-Elevation - Light Azimuth-Elevation
		imageName = modelNames.at(currentModel) + std::string(nameBuffer) + "-" + std::to_string(static_cast<int>(*azimuthCameraPtr)) + "-" + std::to_string(static_cast<int>(*elevationCameraPtr)) + "-" + std::to_string(static_cast<int>(*azimuthLightPtr)) + "-" + std::to_string(static_cast<int>(*elevationLightPtr)) + ".png";
		currentImageNames.insert(currentImageNames.begin(), modelNames.at(currentModel) + "\\" + "shadowmap\\" + imageName);
		currentImageNames.push_back(modelNames.at(currentModel) + "\\" + "rgb\\" + imageName);
		currentImageNames.push_back(modelNames.at(currentModel) + "\\" + "normal\\" + imageName);
		currentImageNames.push_back(modelNames.at(currentModel) + "\\" + "shadow_candidate\\" + imageName);
		
		// todo here exeption bei current Model = 7 weil es das nicht mehr gibt 
		currentImagePaths.clear(); 
		currentImagePaths.insert(currentImagePaths.begin(), dateFolder + "\\" + currentImageNames[0]);
		currentImagePaths.push_back(dateFolder + "\\" + currentImageNames[1]);
		currentImagePaths.push_back(dateFolder + "\\" + currentImageNames[2]);
		currentImagePaths.push_back(dateFolder + "\\" + currentImageNames[3]);
	}					

	bool Autopilot::writeDataCSV()
	{
		//Filename Azimuth	Elevation S_x S_y C_A C_E
		csvFile << currentImageNames[0] << u;
		csvFile << currentImageNames[1] << u;
		csvFile << currentImageNames[2] << u;
		csvFile << currentImageNames[3] << u;

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

		return glm::vec3(y, z, x);
		//return glm::vec3(x, y, z);
	}

	bool Autopilot::createFolders()
	{
		struct tm newtime;
		time_t now = time(0);
		localtime_s(&newtime, &now);

		char nameBuffer[50];
		sprintf_s(nameBuffer, "%02d", newtime.tm_mon + 1);
		dateFolder = newFolderLocation + std::to_string(newtime.tm_year - 100) + nameBuffer;
		//dateFolder = "../bin/ImageData/" + std::to_string(newtime.tm_year - 100) + nameBuffer;
		sprintf_s(nameBuffer, "%02d", newtime.tm_mday);
		dateFolder += nameBuffer;
		sprintf_s(nameBuffer, "%02d", newtime.tm_hour);
		dateFolder += '_';
		dateFolder += nameBuffer;
		sprintf_s(nameBuffer, "%02d", newtime.tm_min);
		dateFolder += nameBuffer;

		DWORD ftyp = GetFileAttributesA("newFolderLocation");
		if (ftyp == INVALID_FILE_ATTRIBUTES)
			std::filesystem::create_directory(newFolderLocation.c_str());  //path does not exist --> create it! 

		std::filesystem::create_directory(dateFolder.c_str());
		std::cout << "Folder: " << dateFolder << " created" << std::endl;

		for (std::string modelName : modelNames)
		{
			std::filesystem::create_directory((dateFolder + "\\" + modelName).c_str());
			std::filesystem::create_directory((dateFolder + "\\" + modelName + "\\shadowmap").c_str());
			std::filesystem::create_directory((dateFolder + "\\" + modelName + "\\rgb").c_str());
			std::filesystem::create_directory((dateFolder + "\\" + modelName + "\\normal").c_str());
			std::filesystem::create_directory((dateFolder + "\\" + modelName + "\\shadow_candidate").c_str());

		}

		return true;
	}

}
