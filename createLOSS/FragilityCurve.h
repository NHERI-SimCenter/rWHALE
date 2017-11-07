#ifndef FRAGILITYCURVE_H
#define FRAGILITYCURVE_H

#include <iostream>
#include <vector>
#include "include/tinyxml2.h"
using namespace std;
using namespace tinyxml2;

class FragilityCurve
{
public:
    FragilityCurve();
    enum DamageStateType{sequential, mutually_exclusive, simultaneous};
    enum EDPType{story_drift_ratio,peak_floor_acceleration};
    enum CurveType{normal, lognormal};
    enum ComponentType{Str,NSD,NSA,Content};
    enum Tag{none,red};

    struct ConsequenceCurve{
        double lowerQuantity;
        double maxAmount;
        double upperQuantity;
        double minAmount;
        double uncertainty;
        CurveType curve_type;
    };

    struct DamageState{
        double  percent;
        ConsequenceCurve cost;
        ConsequenceCurve time;
        Tag tagState;
        double redTagMedian;
        double redTagBeta;
    };


    // Seq(MutEx(DS1,DS2),MutEx(DS3,DS4),DS5); Simul(DS1,DS2,DS3); Seq(DS1,DS2); MutEx(DS1,DS2); Seq(DS1,MutEx(DS2,DS3)); ...
    // "Seq(DS1,DS2)" is treated as "Seq(Seq(DS1),Seq(DS2))"; MutEx(DS1,DS2) as Seq(MutEx(DS1,DS2));
    struct DSGroup{
        double median;
        double beta;
        DamageStateType dsType;
        vector<DamageState> dstates;
    };

    string ID;
    bool useEDPValueOfFloorAbove;
    bool correlation;
    bool directional;
    bool incomplete;


    string name;
    string description;
    EDPType edp_type;
    ComponentType component_type;
    vector<DSGroup> dsGroups;


    void LoadFragility();
    ComponentType JudgeComponentType();
    //double _CalcConseq(double q, ConsequenceCurve * curve); //simulate consequence, given a quantity and a consequence curve


private:
    double toDoubleofXML(XMLElement * node,string s);
    DamageState _SetDamageState(XMLElement * dsNode);

};

#endif // FRAGILITYCURVE_H
