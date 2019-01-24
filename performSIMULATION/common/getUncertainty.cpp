#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <fstream>

#include "OpenSeesPreprocessor.h"

int main(int argc, char **argv)
{
  if (argc != 5) {
    printf("ERROR: correct usage: getUncertainty fileNameBIM fileNameSAM fileNameEVENT filenameSIMULATION\n");
    exit(0);
  }


  char *filenameBIM = argv[1];
  char *filenameSAM = argv[2];
  char *filenameEVENT = argv[3];
  char *filenameSIMULATION = argv[4];

  std::cerr << "filenameBIM: " << argv[1] << "\n";
  std::cerr << "filenameSAM: " << argv[2] << "\n";
  std::cerr << "filenameEVENT: " << argv[3] << "\n";
  std::cerr << "filenameSIMULATION: " << argv[4] << "\n";

  OpenSeesPreprocessor *thePreprocessor = new OpenSeesPreprocessor();
  thePreprocessor->writeRV(filenameBIM, 
			   filenameSAM, 
			   filenameEVENT,
			   filenameSIMULATION);

  delete thePreprocessor;
  return 0;
}

