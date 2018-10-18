#include "BIM.h"
using namespace BIM;

void BuildingInfo::setLocation(double latitude, double longitude)
{
	Location.set(latitude, longitude);
}

void GeoLocation::set(double latitude, double longitude)
{
	Latitude = latitude;
	Longitude = longitude;
}
