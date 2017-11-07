#ifndef NORMATIVEQTYSTR_H
#define NORMATIVEQTYSTR_H

#include "Building.h"
#include <map>

class NormativeQtyStr
{
public:
    NormativeQtyStr();

    string fid; //fragility ID. e.g., B1035.011a
    double unit;
    //map<Building::StruType, double> q10;     //different building StruType
    map<Building::StruType, double> median;
    //map<Building::StruType, double> q90;
    map<Building::StruType, double> beta;   //dispersion

};

#endif // NORMATIVEQTYSTR_H
