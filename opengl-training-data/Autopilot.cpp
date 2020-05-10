#include "Autopilot.h"
#include <cg\CGRenderer.h>
using namespace cgbv;

bool Autopilot::setupAzimuthObject()
{
	// define the different turning ranges for the different models 
	std::vector<int> tmp;
	for (auto maxturn : cgbv::CGRenderer::modelfbx.modelMaxTurn)
	{
		for (int i = 0; i < maxturn; i + 5) {
			tmp.push_back(i);
		}
		tmp.push_back(-1); // add termination criterion in the end
		azimuthObject.push_back(tmp);
		tmp.clear();
	}
	azimuthObject.push_back(tmp);

	return true;
}

bool Autopilot::setupVector(int from, int to, int step_size, std::vector<int> vector)
{
	for (int i = from; i < to; i + step_size) {
		vector.push_back(i);
	}
	return true;
}

bool Autopilot::setup()
{
	setupAzimuthObject();
	setupVector(0, 90, 5, elevation);
	setupVector(0, 360, 5, azimuthLight);
		
	return true;
}
bool Autopilot::run()
{

	// opens an existing csv file or creates a new file. 
	//autopilot.reportCSV.open(autopilot.outputFile, autopilot.csvFile);
	// write the file headers
	outputFile << "Filename" << "," << "Anzimuth Light" << "," << "Elevation Light" << "," << "Anzimuth object" << "," << "Elevation object" << "," << std::endl;
	std::string imageName;
	// iterate over all models
	for (auto mod : cgbv::CGRenderer::modelfbx.modelPaths)
	{
		loadFBX(mod);
		// iterate over anzimuth of the light 
		for (auto elL : elevation)
		{
			// iterate over the elevation 
			for (auto anzL : azimuthLight)
			{
				for (auto elO : elevation)
				{
					for (auto anzORange : azimuthObject)
					{
						for (auto anzO : anzORange)
						{
							//imageName = mod;
							//std::size_t found = mod.find("/");
							//if (found != std::string::npos)
							//	std::cout << "first 'needle' found at: " << found << '\n';

							//found = str.find("needles are small", found + 1, 6);

							//std::string str3 = str.substr(pos)
							//imageName = model + "AL" + anzL + "EL" + elL + "AO" + anzO + "EO" + elO;
							//// write the data to the output file
							//autopilot.outputFile << imageName << "," << anzL << "," << elL << "," << anzO << "," << elO << std::endl;
						}
					}
				}
			}
		}

	}
	// close the output file
	reportCSV.close();
	return true;
}