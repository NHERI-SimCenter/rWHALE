#include <iostream>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/options/find.hpp>

using namespace bsoncxx::builder::stream;

int main(int argc, const char **argv)
{
	//Creating the mongo driver instance
	mongocxx::instance instance{};
	
	//Connecting to local mongo server
	mongocxx::uri uri("mongodb://localhost:27017");

	//Creating the client
	mongocxx::client mongoClient(uri);

	//Connecting to SimCenter database
	mongocxx::database db = mongoClient["SimCenter"];

	//Reading the Buildings collection
	mongocxx::collection buildings = db["Buildings"];

	//Building the query
	char* min = "1";
	char* max = "10";

	//Setting collation to use numeric ordering
	mongocxx::options::find findOptions;
	auto collation = document{} << "locale" << "en_US" << "numericOrdering" << true << finalize;	
	findOptions.collation(collation.view());
	
	//Getting a cursor to the query results
	mongocxx::cursor cursor = buildings.find(
		document{} << "_id" << open_document <<
		"$gte" << min << "$lte" << max << close_document << finalize , findOptions);
	
	//Looping over results
	for (auto doc : cursor) {
		std::cout << bsoncxx::to_json(doc) << "\n\n\n";
	}
	
	int a;
	std::cin >> a;

    return 0;
}