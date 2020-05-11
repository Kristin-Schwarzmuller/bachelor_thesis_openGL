#pragma once

// Includes																																														 Includes
//	===========================================================================================================================================================================================		#####
#include <string>
#include <vector>
//	===========================================================================================================================================================================================		#####
using namespace cgbv; 

class Autopilot
{

    // array with elevation angels --> from 0 to 90 degrees
    const std::vector<int> elevation;
    //std::vector<int>::iterator elevationLightPtr;
    //std::vector<int>::iterator elevationCameraPtr;
    int elevationLightPtr = 0;
    int elevationCameraPtr = 0;

    // array with anzimult angels --> from 0 to 355 degrees
    std::vector<int> azimuthLight;
    //std::vector<int>::iterator azimuthLightPtr;
    int azimuthLightPtr = 0;;

    // array with anzimult angels --> from 0 to 355 degrees
    std::vector<std::vector<int>> azimuthObject;
    //std::vector<int>::iterator azimuthObjectPtr;
    int azimuthObjectPtr = 0;

    // create an ofstream for the file output (see the link on streams for
    // more info)
    std::ofstream outputFile;
    std::ofstream reportCSV;

    // create a name for the file output
    std::string csvFile = "imageInfo.csv";
    bool setupAzimuthObject();
    bool setupVector(int from, int to, int step_size, std::vector<int> vector);

public:
    bool setup();
    bool run();

    bool tick();
    bool tickElevationLight();
    bool tickAzimuthLight(); 
    bool tickElevationCamera();
    bool tickAzimuthObject();
};



