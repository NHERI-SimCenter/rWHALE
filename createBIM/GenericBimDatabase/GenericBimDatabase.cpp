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
#include "BuildingPropertyMapper.h"

using namespace std;

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
    json_t* pJsonConfig = NULL;
	BIM::readBIMConfig(pJsonConfig, configFile);
	if (NULL == pJsonConfig)
	{
		std::cerr << "Failed to read BIM config!";
		exit(-1);
	}

    //Reading default area from config
    double defaultStoryArea = BIM::parseDefaultArea(pJsonConfig);

    //We will try to read the struct type map from the json
	std::map<int, const char*> structTypesMap = BIM::parseStructureTypeMap(pJsonConfig);
    
	//Parse occupancy map from BIM Config
    std::map<int, std::string> occupancyMap;

	//Parse replacement cost map from BIM Config
	std::map<int, BIM::ReplacementCost> replacementCostsMap = BIM::parseReplacementCostMap(pJsonConfig);

	//Parsing default replacement cost from BIM Config
    auto defaultReplacementCost = BIM::parseDefaultReplacementCost(pJsonConfig);

	//Parsing default occupancy from BIM Config
	auto defaultOccupancy = BIM::parseDefaultOccupancy(pJsonConfig);

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
  
    json_t *root = json_object();

    while ((row = CsvParser_getRow(csvparser))) 
    {
        if (currentRow >= minRow && currentRow <= maxRow)
        {
            const char **rowFields = CsvParser_getFields(row);

            json_t *GI = json_object();
            json_t *distribution = json_array();
            json_t *structtype = json_object();
            json_t *height = json_object();
            json_t *valuenames = json_array();

            //Reading info from CSV
            BIM::BuildingInfo bldgInfo;
            bldgInfo.Id = rowFields[0];
            bldgInfo.Stories = atoi(rowFields[2]);
            bldgInfo.Area = strtod(rowFields[1], NULL)/10.764; //Converting area from sqft to meters squared
            bldgInfo.YearBuilt = atoi(rowFields[3]);
            bldgInfo.TypeId = atoi(rowFields[4]);
            bldgInfo.Location.Latitude = strtod(rowFields[5], NULL);
            bldgInfo.Location.Longitude = strtod(rowFields[6], NULL);
            
            if (bldgInfo.Area <= 0)
                bldgInfo.Area = defaultStoryArea * bldgInfo.Stories;
            
            //Mapping Occupancy
			BuildingPropertyMapper::mapBuildingOccupancy(bldgInfo, occupancyMap, defaultOccupancy);
            //Mapping Replacement Cost
			BuildingPropertyMapper::mapBuildingReplacementCost(bldgInfo, replacementCostsMap, defaultReplacementCost);
            //Mapping Structure Type
            json_t* structTypeIdMappings = json_object_get(pJsonConfig, "StructTypeIdMappings");
			BuildingPropertyMapper::mapBuildingStructType(bldgInfo, structTypeIdMappings);

            //Setting info in Json
            json_object_set(GI, "area", json_real(bldgInfo.Area/bldgInfo.Stories));
            json_object_set(GI, "name", json_string(bldgInfo.Id.c_str()));
            json_object_set(GI, "numStory", json_integer(bldgInfo.Stories));
            json_object_set(GI, "yearBuilt", json_integer(bldgInfo.YearBuilt));
            
            int numMappedTypes = bldgInfo.MappedStructTypes.size();
            if (numMappedTypes == 1)
            {
                int structTypeId = bldgInfo.MappedStructTypes[0];
                json_object_set(GI,"structType", json_string(structTypesMap[structTypeId]));
            }
            else
            {                
                json_object_set(GI, "structType", json_string("RV.structType"));

                for(int i = 0; i < bldgInfo.MappedStructTypes.size(); i++) 
                {
                    int structTypeId = bldgInfo.MappedStructTypes[i];
                    json_array_append(valuenames, json_string(structTypesMap[structTypeId]));
                }

                json_object_set(structtype,"distribution", json_string("discrete_design_set_string"));
                json_object_set(structtype,"name", json_string("structType"));
                json_object_set(structtype,"value", json_string("RV.structType"));
                json_object_set(structtype,"elements", valuenames);

                json_array_append(distribution, structtype);	
            }

            json_object_set(GI,"occupancy",json_string(bldgInfo.Occupancy.c_str()));
            json_object_set(GI,"height",json_string("RV.height"));
            
            json_object_set(GI,"replacementCost", json_real(bldgInfo.ReplacementCost * bldgInfo.Area * 10.764));

            double replacementTime = 180.0;      
            json_t* pReplacementTime = json_object_get(pJsonConfig, "ReplacementTime");
            if(NULL != pReplacementTime)
                replacementTime = json_real_value(pReplacementTime);

            json_object_set(GI,"replacementTime", json_real(replacementTime));
            
            json_t *location = json_object();
            json_object_set(location, "latitude", json_real(bldgInfo.Location.Latitude));
            json_object_set(location, "longitude", json_real(bldgInfo.Location.Longitude));
            json_object_set(GI, "location", location);

            json_object_set(height,"distribution", json_string("normal"));
            json_object_set(height,"name", json_string("height"));
            json_object_set(height,"value", json_string("RV.height"));
            
            double storyHeightMean = 3.0;
            double storyHeightStdDev = 0.16667;
            
            json_t* pStoryHeight = json_object_get(pJsonConfig, "StoryHeight");
            
            if(NULL != pStoryHeight)
            {
                json_t* mean = json_object_get(pStoryHeight, "Mean");
                storyHeightMean = json_real_value(mean);
                json_t* stdDev = json_object_get(pStoryHeight, "StdDev");
                storyHeightStdDev = json_real_value(stdDev);
            }

            json_object_set(height, "mean", json_real(bldgInfo.Stories * storyHeightMean));
            json_object_set(height, "stdDev", json_real(storyHeightStdDev * bldgInfo.Stories));

            json_array_append(distribution, height);

            json_object_set(root,"RandomVariables", distribution);
            json_object_set(root,"GI", GI);
            std::string filename;

            filename = bldgInfo.Id + std::string("-BIM.json");

            json_t *bldg = json_object();
            json_object_set(bldg,"id", json_string(bldgInfo.Id.c_str()));
            json_object_set(bldg,"file",json_string(filename.c_str()));
            json_array_append(buildingsArray, bldg);
            
            // write the file & clean memory
            json_dump_file(root, filename.c_str(), 0);
            json_object_clear(root);
            CsvParser_destroy_row(row);
        }

        currentRow++;
        
        if (currentRow > maxRow)
            break;
    }

    json_dump_file(buildingsArray, outputFile, 0);

    CsvParser_destroy(csvparser);
    
    return 0;
}



