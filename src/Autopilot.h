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
		std::string currentImageName;

		int stepSizeElevationLight = 5;
		int stepSizeElevationCamera = 30;
		int stepSizeAzimuthLight = 5;
		int stepSizeAzimuthCamera = 45;
		// Light Elevation
		std::vector<int> elevationLight;
		std::vector<int>::const_iterator elevationLightPtr;

		// Camera Elevation
		std::vector<int> elevationCamera;
		std::vector<int>::const_iterator elevationCameraPtr;

		// array with anzimult angels for the light --> from 0 to 355 degrees
		std::vector<int> azimuthLight;
		std::vector<int>::const_iterator azimuthLightPtr;

		// array with anzimult angels --> from 0 to 355 degrees
		std::vector<std::vector<int>> azimuthCamera;
		std::vector<int>::const_iterator azimuthCameraPtr;

		// Integer values how much every model needs to be turned around it self 
		std::vector<int> modelMaxTurn;
		// Names of the models for the filenames
		std::vector<std::string> modelNames;

		float distanceCamera;
		float distanceLight;

		// the coordinates for the distance calculation 
		float x;
		float y;
		float z;

		// Used to write the CSV 
	   // create an ofstream for the file output (see the link on streams for
	   // more info
		std::string csvName = "labelsKS.csv";
		std::string u = ";";
		std::string n = "\n";
		std::vector<std::string> colname{ "Filename", "Azimuth_objektiv", "Elevation_objectiv", "C_A", "C_E", "Azimuth_subjektiv", "Elevation_subjectiv" };
		std::ofstream csvFile;
		// ========================================================================================================================
		// set up the vectors to iterate over them 
		bool setupAzimuthCamera();
		std::vector<int> setupVector(int from, int to, int step_size);

		// Tick to the next position
		bool tick();
		bool tickLight();
		bool tickCamera();


		void defImageName();
		bool writeDataCSV();
		glm::vec3 calPos(int azimuthPtr, int elevationPtr, float distance);

	public:
		// evtl todo: reset
		// ========================================================================================================================
		// Output that needs to be processed in the CGRederer Class 
		class ReturnValues {

			glm::vec3 lightPos;
			glm::vec3 cameraPos;
			unsigned int modelRotation;
			unsigned int modelID;
			std::string imageName;

		public:
			ReturnValues();
			ReturnValues(glm::vec3 lightPos, glm::vec3 cameraPos, unsigned int modelRotation, unsigned int modelID, std::string imageName);
			glm::vec3 getLightPos();
			glm::vec3 getCameraPos();
			int getModelRotation();
			unsigned int getModelID();
			std::string getImageName();
		};
		// ========================================================================================================================
		Autopilot();
		~Autopilot();
		bool init(const std::vector<int> modelMaxTurn, const std::vector<std::string> modelNames,
			float lightDistance, float cameraDistance);
		ReturnValues getValues();
		void step();
	};
}



