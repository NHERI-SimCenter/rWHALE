/*-----------------------------------------------
This program uses MDOF shear models to simulate building seismic behaviors.
For the theory details, please refer to the published paper:
    Lu XZ, Han B, Hori M, Xiong C, Xu Z, A coarse-grained parallel approach for seismic damage simulations of urban areas based on refined models and GPU/CPU cooperative computing. Advances in Engineering Software, 2014, 70: 90-103.
-----------------------------------------------*/

#ifndef HAZUS_SAM_GENERATOR_H
#define HAZUS_SAM_GENERATOR_H

#include <string>
#include <map>

using namespace std;

class Building;

class HazusSAM_Generator
{
public:
  struct HazusData {
    int lowlmt,highlmt;	  //low story, high story
    double Props[10];	  //10-parameter hysteretic model.
    double damage[4];	  //damage criteria
    double T1,T2,damp;	  //T1=T0/N  (T0:foundamental period, N: number of stories)
                          // T2=0.333
    double hos;		  //story height
  };
  
  HazusSAM_Generator();
  ~HazusSAM_Generator();
  
  void CalcBldgPara(Building *bldg);
  
 private:
  map<string, HazusData> *hazus;
  void ReadHazusData();       //Load Hazus database
};

#endif // HAZUS_SAM_GENERATOR
