

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <cctype>




#include "ConcreteShearWall.h"




int main(int argc, char **argv)
{

  if (argc != 5)
  {
    printf("ERROR: correct usage: createSAM fileNameBIM fileNameSAM nL nH\n");
    // nL is the number of element in the length-direction of a section
    // nH is the number of element in the height-direction of a section
    exit(0);
  }

  char *filenameBIM = argv[1];
  char *filenameSAM = argv[2];
  int nL = atoi(argv[3]);
  int nH = atoi(argv[4]);
  char *filenameEVENT = 0;



  ConcreteShearWall *theBuilding = new ConcreteShearWall();
  theBuilding->readBIM(filenameEVENT, filenameBIM);
  theBuilding->writeSAM(filenameSAM, nL, nH);

  return 0;
}
