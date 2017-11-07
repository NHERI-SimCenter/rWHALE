#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <fstream>

#include "OpenSeesPreprocessor.h"

int main(int argc, char **argv)
{
  printf("%d\n", argc);
  if (argc != 6 && argc != 7) {
    printf("HELLO\n");
    printf("ERROR: correct usage: preprocessOpenSees fileNameBIM fileNameSAM fileNameEVENT filenameEDP filnameTCL\n");
    exit(0);
  }

  char *filenameBIM = argv[1];
  char *filenameSAM = argv[2];
  char *filenameEVENT = argv[3];
  char *filenameEDP = argv[4];
  char *filenameTCL = argv[5];

  OpenSeesPreprocessor *thePreprocessor = new OpenSeesPreprocessor();

  if (argc == 6) {

    thePreprocessor->createInputFile(filenameBIM, 
				     filenameSAM, 
				     filenameEVENT,
				     filenameEDP,
				     filenameTCL);
  } else {

    thePreprocessor->createInputFile(filenameBIM, 
				     filenameSAM, 
				     filenameEVENT,
				     filenameEDP,
				     filenameTCL,
				     argv[6]);
  }
  delete thePreprocessor;
  return 0;
}

