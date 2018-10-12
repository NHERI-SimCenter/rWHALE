#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <fstream>

#include "OpenSeesPostprocessor.h"

int main(int argc, char **argv)
{

  printf("num args: %d\n",argc);

  if (argc != 5) {
    printf("ERROR %d: correct usage: postprocessOpenSees fileNameBIM fileNameSAM fileNameEVENT filenameEDP\n", argc);
    exit(0);
  }

  char *filenameBIM = argv[1];
  char *filenameSAM = argv[2];
  char *filenameEVENT = argv[3];
  char *filenameEDP = argv[4];

  OpenSeesPostprocessor *thePostprocessor = new OpenSeesPostprocessor();

  thePostprocessor->processResults(filenameBIM, filenameEDP);

  delete thePostprocessor;
  return 0;
}

