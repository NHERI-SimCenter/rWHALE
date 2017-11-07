#include <iostream>
#include <fstream>
#include "HazusLossEstimator.h"

using namespace std;

int main(int argc, char *argv[])
{
  if (argc != 4) {
    printf("ERROR: correct usage: determineLOSS fileNameBIM fileNameEDP filenameLOSS\n");
    exit(0);
  }

  char *filenameBIM = argv[1];
  char *filenameEDP = argv[2];
  char *filenameLOSS = argv[3];
  char *loss = argv[3];

  HazusLossEstimator * le = new HazusLossEstimator();

  le->determineLOSS(filenameBIM,filenameEDP,filenameLOSS);

  delete le;
  return 0;
}
