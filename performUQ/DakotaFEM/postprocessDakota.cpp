#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <cctype>
#include <vector>

#include <jansson.h>  // for Json
#include "Stat.h"

double getReplacementCost(const char* filenameBIM);
void processEDPs(const char* filenameEDP, const char* dakotaOUT, int numRV, int numSample);
void processLosses(const char *bldgDir, const char *filenameLOSS, int numSample, double replacementCost);

int main(int argc, const char **argv)
{
  const char *numberRVs = argv[1];
  const char *numberSamples = argv[2];
  const char *filenameBIM = argv[3];
  const char *filenameEDP = argv[4];
  const char *filenameLOSS = argv[5];
  const char *dakotaOUT = argv[6];
  const char *bldgDir = argv[7];

  int numRV = atoi(numberRVs);
  int numSample = atoi(numberSamples);

  //read replacementCost from the BIM file
  double replacementCost = getReplacementCost(filenameBIM);
  
  //
  processEDPs(filenameEDP, dakotaOUT, numRV, numSample);

  processLosses(bldgDir, filenameLOSS, numSample, replacementCost);

  return 0;
}

void processEDPs(const char* filenameEDP, const char* dakotaOUT, int numRV, int numSample)
{
  json_error_t error;
  json_t *root = json_load_file(filenameEDP, 0, &error);
  json_t *eventsArray = json_object_get(root,"EngineeringDemandParameters");  

  int index1, index2;
  json_t *value1, *value2;

  int numEDP = 0;

  json_array_foreach(eventsArray, index1, value1) {
    json_t *responsesArray = json_object_get(value1,"responses");  
    json_array_foreach(responsesArray, index2, value2) {
      json_t *sType = json_object_get(value2,"scalar_data");
      double value = json_number_value(sType);
      numEDP++;
    }
  }
  printf("NUM_EDP= %d\n",numEDP);

  //
  // read the dakotaTag data
  //

  std::ifstream fIn(dakotaOUT);
  


  double *data = new double[numSample*numEDP];

  // throw out fits line
  std::string gibberish;
  getline(fIn, gibberish);

  for (int i=0; i<numSample; i++) {
    for (int j=0; j<numRV+2; j++) {
      std::string crappyData;
      fIn >> crappyData;
    }
    for (int j=0; j<numEDP; j++) {
      fIn >> data[i*numEDP+j];
    }
  }

  fIn.close();

  // now put data into json
  int edp = 0;
  json_array_foreach(eventsArray, index1, value1) {
    json_t *responsesArray = json_object_get(value1,"responses");  
    json_array_foreach(responsesArray, index2, value2) {
      json_t *sType = json_object();
      json_t *sResults = json_array();
      for (int i=0; i<numSample; i++)
	json_array_append(sResults,json_real(data[i*numEDP+edp]));
      json_object_set(value2,"scalar_data",sResults);
      edp++;
    }
  }

  for (int i=0; i<numSample; i++) {
    for (int j=0; j<numEDP; j++) {
      printf("%f ", data[i*numEDP+j]); 
    }
    printf("\n");
  }

  json_dump_file(root,filenameEDP,0);
}

void processLosses(const char *bldgDir, const char *filenameLOSS, int numSample, double replacementCost)
{
    //TODO
    std::vector<double> vLoss, vDowntime;
    std::vector<int> vRedTags;

    vLoss.reserve(1000 * numSample);
    vDowntime.reserve(1000 * numSample);
    vRedTags.reserve(1000 * numSample);

    json_error_t error;
    size_t index;
    json_t* value = NULL;

    json_t* maxPGA;
    json_t* bldgName;

    for(int i = 0; i < numSample; i++)
    {
        char sampleDLFile[256];
        sprintf(sampleDLFile, "%sworkdir.%u/%s", bldgDir, i + 1, filenameLOSS);
        json_t* root = json_load_file(sampleDLFile, 0, &error);
        json_t* samplesJson = json_object_get(root, "Samples");

        //Reading losses
        json_t* lossJson = json_object_get(samplesJson, "Loss");
        json_array_foreach(lossJson, index, value)
            vLoss.push_back(json_real_value(value));

        //Reading repair time
        json_t* downtimeJson = json_object_get(samplesJson, "RepairTime");
        json_array_foreach(downtimeJson, index, value)
            vDowntime.push_back(json_real_value(value));

        json_t* redTagsJson = json_object_get(samplesJson, "RedTags");
        json_array_foreach(redTagsJson, index, value)
            vRedTags.push_back(json_integer_value(value));

        //We need to read building name, replacement cost and PGA
        bldgName = json_object_get(root, "Name");
        maxPGA = json_object_get(root, "MaxPGA");
    }

    //Now we need to compute stats
    Stat stat;
    double medianRepairCost = stat.getMedian(vLoss);
    double meanRepairCost = stat.getMean(vLoss);
    double repairCostStdDev = stat.getStd(vLoss);
    double lossratio = medianRepairCost/replacementCost;
    double lossPercentile10 = stat.getPercentile(vLoss,10);
    double lossPercentile90 = stat.getPercentile(vLoss,90);
    double medianRepairtime = stat.getMedian(vDowntime);
    
    int numRedTags = 0;
    for(int i = 0; i < vRedTags.size(); i++)
        numRedTags += vRedTags[i];
    double redTagProb = (double)numRedTags/vRedTags.size();

    //Now we can write the output
    json_t* root = json_object();
    json_t* dlJson = json_object();
    json_object_set(dlJson, "MedianLossRatio", json_real(lossratio));
    json_object_set(dlJson, "MedianRepairCost", json_real(medianRepairCost));
    json_object_set(dlJson, "MeanRepairCost", json_real(meanRepairCost));
    json_object_set(dlJson, "StdRepairCost", json_real(repairCostStdDev));
    json_object_set(dlJson, "10PercentileLoss", json_real(lossPercentile10));
    json_object_set(dlJson, "90PercentileLoss", json_real(lossPercentile90));
    json_object_set(root, "EconomicLoss", dlJson);

    json_t* repairTimeJson = json_object();
    json_object_set(repairTimeJson, "MedianRepairTime", json_real(medianRepairtime));
    json_object_set(root, "RepairTime", repairTimeJson);
    
    json_t* tagJson = json_object();
    if(redTagProb > 0.5)
        json_object_set(tagJson,"Tag",json_string("red"));
    else
        json_object_set(tagJson, "Tag",json_string("none"));

    json_object_set(tagJson, "RedTagProbability", json_real(redTagProb));
    json_object_set(root, "UnsafePlacards", tagJson);

    json_object_set(root, "Name", bldgName);
    json_object_set(root, "MaxPGA", maxPGA);

    json_dump_file(root, filenameLOSS, 0);
}

double getReplacementCost(const char* filenameBIM)
{
    double replacementCost = 0.0;

    json_error_t error;
    json_t* bimJson = json_load_file(filenameBIM, 0, &error);
    json_t* giJson = json_object_get(bimJson, "GI");

    json_t* replacementCostJson = json_object_get(giJson, "replacementCost");
    replacementCost = json_real_value(replacementCostJson);

    return replacementCost;
}