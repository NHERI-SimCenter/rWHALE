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
    FILE * pCfgFile = NULL;
    if (configFile != 0)
        pCfgFile = fopen (configFile , "r");

    json_error_t error;
    size_t flags = 0;

    json_t* pJsonConfig;
    if(NULL != pCfgFile)
    {
        pJsonConfig = json_loadf(pCfgFile, flags, &error);
        fclose(pCfgFile);
    }
    else
        pJsonConfig = json_loads(defaultConfig, flags, &error);

    //Reading default area from config
    double defaultStoryArea = 0.0;
    json_t* pDefaultsJson = json_object_get(pJsonConfig, "Defaults");
    if(NULL != pDefaultsJson)
    {
        json_t* pDefaultAreaJson = json_object_get(pDefaultsJson, "StoryArea");
        if(NULL != pDefaultAreaJson)
            defaultStoryArea = json_real_value(pDefaultAreaJson);
    }

    //We will try to read the struct type map from the json
    json_t* pStructTypes = json_object_get(pJsonConfig, "StructTypes");
  
    std::map<int, const char*> structTypesMap;
    const char *structTypeKey;
    json_t *structTypeValue;
    json_object_foreach(pStructTypes, structTypeKey, structTypeValue)
    {
        structTypesMap.insert(std::pair<int, const char*>(json_integer_value(structTypeValue), structTypeKey));
    }

    std::map<int, std::string> occupancyMap;
    std::map<int, ReplacementCost> replacementCostsMap;

    json_t* pBuildingOccupancies = json_object_get(pJsonConfig, "BuildingOccupancyTypes");
    size_t index;
    json_t* pBuildingOccupancy;
    json_array_foreach(pBuildingOccupancies, index, pBuildingOccupancy) 
    {
        json_t* typeId = json_object_get(pBuildingOccupancy, "TypeId");
        json_t* occupancy = json_object_get(pBuildingOccupancy, "Occupancy");
        occupancyMap.insert(std::pair<int, std::string>(json_integer_value(typeId), json_string_value(occupancy)));
        
        json_t* pReplacementCost = json_object_get(pBuildingOccupancy, "ReplacementCost");
        json_t* unitCost = json_object_get(pReplacementCost, "UnitCost");
        json_t* contentsFactor = json_object_get(pReplacementCost, "ContentsFactor");

        ReplacementCost replacementCost;
        replacementCost.UnitCost = json_real_value(unitCost);
        replacementCost.ContentsFactor = json_real_value(contentsFactor);

        replacementCostsMap.insert(std::pair<int, ReplacementCost>(json_integer_value(typeId), replacementCost));
    }
   
    json_t* pDefaultOccupancy = json_object_get(pJsonConfig, "DefaultOccupancy");
    const char* defaultOccupancy = json_string_value(pDefaultOccupancy);

    json_t* pDefaultReplacementCost = json_object_get(pJsonConfig, "DefaultReplacementCost");
    json_t* unitCost = json_object_get(pDefaultReplacementCost, "UnitCost");
    json_t* contentsFactor = json_object_get(pDefaultReplacementCost, "ContentsFactor");

    ReplacementCost defaultReplacementCost;
    defaultReplacementCost.UnitCost = json_real_value(unitCost);
    defaultReplacementCost.ContentsFactor = json_real_value(contentsFactor);

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
            BuildingInfo bldgInfo;
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
            mapBuildingOccupancy(bldgInfo, occupancyMap, defaultOccupancy);
            //Mapping Replacement Cost
            mapBuildingReplacementCost(bldgInfo, replacementCostsMap, defaultReplacementCost);
            //Mapping Structure Type
            json_t* structTypeIdMappings = json_object_get(pJsonConfig, "StructTypeIdMappings");
            mapBuildingStructType(bldgInfo, structTypeIdMappings);

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


//This method determines the structure type
//It takes as input the building info (year, building type id and number of stories)
//and the mapping of structure types
//Structure types maps are specified in the json config file
//The output is a json object with either a particular structure type or an array of structure types
//if the structure type is random (uniformly distributed discrete random variable)
void mapBuildingStructType(BuildingInfo& bldgInfo, json_t* pStructTypeIdMappings)
{
    int year = bldgInfo.YearBuilt;
    int numStories = bldgInfo.Stories;
    int bldgTypeId = bldgInfo.TypeId;

    size_t index;
    json_t* structTypeMap;
    json_array_foreach(pStructTypeIdMappings, index, structTypeMap) {
        //we need to check if this mapping has a year range and read it

        json_t* yearRange = json_object_get(structTypeMap, "YearRange");
        if(NULL != yearRange)
        {
            json_t* min = json_object_get(yearRange, "Min");
            json_t* max = json_object_get(yearRange, "Max");
            
            int minYear = 0;
            int maxYear = 9999;
            
            if(NULL != min)
                minYear = json_integer_value(min);

            if(NULL != max)
                maxYear = json_integer_value(max);

            if(year >= minYear && year <= maxYear)
            {
                //We need to check if there is story range maps
                json_t* storyRangeMaps = json_object_get(structTypeMap, "StoryRangeMaps");
                if(NULL != storyRangeMaps)
                {
                    size_t index;
                    json_t* storyRangeMap;
                    json_array_foreach(storyRangeMaps, index, storyRangeMap) {
                        json_t* storyRange = json_object_get(storyRangeMap, "StoryRange");

                        json_t* min = json_object_get(storyRange, "Min");
                        json_t* max = json_object_get(storyRange, "Max");
                        
                        int minStory = 1;
                        int maxStory = 9999;
                        
                        if(NULL != min)
                            minStory = json_integer_value(min);

                        if(NULL != max)
                            maxStory = json_integer_value(max);

                        if(numStories >= minStory && numStories <= maxStory)
                        {
                            //Now we need to check if BuildingIdsMaps are defined
                            json_t* buildingIdsMaps = json_object_get(storyRangeMap, "BuildingIdMaps");
                            size_t index;
                            json_t* buildingIdsMap;
                            json_array_foreach(buildingIdsMaps, index, buildingIdsMap) {
                                json_t* buildingIds = json_object_get(buildingIdsMap, "BuildingIds");
                                size_t index;
                                json_t* buildingTypeId;
                                json_array_foreach(buildingIds, index, buildingTypeId) {
                                    if(bldgTypeId == json_integer_value(buildingTypeId))
                                    {
                                        //This means we found the proper map, so we can return the result
                                        json_t* structureTypeId = json_object_get(buildingIdsMap, "StructureTypeId");
                                        bldgInfo.MappedStructTypes = getTypeIds(structureTypeId);
                                        return;
                                    }
                                }
                            }

                            //Handle default mapping for this story range
                            json_t* defaultStoryRangeMapping = json_object_get(storyRangeMap, "DefaultMapping");
                            if(NULL != defaultStoryRangeMapping)
                            {
                                json_t* structureTypeId = json_object_get(defaultStoryRangeMapping, "StructureTypeId");
                                bldgInfo.MappedStructTypes = getTypeIds(structureTypeId);
                                return;
                            }
                        }
                    }                    
                }

                //Handle default mapping for this year range
                json_t* defaultYearRangeMapping = json_object_get(structTypeMap, "DefaultMapping");
                if(NULL != defaultYearRangeMapping)
                {
                    json_t* structureTypeId = json_object_get(defaultYearRangeMapping, "StructureTypeId");
                    bldgInfo.MappedStructTypes = getTypeIds(structureTypeId);
                    return;
                }
            }
        }
    }
    
    return;
}

//This method takes a structure type id json object and 
//return a vector containing the mapped structure type id values
std::vector<int> getTypeIds(json_t* structTypeJson)
{
    std::vector<int> typeIds;

    int type = json_typeof(structTypeJson);
    if(JSON_INTEGER == type)
        typeIds.push_back(json_integer_value(structTypeJson));
    else if(JSON_OBJECT == type)
    {
        json_t* typeValues = json_object_get(structTypeJson, "Values");
        size_t index;
        json_t* typeId;
        json_array_foreach(typeValues, index, typeId) {
            typeIds.push_back(json_integer_value(typeId));
        }
    }
    return typeIds;
}

//This method determines the building occupancy based on the occupancy mapping defined in the json config file
//if no mapping is found, it returns the default mapping
void mapBuildingOccupancy(BuildingInfo& bldgInfo, std::map<int, std::string> occupancyMap, std::string defaultOccupancy)
{
    if(occupancyMap.end() != occupancyMap.find(bldgInfo.TypeId))
        bldgInfo.Occupancy = occupancyMap[bldgInfo.TypeId];
    else
        bldgInfo.Occupancy = defaultOccupancy;    
}

//This method calculates the replacement cost for the specified buiding type or returns the default one
void mapBuildingReplacementCost(BuildingInfo& bldgInfo, std::map<int, ReplacementCost> replacementCostsMap, ReplacementCost defaultReplacementCost)
{
    double cost = 0.0;
    ReplacementCost replacementCost = defaultReplacementCost;
    if(replacementCostsMap.end() != replacementCostsMap.find(bldgInfo.TypeId))
        replacementCost = replacementCostsMap[bldgInfo.TypeId];

    bldgInfo.ReplacementCost = replacementCost.UnitCost * (1 + replacementCost.ContentsFactor);
}
