#include "BIM.h"

void BIM::readBIMConfig(json_t* pJsonConfig, const char * bimConfig)
{
	//We need to make sure we are able to read the createBim json config
	FILE * pCfgFile = NULL;
	if (NULL != bimConfig)
		pCfgFile = fopen(bimConfig, "r");

	json_error_t error;
	size_t flags = 0;

	if (NULL != pCfgFile)
	{
		pJsonConfig = json_loadf(pCfgFile, flags, &error);
		fclose(pCfgFile);
	}
	else
		pJsonConfig = json_loads(BIM::DefaultBIMConfig, flags, &error);
}

std::map<int, std::string> BIM::parseOccupancyMap(const json_t * pJsonConfig)
{
	std::map<int, std::string> occupancyMap;
	json_t* pBuildingOccupancies = json_object_get(pJsonConfig, "BuildingOccupancyTypes");
	size_t index;
	json_t* pBuildingOccupancy;
	json_array_foreach(pBuildingOccupancies, index, pBuildingOccupancy)
	{
		json_t* typeId = json_object_get(pBuildingOccupancy, "TypeId");
		json_t* occupancy = json_object_get(pBuildingOccupancy, "Occupancy");
		occupancyMap.insert(std::pair<int, std::string>(json_integer_value(typeId), json_string_value(occupancy)));
	}
	return occupancyMap;
}

std::map<int, BIM::ReplacementCost> BIM::parseReplacementCostMap(const json_t * pJsonConfig)
{
	std::map<int, BIM::ReplacementCost> replacementCostsMap;

	json_t* pBuildingOccupancies = json_object_get(pJsonConfig, "BuildingOccupancyTypes");
	size_t index;
	json_t* pBuildingOccupancy;
	json_array_foreach(pBuildingOccupancies, index, pBuildingOccupancy)
	{
		json_t* typeId = json_object_get(pBuildingOccupancy, "TypeId");
		json_t* pReplacementCost = json_object_get(pBuildingOccupancy, "ReplacementCost");
		json_t* unitCost = json_object_get(pReplacementCost, "UnitCost");
		json_t* contentsFactor = json_object_get(pReplacementCost, "ContentsFactor");

		BIM::ReplacementCost replacementCost;
		replacementCost.UnitCost = json_real_value(unitCost);
		replacementCost.ContentsFactor = json_real_value(contentsFactor);

		replacementCostsMap.insert(std::pair<int, BIM::ReplacementCost>(json_integer_value(typeId), replacementCost));
	}

	return replacementCostsMap;
}

double BIM::parseDefaultArea(const json_t * pJsonConfig)
{
	double defaultStoryArea = 0.0;
	json_t* pDefaultsJson = json_object_get(pJsonConfig, "Defaults");
	if (NULL != pDefaultsJson)
	{
		json_t* pDefaultAreaJson = json_object_get(pDefaultsJson, "StoryArea");
		if (NULL != pDefaultAreaJson)
			defaultStoryArea = json_real_value(pDefaultAreaJson);
	}
	return defaultStoryArea;
}

const char * BIM::parseDefaultOccupancy(const json_t * pJsonConfig)
{
	json_t* pDefaultOccupancy = json_object_get(pJsonConfig, "DefaultOccupancy");
	const char* defaultOccupancy = json_string_value(pDefaultOccupancy);
	return defaultOccupancy;
}

BIM::ReplacementCost BIM::parseDefaultReplacementCost(const json_t * pJsonConfig)
{
	json_t* pDefaultReplacementCost = json_object_get(pJsonConfig, "DefaultReplacementCost");
	json_t* unitCost = json_object_get(pDefaultReplacementCost, "UnitCost");
	json_t* contentsFactor = json_object_get(pDefaultReplacementCost, "ContentsFactor");

	BIM::ReplacementCost defaultReplacementCost;
	defaultReplacementCost.UnitCost = json_real_value(unitCost);
	defaultReplacementCost.ContentsFactor = json_real_value(contentsFactor);

	return defaultReplacementCost;
}

std::map<int, const char*> BIM::parseStructureTypeMap(const json_t* pJsonConfig)
{
	json_t* pStructTypes = json_object_get(pJsonConfig, "StructTypes");

	std::map<int, const char*> structTypesMap;
	const char *structTypeKey;
	json_t *structTypeValue;
	json_object_foreach(pStructTypes, structTypeKey, structTypeValue)
	{
		structTypesMap.insert(std::pair<int, const char*>(json_integer_value(structTypeValue), structTypeKey));
	}
	return structTypesMap;
}

