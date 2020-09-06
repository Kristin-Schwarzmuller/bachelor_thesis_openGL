#include <Autopilot.h>
#include <cg\CGRenderer.h>
#include <windows.h>
#include <iostream>
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
		this->modelRotation = modelRotation;
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
		elevationLight = setupVector(0, 90, stepSizeElevationLight, true, false);
		elevationCamera = setupVector(0, 90, stepSizeElevationCamera, true, true); // here no 90 degrees because otherwise the iamge flipes upside down 
		azimuthLight = setupVector(0, 355, stepSizeAzimuthLight, false, false);

		// Initialize the iterators  
		elevationLightPtr = elevationLight.begin();
		elevationCameraPtr = elevationCamera.begin();
		azimuthLightPtr = azimuthLight.begin();
		azimuthCameraPtr = azimuthCamera.at(currentModel).begin();

		lightPos = calPos(*azimuthLightPtr, *elevationLightPtr, distanceLight);
		camPos = calPos(0.f, *elevationCameraPtr, distanceCamera);
			
		// storage and csv
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
		writeDataCSV();
		return true;
	}

	Autopilot::ReturnValues Autopilot::getValues()
	{
			return Autopilot::ReturnValues::ReturnValues(
				lightPos,
				camPos,
				-(*azimuthCameraPtr),
				currentModel,
				currentImagePaths);
	}

	bool Autopilot::step()
	{
		if (tick())
		{
			lightPos = calPos(*azimuthLightPtr, *elevationLightPtr, distanceLight);
			camPos = calPos(0.f, *elevationCameraPtr, distanceCamera);

			defImageNames();
			writeDataCSV();
			return true;
		}
		std::cout << " ====== Yeah! All images have been created! =====" << std::endl;
		return false; 
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

	std::vector<float> Autopilot::setupVector(float from, float to, float step_size, bool firstNotZero, bool lastnotninety)
	{
		std::vector<float> vec;
		for (float i = from; i <= to; i += step_size) {
			vec.push_back(i);
		}
		if (firstNotZero)
		{
			vec.at(0) = 1;
		}
		if (lastnotninety)
		{
			vec.at(vec.size() - 1) = 89.9;
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
		currentImageNames.insert(currentImageNames.begin(), modelNames.at(currentModel) + "\\" + "depth\\" + imageName);
		currentImageNames.push_back(modelNames.at(currentModel) + "\\" + "rgb\\" + imageName);
		currentImageNames.push_back(modelNames.at(currentModel) + "\\" + "normal\\" + imageName);
		currentImageNames.push_back(modelNames.at(currentModel) + "\\" + "shadow_candidate\\" + imageName);
		currentImageNames.push_back(modelNames.at(currentModel) + "\\" + "depth_normalized\\" + imageName);
		
		currentImagePaths.clear(); 
		currentImagePaths.insert(currentImagePaths.begin(), dateFolder + "\\" + currentImageNames[0]);
		currentImagePaths.push_back(dateFolder + "\\" + currentImageNames[1]);
		currentImagePaths.push_back(dateFolder + "\\" + currentImageNames[2]);
		currentImagePaths.push_back(dateFolder + "\\" + currentImageNames[3]);
		currentImagePaths.push_back(dateFolder + "\\" + currentImageNames[4]);
	}					

	bool Autopilot::writeDataCSV()
	{
		//Filename Azimuth	Elevation S_x S_y C_A C_E
		csvFile << currentImageNames[0] << u;
		csvFile << currentImageNames[1] << u;
		csvFile << currentImageNames[2] << u;
		csvFile << currentImageNames[3] << u;
		csvFile << currentImageNames[4] << u;

		csvFile << *azimuthLightPtr << u;
		csvFile << *elevationLightPtr << u;

		csvFile << lightPos.x << u;
		csvFile << lightPos.y << u;
		csvFile << lightPos.z << u;

		csvFile << *azimuthCameraPtr << u;
		csvFile << *elevationCameraPtr << u;

		csvFile << camPos.x << u;
		csvFile << camPos.y << u;
		csvFile << camPos.z << n;

		csvFile.flush();
		return true;
	}

	glm::vec3 Autopilot::calPos(float azimuthPtr, float elevationPtr, float distance)
	{
		// https://www.mathworks.com/help/matlab/ref/sph2cart.html
		x = distance * cosf(glm::radians(elevationPtr)) * sinf(glm::radians(azimuthPtr));
		y = distance * sinf(glm::radians(elevationPtr));
		z = distance * cosf(glm::radians(elevationPtr)) * cosf(glm::radians(azimuthPtr));

		return glm::vec3(x, y, z);
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
			std::filesystem::create_directory((dateFolder + "\\" + modelName + "\\depth").c_str());
			std::filesystem::create_directory((dateFolder + "\\" + modelName + "\\rgb").c_str());
			std::filesystem::create_directory((dateFolder + "\\" + modelName + "\\normal").c_str());
			std::filesystem::create_directory((dateFolder + "\\" + modelName + "\\shadow_candidate").c_str());
			std::filesystem::create_directory((dateFolder + "\\" + modelName + "\\depth_normalized").c_str());

		}

		return true;
	}

}
