#ifndef HAZUS_LOSS_ESTIMATOR_H
#define HAZUS_LOSS_ESTIMATOR_H

#include <string>
#include <map>

using namespace std;

class HazusLossEstimator
{
public:
  HazusLossEstimator();
  ~HazusLossEstimator();
  
  int createEDP(const char *filenameBIM, 
		const char *filenameEVENT,
		const char *filenameSAM,
		const char *filenameEDP);

  int determineLOSS(const char *filenameBIM, 
		    const char *filenameEVENT,
		    const char *filenameSAM,
		    const char *filenameEDP,
		    const char *filenameLOSS);
  
 private:

};

#endif // HAZUS_LOSS_ESTIMATOR
