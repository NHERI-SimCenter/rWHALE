#include <iostream>
#include <fstream>
#include "HazusLossEstimator.h"

using namespace std;

int main(int argc, const char *argv[])
{
  const char *filenameBIM = 0;
  const char *filenameEDP = 0;
  const char *filenameLOSS = 0;
  const char *fileSettings = 0;
  const char *pathCurves = 0;
  const char *pathNormative = 0;

  int arg = 1;
  while (arg < argc) {
    if (strcmp(argv[arg], "-filenameBIM") ==0) {
      arg++;
      filenameBIM = argv[arg];
    }
    else if (strcmp(argv[arg], "-filenameEDP") ==0) {
      arg++;
      filenameEDP = argv[arg];
    }
    else if (strcmp(argv[arg], "-filenameLOSS") ==0) {
      arg++;
      filenameLOSS = argv[arg];
    }
    else if (strcmp(argv[arg], "-filenameSettings") ==0) {
      arg++;
      fileSettings = argv[arg];
    }
    else if (strcmp(argv[arg], "-pathCurves") ==0) {
      arg++;
      pathCurves = argv[arg];
    }
    else if (strcmp(argv[arg], "-pathNormative") ==0) {
      arg++;
      pathNormative = argv[arg];
    }

    arg++;
  }

    std::cerr << "FEMAP58-LU: HI\n";
  
  if (filenameBIM == 0 ||
      filenameEDP == 0 ||
      filenameLOSS == 0 ||
      fileSettings == 0 ||
      pathNormative == 0 ||
      pathCurves == 0) {

    std::cerr << "FEMAP58-LU: missing args\n";
    exit(-1);
  }

  HazusLossEstimator * le = new HazusLossEstimator(fileSettings, pathCurves, pathNormative);

  le->determineLOSS(filenameBIM,filenameEDP,filenameLOSS);

  delete le;
  return 0;
}
