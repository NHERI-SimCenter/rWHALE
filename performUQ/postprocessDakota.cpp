#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <cctype>
//using namespace std

#include <jansson.h>  // for Json

int main(int argc, const char **argv)
{
  const char *numberRVs = argv[1];
  const char *numberSamples = argv[2];
  const char *filenameEDP = argv[3];
  const char *dakotaOUT = argv[4];

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
  
  int numRV = atoi(numberRVs);
  int numSample = atoi(numberSamples);

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

  return 0;
}

