
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <cctype>
#include "Building.h"

int main(int argc, char **argv)
{
  if (argc != 4) {
    printf("ERROR: correct usage: createSAM fileNameBIM fileNameEVENT fileNameSAM\n");
    exit(0);
  }

  char *filenameBIM = argv[1];
  char *filenameEVENT = argv[2];
  char *filenameSAM = argv[3];


  Building *theBuilding = new Building();
  theBuilding->writeRV(filenameSAM);

  //  delete aim;
  return 0;
}

