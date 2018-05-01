// createBIM.c
// purpose - give the building.csv and parcel.csv files create a BIM model
// written: fmckenna

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>

#include "csvparser.h"
#include <iostream>
#include <jansson.h>
#include <map>
#include <sstream>
#include <fstream>
#include <limits>
#include "UrbanSimDatabase.h"

using namespace std;


// function prototypes
const char* deteroccupancy(int building_type);
double replacementcost(int building_type);
json_t *deterStructtype(int year, int bldtypeid, int story);

int main(int argc, const char **argv) {

  //
  // parse inputs
  //
  int minRow = -1;
  int maxRow = -1;
  bool getRV = false;

  const char*outputFilename = argv[1];
  const char*parcelsFilename = 0;
  const char*buildingsFilename = 0;
  const char*buildingsConfigFile = 0;
  const char*filenameBIM = 0;

  int arg = 2;
  while(arg < argc) {

    if (strcmp(argv[arg], "-Min") ==0) {
      arg++;
      minRow = atoi(argv[arg]);
    }
    else if (strcmp(argv[arg], "-Max") ==0) {
      arg++;
      maxRow = atoi(argv[arg]);
    }
    else if (strcmp(argv[arg], "-parcelsFile") ==0) {
      arg++;
      parcelsFilename = argv[arg];
      std::cerr << "PARCELS: " << parcelsFilename;    
    }
    else if (strcmp(argv[arg], "-buildingsFile") ==0) {
      arg++;
      buildingsFilename = argv[arg];
      std::cerr << "BUILDING: " << buildingsFilename;    
    }
    else if (strcmp(argv[arg], "-filenameBIM") ==0) {
      arg++;
      filenameBIM = argv[arg];
      std::cerr << "BUILDING: " << buildingsFilename;    
    }
    else if (strcmp(argv[arg], "-getRV") ==0) {
      getRV = true;
    }
    arg++;
  }

  if (getRV == false) {
    // here we would process the BIM file given the RV value .. this app does nothing at present
    return 0;
  }

  // check for need to swap as someone will be an idiot
  if (minRow > maxRow) {
    int tmp = minRow;
    minRow = maxRow;
    maxRow = tmp;
  }

  if (minRow == -1 || maxRow == -1 || parcelsFilename == 0 || buildingsFilename == 0) {
    std::cerr << "INVALID INPUT\n";    
    exit(-1);
  }
  std::cerr << "UrbanSimDatabase: min: " << minRow << " max: " << maxRow << "\n";

  //
  // json array containing output
  // 
  json_t *rootBuildings = json_object();
  json_t *buildingsArray=json_array();
    
  //We need to make sure we are able to read the createBim json config
  FILE * pCfgFile = NULL;
  if (buildingsConfigFile != 0)
    pCfgFile = fopen (buildingsConfigFile , "r");

  json_error_t error;
  size_t flags = 0;

  json_t* pJsonConfig;
  if(NULL != pCfgFile)
    pJsonConfig = json_loadf(pCfgFile, flags, &error);
  else
    pJsonConfig = json_loads(defaultConfig, flags, &error);
  
  //We will try to read the struct type map from the json
  json_t* pStructTypes = json_object_get(pJsonConfig, "StructTypes");
  
  std::map<int, const char*> structTypesMap;
  const char *structTypeKey;
  json_t *structTypeValue;
  json_object_foreach(pStructTypes, structTypeKey, structTypeValue) {
      structTypesMap.insert(std::pair<int, const char*>(json_integer_value(structTypeValue), structTypeKey));
    }

  std::map<int, const char*> occupancyMap;
  std::map<int, ReplacementCost> replacementCostsMap;

  json_t* pBuildingOccupancies = json_object_get(pJsonConfig, "BuildingOccupancyTypes");
  size_t index;
  json_t* pBuildingOccupancy;
  json_array_foreach(pBuildingOccupancies, index, pBuildingOccupancy) {
      json_t* typeId = json_object_get(pBuildingOccupancy, "TypeId");
      json_t* occupancy = json_object_get(pBuildingOccupancy, "Occupancy");
      occupancyMap.insert(std::pair<int, const char*>(json_integer_value(typeId), json_string_value(occupancy)));
      
      json_t* pReplacementCost = json_object_get(pBuildingOccupancy, "ReplacementCost");
      json_t* unitCost = json_object_get(pReplacementCost, "UnitCost");
      json_t* contentsFactor = json_object_get(pReplacementCost, "ContentsFactor");

      ReplacementCost replacementCost;
      replacementCost.UnitCost = json_real_value(unitCost);
      replacementCost.ContentsFactor = json_real_value(contentsFactor);

      replacementCostsMap.insert(std::pair<int, ReplacementCost>(json_integer_value(typeId), replacementCost));
    }
   
  json_t* pDefaultOccupancy = json_object_get(pJsonConfig, "DefaultOccupancy");
  const char* defaultOccupancy = json_string_value(pDefaultOccupancy);

  json_t* pDefaultReplacementCost = json_object_get(pJsonConfig, "DefaultReplacementCost");
  json_t* unitCost = json_object_get(pDefaultReplacementCost, "UnitCost");
  json_t* contentsFactor = json_object_get(pDefaultReplacementCost, "ContentsFactor");

  ReplacementCost defaultReplacementCost;
  defaultReplacementCost.UnitCost = json_real_value(unitCost);
  defaultReplacementCost.ContentsFactor = json_real_value(contentsFactor);

  int i = 0;

  std::map<int, locations> parcelLocations;
  std::map<int, locations>::iterator parcelIter;
  //ofstream opt("ID.txt");
  //system("md fileBIM");
  //
  // first parse the parcel file, storing parcel location in a map
  //
  string valuename[9]={"W1","S1","S2","C1","C2","C3","RM1","RM2","URM"};
  CsvParser *csvparser = CsvParser_new(parcelsFilename, ",", 1);
  const CsvRow *header = CsvParser_getHeader(csvparser);
  
  if (header == NULL) {
    printf("%s\n", CsvParser_getErrorMessage(csvparser));
    return 1;
  }
  
  const char **headerFields = CsvParser_getFields(header);
  for (i = 0 ; i < CsvParser_getNumFields(header) ; i++) {
    //    printf("TITLE: %d %s\n", i, headerFields[i]);
  }    
  CsvRow *row;
  
  while ((row = CsvParser_getRow(csvparser))) {
    const char **rowFields = CsvParser_getFields(row);
    // for (i = 0 ; i < CsvParser_getNumFields(row) ; i++) {
    //   printf("FIELD: %s\n", rowFields[i]);
    // }
    char *pEnd;
    int parcelID = atoi(rowFields[0]);
    double x = strtod(rowFields[12],&pEnd);
    double y = strtod(rowFields[13],&pEnd);
    parcelLocations[parcelID]=locations(x,y);
  }
  
  CsvParser_destroy(csvparser);
  
  //We need to determine the average area of a building to use it when no area is provided
  double averageArea = getAverageArea();

  //
  // now parse the building file, obtaining location form parcel info
  // writing and write a BIM file
  //
  
  csvparser = CsvParser_new(buildingsFilename, ",", 1);
  header = CsvParser_getHeader(csvparser);
  
  if (header == NULL) {
    printf("%s\n", CsvParser_getErrorMessage(csvparser));
    return 1;
  }
  
  headerFields = CsvParser_getFields(header);
  for (i = 0 ; i < CsvParser_getNumFields(header) ; i++) {
    //      printf("TITLE: %d %s\n", i, headerFields[i]);
  }
  
  int currentRow = 1;
  
  json_t *root = json_object();
  
  while ((row = CsvParser_getRow(csvparser))) {
    if (currentRow >= minRow && currentRow <= maxRow) {
      const char **rowFields = CsvParser_getFields(row);
      char *pEnd;

      json_t *GI = json_object();
      json_t *distribution=json_array();
      json_t *structtype = json_object();
      json_t *height = json_object();
      json_t *values=json_array();
      json_t *valuenames=json_array();

      const char *name = rowFields[0];
      int numStory = atoi(rowFields[10]);
      double area=strtod(rowFields[8],&pEnd)/10.764/(double)numStory;
      if (area <= 0)
        area = averageArea;

      json_object_set(GI,"area",json_real(area));
      //json_object_set(GI,"structType",json_string("UNKNOWN"));
      //json_object_set(GI,"structType",json_string(deterStructtype(atoi(rowFields[11]),atoi(rowFields[2]),atoi(rowFields[10]))));

      json_object_set(GI,"name",json_string(name));
      //json_object_set(GI,"area",json_real(strtod(rowFields[8],&pEnd)));
      json_object_set(GI,"numStory",json_integer(numStory));
      json_object_set(GI,"yearBuilt",json_integer(atoi(rowFields[11])));


      //values=deterStructtype(atoi(rowFields[11]),atoi(rowFields[2]),atoi(rowFields[10]));
      int yearBuilt = atoi(rowFields[11]);
      int buildingTypeId = atoi(rowFields[15]);
      json_t* structTypeIdMappings = json_object_get(pJsonConfig, "StructTypeIdMappings");

      values = getStructType(yearBuilt, buildingTypeId, numStory, structTypeIdMappings);

      int numValues = json_array_size(values);
      if (numValues == 1) {
	    json_t *index = json_array_get(values, 0);
	    //json_object_set(GI,"structType",json_string(valuename[json_integer_value(index)-1].c_str())); //Old code
	    json_object_set(GI,"structType",json_string(structTypesMap[json_integer_value(index)]));

      } else {

	json_object_set(GI,"structType",json_string("RV.structType"));

	//size_t index;
	//json_t *value;
	//json_array_foreach(values, index, value) {
	//    json_array_append(valuenames, json_string(valuename[index].c_str()));
	//}
	for(i = 0; i < json_array_size(values); i++) {
	    json_t *index = json_array_get(values, i);
	    json_array_append(valuenames, json_string(valuename[json_integer_value(index)-1].c_str()));
	}

	json_object_set(structtype,"distribution",json_string("discrete_design_set_string"));
	json_object_set(structtype,"name",json_string("structType"));
	json_object_set(structtype,"value",json_string("RV.structType"));
	//	json_object_set(structtype,"values",values);
	json_object_set(structtype,"elements",valuenames);

	json_array_append(distribution, structtype);	
      }

      // unknown
      //json_object_set(GI,"occupancy",json_string("office"));
      //deteroccupancy(atoi(rowFields[15]), buildoccupancy, replacementcost);
      //json_object_set(GI,"occupancy",json_string(deteroccupancy(atoi(rowFields[15]))));//Old code
      json_object_set(GI,"occupancy",json_string(getOccupancy(atoi(rowFields[15]), occupancyMap, defaultOccupancy)));

      json_object_set(GI,"height",json_string("RV.height"));
      //json_object_set(GI,"replacementCost",json_real(replacementcost(atoi(rowFields[15]))*strtod(rowFields[8],&pEnd)));
      double replacementCost = getReplacementcost(atoi(rowFields[15]), replacementCostsMap, defaultReplacementCost);
      json_object_set(GI,"replacementCost",json_real(replacementCost * strtod(rowFields[8],&pEnd)));


      double replacementTime = 180.0;      
      json_t* pReplacementTime = json_object_get(pJsonConfig, "ReplacementTime");
      if(NULL != pReplacementTime)
        replacementTime = json_real_value(pReplacementTime);

      json_object_set(GI,"replacementTime",json_real(replacementTime));
      //json_object_set(GI,"structType",json_string("C2"));
      
      int parcelID = atoi(rowFields[1]);
      parcelIter = parcelLocations.find(parcelID);
      if (parcelIter != parcelLocations.end()) {
	//	double x = parcelIter->second.x;
	json_t *location = json_object();
	json_object_set(location,"latitude",json_real(parcelIter->second.y));
	json_object_set(location,"longitude",json_real(parcelIter->second.x));
	json_object_set(GI,"location",location);
      }

      json_object_set(height,"distribution",json_string("normal"));
      json_object_set(height,"name",json_string("height"));
      json_object_set(height,"value",json_string("RV.height"));
      double storyHeightMean = 3.0;
      double storyHeightStdDev = 0.16667;
      json_t* pStoryHeight = json_object_get(pJsonConfig, "StoryHeight");
      if(NULL != pStoryHeight)
      {
          json_t* mean = json_object_get(pStoryHeight, "Mean");
          storyHeightMean = json_real_value(mean);
          json_t* stdDev = json_object_get(pStoryHeight, "StdDev");
          storyHeightStdDev = json_real_value(stdDev);
      }

      json_object_set(height,"mean",json_real(numStory * storyHeightMean));
      json_object_set(height,"stdDev",json_real(storyHeightStdDev * double(numStory)));

      json_array_append(distribution, height);

      json_object_set(root,"RandomVariables",distribution);
      json_object_set(root,"GI",GI);
      std::string filename;
      filename = "exampleBIM.json";

      if (argc > 1) {
	filename = std::string(name) + std::string("-BIM.json");
      }

      json_t *bldg = json_object();
      json_object_set(bldg,"id",json_string(name));
      json_object_set(bldg,"file",json_string(filename.c_str()));
      json_array_append(buildingsArray, bldg);
	
      // write the file & clean memory
      json_dump_file(root,filename.c_str(),0);
      json_object_clear(root);
      CsvParser_destroy_row(row);
    }


    currentRow++;
    
    if (currentRow > maxRow)
      break;
  }

  json_dump_file(buildingsArray, outputFilename,0);

  CsvParser_destroy(csvparser);
  
  return 0;
}


//This method determines the structure type
//It takes as input the year, building type id, number of stores and the mapping of structure types
//Structure types maps are specified in the json config file
//The output is a json object with either a particular structure type or an array of structure types
//if the structure type is random (uniformly distributed discrete random variable)
json_t* getStructType(int year, int bldtypeid, int numStory, json_t* pStructTypeIdMappings)
{    
    json_t* value = json_array();

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

                        if(numStory >= minStory && numStory <= maxStory)
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
                                    if(bldtypeid == json_integer_value(buildingTypeId))
                                    {
                                        //This means we found the proper map, so we can return the result
                                        json_t* structureTypeId = json_object_get(buildingIdsMap, "StructureTypeId");
                                        populateTypeIds(structureTypeId, value);
                                        return value;
                                    }
                                }
                            }

                            //Handle default mapping for this store range
                            json_t* defaultStoryRangeMapping = json_object_get(storyRangeMap, "DefaultMapping");
                            if(NULL != defaultStoryRangeMapping)
                            {
                                json_t* structureTypeId = json_object_get(defaultStoryRangeMapping, "StructureTypeId");
                                populateTypeIds(structureTypeId, value);
                                return value;
                            }
                        }
                    }                    
                }

                //Handle default mapping for this year range
                json_t* defaultYearRangeMapping = json_object_get(structTypeMap, "DefaultMapping");
                if(NULL != defaultYearRangeMapping)
                {
                    json_t* structureTypeId = json_object_get(defaultYearRangeMapping, "StructureTypeId");
                    populateTypeIds(structureTypeId, value);
                    return value;
                }
            }
        }
    }
    
    return value;
}


//This method takes a structure type id json object and 
//populates a json array with the mapped structure type values
void populateTypeIds(json_t* structureTypeId, json_t* valuesArray)
{
    int type = json_typeof(structureTypeId);
    if(JSON_INTEGER == type)
        json_array_append(valuesArray, json_integer(json_integer_value(structureTypeId)));
    else if(JSON_OBJECT == type)
    {
        json_t* typeValues = json_object_get(structureTypeId, "Values");
        size_t index;
        json_t* typeId;
        json_array_foreach(typeValues, index, typeId) {
            json_array_append(valuesArray, json_integer(json_integer_value(typeId)));
        }
    }
}


//This method determines the building occupancy based on the occupancy mapping defined in the json config file
//if no mapping is found, it returns the default mapping
const char* getOccupancy(int building_type, std::map<int, const char*> occupancyMap, const char* defaultOccupancy)
{    
    const char * buildoccupancy;

    if(occupancyMap.end() != occupancyMap.find(building_type))
        buildoccupancy = occupancyMap[building_type];
    else
        buildoccupancy = defaultOccupancy;    

    return buildoccupancy;
}

//This method will loop through all the buildings and calculate the average area
//This may not be efficient, it take few seconds, but it is only calculated once
double getAverageArea()
{
    CsvParser *csvparser = CsvParser_new("buildings.csv", ",", 1);  

    CsvRow *row;
    int count =0;
    double areaSum = 0.0;
    while ((row = CsvParser_getRow(csvparser))) {
        const char **rowFields = CsvParser_getFields(row);
        int numStory = atoi(rowFields[10]);
        double area = strtod(rowFields[8], NULL)/10.764/(double)numStory;
        if(area >= std::numeric_limits<double>::min())
        {
            areaSum += area;
            count++;
        }
    }
    CsvParser_destroy(csvparser);

    if (count > 0)
        return areaSum/(double)count;
    else
        return 0.0;
}

//This method calculates the replacement cost for the specified buiding type or returns the default one
double getReplacementcost(int building_type, std::map<int, ReplacementCost> replacementCostsMap, ReplacementCost defaultReplacementCost)
{
    double cost = 0.0;
    ReplacementCost replacmentCost = defaultReplacementCost;
    if(replacementCostsMap.end() != replacementCostsMap.find(building_type))
        replacmentCost = replacementCostsMap[building_type];
    
    cost = replacmentCost.UnitCost * (1 + replacmentCost.ContentsFactor);

    return cost;
}

const char* deteroccupancy(int building_type){
    const char * buildoccupancy;
    switch (building_type) {
    case 1:
        buildoccupancy="Residential";
        break;
    case 2:
        buildoccupancy="Residential";
        break;
    case 3:
        buildoccupancy="Residential";
        break;
    case 4:
        buildoccupancy="Office";
        break;
    case 5:
        buildoccupancy="Hotel";
        break;
    case 6:
        buildoccupancy="School";
        break;
    case 7:
        buildoccupancy="Industrial";
        break;
    case 8:
        buildoccupancy="Industrial";
        break;
    case 9:
        buildoccupancy="Industrial";
        break;
    case 10:
        buildoccupancy="Retail";
        break;
    case 11:
        buildoccupancy="Retail";
        break;
    case 12:
        buildoccupancy="Residential";
        break;
    case 13:
        buildoccupancy="Retail";
        break;
    case 14:
        buildoccupancy="Office";
        break;
    default:
        buildoccupancy="Residential";
        break;
    }
    return buildoccupancy;
}

double replacementcost(int building_type){
    double replacementcost;
    switch (building_type) {
    case 1:
        replacementcost=137.5452*(1+0.5);
        break;
    case 2:
        replacementcost=137.5452*(1+0.5);
        break;
    case 3:
        replacementcost=137.5452*(1+0.5);
        break;
    case 4:
        replacementcost=131.8863*(1+1);
        break;
    case 5:
        replacementcost=137.271225*(1+0.5);
        break;
    case 6:
        replacementcost=142.134265*(1+1.25);
        break;
    case 7:
        replacementcost=97.5247*(1+1.5);
        break;
    case 8:
        replacementcost=85.9586*(1+1.5);
        break;
    case 9:
        replacementcost=104.033475*(1+1.5);
        break;
    case 10:
        replacementcost=105.33705*(1+1);
        break;
    case 11:
        replacementcost=105.33705*(1+1);
        break;
    case 12:
        replacementcost=137.5452*(1+0.5);
        break;
    case 13:
        replacementcost=105.33705*(1+1);
        break;
    case 14:
        replacementcost=131.8863*(1+1);
        break;
    default:
        replacementcost=137.5452*(1+0.5);
        break;
    }
    return replacementcost;
}

json_t *deterStructtype(int year, int bldtypeid, int story)
{
    json_t *value=json_array();
    if(year<=1900){
        json_array_append(value, json_integer(1));
        json_array_append(value, json_integer(7));
        json_array_append(value, json_integer(8));
        json_array_append(value, json_integer(9));
    }
    else if(year>1900){
        if(story<4){
            if(bldtypeid==1||bldtypeid==2||bldtypeid==3||bldtypeid==12){
                json_array_append(value, json_integer(1));
            }
            else if(bldtypeid==4||bldtypeid==5||bldtypeid==6||bldtypeid==10||bldtypeid==11||bldtypeid==13||bldtypeid==14){
                json_array_append(value, json_integer(1));
                json_array_append(value, json_integer(2));
                json_array_append(value, json_integer(3));
                json_array_append(value, json_integer(4));
                json_array_append(value, json_integer(5));
                json_array_append(value, json_integer(6));
                json_array_append(value, json_integer(7));
                json_array_append(value, json_integer(8));
            }
            else if(bldtypeid<10&&bldtypeid>6){
                json_array_append(value, json_integer(2));
                json_array_append(value, json_integer(3));
                json_array_append(value, json_integer(4));
                json_array_append(value, json_integer(5));
                json_array_append(value, json_integer(6));
                json_array_append(value, json_integer(7));
                json_array_append(value, json_integer(8));
            }
            else {
                json_array_append(value, json_integer(1));
            }

        }
        else if(story>3&&story<8){
            if(bldtypeid<7&&bldtypeid>0||bldtypeid>9&&bldtypeid<15){
                json_array_append(value, json_integer(1));
                json_array_append(value, json_integer(2));
                json_array_append(value, json_integer(3));
                json_array_append(value, json_integer(4));
                json_array_append(value, json_integer(5));
            }
            else if(bldtypeid<10&&bldtypeid>6){
                json_array_append(value, json_integer(2));
                json_array_append(value, json_integer(3));
                json_array_append(value, json_integer(4));
                json_array_append(value, json_integer(5));
            }
            else {
                json_array_append(value, json_integer(1));
                json_array_append(value, json_integer(2));
                json_array_append(value, json_integer(3));
                json_array_append(value, json_integer(4));
                json_array_append(value, json_integer(5));
            }

        }
        else {
            json_array_append(value, json_integer(2));
            json_array_append(value, json_integer(3));
            json_array_append(value, json_integer(4));
            json_array_append(value, json_integer(5));
        }
    }
    return value;
}

