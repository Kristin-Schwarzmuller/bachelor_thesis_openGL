#pragma once

// Includes																																														 Includes
//	===========================================================================================================================================================================================		#####
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <fstream>
//	===========================================================================================================================================================================================		#####
namespace cgbv
{

	class Autopilot
	{
		// Number of FBX models that need to be drawn
		unsigned int currentModel = 0;
		// Number of pictures taken
		unsigned int counter = 0;
		char nameBuffer[50];
		// name of the currently drawn iamge
		std::vector<std::string> currentImageNames;
		std::vector<std::string> currentImagePaths;
		std::string imageName;

		float stepSizeElevationLight = 5.f;
		float startElevationLight = 5.f;

		float stepSizeElevationCamera = 30.f;
		float startElevationCamera = 5.f;

		float stepSizeAzimuthLight = 5.f;
		float stepSizeAzimuthCamera = 45.f;
		// Light Elevation
		std::vector<float> elevationLight;
		std::vector<float>::const_iterator elevationLightPtr;

		// Camera Elevation
		std::vector<float> elevationCamera;
		std::vector<float>::const_iterator elevationCameraPtr;

		// array with anzimult angels for the light --> from 0 to 355 degrees
		std::vector<float> azimuthLight;
		std::vector<float>::const_iterator azimuthLightPtr;

		// array with anzimult angels --> from 0 to 355 degrees
		std::vector<std::vector<float>> azimuthCamera;
		std::vector<float>::const_iterator azimuthCameraPtr;

		// Integer values how much every model needs to be turned around it self 
		std::vector<float> modelMaxTurn;
		// Names of the models for the filenames
		std::vector<std::string> modelNames;

		float distanceCamera;
		float distanceLight;

		// the coordinates for the distance calculation 
		float x, y, z, coA, coE, siA, siE;
		glm::vec3 lightPos;
		glm::vec3 camPos;

		bool first = true;
		bool finished = false;

		// Used to write the CSV 
	   // create an ofstream for the file output (see the link on streams for
	   // more info
		std::string newFolderLocation = "..\\..\\dataset\\";
		std::string dateFolder;
		std::string csvName = "labels_ks.csv";
		std::string u = ",";
		std::string n = "\n";
		const std::vector<std::string> colname{ "Filename Depth", "Filename RGB", "Filename Normal", "Filename Shadow Candidate", "Filename RGBD", "Azimuth", "Elevation", "C_A", "C_E", "L_x", "L_y", "L_z"};
		std::ofstream csvFile;
		// ========================================================================================================================
		// set up the vectors to iterate over them 
		bool setupAzimuthCamera();
		std::vector<float> setupVector(float from, float to, float step_size, float start);

		// Tick to the next position
		bool tick();
		bool tickLight();
		bool tickCamera();


		void defImageNames();
		bool writeDataCSV();
		bool createFolders();
		float cutDecimal(float x, int remainingDecimals);

	public:
		// evtl todo: reset
		// ========================================================================================================================
		// Output that needs to be processed in the CGRederer Class 
		class ReturnValues {

			glm::vec3 lightPos;
			glm::vec3 cameraPos;
			float modelRotation;
			unsigned int modelID;
			std::vector<std::string> imageNames;

		public:
			ReturnValues();
			ReturnValues(glm::vec3 lightPos, glm::vec3 cameraPos, float modelRotation, unsigned int modelID, std::vector<std::string> imageNames);
			glm::vec3 getLightPos();
			glm::vec3 getCameraPos();
			float getModelRotation();
			unsigned int getModelID();
			std::vector<std::string> getImageNames();
		};
		// ========================================================================================================================
		Autopilot();
		~Autopilot();
		bool init(const std::vector<float> modelMaxTurn, const std::vector<std::string> modelNames,
			float lightDistance, float cameraDistance);
		ReturnValues getValues();
		bool step();
		glm::vec3 sph2cart(float azimuthPtr, float elevationPtr, float distance);
		glm::vec2 cart2sph(glm::vec3 position);
	};
}



