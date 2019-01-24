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

		void set(double latitude, double longitude);
	};

	struct BuildingInfo
	{
		std::string Id = "";
		double Area = 0.0;
		int Stories = 0;
		int YearBuilt = 0;
		int OccupancyId = -1;
		std::vector<std::string> MappedStructTypes;
		double ReplacementCost = 0.0;
		std::string Occupancy = "";
		GeoLocation Location;
		double ReplacementTime = 0.0;
		double storyHeightMean;
		double storyHeightStdDev;

		void setLocation(double latitude, double longitude);
	};
}
#endif