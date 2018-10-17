#include "BIM.h"
#include "BuildingPropertyMapper.h"

//This method determines the structure type
//It takes as input the building info (year, building type id and number of stories)
//and the mapping of structure types
//Structure types maps are specified in the json config file
//The output is a json object with either a particular structure type or an array of structure types
//if the structure type is random (uniformly distributed discrete random variable)
void BuildingPropertyMapper::mapBuildingStructType(BIM::BuildingInfo& bldgInfo, json_t* pStructTypeIdMappings)
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
std::vector<int> BuildingPropertyMapper::getTypeIds(json_t* structTypeJson)
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
void BuildingPropertyMapper::mapBuildingOccupancy(BIM::BuildingInfo& bldgInfo, std::map<int, std::string> occupancyMap, std::string defaultOccupancy)
{
    if(occupancyMap.end() != occupancyMap.find(bldgInfo.TypeId))
        bldgInfo.Occupancy = occupancyMap[bldgInfo.TypeId];
    else
        bldgInfo.Occupancy = defaultOccupancy;    
}

//This method calculates the replacement cost for the specified buiding type or returns the default one
void BuildingPropertyMapper::mapBuildingReplacementCost(BIM::BuildingInfo& bldgInfo, std::map<int, BIM::ReplacementCost> replacementCostsMap, BIM::ReplacementCost defaultReplacementCost)
{
    double cost = 0.0;
    BIM::ReplacementCost replacementCost = defaultReplacementCost;
    if(replacementCostsMap.end() != replacementCostsMap.find(bldgInfo.TypeId))
        replacementCost = replacementCostsMap[bldgInfo.TypeId];

    bldgInfo.ReplacementCost = replacementCost.UnitCost * (1 + replacementCost.ContentsFactor);
}