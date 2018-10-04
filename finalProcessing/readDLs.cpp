
// readDLs.cpp
// purpose - aggregates the damage and loss outputs into a single CSV file
// written: fmckenna

#include <iostream>
#include <fstream>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>

#include <jansson.h>     // for writing json

int main(int argc, char **argv) {

  int minRow = atoi(argv[1]);
  int maxRow = atoi(argv[2]);

  ofstream output;
  output.open (argv[3]);

  if (!output.is_open())
    return -1;
  
  output.precision(10);

  output << "Id,MedianRepairCost,RepairCostStdDev,MedianDowntime,RedTagged,PGA,LossRatio,Latitude,Longitude\n";

  for (int i=minRow; i<=maxRow; i++) {
    //First, we will read the location from the BIM file
    string bimFilename = std::to_string(i) + "-BIM.json";
    json_error_t error;
    json_t *bimJson = json_load_file(bimFilename.c_str(), 0, &error);
    double latitude = -1.0;
    double longitude = -1.0;
    if(bimJson)
    {
      json_t* giJson = json_object_get(bimJson, "GI");
      if(giJson)
      {
        json_t* locationJson = json_object_get(giJson, "location");

        json_t* latitudeJson = json_object_get(locationJson, "latitude");
        if (latitudeJson)
          latitude = json_real_value(latitudeJson);

        json_t* longitudeJson = json_object_get(locationJson, "longitude");
        if (longitudeJson)
          longitude = json_real_value(longitudeJson);
      }
    }
    // now parse the DL file for the building
    //
    string lossFilename = to_string(i) + "-DL.json";

    json_t *root = json_load_file(lossFilename.c_str(), 0, &error);
    
    if(!root) {
      output << ",,,,,\n";
    } else {

      double loss = 0.;
      double medianRepairCost = 0.;
      double stdRepairCost = 0.;
      double medianDowntime = 0.;
      const char *placard = "none";
      int placardValue = 0;
      double pga = 0.0;

      json_t *lossO = json_object_get(root,"EconomicLoss"); 
      if (lossO != 0) {
	loss = json_real_value(json_object_get(lossO,"MedianLossRatio"));
	medianRepairCost = json_real_value(json_object_get(lossO,"MedianRepairCost"));
	stdRepairCost = json_real_value(json_object_get(lossO,"StdRepairCost"));
      }

      json_t *downtimeO = json_object_get(root,"RepairTime");
      if (downtimeO != 0)
	medianDowntime = json_real_value(json_object_get(downtimeO,"MedianRepairTime"));

      const char *name;

      json_t *nameO = json_object_get(root,"Name");
      if (nameO != 0)
	name = json_string_value(nameO);

      json_t *pgaO = json_object_get(root,"MaxPGA");
      if (pgaO != 0)
	pga = json_real_value(pgaO)/9.81;

      json_t *placardsO = json_object_get(root,"UnsafePlacards");
      if (placardsO != 0) {
	placard = json_string_value(json_object_get(placardsO,"Tag"));
	if (strcmp(placard,"red") == 0) {
	  placardValue = 1;
	}
      }
      
      
      output << name << "," << medianRepairCost << "," << stdRepairCost
	     << "," << medianDowntime << "," << placardValue << "," << pga
       << "," << loss << "," << latitude << "," << longitude <<"\n";
      
      json_object_clear(root);  
    }
  }

  output.close();

  return 0;
}
