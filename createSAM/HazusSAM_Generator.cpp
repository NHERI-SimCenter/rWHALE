#include "HazusSAM_Generator.h"

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include "Building.h"
#include <cmath>
using namespace std;

#include <jansson.h>  // for Json

HazusSAM_Generator::HazusSAM_Generator()
{
    ReadHazusData();
}

HazusSAM_Generator::~HazusSAM_Generator()
{
    delete []hazus;
}


void HazusSAM_Generator::ReadHazusData()
{
    //Load Hazus database
    const int nCodeLevel=4;
    const int nBldgType=36;
    hazus = new map<string, HazusData>[nCodeLevel];

    string temps="";
    string path="data/HazusData.txt";
    ifstream fHazus(path.c_str());
    if( !fHazus.is_open() )
    {
        cout << "Error opening file "<<path.c_str()<<"!\n";
        exit(1);
    }
    for (int i=0;i<nCodeLevel;++i)
    {
        getline(fHazus,temps);
        for(int j=0;j<nBldgType;++j)
        {
            string type="";
            HazusData hd;
            fHazus>>temps>>type>>hd.lowlmt>>hd.highlmt;
            //fHazus>>temps>>hazus[i][j].name>>hazus[i][j].lowlmt>>hazus[i][j].highlmt;
            for (int k=0;k<10;k++)
            {
                fHazus>>hd.Props[k];
            }
            for (int k=0;k<4;k++)
            {
                fHazus>>hd.damage[k];
            }
            fHazus>>hd.T1>>hd.T2>>hd.hos>>hd.damp;
            hazus[i].insert(pair<string,HazusData>(type,hd));
        }
    }
    fHazus.close();
}

void HazusSAM_Generator::CalcBldgPara(Building *bldg)
{
    const double PI=3.14159265358979;
    const double UNIT_MASS=1000.0;
    string strucType=bldg->GetHazusType();

    bldg->interstoryParams.clear();
    bldg->floorParams.clear();
    bldg->interstoryParams.resize(bldg->nStory);
    bldg->floorParams.resize(bldg->nStory);

    //Determine seismic design level
    // (Need further improvement: does not include low-code, does not consider seismic zone)
    int codelevel=0;
    if(1973<bldg->year)
        codelevel=0;    //high-code
    else if (1941<=bldg->year && bldg->year<=1973)
        codelevel=1;    //moderate-code
    else
        codelevel=3;    //pre-code

    bldg->dampingRatio=hazus[codelevel][strucType].damp;


    /*
    for (int i=0;i<4;i++)
    {
        bldg->damageCriteria[i]=hazus[codelevel][strucType].damage[i];
    }
    */
    bldg->T0=bldg->nStory*hazus[codelevel][strucType].T1;
    bldg->T2=bldg->T0*hazus[codelevel][strucType].T2;
    for (int i=0;i<bldg->nStory;++i)
    {
        bldg->floorParams[i].floor=i+1;
        bldg->floorParams[i].mass=bldg->area*UNIT_MASS;
        bldg->interstoryParams[i].story=i+1;
        bldg->interstoryParams[i].K0=4.0*PI*PI*bldg->lambda(bldg->nStory)*bldg->floorParams[i].mass/bldg->T0/bldg->T0;

        double r = 1.0-double(i+1)*double(i)/double(bldg->nStory)/double(bldg->nStory+1);
        bldg->interstoryParams[i].Sy=hazus[codelevel][strucType].Props[1]*bldg->floorParams[i].mass*9.8*double(bldg->nStory)*r;
        bldg->interstoryParams[i].eta=hazus[codelevel][strucType].Props[2];
        bldg->interstoryParams[i].C=hazus[codelevel][strucType].Props[3];
        bldg->interstoryParams[i].gamma=hazus[codelevel][strucType].Props[4];
        bldg->interstoryParams[i].eta_soft=hazus[codelevel][strucType].Props[5];
        bldg->interstoryParams[i].alpha=hazus[codelevel][strucType].Props[6];
        bldg->interstoryParams[i].beta=hazus[codelevel][strucType].Props[7];
        bldg->interstoryParams[i].a_k=hazus[codelevel][strucType].Props[8];
        bldg->interstoryParams[i].omega=hazus[codelevel][strucType].Props[9];
    }
}
