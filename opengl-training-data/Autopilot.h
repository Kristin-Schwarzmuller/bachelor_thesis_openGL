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

    // array with anzimult angels --> from 0 to 355 degrees
    std::vector<int> azimuthLight;

    // array with anzimult angels --> from 0 to 355 degrees
    std::vector<std::vector<int>> azimuthObject;

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
};



