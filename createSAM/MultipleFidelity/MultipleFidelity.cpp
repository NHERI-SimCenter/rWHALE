#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <cctype>
#include <HazusSAM_Generator.h>
#include <Building.h>
#include <cstring>
#include <jansson.h>
#include <ConcreteShearWall.h>

int main(int argc, char **argv)
{
	char *filenameBIM = 0;
	char *filenameEVENT = 0;
	char *filenameSAM = 0;
	char *filenameHazusData = 0;
	bool getRV = false;

	int arg = 1;
	while (arg < argc) {
		if (strcmp(argv[arg], "-filenameBIM") == 0) {
			arg++;
			filenameBIM = argv[arg];
		}
		else if (strcmp(argv[arg], "-filenameEVENT") == 0) {
			arg++;
			filenameEVENT = argv[arg];
		}
		else if (strcmp(argv[arg], "-filenameSAM") == 0) {
			arg++;
			filenameSAM = argv[arg];
		}
		else if (strcmp(argv[arg], "-hazusData") == 0) {
			arg++;
			filenameHazusData = argv[arg];
		}
		else if (strcmp(argv[arg], "-getRV") == 0) {
			getRV = true;
		}
		arg++;
	}
	
	json_error_t error;
	bool isShearWall = false;

	json_t* bimJson = json_load_file(filenameBIM, 0, &error);
	json_t* SI = json_object_get(bimJson, "StructuralInformation");
	if (NULL != SI)
	{
		isShearWall = true;
	}
	
	delete bimJson;

	//Creating Different FidelitySAM
	if (isShearWall)
	{
		if (getRV == true)
		{
			int nL = 5;
			int nH = 0;
			ConcreteShearWall* concShearWall = new ConcreteShearWall();
			concShearWall->readBIM(filenameEVENT, filenameBIM);
			concShearWall->writeSAM(filenameSAM, nL, nH);
		}
	}
	else
	{
		HazusSAM_Generator* aim = new HazusSAM_Generator(filenameHazusData);
		Building *theBuilding = new Building();

		if (getRV == true) {
			theBuilding->writeRV(filenameSAM);
		}
		else {
			theBuilding->readBIM(filenameEVENT, filenameBIM, filenameSAM);
			aim->CalcBldgPara(theBuilding);
			theBuilding->writeSAM(filenameSAM);
		}

		delete aim;
	}
	return 0;
}

