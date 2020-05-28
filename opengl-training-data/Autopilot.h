#pragma once

// Includes																																														 Includes
//	===========================================================================================================================================================================================		#####
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
//	===========================================================================================================================================================================================		#####
using namespace cgbv; 

class Autopilot
{
    // Number of FBX models that need to be drawn
    int currentModel = 0;
    // Number of pictures taken
    unsigned int counter = 0;

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

    std::vector<int> modelMaxTurn;

    glm::mat4 rotationMat = glm::mat4(1.0f);
   
    // Output that needs to be processed in the CGRederer Class 
    class ReturnValues {
        
        std::vector<float> cameraPos;
        std::vector<float> lightPos;
        unsigned int currentModel;

    };

    // create an ofstream for the file output (see the link on streams for
    // more info
    std::ofstream outputFile;
    std::ofstream reportCSV;

    // create a name for the file output
    std::string csvFile = "imageInfo.csv";
    bool setupAzimuthObject();
    bool setupVector(int from, int to, int step_size, std::vector<int> vector);

public:
    Autopilot(std::vector<int> modelMaxs);
    ~Autopilot();
    bool run();
    
    bool defImageName();
    bool writeDataCSV();
    bool tick();
    bool tickLight();
    bool tickCamera();
    bool tickAzimuthObject();
};



