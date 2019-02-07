#include "Building.h"
#include <jansson.h> // for Json
#include <cstring>
#include <iostream>

Building::Building()
  :kFactor(1.0), dampFactor(1.0)
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

    return UNKNOWN;
}

Building::BldgOccupancy Building::s2BldgOccupancy(string s)
{
    transform(s.begin(), s.end(), s.begin(), ::tolower);

    if(s=="office")
        return office;
    if(s=="education")
        return education;
    if(s=="healthcare")
        return healthcare;
    if(s=="hospitality")
        return hospitality;
    if(s=="residence")
        return residence;
    if(s=="retail")
        return retail;
    if(s=="warehouse")
        return warehouse;
    if(s=="research")
        return research;

    return unknown;
}

Building::SeismicZone Building::s2SeismicZone(string s)
{
	transform(s.begin(), s.end(), s.begin(), ::toupper);

	if (s == "Z0")
		return Z0;
	if (s == "Z1")
		return Z1;
	if (s == "Z2A")
		return Z2A;
	if (s == "Z2B")
		return Z2B;
	if (s == "Z3")
		return Z3;
	if (s == "Z4")
		return Z4;

	return UNKNOWNZONE;
}

void
Building::readBIM(const char *event, const char *bim)
{
  //Parse BIM Json input file
  json_error_t error;
  json_t *rootBIM = json_load_file(bim, 0, &error);

  json_t *GI = json_object_get(rootBIM,"GI");  
  json_t *sType = json_object_get(GI,"structType");
  json_t *aType = json_object_get(GI,"area");
  json_t *nType = json_object_get(GI,"numStory");
  json_t *hType = json_object_get(GI,"height");
  json_t *yType = json_object_get(GI,"yearBuilt");

  const char *type = json_string_value(sType);
  string s(type);

  strutype=s2StruType(s);

  year=json_integer_value(yType);
  nStory=json_integer_value(nType);
  area=json_number_value(aType);
  storyheight=json_number_value(hType)/(nStory*1.);

  // parse EVENT (to see dimensionality needed
  ndf = 1;
  ndf = 2;
  
  // clean up
  json_object_clear(rootBIM);
}


//
// method to read BIM contents, with scale factors given in SAM
//

void
Building::readBIM(const char *event, const char *bim, const char *sam)
{
  //Parse BIM Json input file
  json_error_t error;
  json_t *rootBIM = json_load_file(bim, 0, &error);

  json_t *GI = json_object_get(rootBIM,"GI");  
  json_t *sType = json_object_get(GI,"structType");
  json_t *aType = json_object_get(GI,"area");
  json_t *nType = json_object_get(GI,"numStory");
  json_t *hType = json_object_get(GI,"height");
  json_t *yType = json_object_get(GI,"yearBuilt");
  
  json_t *zType = json_object_get(GI, "seismicZone");
  const char *type = json_string_value(sType);
  string s(type);
  strutype=s2StruType(s);

  const char *seismicZoneType = NULL;
  if (NULL == zType)
  {
	  std::cout << "No seismic zone specificed, Assuming seismic zone 4.\n";
	  seismicZoneType = "Z4";
  }
  else
	  seismicZoneType = json_string_value(zType);

  string s1(seismicZoneType);
  zone = s2SeismicZone(s1);

  year=json_integer_value(yType);
  nStory=json_integer_value(nType);
  area=json_number_value(aType);
  storyheight=json_number_value(hType)/(nStory*1.);

  json_object_clear(rootBIM);

  // parse EVENT (to see dimensionality needed
  ndf = 1;
  ndf = 2;

  // parse EVENT to see factors
  json_t *rootSAM = json_load_file(sam, 0, &error);

  // clean up
  json_t *randomVariables = json_object_get(rootSAM,"RandomVariables");
  if (randomVariables != 0) {
    int numRV = json_array_size(randomVariables);
    for (int i=0; i<numRV; i++) {
      json_t *theRV = json_array_get(randomVariables, i);
      const char *type = json_string_value(json_object_get(theRV, "name"));
      if (strcmp(type,"kFactor") == 0)
	kFactor = json_number_value(json_object_get(theRV, "value"));
      else if (strcmp(type,"dampFactor") == 0)
	dampFactor = json_number_value(json_object_get(theRV, "value"));
    }
  }
  json_object_clear(rootSAM);
}

void
Building::writeRV(const char *path)
{
    json_t *root = json_object();
    json_t *randomVariables = json_array();

    json_t *kFactor = json_object();
    json_object_set(kFactor, "distribution", json_string("normal"));
    json_object_set(kFactor, "mean", json_real(1.0));
    json_object_set(kFactor, "name", json_string("kFactor"));
    json_object_set(kFactor, "value", json_string("RV.kFactor"));
    json_object_set(kFactor, "stdDev", json_real(.1));
    json_t *kFactorVariable = json_object();

    json_t *dampFactor = json_object();
    json_object_set(dampFactor, "distribution", json_string("normal"));
    json_object_set(dampFactor, "mean", json_real(1.0));
    json_object_set(dampFactor, "name", json_string("dampFactor"));
    json_object_set(dampFactor, "value", json_string("RV.dampFactor"));
    json_object_set(dampFactor, "stdDev", json_real(.1));

    json_array_append(randomVariables, kFactor);
    json_array_append(randomVariables, dampFactor);
    json_object_set(root,"RandomVariables", randomVariables);

    // write the file & clean memory
    json_dump_file(root,path,0);
    json_object_clear(root);
}

void
Building::writeSAM(const char *path)
{
    json_t *root = json_object();
    json_t *properties = json_object();
    json_t *geometry = json_object();
    json_t *materials = json_array();
    json_t *nodes = json_array();
    json_t *elements = json_array();
    json_t *nodeMapping = json_array();

    // add node at ground
    json_t *node = json_object();
    json_object_set(node, "name", json_integer(1)); // +2 as we need node at 1 
    json_t *nodePosn = json_array();      
    json_array_append(nodePosn,json_real(0.0));
    json_array_append(nodePosn,json_real(0.0));
    json_object_set(node, "crd", nodePosn);
    json_object_set(node, "ndf", json_integer(ndf));
    json_array_append(nodes,node);

    json_t *nodeMap = json_object();
    json_object_set(nodeMap,"cline",json_integer(1));
    json_object_set(nodeMap,"floor",json_integer(1));
    json_object_set(nodeMap,"node",json_integer(1));
    json_array_append(nodeMapping, nodeMap);

    for(int i=0;i<nStory;++i) {

      json_t *node = json_object();
      json_t *element = json_object();
      json_t *material = json_object();

      json_object_set(node, "name", json_integer(i+2)); // +2 as we need node at 1 
      json_object_set(node, "mass", json_real(floorParams[i].mass));
      json_t *nodePosn = json_array();      
      json_array_append(nodePosn,json_real(floorParams[i].floor*storyheight));
      json_array_append(nodePosn,json_real(0.0));
      json_object_set(node, "crd", nodePosn);
      json_object_set(node, "ndf", json_integer(ndf));

      nodeMap = json_object();
      json_object_set(nodeMap,"cline",json_integer(1));
      json_object_set(nodeMap,"floor",json_integer(i+2));
      json_object_set(nodeMap,"node",json_integer(i+2));
      json_array_append(nodeMapping, nodeMap);

      json_object_set(element, "name", json_integer(interstoryParams[i].story));
      if (ndf == 1)
	json_object_set(element, "type", json_string("shear_beam"));
      else
	json_object_set(element, "type", json_string("shear_beam2d"));

      json_object_set(element, "uniaxial_material", json_integer(i+1));
      json_t *eleNodes = json_array();      
      json_array_append(eleNodes,json_integer(i+1));
      json_array_append(eleNodes,json_integer(i+2));
      json_object_set(element, "nodes", eleNodes);
      
      json_object_set(material,"name",json_integer(i+1));
      json_object_set(material,"type",json_string("shear"));
      json_object_set(material,"K0",json_real(interstoryParams[i].K0*kFactor));
      json_object_set(material,"Sy",json_real(interstoryParams[i].Sy));
      json_object_set(material,"eta",json_real(interstoryParams[i].eta));
      json_object_set(material,"C",json_real(interstoryParams[i].C));
      json_object_set(material,"gamma",json_real(interstoryParams[i].gamma));
      json_object_set(material,"alpha",json_real(interstoryParams[i].alpha));
      json_object_set(material,"beta",json_real(interstoryParams[i].beta));
      json_object_set(material,"omega",json_real(interstoryParams[i].omega));
      json_object_set(material,"eta_soft",json_real(interstoryParams[i].eta_soft));
      json_object_set(material,"a_k",json_real(interstoryParams[i].a_k));
      
      json_array_append(nodes,node);
      json_array_append(materials,material);
      json_array_append(elements, element);
    }
    
    json_object_set(properties,"dampingRatio",json_real(dampingRatio*dampFactor));
    json_object_set(properties,"uniaxialMaterials",materials);
    json_object_set(root,"Properties",properties);

    json_object_set(geometry,"nodes",nodes);
    json_object_set(geometry,"elements",elements);
    json_object_set(root,"Geometry",geometry);

    json_object_set(root,"NodeMapping",nodeMapping);

    // write the file & clean memory
    json_dump_file(root,path,0);
    json_object_clear(root);
}

string Building::GetHazusType()
{
    string s="";
    string suffix="";
    if(nStory<=3)
        suffix="L";
    else if (nStory<=7)
        suffix="M";
    else
        suffix="H";

    switch (strutype) {
    case RM1:
        if(nStory<=3)
            s="RM1L";
        else
            s="RM1M";
        break;
    case RM2:
        s="RM2"+suffix;
        break;
    case URM:
        if(nStory<=2)
           s="URML";
        else
            s="URMM";
        break;
    case C1:
        s="C1"+suffix;
        break;
    case C2:
        s="C2"+suffix;
        break;
    case C3:
        s="C3"+suffix;
        break;
    case W1:
        s="W1";
        break;
    case W2:
        s="W2";
        break;
    case S1:
        s="S1"+suffix;
        break;
    case S2:
        s="S2"+suffix;
        break;
    case S3:
        s="S3";
        break;
    case S4:
        s="S4"+suffix;
        break;
    case S5:
        s="S5"+suffix;
        break;
    case PC1:
        s="PC1";
        break;
    case PC2:
        s="PC2"+suffix;
        break;
    case MH:
        s="MH";
        break;
    default:
        break;
    }

    return s;
}
