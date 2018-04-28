/*-----------------------------------------------
This program uses FEMA P-58 method to simulate building seismic loss.
For the theory details, please refer to the published paper:
    Zeng X, Lu XZ, Yang TY, Xu Z. Application of the FEMA-P58 methodology for regional earthquake loss prediction. Natural Hazards, 2016, 83(1): 177-192.
-----------------------------------------------*/

#ifndef HAZUS_LOSS_ESTIMATOR_H
#define HAZUS_LOSS_ESTIMATOR_H

#include "Building.h"
#include "FragilityCurve.h"
#include "NormativeQty.h"
#include "NormativeQtyStr.h"
#include "Stat.h"
#include "include/inifile.h"
#include "include/csv.h"


#include <map>
#include <iostream>
#include <fstream>

using namespace std;

class HazusLossEstimator
{
public:
  HazusLossEstimator(const char *fileHazusData =0, const char *fragilityCurvesPath = 0, const char *pathNormative = 0);
  ~HazusLossEstimator();

    int determineLOSS(const char *filenameBIM,
                      const char *filenameEDP,
                      const char *filenameLOSS);

private:
    Stat * stat;  //statistical functions
    struct qrepair{
        vector<double> q_ds;   //total quantity of damaged components (each damage states). size = number of damage states
        double q;   //total quantity
    };
    map<string,qrepair> _qRepair;    //quantities of each kind of component to be repaired

    int nor=1000;   //number of realizations
    bool calc_collapse=false;     // consider or not consider collapse effects
    bool calc_residual=false;     //consider or not consider irreparable deformation described using residual drift
    double beta_m=0.0;      //modeling_uncertainty
    double beta_gm=0.0;     //ground_motion_uncertainty
    double max_worker_per_square_meter=0.010763;    //for downtime calculation
    map<string,FragilityCurve> fragilityLib;
    map<string,NormativeQty>    normativeQtyLib;    //nonstructural and contents
    map<string,NormativeQtyStr> normativeQtyStrLib; //structural

    int _Init();
    int _LoadNormativeQty();    //Load normativeQtyLib

    void _AutoCalcTotalDowntime(Building *bldg);
    void _AutoCalcTotalValueAndDowntime(Building *bldg);
    void _AutoGenComponents(Building *bldg);
    void _CalcBldgConseqScenario(Building *bldg);   //only one ground motion input
    void _CalcComponentDamage(Component *cpn,int currRealization, double edp);
    void _CalcComponentConseq(Component *cpn,int currRealization);
    double _SimulateConseq(double q,double q_total, const FragilityCurve::ConsequenceCurve * curve); //simulate consequence, given a quantity and a consequence curve
    FragilityCurve::Tag _SimulateBldgTag();
    void _GenRealizations(Building *bldg);
    void _SimulateDS(Component *cpn, double edp);    //simulate damage state, given an edp

 private:
  string pathHazusData;
  string fragilityCurvesPath;
  string normativePath;

};


#endif // HAZUS_LOSS_ESTIMATOR_H
