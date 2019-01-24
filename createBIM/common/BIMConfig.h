#ifndef BIMConfig_H
#define BIMConfig_H

#include "BIM.h"

namespace BIM
{
	class BIMConfig
	{
	public:
		BIMConfig(const char* configPath);
		static BIMConfig* defaultConfig();

		void mapOccupancy(BuildingInfo& building);
		void mapBuildingStructureType(BuildingInfo& building);
		void mapReplacementCost(BuildingInfo& building);
		void mapReplacementTime(BuildingInfo& building);
		void mapStoryHeight(BuildingInfo& building);
		void validateArea(BuildingInfo& building);


	private:
		BIMConfig();
		BIMConfig(const std::string bimConfig);

		bool parseConfig(json_t* jsonConfig);
		
		//Parse Defaults
		bool parseDefaultReplacementCost(const json_t * pJsonConfig);
		bool parseDefaultOccupancy(const json_t * pJsonConfig);
		bool parseDefaultStoryArea(const json_t * pJsonConfig);

		//Parse Property Maps
		bool parseOccupancyMap(const json_t * pJsonConfig);
		bool parseReplacementCostMap(const json_t * pJsonConfig);
		bool parseStructureTypeIdMap(const json_t* pJsonConfig);
		bool parseStructureTypeMappings(const json_t* pJsonConfig);

		bool parseReplacementTime(const json_t * pJsonConfig);
		bool parseStoryHeight(const json_t* pJsonConfig);

		std::vector<std::string> getStructureTypes(json_t* structTypeJson);
		
		//Defaults
		ReplacementCost defaultReplacementCost;
		double defaultReplacementTime;
		double defaultStoryArea;
		std::string defaultOccupancy;
		
		//Building property maps
		std::map<int, std::string> occupancyMap;
		std::map<int, ReplacementCost> replacementCostMap;
		std::map<int, std::string> structureTypeMap;
		
		double replacementTime;
		double storyHeightMean;
		double storyHeightStdDev;

		json_t* structureTypeMappings = NULL;
		const static std::string DefaultBIMConfig;
	};	
}

#endif
