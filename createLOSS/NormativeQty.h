#ifndef NORMATIVEQTY_H
#define NORMATIVEQTY_H

#include "Building.h"
#include <map>

class NormativeQty
{
public:
    NormativeQty();

    string fid; //fragility ID. e.g., B1035.011a
    double unit;
    //map<Building::BldgOccupancy, double> q10;     //different building occupancies
    map<Building::BldgOccupancy, double> median;
    //map<Building::BldgOccupancy, double> q90;
    map<Building::BldgOccupancy, double> beta;   //dispersion


};

#endif // NORMATIVEQTY_H
