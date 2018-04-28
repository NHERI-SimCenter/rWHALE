#include <string.h>
#include <string>

#include "Building.h"

Building::Building()
{

}

Building::StruType Building::s2StruType(string s)
{
    transform(s.begin(), s.end(), s.begin(), ::toupper);
    if(s=="RM1")
        return RM1;
    if(s=="RM2")
        return RM2;
    if(s=="URM")
        return URM;
    if(s=="C1")
        return C1;
    if(s=="C2")
        return C2;
    if(s=="C3")
        return C3;
    if(s=="W1")
        return W1;
    if(s=="W2")
        return W2;
    if(s=="S1")
        return S1;
    if(s=="S2")
        return S2;
    if(s=="S3")
        return S3;
    if(s=="S4")
        return S4;
    if(s=="S5")
        return S5;
	if(s=="PC1")
        return PC1;
    if(s=="PC2")
        return PC2;
    if(s=="MH")
        return MH;

    cout<<"\nWarning: unknown structural type of building "<<this->name<<endl;
    return UNKNOWN;
}

Building::BldgOccupancy Building::s2BldgOccupancy(string s)
{
    transform(s.begin(), s.end(), s.begin(), ::tolower);

    if(s=="office")
        return office;
    if(s=="education"||s=="school")
        return education;
    if(s=="healthcare")
        return healthcare;
    if(s=="hospitality"||s=="hotel")
        return hospitality;
    if(s=="residence"||s=="residential")
        return residence;
    if(s=="retail")
        return retail;
    if(s=="warehouse"||s=="industrial")
        return warehouse;
    if(s=="research")
        return research;

    cout<<"\nWarning: unknown occupancy of building "<<this->name<<endl;
    return unknown;
}

void Building::readBIM(const char *path)
{
    //TODO
    //Parse Json input file
    json_error_t error;
    json_t *rootBIM = json_load_file(path, 0, &error);

    json_t *GI = json_object_get(rootBIM,"GI");
    json_t *sType = json_object_get(GI,"structType");
    json_t *aType = json_object_get(GI,"area");
    json_t *nType = json_object_get(GI,"numStory");
    json_t *hType = json_object_get(GI,"height");
    json_t *yType = json_object_get(GI,"yearBuilt");
    json_t *oType = json_object_get(GI,"occupancy");
    json_t *rType = json_object_get(GI,"replacementCost");
    json_t *tType = json_object_get(GI,"replacementTime");

    name=json_string_value(json_object_get(GI,"name"));

    const char *type = json_string_value(sType);
    string s(type);
    strutype=s2StruType(s);

    const char *type2 = json_string_value(oType);
    string s2(type2);
    occupancy=s2BldgOccupancy(s2);

    year=json_integer_value(yType);
    storyheight=json_number_value(hType);
    nStory=json_integer_value(nType);
    area=json_number_value(aType);
    replacementCost=json_number_value(rType);
    replacementTime=json_number_value(tType);
}

void Building::readEDP(const char *filenameEDP,int resp_index)
{

    edp.IDR.resize(nStory);
    edp.PFA.resize(nStory+1);
    edp.PFV.resize(nStory+1);

    /* TODO: should be modified using "exampleEDP.json"
    ifstream fin(path);

    for(int i=0;i<nStory;i++)
        fin>>edp.IDR[i];
    for(int i=0;i<=nStory;i++)
        fin>>edp.PFA[i];
    fin>>edp.residual;

    fin.close();
    *************************/


    // set to zero
    for(int i=0;i<nStory;i++)
      edp.IDR[i] =0;
    for(int i=0;i<=nStory;i++)
      edp.PFA[i] =0;
    edp.residual =0;

    json_error_t error;
    json_t *rootEDP = json_load_file(filenameEDP, 0, &error);

    // foreach EVENT
    //   processEDPs, i.e. read edp, if bigger than current set to current
    //
    
    int index;
    json_t *event;
    
    json_t *edps = json_object_get(rootEDP,"EngineeringDemandParameters");  
    
    int numEvents = json_array_size(edps);
    
    for (int i=0; i<numEvents; i++) {
      
      // process event
      json_t *eventEDPs = json_array_get(edps,i);
      
      // loop through EDPs
      for (int j=0; j<numEvents; j++) {
	
	json_t *eventEDPs = json_array_get(edps, j);
	
	json_t *eventEDP = json_object_get(eventEDPs,"responses");
	int numResponses = json_array_size(eventEDP);
	for (int k=0; k<numResponses; k++) {
	  
	  json_t *response = json_array_get(eventEDP, k);
	  const char *type = json_string_value(json_object_get(response, "type"));
	  
	  if (strcmp(type,"max_abs_acceleration") == 0) {
	    int cline = json_integer_value(json_object_get(response, "cline"));
	    int floor = json_integer_value(json_object_get(response, "floor"));
        //double value = json_real_value(json_object_get(response, "scalar_data"));
        json_t *values = json_object_get(response,"scalar_data");
        double value = json_real_value(json_array_get(values, resp_index));

	    if (edp.PFA[floor-1] < value)	    
	      edp.PFA[floor-1] = value;

	  } else if (strcmp(type,"max_drift") == 0) {
	    int cline = json_integer_value(json_object_get(response, "cline"));
	    int floor1 = json_integer_value(json_object_get(response, "floor1"));
	    int floor2 = json_integer_value(json_object_get(response, "floor1"));
        //double value = json_real_value(json_object_get(response, "scalar_data"));
        json_t *values = json_object_get(response,"scalar_data");
        double value = json_real_value(json_array_get(values, resp_index));

	    if (edp.IDR[floor1-1] < value)	    
	      edp.IDR[floor1-1] = value;	    
	  }
	  
	  else if (strcmp(type,"residual_disp") == 0) {
	    int cline = json_integer_value(json_object_get(response, "cline"));
	    int floor = json_integer_value(json_object_get(response, "floor"));
        //double value = json_real_value(json_object_get(response, "scalar_data"));
        json_t *values = json_object_get(response,"scalar_data");
        double value = json_real_value(json_array_get(values, resp_index));
	    if (edp.residual < value)
	      edp.residual =value;
	  }
	}
      }
    }
}

int Building::getNumResponses(const char *filenameEDP)
{

    json_error_t error;
    json_t *rootEDP = json_load_file(filenameEDP, 0, &error);
    json_t *edps = json_object_get(rootEDP,"EngineeringDemandParameters");
    json_t *eventEDPs = json_array_get(edps, 0);
    json_t *eventEDP = json_object_get(eventEDPs,"responses");
    json_t *response = json_array_get(eventEDP, 0);
    json_t *values = json_object_get(response,"scalar_data");
    int numValues = json_array_size(values);
    return numValues;
}
