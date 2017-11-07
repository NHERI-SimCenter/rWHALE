#ifndef BUILDING_H
#define BUILDING_H

#include <string>
#include <vector>
#include <algorithm>
#include <jansson.h>
#include <fstream>

#include "Component.h"
#include "FragilityCurve.h"
using namespace std;

class Building
{
public:


    enum BldgOccupancy{office, education, healthcare, hospitality, residence, retail, warehouse, research, unknown};
    enum StruType{RM1, RM2, URM, C1, C2, C3, W1, W2, S1, S2, S3, S4, S5, PC1, PC2, MH, UNKNOWN}; //Hazus structural type
    //RM: reinforced Masonry. URM: unrefinforced Masonry. C1: RC frame...
    StruType s2StruType(string s);
    BldgOccupancy s2BldgOccupancy(string s);

    struct EDP{     //engineering demand parameters
        vector<double> IDR; // Inter-story drift ratio. size = nStory
        vector<double> PFA; // Peak floor acceleration. size = nStory+1
        double residual;    // residual drift
        vector<double> PFV; // Peak floor velocity. size = nStory
        //vector<double> rotation; // rotation. size = nStory
    };


    Building();
    void readBIM(const char *path);
    //void readEDP(const char *path);
    void readEDP(const char *path,int resp_index=0);
    int getNumResponses(const char *path);

    //==========basic parameters===============================
    int id;
    string name;
    StruType strutype;
    int year;
    BldgOccupancy occupancy;
    int nStory;
    double storyheight;   //unit: m
    double area;		//story area. unit: m^2
    double replacementCost;	//unit: $
    double replacementTime; //unit: day
    int workers;    //number of workers to repair the building.


    //==========economic loss===============================
    double residualMedian=0.0125;					//Median value of residual drift Curve
    double residualDispersion=0.3;              	//Dispersion of residual drift Curve (lognormal distribution)
    vector <vector <Component> > components;	// components[i][j]: jth component at ith story. 0<=i<=nStory. Roof is the last story
    EDP edp;
    vector<double> totalLoss;		//size=number of realizations
    double totalLossMedian;

    //==========downtime===============================
    vector<double> totalDowntime;		//size=number of realizations
    double totalDowntimeMedian;

    //==========Placards===============================
    vector<FragilityCurve::Tag> redTag;		//size=number of realizations
    double redTagProb;   //Probability that this building is red tagged (unsafe)

private:

};

#endif // BUILDING_H
