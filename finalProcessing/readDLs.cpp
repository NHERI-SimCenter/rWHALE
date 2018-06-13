
// createEDP.cpp
// purpose - given a building, return an EVENT for the Haywired data.
// written: fmckenna

#include <iostream>
#include <fstream>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <jansson.h>     // for writing json
#include <nanoflann.hpp> // for searching for nearest point

int main(int argc, char **argv) {

  int minRow = atoi(argv[1]);
  int maxRow = atoi(argv[2]);

  ofstream output;
  output.open (argv[3]);

  if (!output.is_open())
    return -1;
    
  for (int i=minRow; i<=maxRow; i++) {
    string inputFilename = to_string(i) + "-DL.json";
    // now parse the DL file for the building
    //
    
    json_error_t error;
    json_t *root = json_load_file(inputFilename.c_str(), 0, &error);
    
    if(!root) {
      output << "0,\"UNKNOWN\",0,0,0,0,0\n";
    } else {

      double loss = 0.;
      double medianRepairCost = 0.;
      double stdRepairCost = 0.;
      double medianDowntime = 0.;
      double prob = 0.;
      const char *placard = "none";
      int placardValue = 0;
      double pga = 0.0;

      json_t *lossO = json_object_get(root,"EconomicLoss"); 
      if (lossO != 0) {
	loss = json_real_value(json_object_get(lossO,"MedianLossRatio"));
	medianRepairCost = json_real_value(json_object_get(lossO,"MedianRepairCost"));
	stdRepairCost = json_real_value(json_object_get(lossO,"StdRepairCost"));
      }

      json_t *downtimeO = json_object_get(root,"Downtime");
      if (downtimeO != 0)
	medianDowntime = json_real_value(json_object_get(downtimeO,"MedianDowntime"));

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
	prob = json_real_value(json_object_get(placardsO,"RedTagProbability"));
      }
      
      
      output << name << ",\"UNKNOWN\"," << medianRepairCost << "," << stdRepairCost
	     << "," << medianDowntime << "," << placardValue << "," << pga << "\n";
      
      json_object_clear(root);  
    }
  }

  output.close();

  return 0;
}
