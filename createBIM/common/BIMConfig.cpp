#include "BIMConfig.h"

using namespace BIM;

BIMConfig::BIMConfig()
{
}

BIMConfig::BIMConfig(const char* configPath)
{
	FILE * pCfgFile = NULL;
	if (NULL == configPath)
		throw std::logic_error("Failed to read BIM Config!");

	pCfgFile = fopen(configPath, "r");

	json_error_t error;
	size_t flags = 0;
	json_t* pJsonConfig;

	if (NULL != pCfgFile)
	{
		pJsonConfig = json_loadf(pCfgFile, flags, &error);
		fclose(pCfgFile);
	}
	else
		throw std::logic_error("Failed to read BIM Config!");

	if (!parseConfig(pJsonConfig))
	{
		delete pJsonConfig;
		throw std::logic_error("Failed to parse BIM Config!");
	}

	delete pJsonConfig;

}

BIMConfig* BIM::BIMConfig::defaultConfig()
{
	BIMConfig* pConfig = new BIMConfig();

	json_error_t error;
	size_t flags = 0;
	json_t* pJsonConfig = json_loads(BIMConfig::DefaultBIMConfig.c_str(), flags, &error);

	if (!pConfig->parseConfig(pJsonConfig))
	{
		delete pJsonConfig;
		throw "Failed to parse BIM Config!";
	}

	delete pJsonConfig;

	return pConfig;
}

void BIMConfig::mapOccupancy(BuildingInfo & building)
{
	if (occupancyMap.end() != occupancyMap.find(building.OccupancyId))
		building.Occupancy = occupancyMap[building.OccupancyId];
	else
		building.Occupancy = defaultOccupancy;
}

//This method determines the structure type
//It takes as input the building info (year, building type id and number of stories)
//and the mapping of structure types
//Structure types maps are specified in the json config file
//The output is a json object with either a particular structure type or an array of structure types
//if the structure type is random (uniformly distributed discrete random variable)
void BIMConfig::mapBuildingStructureType(BIM::BuildingInfo& bldgInfo)
{
	int year = bldgInfo.YearBuilt;
	int numStories = bldgInfo.Stories;
	int bldgTypeId = bldgInfo.OccupancyId;

	size_t index;
	json_t* structTypeMap;
	json_array_foreach(structureTypeMappings, index, structTypeMap) {
		//we need to check if this mapping has a year range and read it

		json_t* yearRange = json_object_get(structTypeMap, "YearRange");
		if (NULL != yearRange)
		{
			json_t* min = json_object_get(yearRange, "Min");
			json_t* max = json_object_get(yearRange, "Max");

			int minYear = 0;
			int maxYear = 9999;

			if (NULL != min)
				minYear = json_integer_value(min);

			if (NULL != max)
				maxYear = json_integer_value(max);

			if (year >= minYear && year <= maxYear)
			{
				//We need to check if there is story range maps
				json_t* storyRangeMaps = json_object_get(structTypeMap, "StoryRangeMaps");
				if (NULL != storyRangeMaps)
				{
					size_t index;
					json_t* storyRangeMap;
					json_array_foreach(storyRangeMaps, index, storyRangeMap) {
						json_t* storyRange = json_object_get(storyRangeMap, "StoryRange");

						json_t* min = json_object_get(storyRange, "Min");
						json_t* max = json_object_get(storyRange, "Max");

						int minStory = 1;
						int maxStory = 9999;

						if (NULL != min)
							minStory = json_integer_value(min);

						if (NULL != max)
							maxStory = json_integer_value(max);

						if (numStories >= minStory && numStories <= maxStory)
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
									if (bldgTypeId == json_integer_value(buildingTypeId))
									{
										//This means we found the proper map, so we can return the result
										json_t* structureTypeId = json_object_get(buildingIdsMap, "StructureTypeId");
										bldgInfo.MappedStructTypes = getStructureTypes(structureTypeId);
										return;
									}
								}
							}

							//Handle default mapping for this story range
							json_t* defaultStoryRangeMapping = json_object_get(storyRangeMap, "DefaultMapping");
							if (NULL != defaultStoryRangeMapping)
							{
								json_t* structureTypeId = json_object_get(defaultStoryRangeMapping, "StructureTypeId");
								bldgInfo.MappedStructTypes = getStructureTypes(structureTypeId);
								return;
							}
						}
					}
				}

				//Handle default mapping for this year range
				json_t* defaultYearRangeMapping = json_object_get(structTypeMap, "DefaultMapping");
				if (NULL != defaultYearRangeMapping)
				{
					json_t* structureTypeId = json_object_get(defaultYearRangeMapping, "StructureTypeId");
					bldgInfo.MappedStructTypes = getStructureTypes(structureTypeId);
					return;
				}
			}
		}
	}

	return;
}

//This method takes a structure type id json object and 
//return a vector containing the mapped structure type id values
std::vector<std::string> BIMConfig::getStructureTypes(json_t* structTypeJson)
{
	std::vector<std::string> structureTypes;

	int type = json_typeof(structTypeJson);
	if (JSON_INTEGER == type)
	{	
		int typeId = json_integer_value(structTypeJson);
		structureTypes.push_back(structureTypeMap[typeId]);
	}
	else if (JSON_OBJECT == type)
	{
		json_t* typeValues = json_object_get(structTypeJson, "Values");
		size_t index;
		json_t* typeIdJson;
		json_array_foreach(typeValues, index, typeIdJson) {
			int typeId = json_integer_value(typeIdJson);
			structureTypes.push_back(structureTypeMap[typeId]);
		}
	}
	return structureTypes;
}

void BIMConfig::mapReplacementCost(BuildingInfo & building)
{
	double cost = 0.0;
	BIM::ReplacementCost replacementCost = defaultReplacementCost;
	if (replacementCostMap.end() != replacementCostMap.find(building.OccupancyId))
		replacementCost = replacementCostMap[building.OccupancyId];

	building.ReplacementCost = replacementCost.UnitCost * (1 + replacementCost.ContentsFactor);
}

void BIMConfig::mapReplacementTime(BuildingInfo & building)
{
	building.ReplacementTime = replacementTime;
}

void BIM::BIMConfig::mapStoryHeight(BuildingInfo & building)
{
	building.storyHeightMean = storyHeightMean;
	building.storyHeightStdDev = storyHeightStdDev;
}

void BIMConfig::validateArea(BuildingInfo & building)
{
	if (building.Area <= 0)
		building.Area = defaultStoryArea * building.Stories;
}


bool BIMConfig::parseOccupancyMap(const json_t * pJsonConfig)
{
	occupancyMap.clear();
	json_t* pBuildingOccupancies = json_object_get(pJsonConfig, "BuildingOccupancyTypes");
	if (NULL == pBuildingOccupancies)
		return false;

	size_t index;
	json_t* pBuildingOccupancy;

	json_array_foreach(pBuildingOccupancies, index, pBuildingOccupancy)
	{
		json_t* typeId = json_object_get(pBuildingOccupancy, "TypeId");
		json_t* occupancy = json_object_get(pBuildingOccupancy, "Occupancy");
		occupancyMap.insert(std::pair<int, std::string>(json_integer_value(typeId), json_string_value(occupancy)));
	}

	return true;
}

bool BIMConfig::parseReplacementCostMap(const json_t * pJsonConfig)
{
	json_t* pBuildingOccupancies = json_object_get(pJsonConfig, "BuildingOccupancyTypes");
	if (NULL == pBuildingOccupancies)
		return false;

	size_t index;
	json_t* pBuildingOccupancy;
	json_array_foreach(pBuildingOccupancies, index, pBuildingOccupancy)
	{
		json_t* typeId = json_object_get(pBuildingOccupancy, "TypeId");
		json_t* pReplacementCost = json_object_get(pBuildingOccupancy, "ReplacementCost");
		json_t* unitCost = json_object_get(pReplacementCost, "UnitCost");
		json_t* contentsFactor = json_object_get(pReplacementCost, "ContentsFactor");

		BIM::ReplacementCost replacementCost;
		replacementCost.UnitCost = json_number_value(unitCost);
		replacementCost.ContentsFactor = json_number_value(contentsFactor);

		replacementCostMap.insert(std::pair<int, BIM::ReplacementCost>(json_integer_value(typeId), replacementCost));
	}

	return true;
}

bool BIMConfig::parseDefaultStoryArea(const json_t * pJsonConfig)
{
	defaultStoryArea = 0.0;
	json_t* pDefaultsJson = json_object_get(pJsonConfig, "Defaults");
	if (NULL != pDefaultsJson)
	{
		json_t* pDefaultAreaJson = json_object_get(pDefaultsJson, "StoryArea");
		if (NULL != pDefaultAreaJson)
			defaultStoryArea = json_number_value(pDefaultAreaJson);
	}
	return true;
}

bool BIMConfig::parseDefaultOccupancy(const json_t * pJsonConfig)
{
	json_t* pDefaultOccupancy = json_object_get(pJsonConfig, "DefaultOccupancy");
	if (NULL == pDefaultOccupancy)
		return false;

	defaultOccupancy = std::string(json_string_value(pDefaultOccupancy));

	return true;
}

bool BIMConfig::parseStructureTypeMappings(const json_t * pJsonConfig)
{
	structureTypeMappings = json_object_get(pJsonConfig, "StructTypeIdMappings");

	return true;
}

bool BIMConfig::parseReplacementTime(const json_t * pJsonConfig)
{
	replacementTime = 180.0;
	json_t* pReplacementTime = json_object_get(pJsonConfig, "ReplacementTime");
	replacementTime = json_number_value(pReplacementTime);

	return true;
}

bool BIMConfig::parseStoryHeight(const json_t * pJsonConfig)
{
	storyHeightMean = 3.0;
	storyHeightStdDev = 0.16667;

	json_t* pStoryHeight = json_object_get(pJsonConfig, "StoryHeight");

	if (NULL != pStoryHeight)
	{
		json_t* mean = json_object_get(pStoryHeight, "Mean");

		if(NULL != mean)
			storyHeightMean = json_number_value(mean);

		json_t* stdDev = json_object_get(pStoryHeight, "StdDev");

		if(NULL != stdDev)
			storyHeightStdDev = json_number_value(stdDev);
	}
	return true;
}

bool BIMConfig::parseConfig(json_t * jsonConfig)
{
	//Parsing defaults
	if (!parseDefaultStoryArea(jsonConfig))
		return false;

	if (!parseDefaultOccupancy(jsonConfig))
		return false;

	if (!parseDefaultReplacementCost(jsonConfig))
		return false;
	
	//Parsing property maps
	if (!parseReplacementCostMap(jsonConfig))
		return false;

	if (!parseStructureTypeIdMap(jsonConfig))
		return false;

	if (!parseOccupancyMap(jsonConfig))
		return false;

	if (!parseReplacementTime(jsonConfig))
		return false;

	if (!parseStructureTypeMappings(jsonConfig))
		return false;

	if (!parseStoryHeight(jsonConfig))
		return false;

	return true;
}

bool BIMConfig::parseDefaultReplacementCost(const json_t * pJsonConfig)
{
	json_t* pDefaultReplacementCost = json_object_get(pJsonConfig, "DefaultReplacementCost");
	if (NULL == pDefaultReplacementCost)
		return false;

	json_t* unitCost = json_object_get(pDefaultReplacementCost, "UnitCost");
	if (NULL == unitCost)
		return false;

	json_t* contentsFactor = json_object_get(pDefaultReplacementCost, "ContentsFactor");
	if (NULL == contentsFactor)
		return false;

	defaultReplacementCost.UnitCost = json_number_value(unitCost);
	defaultReplacementCost.ContentsFactor = json_number_value(contentsFactor);

	return true;
}

bool BIMConfig::parseStructureTypeIdMap(const json_t* pJsonConfig)
{
	json_t* pStructTypes = json_object_get(pJsonConfig, "StructTypes");
	if (NULL == pStructTypes)
		return false;

	const char *structTypeKey;
	json_t *structTypeValue;
	json_object_foreach(pStructTypes, structTypeKey, structTypeValue)
	{
		structureTypeMap.insert(std::pair<int, std::string>(json_integer_value(structTypeValue), std::string(structTypeKey)));
	}
	return true;
}


const std::string BIMConfig::DefaultBIMConfig = R"(
{
	"Defaults":	{
		"StoryArea": 288.58525817400027
	},
	"StructTypes": {
		"W1": 1,
		"S1": 2,
		"S2": 3,
		"C1": 4,
		"C2": 5,
		"C3": 6,
		"RM1": 7,
		"RM2": 8,
		"URM": 9
	},
	"StructTypeIdMappings":
	[{
			"YearRange": {
				"Max": 1900
			},
			"DefaultMapping": {
				"StructureTypeId": {
					"Type": "Random_Discrete_Uniform",
					"Values": [1, 7, 8, 9]
				},
				"Description": "Wooden or Masonary"
			}
		}, {
			"YearRange": {
				"Min": 1900
			},
			"StoryRangeMaps": [{
					"StoryRange": {
						"Min": 1,
						"Max": 3
					},
					"BuildingIdMaps": [{
							"BuildingIds": [1, 2, 3, 12],
							"StructureTypeId": 1,
							"Description": "Residential"
						}, {

							"BuildingIds": [4, 5, 6, 10, 11, 13, 14],
							"StructureTypeId": {
								"Type": "Random_Discrete_Uniform",
								"Values": [1, 2, 3, 4, 5, 6, 7, 8]
							},
							"Description": "Commercial"
						}, {
							"BuildingIds": [7, 8, 9],
							"StructureTypeId": {
								"Type": "Random_Discrete_Uniform",
								"Values": [2, 3, 4, 5, 6, 7, 8]
							},
							"Description": "Commercial"
						}
					],
					"DefaultMapping": {
						"StructureTypeId": 1,
						"Description": "Other"
					}
				}, {
					"StoryRange": {
						"Min": 4,
						"Max": 7
					},
					"BuildingIdMaps": [{
							"BuildingIds": [1, 2, 3, 4, 5, 6, 10, 11, 12, 13, 14],
							"StructureTypeId": {
								"Type": "Random_Discrete_Uniform",
								"Values": [1, 2, 3, 4, 5]
							},
							"Description": "Residential & Commercial"
						}, {
							"BuildingIds": [7, 8, 9],
							"StructureTypeId": {
								"Type": "Random_Discrete_Uniform",
								"Values": [2, 3, 4, 5]
							},
							"Description": "Industrial"
						}
					],
					"DefaultMapping": {
						"StructureTypeId": {
							"Type": "Random_Discrete_Uniform",
							"Values": [1, 2, 3, 4, 5]
						},
						"Description": "Other"
					}
				}
			],
			"DefaultMapping": {
				"StructureTypeId": {
					"Type": "Random_Discrete_Uniform",
					"Values": [2, 3, 4, 5]
				},
				"Description": "Other"
			}
		}
	],
	"BuildingOccupancyTypes":
	[{
			"TypeId": 0,
			"Occupancy": "Other/Unknown",
			"ReplacementCost": {
				"UnitCost": 137.5452,
				"ContentsFactor": 0.5
			}
		}, {
			"TypeId": 1,
			"Occupancy": "Residential",
			"ReplacementCost": {
				"UnitCost": 137.5452,
				"ContentsFactor": 0.5
			}
		}, {
			"TypeId": 2,
			"Occupancy": "Residential",
			"ReplacementCost": {
				"UnitCost": 137.5452,
				"ContentsFactor": 0.5
			}
		}, {
			"TypeId": 3,
			"Occupancy": "Residential",
			"ReplacementCost": {
				"UnitCost": 137.5452,
				"ContentsFactor": 0.5
			}
		}, {
			"TypeId": 4,
			"Occupancy": "Office",
			"ReplacementCost": {
				"UnitCost": 131.8863,
				"ContentsFactor": 1.0
			}
		}, {
			"TypeId": 5,
			"Occupancy": "Hotel",
			"ReplacementCost": {
				"UnitCost": 137.271225,
				"ContentsFactor": 0.5
			}
		}, {
			"TypeId": 6,
			"Occupancy": "School",
			"ReplacementCost": {
				"UnitCost": 142.134265,
				"ContentsFactor": 1.25
			}
		}, {
			"TypeId": 7,
			"Occupancy": "Industrial",
			"ReplacementCost": {
				"UnitCost": 97.5247,
				"ContentsFactor": 1.5
			}
		}, {
			"TypeId": 8,
			"Occupancy": "Industrial",
			"ReplacementCost": {
				"UnitCost": 85.9586,
				"ContentsFactor": 1.5
			}
		}, {
			"TypeId": 9,
			"Occupancy": "Industrial",
			"ReplacementCost": {
				"UnitCost": 104.033475,
				"ContentsFactor": 1.5
			}
		}, {
			"TypeId": 10,
			"Occupancy": "Retail",
			"ReplacementCost": {
				"UnitCost": 105.33705,
				"ContentsFactor": 1.0
			}
		}, {
			"TypeId": 11,
			"Occupancy": "Retail",
			"ReplacementCost": {
				"UnitCost": 105.33705,
				"ContentsFactor": 0.5
			}
		}, {
			"TypeId": 12,
			"Occupancy": "Residential",
			"ReplacementCost": {
				"UnitCost": 137.5452,
				"ContentsFactor": 0.5
			}
		}, {
			"TypeId": 13,
			"Occupancy": "Retail",
			"ReplacementCost": {
				"UnitCost": 105.33705,
				"ContentsFactor": 1.0
			}
		}, {
			"TypeId": 14,
			"Occupancy": "Office",
			"ReplacementCost": {
				"UnitCost": 131.8863,
				"ContentsFactor": 1.0
			}
		}, {
			"TypeId": 15,
			"Occupancy": "Parking",
			"ReplacementCost": {
				"UnitCost": 137.5452,
				"ContentsFactor": 0.5
			}
		}, {
			"TypeId": 16,
			"Occupancy": "Parking",
			"ReplacementCost": {
				"UnitCost": 137.5452,
				"ContentsFactor": 0.5
			}
		}
	],
	"DefaultOccupancy": "Residential",
	"DefaultReplacementCost": {
		"UnitCost": 137.5452,
		"ContentsFactor": 0.5
	},
	"StoryHeight": {
		"Mean": 3.0,
		"StdDev": 0.16667
	},
	"ReplacementTime": 180.0
}
)";
