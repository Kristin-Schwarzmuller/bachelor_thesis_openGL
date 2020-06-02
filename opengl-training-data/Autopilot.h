#pragma once

// Includes																																														 Includes
//	===========================================================================================================================================================================================		#####
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <math.h>
//	===========================================================================================================================================================================================		#####
using namespace cgbv; 

class Autopilot
{
    // Number of FBX models that need to be drawn
    int currentModel = 0;
    // Number of pictures taken
    unsigned int counter = 0;
    // name of the currently drawn iamge
    std::string imageName = "i bims 1 Bild";

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
    // ========================================================================================================================
    // Output that needs to be processed in the CGRederer Class 
    class ReturnValues {
        
        glm::vec3 cameraPos;
        glm::vec3 lightPos;
        unsigned int currentModel;

     public: 
        ReturnValues(glm::vec3 cameraPos, glm::vec3 lightPos, unsigned int currentModel);
    };
    // ========================================================================================================================
    // Used to write the CSV 
    // create an ofstream for the file output (see the link on streams for
    // more info
    std::string csvName = "labelsKS.csv";
    std::string u = ";";
    std::string n = "\n";
    std::vector<std::string> colname{ "Filename", "Azimuth", "Elevation", "S_x", "S_y", "C_A", "C_E"};
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
    glm::vec3 calPos(std::vector<int>::const_iterator azimuthPtr, std::vector<int>::const_iterator elevationPtr, float distance);
    // todo: cal stereographisch projizierten Koordinaten

public:
    Autopilot(std::vector<int> modelMaxTurn, float lightDistance, float cameraDistance);
    ~Autopilot();
    ReturnValues run();
};



