#include <iostream>
#include <fstream>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/element.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/options/find.hpp>

#include "BIM.h"
#include "BIMConfig.h"

using namespace bsoncxx::builder::stream;
void parseGeneralInformation(BIM::BuildingInfo& building, bsoncxx::document::view& doc);
void writeBIMJson(BIM::BuildingInfo & bldgInfo, std::string filename);
void writeBIMJson(BIM::BuildingInfo & bldgInfo, bsoncxx::document::view& doc, std::string filename);

int main(int argc, const char **argv)
{
	//
	// parse inputs
	//
	int minRow = -1;
	int maxRow = -1;
	bool getRV = false;

	const char* connectionString = NULL;
	const char* configFile = NULL;
	const char* outputFile = argv[1];

	int arg = 2;
	while (arg < argc)
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
		else if (strcmp(argv[arg], "-connectionString") == 0)
		{
			arg++;
			connectionString = argv[arg];
			std::cerr << "MongoDB connection string: " << connectionString << std::endl;
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

	// check for need to swap
	if (minRow > maxRow)
	{
		int tmp = minRow;
		minRow = maxRow;
		maxRow = tmp;
	}

	if (minRow == -1 || maxRow == -1 || connectionString == 0)
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

	BIM::BIMConfig* bimConfig = NULL;

	try
	{
		bimConfig = new BIM::BIMConfig(configFile);
		if (NULL == bimConfig)
		{
			std::cerr << "Failed to read BIM config!";
			exit(-1);
		}
	}
	catch (std::exception& e)
	{
		bimConfig = BIM::BIMConfig::defaultConfig();
	}


	//Creating the mongo driver instance
	mongocxx::instance instance{};
	
	//Connecting to local mongo server
	mongocxx::uri uri(connectionString);

	//Creating the client
	mongocxx::client mongoClient(uri);

	//Connecting to SimCenter database
	mongocxx::database db = mongoClient["SimCenter"];

	//Reading the Buildings collection
	mongocxx::collection buildings = db["Buildings"];

	//Building the min-max query
	std::string min = std::to_string(minRow);
	std::string max = std::to_string(maxRow);

	//Setting collation to use numeric ordering
	mongocxx::options::find findOptions;
	auto collation = document{} << "locale" << "en_US" << "numericOrdering" << true << finalize;	
	findOptions.collation(collation.view());
	
	//Getting a cursor to the query results
	mongocxx::cursor cursor = buildings.find(
		document{} << "_id" << open_document <<
		"$gte" << min.c_str() << "$lte" << max.c_str() << close_document << finalize , findOptions);
	
	//Looping over results
	for (auto doc : cursor) {
		//std::cout << bsoncxx::to_json(doc) << "\n\n\n";

		//Reading info from CSV
		BIM::BuildingInfo bldgInfo;
		//parseRow(bldgInfo, row);
		parseGeneralInformation(bldgInfo, doc);

		std::string filename;
		filename = bldgInfo.Id + std::string("-BIM.json");

		if (!doc["StructuralInformation"] && doc["GeneralInformation"])
		{
			//This is a building with only General Information

			//Validating area
			bimConfig->validateArea(bldgInfo);

			//Mapping Occupancy
			bimConfig->mapOccupancy(bldgInfo);

			//Mapping Replacement Cost
			bimConfig->mapReplacementCost(bldgInfo);

			//Mapping Structure Type
			bimConfig->mapBuildingStructureType(bldgInfo);

			//Mapping story height
			bimConfig->mapStoryHeight(bldgInfo);

			//Map replacement time
			bimConfig->mapReplacementTime(bldgInfo);
			
			//Writing the BIM file
			writeBIMJson(bldgInfo, filename);
		}
		else if(doc["StructuralInformation"])
		{
			//Mapping Occupancy
			bimConfig->mapOccupancy(bldgInfo);

			//Mapping Replacement Cost
			bimConfig->mapReplacementCost(bldgInfo);

			//Map replacement time
			bimConfig->mapReplacementTime(bldgInfo);

			writeBIMJson(bldgInfo, doc, filename);
		}
		else
		{
			std::cerr << "Building Information Model is not valid!";
		}
		
		//Writing the BIM file
		//writeBIMJson(bldgInfo, filename);

		json_t *bldg = json_object();
		json_object_set(bldg, "id", json_string(bldgInfo.Id.c_str()));
		json_object_set(bldg, "file", json_string(filename.c_str()));
		json_array_append(buildingsArray, bldg);
	}

	
    return 0;
}

void parseGeneralInformation(BIM::BuildingInfo & building, bsoncxx::document::view& doc)
{
	auto GI = doc["GeneralInformation"];
	building.Id = doc["GeneralInformation"]["name"].get_value().get_utf8();
	building.Stories = GI["numStory"].get_value().get_int64();
	building.Area = GI["area"].get_value().get_double() / 10.764; //Converting area from sqft to meters squared
	building.YearBuilt = GI["yearBuilt"].get_value().get_int64();
	building.OccupancyId = GI["occupancyId"].get_value().get_int64();
	building.Location.Latitude = GI["location"]["latitude"].get_value().get_double();
	building.Location.Longitude = GI["location"]["longitude"].get_value().get_double();
}


void writeBIMJson(BIM::BuildingInfo & bldgInfo, std::string filename)
{
	json_t *root = json_object();

	json_t *GI = json_object();
	json_t *distribution = json_array();
	json_t *structtype = json_object();
	json_t *height = json_object();
	json_t *valuenames = json_array();

	//Setting info in Json
	json_object_set(GI, "area", json_real(bldgInfo.Area / bldgInfo.Stories));
	json_object_set(GI, "name", json_string(bldgInfo.Id.c_str()));
	json_object_set(GI, "numStory", json_integer(bldgInfo.Stories));
	json_object_set(GI, "yearBuilt", json_integer(bldgInfo.YearBuilt));

	int numMappedTypes = bldgInfo.MappedStructTypes.size();
	if (numMappedTypes == 1)
		json_object_set(GI, "structType", json_string(bldgInfo.MappedStructTypes[0].c_str()));
	else
	{
		json_object_set(GI, "structType", json_string("RV.structType"));

		for (int i = 0; i < bldgInfo.MappedStructTypes.size(); i++)
			json_array_append(valuenames, json_string(bldgInfo.MappedStructTypes[i].c_str()));

		json_object_set(structtype, "distribution", json_string("discrete_design_set_string"));
		json_object_set(structtype, "name", json_string("structType"));
		json_object_set(structtype, "value", json_string("RV.structType"));
		json_object_set(structtype, "elements", valuenames);

		json_array_append(distribution, structtype);
	}

	json_object_set(GI, "occupancy", json_string(bldgInfo.Occupancy.c_str()));
	json_object_set(GI, "height", json_string("RV.height"));

	json_object_set(GI, "replacementCost", json_real(bldgInfo.ReplacementCost * bldgInfo.Area * 10.764));


	json_object_set(GI, "replacementTime", json_real(bldgInfo.ReplacementTime));

	json_t *location = json_object();
	json_object_set(location, "latitude", json_real(bldgInfo.Location.Latitude));
	json_object_set(location, "longitude", json_real(bldgInfo.Location.Longitude));
	json_object_set(GI, "location", location);

	json_object_set(height, "distribution", json_string("normal"));
	json_object_set(height, "name", json_string("height"));
	json_object_set(height, "value", json_string("RV.height"));

	json_object_set(height, "mean", json_real(bldgInfo.Stories * bldgInfo.storyHeightMean));
	json_object_set(height, "stdDev", json_real(bldgInfo.storyHeightStdDev * bldgInfo.Stories));

	json_array_append(distribution, height);

	json_object_set(root, "RandomVariables", distribution);
	json_object_set(root, "GI", GI);

	json_dump_file(root, filename.c_str(), 0);
}

void writeBIMJson(BIM::BuildingInfo & bldgInfo, bsoncxx::document::view & doc, std::string filename)
{
	auto jsonDoc = bsoncxx::to_json(doc["StructuralInformation"].get_value().get_document());
	json_t *root = json_object();

	json_t *GI = json_object();
	json_t *structtype = json_object();
	json_t *valuenames = json_array();

	//Setting info in Json
	json_object_set(GI, "area", json_real(bldgInfo.Area / bldgInfo.Stories));
	json_object_set(GI, "name", json_string(bldgInfo.Id.c_str()));
	json_object_set(GI, "numStory", json_integer(bldgInfo.Stories));
	json_object_set(GI, "yearBuilt", json_integer(bldgInfo.YearBuilt));

	//
	json_object_set(GI, "structType", json_string("C2"));
	

	json_object_set(GI, "occupancy", json_string(bldgInfo.Occupancy.c_str()));
	json_object_set(GI, "height", json_real(doc["GeneralInformation"]["height"].get_value().get_double()));

	json_object_set(GI, "replacementCost", json_real(bldgInfo.ReplacementCost * bldgInfo.Area * 10.764));


	json_object_set(GI, "replacementTime", json_real(bldgInfo.ReplacementTime));

	json_t *location = json_object();
	json_object_set(location, "latitude", json_real(bldgInfo.Location.Latitude));
	json_object_set(location, "longitude", json_real(bldgInfo.Location.Longitude));
	json_object_set(GI, "location", location);

	json_object_set(root, "GI", GI);

	size_t flags;
	json_error_t error;
	json_object_set(root, "StructuralInformation", json_loads(jsonDoc.c_str(), flags, &error));


	json_dump_file(root, filename.c_str(), 0);

}
