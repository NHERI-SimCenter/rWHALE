#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <cctype>
#include "HazusLossEstimator.h"


int main(int argc, char **argv)
{
  if (argc != 5) {
    printf("ERROR: correct usage: createBIM fileNameSAM fileNameEVENT filenameEDP\n");
    exit(0);
  }

  char *filenameBIM = argv[1];
  char *filenameSAM = argv[2];
  char *filenameEVENT = argv[3];
  char *filenameEDP = argv[4];

  HazusLossEstimator* dl = new HazusLossEstimator();

  dl->createEDP(filenameBIM,filenameEVENT,filenameSAM,filenameEDP);

  delete dl;
  return 0;
}

