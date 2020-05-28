#include "Autopilot.h"
#include <cg\CGRenderer.h>
#include <iostream>
#include <iomanip>
using namespace cgbv;

bool Autopilot::setupAzimuthObject()
{
	// define the different turning ranges for the different models 
	std::vector<int> tmp;
	for (auto maxturn : modelMaxTurn)
	{
		for (int i = 0; i < maxturn; i=+5) {
			tmp.push_back(i);
		}
		tmp.push_back(-1); // add termination criterion in the end
		azimuthCamera.push_back(tmp);
		tmp.clear();
	}
	azimuthCamera.push_back(tmp);

	return true;
}

bool Autopilot::setupVector(int from, int to, int step_size, std::vector<int> vector)
{
	for (int i = from; i < to; i + step_size) {
		vector.push_back(i);
	}
	return true;
}

Autopilot::Autopilot(std::vector<int> modelMaxs)
{
	modelMaxTurn = modelMaxs;
	// Write the values into the vectors to iterate over it later 
	setupAzimuthObject();
	setupVector(0, 95, 5, elevation); // 95 because 90 should be inclueded 
	setupVector(0, 360, 5, azimuthLight); // 360 should be excluded because 360 ~> 0
}

Autopilot::~Autopilot()
{
}

bool Autopilot::run()
{

	// opens an existing csv file or creates a new file. 
	//autopilot.reportCSV.open(autopilot.outputFile, autopilot.csvFile);
	// write the file headers
	outputFile << "Filename" << "," << "Anzimuth Light" << "," << "Elevation Light" << "," << "Anzimuth object" << "," << "Elevation object" << "," << std::endl;
	std::string imageName;
	// iterate over all models
	//for (auto mod : cgbv::CGRenderer::modelfbx.modelPaths)
	for (currentModel = 0; currentModel < modelMaxTurn.size(); currentModel++)
	{
		// for (std::vector<int>::iterator it = elevation.begin(); it != elevation.end(); ++it)
		elevationLightPtr = elevation.begin();
		elevationCameraPtr = elevation.begin();
		azimuthLightPtr = azimuthLight.begin();
		azimuthCameraPtr = azimuthCamera.at(currentModel).begin();

		do
		{
			defImageName();
			writeDataCSV();
			counter++;
		} while (Autopilot::tick);


		//// iterate over anzimuth of the light 
		//for (auto elL : elevation)
		//{
		//	// iterate over the elevation 
		//	for (auto anzL : azimuthLight)
		//	{
		//		for (auto elO : elevation)
		//		{
		//			for (auto anzORange : azimuthObject)
		//			{
		//				for (auto anzO : anzORange)
		//				{
		//					//imageName = mod;
		//					//std::size_t found = mod.find("/");
		//					//if (found != std::string::npos)
		//					//	std::cout << "first 'needle' found at: " << found << '\n';

		//					//found = str.find("needles are small", found + 1, 6);

		//					//std::string str3 = str.substr(pos)
		//					//imageName = model + "AL" + anzL + "EL" + elL + "AO" + anzO + "EO" + elO;
		//					//// write the data to the output file
		//					//autopilot.outputFile << imageName << "," << anzL << "," << elL << "," << anzO << "," << elO << std::endl;
		//				}
		//			}
		//		}
		//	}
		//}

	}
	// close the output file
	reportCSV.close();
	return true;
}

bool Autopilot::tickLight()
{
	if (++elevationLightPtr != elevation.end())
	{
		return true;
	}

	//else reached end of elevation vector = 90 degres on top 
	// --> reset elevation and tick azimuth light 
	elevationLightPtr = elevation.begin();
	if (++azimuthLightPtr != azimuthLight.end())
	{
		return true;
	}
	// both vectors ended --> all light directions from this camera position are done
	return false; 	
}

bool Autopilot::tickCamera()
{
	if (++elevationCameraPtr != elevation.end())
	{
		return true;
	}
	//else reached end of elevation vector = 90 degres on top 
	// --> reset elevation and tick azimuth camera 
	elevationCameraPtr = elevation.begin();
	if (++azimuthCameraPtr != azimuthCamera.at(currentModel).end())
	{
		return true;
	}
	return false;
}

bool Autopilot::tick()
{
	// first try to move the light 
	if (tickLight) {
		return true;
	}
	// if the vectors already reached their end, reset them and tick the camera  
	elevationLightPtr = elevation.begin();
	azimuthLightPtr = azimuthLight.begin();
	if (tickCamera) {
		return true;
	}
	return false;
}

bool Autopilot::defImageName()
{

	return true;
}

bool Autopilot::writeDataCSV()
{
	return false;
}
