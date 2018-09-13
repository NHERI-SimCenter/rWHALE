
#include "OpenSeesPostprocessor.h"
#include <jansson.h> 
#include <string.h>
#include <math.h>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

OpenSeesPostprocessor::OpenSeesPostprocessor()
  :filenameEDP(0), filenameBIM(0)
{

}

OpenSeesPostprocessor::~OpenSeesPostprocessor(){
  if (filenameEDP != 0)
    delete [] filenameEDP;
  if (filenameBIM != 0)
    delete [] filenameBIM;
}

int 
OpenSeesPostprocessor::processResults(const char *BIM, const char *EDP)
{
  //
  // make copies of filenames in case methods need them
  //

  if (filenameEDP != 0)
    delete [] filenameEDP;
  if (filenameBIM != 0)
    delete [] filenameBIM;

  filenameEDP=(char*)malloc((strlen(EDP)+1)*sizeof(char));
  strcpy(filenameEDP,EDP);
  filenameBIM=(char*)malloc((strlen(BIM)+1)*sizeof(char));
  strcpy(filenameBIM,BIM);

  json_error_t error;
  rootEDP = json_load_file(filenameEDP, 0, &error);
  
  processEDPs();

  json_dump_file(rootEDP,filenameEDP,0);
  json_object_clear(rootEDP);  
  return 0;
}


int 
OpenSeesPostprocessor::processEDPs(){

  //
  // foreach EVENT
  //   processEDPs, i.e. open ouputfile, read data, write to edp and dump
  //

  int numTimeSeries = 1;
  int numPatterns = 1;

  int index;
  json_t *event;

  json_t *edps = json_object_get(rootEDP,"EngineeringDemandParameters");  
  
  int numEvents = json_array_size(edps);

  for (int i=0; i<numEvents; i++) {

    // process event
    json_t *eventEDPs = json_array_get(edps,i);
    const char *eventName = json_string_value(json_object_get(eventEDPs,"name"));

    // loop through EDPs
    for (int j=0; j<numEvents; j++) {

      json_t *eventEDPs = json_array_get(edps, j);
      const char *edpEventName = json_string_value(json_object_get(eventEDPs,"name"));
      
      json_t *eventEDP = json_object_get(eventEDPs,"responses");
      int numResponses = json_array_size(eventEDP);
      for (int k=0; k<numResponses; k++) {

	json_t *response = json_array_get(eventEDP, k);
	const char *type = json_string_value(json_object_get(response, "type"));

	if (strcmp(type,"max_abs_acceleration") == 0) {
	  int cline = json_integer_value(json_object_get(response, "cline"));
	  int floor = json_integer_value(json_object_get(response, "floor"));
	  
	  string fileString;
	  ostringstream temp;  //temp as in temporary
	  temp << filenameBIM << edpEventName << "." << type << "." << cline << "." << floor << ".out";
	  fileString=temp.str(); 

	  const char *fileName = fileString.c_str();
	    
	  // openfile & process data
	  ifstream myfile;
	  myfile.open (fileName);
	  double abs1Min, abs1Max, abs1Value;
	  double abs2Min, abs2Max, abs2Value;

	  if (myfile.is_open()) {
	    myfile >> abs1Min >> abs2Min >> abs1Max >> abs2Max >> abs1Value >> abs2Value;
	    myfile.close();
	  }
	  if (abs2Value > abs1Value)
	    abs1Value = abs2Value;

	  json_object_set(response,"scalar_data",json_real(abs2Value));
	  /*
	  json_t *scalarValues = json_object_get(response,"scalar_data");
	  json_array_append(scalarValues,json_real(abs2Value));
	  */

	} else if (strcmp(type,"max_drift") == 0) {

	  int cline = json_integer_value(json_object_get(response, "cline"));
	  int floor1 = json_integer_value(json_object_get(response, "floor1"));
	  int floor2 = json_integer_value(json_object_get(response, "floor2"));

	  //
	  string fileString1;
	  ostringstream temp1;  //temp as in temporary
	  temp1 << filenameBIM << edpEventName << "." << type << "." << cline << "." 
		<< floor1 << "." << floor2 << "-1.out";
	  fileString1=temp1.str(); 

	  const char *fileName1 = fileString1.c_str();

	  // openfile & process data
	  ifstream myfile;
	  myfile.open (fileName1);
	  double absMin, absMax, absValue;
	  
	  absValue = 0.0;
	  if (myfile.is_open()) {
	    myfile >> absMin >> absMax >> absValue;
	    myfile.close();
	  } 

	  //
	  string fileString2;
	  ostringstream temp2;  //temp as in temporary
	  temp2 << filenameBIM << edpEventName << "." << type << "." << cline << "." 
		<< floor1 << "." << floor2 << "-2.out";
	  fileString2=temp2.str(); 

	  const char *fileName2 = fileString2.c_str();

	  // openfile & process data
	  myfile.open (fileName2);
	  
	  if (myfile.is_open()) {
	    myfile >> absMin >> absMax >> absMax;
	    myfile.close();
	  } 
	    
	  if (absMax > absValue)
	    absValue = absMax;

	  json_object_set(response,"scalar_data",json_real(absValue));
	  /*
	  json_t *scalarValues = json_object_get(response,"scalar_data");
	  json_array_append(scalarValues,json_real(absValue));
	  json_object_set(response,"scalar_data",scalarValues);
	  */
	}

	else if (strcmp(type,"residual_disp") == 0) {
	  int cline = json_integer_value(json_object_get(response, "cline"));
	  int floor = json_integer_value(json_object_get(response, "floor"));
	  
	  string fileString;
	  ostringstream temp;  //temp as in temporary
	  temp << filenameBIM << edpEventName << "." << type << "." << cline << "." << floor << ".out";
	  fileString=temp.str(); 

	  const char *fileName = fileString.c_str();
	    
	  // openfile & process data
	  ifstream myfile;
	  myfile.open (fileName);
	  double num1 = 0.; 
	  double num2 = 0.;
	  double num = 0.;

	  if (myfile.is_open()) {
	    //	    std::vector<double> scores;
	    //keep storing values from the text file so long as data exists:
	    while (myfile >> num1 >> num2) {
	      //	      scores.push_back(num);
	    }
	    
	    // need to process to get the right value, for now just output last
	    num = fabs(num1);
	    if (fabs(num2) > num)
	      num = fabs(num2);

	    myfile.close();
	  }
	  
	  json_object_set(response,"scalar_data",json_real(num));
	  /*
	  json_t *scalarValues = json_object_get(response,"scalar_data");
	  json_array_append(scalarValues,json_real(num));
	  */
	}
      }
    }
  }
  return 0;
}


