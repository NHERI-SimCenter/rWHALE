#ifndef BuildingPropertyMapper_H
#define BuildingPropertyMapper_H
#include "BIM.h"
#include <map>
#include <jansson.h>

namespace BuildingPropertyMapper
{
    std::vector<int> getTypeIds(json_t* structTypeJson);
    void mapBuildingStructType(BIM::BuildingInfo& buildingInfo, json_t* pStructTypeIdMappings);
    void mapBuildingOccupancy(BIM::BuildingInfo& buildingInfo, std::map<int, std::string> occupancyMap, std::string defaultOccupancy);
    void mapBuildingReplacementCost(BIM::BuildingInfo& buildingInfo, std::map<int, BIM::ReplacementCost> replacementCostsMap, BIM::ReplacementCost defaultReplacementCost);
}

#endif