#ifndef BIM_H
#define BIM_H

#include <string>
#include <vector>
#include <map>
#include <jansson.h>

namespace BIM
{
	struct ReplacementCost
	{
		double UnitCost = 0.0;
		double ContentsFactor = 0.0;
	};

	struct GeoLocation
	{
		double Latitude = 0.0;
		double Longitude = 0.0;
	};

	struct BuildingInfo
	{
		std::string Id = "";
		double Area = 0.0;
		int Stories = 0;
		int YearBuilt = 0;
		int TypeId = -1;
		std::vector<int> MappedStructTypes;
		double ReplacementCost;
		std::string Occupancy = "";
		GeoLocation Location;
	};

	void readBIMConfig(json_t * pJsonConfig, const char * bimConfig);
	double parseDefaultArea(const json_t * pJsonConfig);
	const char * parseDefaultOccupancy(const json_t * pJsonConfig);

	ReplacementCost parseDefaultReplacementCost(const json_t * pJsonConfig);
	std::map<int, std::string> parseOccupancyMap(const json_t * pJsonConfig);
	std::map<int, ReplacementCost> parseReplacementCostMap(const json_t * pJsonConfig);
	std::map<int, const char*> parseStructureTypeMap(const json_t* pJsonConfig);

	static const char* DefaultBIMConfig = R"(
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

}
#endif