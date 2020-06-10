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
        int currentModel = 0;
        // Number of pictures taken
        unsigned int counter = 0;
        // name of the currently drawn iamge
        std::string currentImageName = "i bims 1 Bild";

        // array with elevation angels --> from 0 to 90 degrees
        const std::vector<int> elevation;
        std::vector<int>::const_iterator elevationLightPtr;
        std::vector<int>::const_iterator elevationCameraPtr;

        // array with anzimult angels for the light --> from 0 to 355 degrees
        std::vector<int> azimuthLight;
        std::vector<int>::const_iterator azimuthLightPtr;

        // array with anzimult angels --> from 0 to 355 degrees
        std::vector<std::vector<int>> azimuthCamera;
        std::vector<int>::const_iterator azimuthCameraPtr;

        // Integer values how much every model needs to be turned around it self 
        std::vector<int> modelMaxTurn;

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
        std::vector<std::string> colname{ "Filename", "Azimuth", "Elevation", "S_x", "S_y", "C_A", "C_E" };
        std::ofstream csvFile;
        // ========================================================================================================================
        // set up the vectors to iterate over them 
        bool setupAzimuthCamera();
        bool setupVector(int from, int to, int step_size, std::vector<int> vector);


        // Tick to the next position
        bool tick();
        bool tickLight();
        bool tickCamera();


        std::string defImageName();
        bool writeDataCSV();
        glm::vec3 calPos(int azimuthPtr, int elevationPtr, float distance);

    public:
        // evtl todo: reset
        // ========================================================================================================================
        // Output that needs to be processed in the CGRederer Class 
        class ReturnValues {

            glm::vec3 lightPos; 
            glm::vec3 cameraPos;
            unsigned int modelID;
            std::string imageName;

        public:
            ReturnValues();
            ReturnValues(glm::vec3 lightPos, glm::vec3 cameraPos, unsigned int modelID, std::string imageName); 
            glm::vec3 getLightPos();
            glm::vec3 getCameraPos();
            unsigned int getModelID();
            std::string getImageName();
        };
        // ========================================================================================================================
        Autopilot();
        ~Autopilot();
        bool init(const std::vector<int> modelMaxTurn, float lightDistance, float cameraDistance);
        ReturnValues getValues();
        void step();
    };
}



