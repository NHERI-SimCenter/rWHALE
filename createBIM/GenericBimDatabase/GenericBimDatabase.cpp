// createBIM.c
// purpose - give a single csv file with building information create a BIM model
// written: fmckenna

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>

#include "csvparser.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <limits>
#include "GenericBimDatabase.h"
#include "BIM.h"
#include "BIMConfig.h"

using namespace std;
void parseRow(BIM::BuildingInfo& building, const CsvRow* row);
void writeBIMJson(BIM::BuildingInfo& building, std::string filename);

int main(int argc, const char **argv)
{
    //
    // parse inputs
    //
    int minRow = -1;
    int maxRow = -1;
    bool getRV = false;

    const char* buildingsFilename = NULL;
    const char* configFile = NULL;
    const char* outputFile = argv[1];

    int arg = 2;
    while(arg < argc)
    {
        if (strcmp(argv[arg], "-Min") == 0)
        {
            arg++;
            minRow = atoi(argv[arg]);
        }
        else if (strcmp(argv[arg], "-Max") == 0)
        {
            arg++;
            maxRow = atoi(argv[arg]);
        }
        else if (strcmp(argv[arg], "-buildingsFile") == 0)
        {
            arg++;
            buildingsFilename = argv[arg];
            std::cerr << "Buildings File: " << buildingsFilename << std::endl;    
        }
        else if (strcmp(argv[arg], "-config") == 0)
        {
            arg++;
            configFile = argv[arg];
        }
        else if (strcmp(argv[arg], "-getRV") == 0)
        {
            getRV = true;
        }
        arg++;
    }

    if (getRV == false)
    {
        // here we would process the BIM file given the RV value .. this app does nothing at present
        return 0;
    }

    // check for need to swap as someone will be an idiot
    if (minRow > maxRow)
    {
        int tmp = minRow;
        minRow = maxRow;
        maxRow = tmp;
    }

    if (minRow == -1 || maxRow == -1 || buildingsFilename == 0)
    {
        std::cerr << "INVALID INPUT\n";
        exit(-1);
    }

    std::cerr << "GenericBimDatabase: min: " << minRow << " max: " << maxRow << "\n";

    //
    // Json array containing output
    //
    json_t *rootBuildings = json_object();
    json_t *buildingsArray = json_array();
    
    //We need to make sure we are able to read the createBim json config

	BIM::BIMConfig* bimConfig = NULL;
	
	try
	{
		bimConfig = new BIM::BIMConfig(configFile);
		if (NULL == bimConfig)
		{
			std::cerr << "Failed to read BIM config!";
			exit(-1);
		}
	}
	catch (exception& e)
	{
		bimConfig = BIM::BIMConfig::defaultConfig();
	}
	

    //
    // Now parse the building to get building info
    // writing and write a BIM file
    //
  
    CsvParser* csvparser = CsvParser_new(buildingsFilename, ",", 1);
    const CsvRow* header = CsvParser_getHeader(csvparser);
    CsvRow* row;

    if (header == NULL) {
        printf("%s\n", CsvParser_getErrorMessage(csvparser));
        return 1;
    }
  
    int currentRow = 1;
  

    while ((row = CsvParser_getRow(csvparser))) 
    {
        if (currentRow >= minRow && currentRow <= maxRow)
        {
            //Reading info from CSV
            BIM::BuildingInfo bldgInfo;
			parseRow(bldgInfo, row);
            
			//Validating area
			bimConfig->validateArea(bldgInfo);
            
            //Mapping Occupancy
			bimConfig->mapOccupancy(bldgInfo);
			
			//Mapping Replacement Cost
			bimConfig->mapReplacementCost(bldgInfo);
			
			//Mapping Structure Type
			bimConfig->mapBuildingStructureType(bldgInfo);
			
			//Mapping story height
			bimConfig->mapStoryHeight(bldgInfo);

			//Map replacement time
			bimConfig->mapReplacementTime(bldgInfo);

            std::string filename;
            filename = bldgInfo.Id + std::string("-BIM.json");

			//Writing the BIM file
			writeBIMJson(bldgInfo, filename);

            json_t *bldg = json_object();
            json_object_set(bldg,"id", json_string(bldgInfo.Id.c_str()));
            json_object_set(bldg,"file",json_string(filename.c_str()));
            json_array_append(buildingsArray, bldg);
            
            // write the file & clean memory
            CsvParser_destroy_row(row);
        }

        currentRow++;
        
        if (currentRow > maxRow)
            break;
    }

    json_dump_file(buildingsArray, outputFile, 0);

    CsvParser_destroy(csvparser);
    
	delete bimConfig;
    return 0;
}

void parseRow(BIM::BuildingInfo & building, const CsvRow * row)
{
	const char **rowFields = CsvParser_getFields(row);

	building.Id = rowFields[0];
	building.Stories = atoi(rowFields[2]);
	building.Area = strtod(rowFields[1], NULL) / 10.764; //Converting area from sqft to meters squared
	building.YearBuilt = atoi(rowFields[3]);
	building.OccupancyId = atoi(rowFields[4]);
	building.Location.Latitude = strtod(rowFields[5], NULL);
	building.Location.Longitude = strtod(rowFields[6], NULL);
}

void writeBIMJson(BIM::BuildingInfo & bldgInfo, std::string filename)
{
	json_t *root = json_object();

	json_t *GI = json_object();
	json_t *distribution = json_array();
	json_t *structtype = json_object();
	json_t *height = json_object();
	json_t *valuenames = json_array();

	//Setting info in Json
	json_object_set(GI, "area", json_real(bldgInfo.Area / bldgInfo.Stories));
	json_object_set(GI, "name", json_string(bldgInfo.Id.c_str()));
	json_object_set(GI, "numStory", json_integer(bldgInfo.Stories));
	json_object_set(GI, "yearBuilt", json_integer(bldgInfo.YearBuilt));

	int numMappedTypes = bldgInfo.MappedStructTypes.size();
	if (numMappedTypes == 1)
		json_object_set(GI, "structType", json_string(bldgInfo.MappedStructTypes[0].c_str()));
	else
	{
		json_object_set(GI, "structType", json_string("RV.structType"));

		for (int i = 0; i < bldgInfo.MappedStructTypes.size(); i++)
			json_array_append(valuenames, json_string(bldgInfo.MappedStructTypes[i].c_str()));

		json_object_set(structtype, "distribution", json_string("discrete_design_set_string"));
		json_object_set(structtype, "name", json_string("structType"));
		json_object_set(structtype, "value", json_string("RV.structType"));
		json_object_set(structtype, "elements", valuenames);

		json_array_append(distribution, structtype);
	}

	json_object_set(GI, "occupancy", json_string(bldgInfo.Occupancy.c_str()));
	json_object_set(GI, "height", json_string("RV.height"));

	json_object_set(GI, "replacementCost", json_real(bldgInfo.ReplacementCost * bldgInfo.Area * 10.764));


	json_object_set(GI, "replacementTime", json_real(bldgInfo.ReplacementTime));

	json_t *location = json_object();
	json_object_set(location, "latitude", json_real(bldgInfo.Location.Latitude));
	json_object_set(location, "longitude", json_real(bldgInfo.Location.Longitude));
	json_object_set(GI, "location", location);

	json_object_set(height, "distribution", json_string("normal"));
	json_object_set(height, "name", json_string("height"));
	json_object_set(height, "value", json_string("RV.height"));

	json_object_set(height, "mean", json_real(bldgInfo.Stories * bldgInfo.storyHeightMean));
	json_object_set(height, "stdDev", json_real(bldgInfo.storyHeightStdDev * bldgInfo.Stories));

	json_array_append(distribution, height);

	json_object_set(root, "RandomVariables", distribution);
	json_object_set(root, "GI", GI);

	json_dump_file(root, filename.c_str(), 0);
}
