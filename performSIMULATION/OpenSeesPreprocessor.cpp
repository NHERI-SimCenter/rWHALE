
#include "OpenSeesPreprocessor.h"
#include <jansson.h> 
#include <string.h>
#include <string>
#include <sstream>
#include <map>

OpenSeesPreprocessor::OpenSeesPreprocessor()
  :filenameBIM(0),filenameSAM(0),filenameEVENT(0),filenameEDP(0),
   filenameTCL(0), filenameUQ(0), analysisType(-1), numSteps(0), dT(0.0), nStory(0)
{

}

OpenSeesPreprocessor::~OpenSeesPreprocessor(){
  if (filenameBIM != 0)
    delete [] filenameBIM;
  if (filenameSAM != 0)
    delete [] filenameSAM;
  if (filenameEVENT != 0)
    delete [] filenameEVENT;
  if (filenameEDP != 0)
    delete [] filenameEDP;
  if (filenameTCL != 0)
    delete [] filenameTCL;
  if (filenameUQ != 0)
    delete [] filenameUQ;
}

int 
OpenSeesPreprocessor::writeRV(const char *BIM,
			      const char *SAM,
			      const char *EVENT,
			      const char *outputFilename)
{
  //
  // for now just assume earthquake, dynamic & write the RV
  // 

    json_t *root = json_object();
    json_t *randomVariables = json_array();

    json_t *integration = json_object();
    json_t *integrationTypes=json_array();

    json_object_set(integration, "distribution", json_string("discrete_design_set_string"));
    json_object_set(integration, "name", json_string("integration_scheme"));
    json_object_set(integration, "value", json_string("RV.integration_scheme"));
    //    json_array_append(integrationTypes, json_string("NewmarkLinear"));
    //    json_array_append(integrationTypes, json_string("NewmarkAverage"));
    //    json_array_append(integrationTypes, json_string("HHTpt9"));
    json_array_append(integrationTypes, json_string("1"));
    json_array_append(integrationTypes, json_string("2"));
    json_array_append(integrationTypes, json_string("3"));
    json_object_set(integration,"elements",integrationTypes);

    json_array_append(randomVariables, integration);

    json_object_set(root,"RandomVariables",randomVariables);
    // write the file & clean memory
    json_dump_file(root,outputFilename,0);
    json_object_clear(root);
}

int 
OpenSeesPreprocessor::createInputFile(const char *BIM,
				      const char *SAM,
				      const char *EVENT,
				      const char *EDP,
				      const char *tcl)
{
  //
  // make copies of filenames in case methods need them
  //

  if (filenameBIM != 0)
    delete [] filenameBIM;
  if (filenameSAM != 0)
    delete [] filenameSAM;
  if (filenameEVENT != 0)
    delete [] filenameEVENT;
  if (filenameEDP != 0)
    delete [] filenameEDP;
  if (filenameTCL != 0)
    delete [] filenameTCL;
  if (filenameUQ != 0)
    delete [] filenameUQ;
  
  filenameBIM=(char*)malloc((strlen(BIM)+1)*sizeof(char));
  filenameSAM=(char*)malloc((strlen(SAM)+1)*sizeof(char));
  filenameEVENT=(char*)malloc((strlen(EVENT)+1)*sizeof(char));
  filenameEDP=(char*)malloc((strlen(EDP)+1)*sizeof(char));
  filenameTCL=(char*)malloc((strlen(tcl)+1)*sizeof(char));
  filenameUQ = 0;

  strcpy(filenameBIM,BIM);
  strcpy(filenameSAM,SAM);
  strcpy(filenameEVENT,EVENT);
  strcpy(filenameEDP,EDP);
  strcpy(filenameTCL,tcl);


  //
  // open tcl script
  // 

  ofstream *s = new ofstream;
  s->open(filenameTCL, ios::out);
  ofstream &tclFile = *s;

  //
  // process the SAM to create the model
  //

  json_error_t error;
  rootSAM = json_load_file(filenameSAM, 0, &error);

  mapping = json_object_get(rootSAM,"NodeMapping");  

  processNodes(tclFile);
  processMaterials(tclFile);
  processElements(tclFile);
  processDamping(tclFile);

  rootEVENT = json_load_file(filenameEVENT, 0, &error);
  rootEDP = json_load_file(filenameEDP, 0, &error);

  processEvents(tclFile);

  s->close();
  return 0;
}


int 
OpenSeesPreprocessor::createInputFile(const char *BIM,
				      const char *SAM,
				      const char *EVENT,
				      const char *EDP,
				      const char *tcl,
				      const char *UQ)
{
  //
  // make copies of filenames in case methods need them
  //

  if (filenameBIM != 0)
    delete [] filenameBIM;
  if (filenameSAM != 0)
    delete [] filenameSAM;
  if (filenameEVENT != 0)
    delete [] filenameEVENT;
  if (filenameEDP != 0)
    delete [] filenameEDP;
  if (filenameTCL != 0)
    delete [] filenameTCL;
  if (filenameUQ != 0)
    delete [] filenameUQ;

  filenameBIM=(char*)malloc((strlen(BIM)+1)*sizeof(char));
  filenameSAM=(char*)malloc((strlen(SAM)+1)*sizeof(char));
  filenameEVENT=(char*)malloc((strlen(EVENT)+1)*sizeof(char));
  filenameEDP=(char*)malloc((strlen(EDP)+1)*sizeof(char));
  filenameTCL=(char*)malloc((strlen(tcl)+1)*sizeof(char));
  filenameUQ=(char*)malloc((strlen(UQ)+1)*sizeof(char));

  strcpy(filenameBIM,BIM);
  strcpy(filenameSAM,SAM);
  strcpy(filenameEVENT,EVENT);
  strcpy(filenameEDP,EDP);
  strcpy(filenameTCL,tcl);
  strcpy(filenameUQ,UQ);

  //
  // open tcl script
  // 

  ofstream *s = new ofstream;
  s->open(filenameTCL, ios::out);
  ofstream &tclFile = *s;

  //
  // process the SAM to create the model
  //

  json_error_t error;
  rootSAM = json_load_file(filenameSAM, 0, &error);

  mapping = json_object_get(rootSAM,"NodeMapping");  

  processNodes(tclFile);
  processMaterials(tclFile);
  processElements(tclFile);

  rootEVENT = json_load_file(filenameEVENT, 0, &error);
  rootEDP = json_load_file(filenameEDP, 0, &error);

  processEvents(tclFile);

  s->close();
  return 0;
}


int 
OpenSeesPreprocessor::processMaterials(ofstream &s){
  int index;
  json_t *material;

  json_t *propertiesObject = json_object_get(rootSAM,"Properties");  
  json_t *materials = json_object_get(propertiesObject,"uniaxialMaterials");
  json_array_foreach(materials, index, material) {
    const char *type = json_string_value(json_object_get(material,"type"));
    
    if (strcmp(type,"shear") == 0) {
      int tag = json_integer_value(json_object_get(material,"name"));
      double K0 = json_real_value(json_object_get(material,"K0"));
      double Sy = json_real_value(json_object_get(material,"Sy"));
      double eta = json_real_value(json_object_get(material,"eta"));
      double C = json_real_value(json_object_get(material,"C"));
      double gamma = json_real_value(json_object_get(material,"gamma"));
      double alpha = json_real_value(json_object_get(material,"alpha"));
      double beta = json_real_value(json_object_get(material,"beta"));
      double omega = json_real_value(json_object_get(material,"omega"));
      double eta_soft = json_real_value(json_object_get(material,"eta_soft"));
      double a_k = json_real_value(json_object_get(material,"a_k"));
      //s << "uniaxialMaterial Elastic " << tag << " " << K0 << "\n";
      if (K0==0)
          K0=1.0e-6;
      if (eta==0)
          eta=1.0e-6;
      //uniaxialMaterial Hysteretic $matTag $s1p $e1p $s2p $e2p <$s3p $e3p>
      //$s1n $e1n $s2n $e2n <$s3n $e3n> $pinchX $pinchY $damage1 $damage2 <$beta>
      s << "uniaxialMaterial Hysteretic " << tag << " " << Sy << " " << Sy/K0
        << " " << alpha*Sy << " " << Sy/K0+(alpha-1)*Sy/eta/K0
        << " " << alpha*Sy << " " << 1.0
        << " " << -beta*Sy << " " << -beta*Sy/K0 << " " << -beta*(alpha*Sy)
        << " " << -(beta*Sy/K0 + beta*(alpha-1)*Sy/eta/K0)
        << " " << -beta*(alpha*Sy) << " " << -1.0
        << " " << gamma
        << " " << gamma << " " << 0.0 << " " << 0.0 << " " << a_k << "\n";
    }
  }
  return 0;
}

int 
OpenSeesPreprocessor::processSections(ofstream &s) {
  return 0;
}

int 
OpenSeesPreprocessor::processNodes(ofstream &s){
  int index;
  json_t *node;

  json_t *geometry = json_object_get(rootSAM,"Geometry");  
  json_t *nodes = json_object_get(geometry,"nodes");

  NDM = 0;
  NDF = 0;

  json_array_foreach(nodes, index, node) {

    int tag = json_integer_value(json_object_get(node,"name"));
    if(nStory<tag)  nStory=tag;
    json_t *crds = json_object_get(node,"crd");
    int sizeCRD = json_array_size(crds);
    int ndf = json_integer_value(json_object_get(node,"ndf"));

    if (sizeCRD != NDM || ndf != NDF) {
      NDM = sizeCRD;
      NDF = ndf;
      // issue new model command if node size changes
      s << "model BasicBuilder -ndm " << NDM << " -ndf " << NDF << "\n";
    } 

    s << "node " << tag << " ";
    json_t *crd;
    int crdIndex;
    json_array_foreach(crds, crdIndex, crd) {
      s << json_real_value(crd) << " " ;
    }

    json_t *mass = json_object_get(node,"mass");
    if (mass != NULL) {
      s << "-mass ";
      double massV = json_real_value(mass);
      for (int i=0; i<NDF; i++)
	s << massV << " " ;
    }

    s << "\n";
  }

  int nodeTag = getNode(1,1);
  s << "fix " << nodeTag;
  for (int i=0; i<NDF; i++)
     s << " " << 1;
  s << "\n";

  return 0;
}

int 
OpenSeesPreprocessor::processElements(ofstream &s){
  int index;
  json_t *element;

  json_t *geometry = json_object_get(rootSAM,"Geometry");  
  json_t *elements = json_object_get(geometry,"elements");

  json_array_foreach(elements, index, element) {

    int tag = json_integer_value(json_object_get(element,"name"));
    const char *type = json_string_value(json_object_get(element,"type"));
    if (strcmp(type,"shear_beam") == 0) {
      s << "element zeroLength " << tag << " " ;
      json_t *nodes = json_object_get(element,"nodes");
      json_t *nodeTag;
      int nodeIndex;
      json_array_foreach(nodes, nodeIndex, nodeTag) {
	s << json_integer_value(nodeTag) << " " ;
      }

      int matTag = json_integer_value(json_object_get(element,"uniaxial_material"));
      s << "-mat " << matTag << " -dir 1 \n";
    }
    else if (strcmp(type,"shear_beam2d") == 0) {
      s << "element zeroLength " << tag << " " ;
      json_t *nodes = json_object_get(element,"nodes");
      json_t *nodeTag;
      int nodeIndex;
      json_array_foreach(nodes, nodeIndex, nodeTag) {
	s << json_integer_value(nodeTag) << " " ;
      }

      int matTag = json_integer_value(json_object_get(element,"uniaxial_material"));
      s << "-mat " << matTag << " " << matTag << " -dir 1 2\n";
    }
  }
  return 0;
}


int
OpenSeesPreprocessor::processDamping(ofstream &s){
    json_t *propertiesObject = json_object_get(rootSAM,"Properties");
    double damping = json_real_value(json_object_get(propertiesObject,"dampingRatio"));
    s << "set xDamp " << damping << ";\n"
      << "set MpropSwitch 1.0;\n"
      << "set KcurrSwitch 0.0;\n"
      << "set KinitSwitch 0.0;\n"
      << "set KcommSwitch 1.0;\n"
      << "set nEigenI 1;\n";

    json_t *geometry = json_object_get(rootSAM,"Geometry");
    json_t *nodes = json_object_get(geometry,"nodes");
    int nStory = json_array_size(nodes)-1;
    int nEigenJ=0;
    if(nStory<=2)
        nEigenJ=nStory*2;   //first mode or second mode
    else
        nEigenJ=3*2;          //third mode

     s << "set nEigenJ "<<nEigenJ<<";\n"
       << "set lambdaN [eigen -fullGenLapack "<< nStory*2 <<"];\n"
       << "set lambdaI [lindex $lambdaN [expr $nEigenI-1]];\n"
       << "set lambdaJ [lindex $lambdaN [expr $nEigenJ-1]];\n"
       << "set omegaI [expr pow($lambdaI,0.5)];\n"
       << "set omegaJ [expr pow($lambdaJ,0.5)];\n"
       << "set alphaM [expr $MpropSwitch*$xDamp*(2*$omegaI*$omegaJ)/($omegaI+$omegaJ)];\n"
       << "set betaKcurr [expr $KcurrSwitch*2.*$xDamp/($omegaI+$omegaJ)];\n"
       << "set betaKinit [expr $KinitSwitch*2.*$xDamp/($omegaI+$omegaJ)];\n"
       << "set betaKcomm [expr $KcommSwitch*2.*$xDamp/($omegaI+$omegaJ)];\n"
       << "rayleigh $alphaM $betaKcurr $betaKinit $betaKcomm;\n";
  return 0;
}


int 
OpenSeesPreprocessor::processEvents(ofstream &s){

  //
  // foreach EVENT
  //   create load pattern
  //   add recorders
  //   add analysis script
  //

  int numTimeSeries = 1;
  int numPatterns = 1;

  int index;
  json_t *event;

  json_t *events = json_object_get(rootEVENT,"Events");  
  json_t *edps = json_object_get(rootEDP,"EngineeringDemandParameters");  
  
  int numEvents = json_array_size(events);
  int numEDPs = json_array_size(edps);

  for (int i=0; i<numEvents; i++) {

    // process event
    json_t *event = json_array_get(events,i);
    processEvent(s,event,numPatterns,numTimeSeries);
    const char *eventName = json_string_value(json_object_get(event,"name"));

    // create recorder foreach EDP
    // loop through EDPs and find corresponding EDP
    for (int j=0; j<numEDPs; j++) {

      json_t *eventEDPs = json_array_get(edps, j);
      const char *edpEventName = json_string_value(json_object_get(eventEDPs,"name"));

      if (strcmp(edpEventName, eventName) == 0) {
	json_t *eventEDP = json_object_get(eventEDPs,"responses");
	int numResponses = json_array_size(eventEDP);
	for (int k=0; k<numResponses; k++) {

	  json_t *response = json_array_get(eventEDP, k);
	  const char *type = json_string_value(json_object_get(response, "type"));
	  if (strcmp(type,"max_abs_acceleration") == 0) {
	    int cline = json_integer_value(json_object_get(response, "cline"));
	    int floor = json_integer_value(json_object_get(response, "floor"));

	    int nodeTag = this->getNode(cline,floor);	    
	    //	    std::ostringstream fileString(string(edpEventName)+string(type));
	    string fileString;
	    ostringstream temp;  //temp as in temporary
	    temp << filenameBIM << edpEventName << "." << type << "." << cline << "." << floor << ".out";
	    fileString=temp.str(); 

	    const char *fileName = fileString.c_str();
	    
	    int startTimeSeries = numTimeSeries-NDF;
	    s << "recorder EnvelopeNode -file " << fileName;
	    s << " -timeSeries ";
	    for (int i=0; i<NDF; i++)
	      s << i+startTimeSeries << " " ;
	    s << " -node " << nodeTag << " -dof ";
	    for (int i=1; i<=NDF; i++)
	      s << i << " " ;
	    s << " accel\n";
	  }

	  else if (strcmp(type,"max_drift") == 0) {
	    int cline = json_integer_value(json_object_get(response, "cline"));
	    int floor1 = json_integer_value(json_object_get(response, "floor1"));
	    int floor2 = json_integer_value(json_object_get(response, "floor2"));

	    int nodeTag1 = this->getNode(cline,floor1);	    
	    int nodeTag2 = this->getNode(cline,floor2);	    

	    string fileString1;
	    string fileString2;
	    ostringstream temp1;  //temp as in temporary
	    ostringstream temp2;  //temp as in temporary
	    temp1 << filenameBIM << edpEventName << "." << type << "." << cline << "." << floor1 << "." << floor2 << "-1.out";
	    temp2 << filenameBIM << edpEventName << "." << type << "." << cline << "." << floor1 << "." << floor2 << "-2.out";
	    fileString1=temp1.str(); 
	    fileString2=temp2.str(); 

	    const char *fileName1 = fileString1.c_str();
	    const char *fileName2 = fileString2.c_str();
	    
	    s << "recorder EnvelopeDrift -file " << fileName1;
	    s << " -iNode " << nodeTag1 << " -jNode " << nodeTag2;
	    s << " -dof 1 -perpDirn 1\n";

	    s << "recorder EnvelopeDrift -file " << fileName2;
	    s << " -iNode " << nodeTag1 << " -jNode " << nodeTag2;
	    s << " -dof 2 -perpDirn 1\n";
	  }

	  else if (strcmp(type,"residual_disp") == 0) {

	    int cline = json_integer_value(json_object_get(response, "cline"));
	    int floor = json_integer_value(json_object_get(response, "floor"));

	    int nodeTag = this->getNode(cline,floor);	    

	    string fileString;
	    ostringstream temp;  //temp as in temporary
	    temp << filenameBIM << edpEventName << "." << type << "." << cline << "." << floor << ".out";
	    fileString=temp.str(); 

	    const char *fileName = fileString.c_str();
	    
	    s << "recorder Node -file " << fileName;
	    s << " -node " << nodeTag << " -dof ";
	    for (int i=1; i<=NDF; i++)
	      s << i << " " ;
	    s << " disp\n";
	  }
	}
      }
    }

    // create analysis
    if (analysisType == 1) {
      //      s << "handler Plain\n";
      s << "numberer RCM\n";
      s << "system BandGen\n";

      if (filenameUQ != 0) {
	printf("HI filenameUQ %s\n", filenameUQ);
	json_error_t error;
	json_t *rootUQ = json_load_file(filenameUQ, 0, &error);
	json_dump_file(rootUQ,"TEST",0);
	
	json_t *theRVs  = json_object_get(rootUQ,"RandomVariables");  
	json_t *theRV;
	int index;

	json_array_foreach(theRVs, index, theRV) {
	  const char *type = json_string_value(json_object_get(theRV,"name"));
	  printf("type: %s\n",type);
	  if (strcmp(type,"integration_scheme") == 0) {    
	    //	    const char *typeI = json_string_value(json_object_get(theRV,"value"));
	    int typeI = json_integer_value(json_object_get(theRV,"value"));
	    if (typeI == 1) {    
	      s << "integrator Newmark 0.5 0.25\n";
	    } else if (typeI == 2) {    
	      s << "integrator Newmark 0.5 0.1667\n";
	    } else if (typeI == 3) {    
	      s << "integrator HHT .9\n";
	    }
	    //	  printf("type: %s\n",typeI);
	    /*
	    int typeI = json_integer_value(json_object_get(theRV,"value"));
	    if (strcmp(typeI,"NewmarkLinear") == 0) {    
	      //	      s << "integrator Newmark 0.5 0.25\n";
	    } else if (strcmp(typeI,"NewmarkAverage") == 0) {    
	      //	      s << "integrator Newmark 0.5 0.1667\n";
	    } else if (strcmp(typeI,"HHTpt9") == 0) {    
	      //	      s << "integrator HHT .9\n";
	    }
	    */
	  }
	}
      }

      s << "analysis Transient\n";
      s << "analyze " << numSteps << " " << dT << "\n";
    }
  }
  return 0;
}


// seperate for multi events
int 
OpenSeesPreprocessor::processEvent(ofstream &s, 
				   json_t *event, 
				   int &numPattern, 
				   int &numSeries){
  json_t *timeSeries;
  json_t *pattern;

  const char *eventType = json_string_value(json_object_get(event,"type"));
  if (strcmp(eventType,"Seismic") == 0) {
    analysisType = 1;
    numSteps = json_integer_value(json_object_get(event,"numSteps"));
    dT = json_real_value(json_object_get(event,"dT"));
  } else
    return -1;

  std::map <string,int> timeSeriesList;
  std::map <string,int>::iterator it;

  int index;
  json_t *timeSeriesArray = json_object_get(event,"timeSeries");
  json_array_foreach(timeSeriesArray, index, timeSeries) {
    const char *subType = json_string_value(json_object_get(timeSeries,"type"));        
    if (strcmp(subType,"Value")  == 0) {
      double dt = json_real_value(json_object_get(timeSeries,"dT"));
      json_t *data = json_object_get(timeSeries,"data");
      s << "timeSeries Path " << numSeries << " -dt " << dt;
      s << " -values \{ ";
      json_t *dataV;
      int dataIndex;
      json_array_foreach(data, dataIndex, dataV) {
	s << json_real_value(dataV) << " " ;
      }
      s << " }\n";

      string name(json_string_value(json_object_get(timeSeries,"name")));
      printf("%s\n",name.c_str());
      timeSeriesList[name]=numSeries;
      numSeries++;
    }
  }

  json_t *patternArray = json_object_get(event,"pattern");
  json_array_foreach(patternArray, index, pattern) {
    const char *subType = json_string_value(json_object_get(pattern,"type"));        
    if (strcmp(subType,"UniformAcceleration")  == 0) {
      int dirn = json_integer_value(json_object_get(pattern,"dof"));

      int series = 0;
      string name(json_string_value(json_object_get(pattern,"timeSeries")));
      printf("%s\n",name.c_str());
      it = timeSeriesList.find(name);
      if (it != timeSeriesList.end())
	series = it->second;

      int seriesTag = timeSeriesList[name];
      s << "pattern UniformExcitation " << numPattern << " " << dirn;
      s << " -accel " << series << "\n";
      numPattern++;
    }
  }


  
  //  printf("%d %d %f\n",analysisType, numSteps, dT);

  return 0;
}


int
OpenSeesPreprocessor:: getNode(int cline, int floor){

  int numMapObjects = json_array_size(mapping);
  for (int i=0; i<numMapObjects; i++) {
    json_t *mapObject = json_array_get(mapping, i); 
    int c = json_integer_value(json_object_get(mapObject,"cline"));
    if (c == cline) {
      int f = json_integer_value(json_object_get(mapObject,"floor"));
      if (f == floor)
	return json_integer_value(json_object_get(mapObject,"node"));
    }
  }
  return -1;
}
