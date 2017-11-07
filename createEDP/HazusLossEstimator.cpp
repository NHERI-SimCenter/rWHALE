#include "HazusLossEstimator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
using namespace std;

#include <jansson.h>  // for Json

HazusLossEstimator::HazusLossEstimator()
{

}

HazusLossEstimator::~HazusLossEstimator()
{

}

int 
HazusLossEstimator::createEDP(const char *filenameBIM, 
			      const char *filenameEVENT,
			      const char *filenameSAM,
			      const char *filenameEDP)
{
  // create output JSON object
  json_t *rootEDP = json_object();
  json_t *eventArray = json_array(); // for each analysis event

  // load SAM and EVENT
  json_error_t error;

  json_t *rootBIM = json_load_file(filenameBIM, 0, &error);
  json_t *giROOT = json_object_get(rootBIM,"GI");  
  int numStory =  json_integer_value(json_object_get(giROOT,"numStory"));

  json_t *rootEVENT = json_load_file(filenameEVENT, 0, &error);
  json_t *eventsArray = json_object_get(rootEVENT,"Events");  

  json_t *rootSAM = json_load_file(filenameSAM, 0, &error);
  json_t *mappingArray = json_object_get(rootSAM,"NodeMapping");  

  // 
  // parse each event:
  //  1. make sure earthquake
  //  2. add responses
  //
  
  int index;
  json_t *value;

  int numEDP = 0;

  json_array_foreach(eventsArray, index, value) {

    // check earthquake
    json_t *type = json_object_get(value,"type");  
    const char *eventType = json_string_value(type);
    if (strcmp(eventType,"Seismic") != 0) {
	json_object_clear(rootEVENT);
	printf("ERROR event type %s not Seismic", eventType);
	return -1;
    }

    // add the EDP for the event
    json_t *eventObj = json_object();

    json_t *name = json_object_get(value,"name"); 
    const char *eventName = json_string_value(name);
    json_object_set(eventObj,"name",json_string(eventName));

    //    json_dump_file(eventObj,"TEST",0);

    json_t *responsesArray = json_array(); // for each analysis event

    // create responses for floor accel and story drift 
    
    int mapIndex1;
    json_t *value1;
    //    int numStory = -1;
    if (mappingArray == 0) {
      for (int i=0; i<=numStory; i++) {
	json_t *response = json_object();
	json_object_set(response,"type",json_string("max_abs_acceleration"));      
	json_object_set(response,"cline",json_integer(1));
	json_object_set(response,"floor",json_integer(i+1));
	json_array_append(responsesArray,response);
	numEDP++;
      }
    } else {
      // NOTE THIS SHOULD REALLY FIND SMALLEST CLINE, CLINE 1 MAY NOT BE THERE
      json_array_foreach(mappingArray, mapIndex1, value1) {
	
	int cline = json_integer_value(json_object_get(value1,"cline"));
	int floor = json_integer_value(json_object_get(value1,"floor"));
	int node = json_integer_value(json_object_get(value1,"node"));
	
	//      printf("%d %d %d\n",cline,floor,node);
	
	if (cline == 1) {
	  //	numStory++;
	  json_t *response = json_object();
	  json_object_set(response,"type",json_string("max_abs_acceleration"));      
	  json_object_set(response,"cline",json_integer(cline));
	  json_object_set(response,"floor",json_integer(floor));
	  json_array_append(responsesArray,response);
	  numEDP++;
	}
      }
    }    
    for (int i=0; i<numStory; i++) {
      json_t *response = json_object();
      json_object_set(response,"type",json_string("max_drift"));      
      json_object_set(response,"cline",json_integer(1));
      json_object_set(response,"floor1",json_integer(i+1));
      json_object_set(response,"floor2",json_integer(i+2));
      json_array_append(responsesArray,response);
      numEDP++;
    }
  
    json_t *response = json_object();
    json_object_set(response,"type",json_string("residual_disp"));      
    json_object_set(response,"cline",json_integer(1));
    json_object_set(response,"floor",json_integer(numStory+1));
    json_array_append(responsesArray,response);
    numEDP++;

    json_object_set(eventObj,"responses",responsesArray);
      
    json_array_append(eventArray,eventObj);
  }
 json_object_set(rootEDP,"total_number_edp",json_integer(numEDP));  
  json_object_set(rootEDP,"EngineeringDemandParameters",eventArray);  

  //
  // dump json to file & clean up
  //
  json_dump_file(rootEDP,filenameEDP,0);
  json_object_clear(rootEDP);
  json_object_clear(rootEVENT);
  json_object_clear(rootSAM);

  return 0;
}

int 
HazusLossEstimator::determineLOSS(const char *filenameBIM, 
				  const char *filenameEVENT,
				  const char *filenameSAM,
				  const char *filenameEDP,
				  const char *filenameLOSS)
{
  return 0;
}

