#ifndef OPENSEES_PREPROCESSOR_H
#define OPENSEES_PREPROCESSOR_H
class json_t;
#include <fstream>
using namespace::std;

class OpenSeesPreprocessor {

 public:
  OpenSeesPreprocessor();
  ~OpenSeesPreprocessor();

  int writeRV(const char *BIM,
	      const char *SAM,
	      const char *EVENT,
	      const char *tcl);

  int createInputFile(const char *BIM,
		      const char *SAM,
		      const char *EVENT,
		      const char *EDP,
		      const char *tcl);

  int createInputFile(const char *BIM,
		      const char *SAM,
		      const char *EVENT,
		      const char *EDP,
		      const char *tcl,
		      const char *UQvariables);

  int processMaterials(ofstream &out);
  int processSections(ofstream &out);
  int processNodes(ofstream &out);
  int processElements(ofstream &out);
  int processDamping(ofstream &out);
  int processEvents(ofstream &s);
  int processEvent(ofstream &s, 
		   json_t *event, 
		   int &numPattern, 
		   int &numSeries);

  int getNode(int cline, int floor);

 private:
  char *filenameBIM;
  char *filenameSAM;
  char *filenameEVENT;
  char *filenameEDP;
  char *filenameTCL;
  char *filenameUQ;

  json_t *rootSAM;
  json_t *rootEDP;
  json_t *rootEVENT;
  json_t *mapping;

  int analysisType;
  int numSteps;
  double dT;
  int nStory;   //number of stories

  int NDM;
  int NDF;
};

#endif // OPENSEES_PREPROCESSOR_H
