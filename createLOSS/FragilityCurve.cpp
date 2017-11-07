#include "FragilityCurve.h"

using namespace tinyxml2;

FragilityCurve::FragilityCurve()
{
}

void FragilityCurve::LoadFragility()
{
    XMLDocument doc;
    string path="data/ATCCurves/";
    path=path+ID+".xml";
    if(doc.LoadFile(path.c_str())!=0)
    {
        cout<<"Failed to load fragility of "<<ID.c_str();
        exit(1);
    }
    XMLElement * root = doc.FirstChildElement( "FragilityCurve" );

    const char* text1=root->FirstChildElement( "UseEDPValueOfFloorAbove" )->GetText();
    if(!strncmp(text1,"true",4)||!strncmp(text1,"1",1))
        useEDPValueOfFloorAbove=true;
    else
        useEDPValueOfFloorAbove=false;

    const char* text2= root->FirstChildElement( "Correlation" )->GetText();
    if(!strncmp(text2,"true",4)||!strncmp(text2,"1",1))
        correlation=true;
    else
        correlation=false;

    const char* text3= root->FirstChildElement( "Directional" )->GetText();
    if(!strncmp(text3,"true",4)||!strncmp(text3,"1",1))
        directional=true;
    else
        directional=false;

    const char* text5= root->FirstChildElement( "Incomplete" )->GetText();
    if(!strncmp(text5,"true",4)||!strncmp(text5,"1",1))
    {
        incomplete=true;
        cout<<"Warning: "<<ID.c_str()<<" needs user-defined data\n";
    }
    else
        incomplete=false;

    name= root->FirstChildElement( "Name" )->GetText();
    description= root->FirstChildElement( "Description" )->GetText();

    const char * text4= root->FirstChildElement( "EDPType" )->FirstChildElement( "TypeName" )->GetText();
    if(!strncmp(text4,"Story Drift Ratio",17))
        edp_type=story_drift_ratio;
    else if(!strncmp(text4,"Acceleration",12))
        edp_type=peak_floor_acceleration;
    else if(!strncmp(text4,"Effective Drift",15))
    {
        cout<<"Warning: Unable to deal with EDP type '"<<text4<<"' - "<<ID.c_str()<<", using 'Story Drift Ratio' instead.\n";
        edp_type=story_drift_ratio;
    }
    else
        cout<<"Warning: Unknown EDP type '"<<text4<<"' - "<<ID.c_str()<<"\n";

    XMLElement * node = root->FirstChildElement( "DamageStates" );
//    char * grouptype = node->FirstChildElement("DSGroupType")->GetText();
    dsGroups.clear();
//    if(grouptype=="Sequential")
//    {
        XMLElement * dsNode = node->FirstChildElement( "DamageState" );
        while(dsNode!=NULL)
        {
            DSGroup dsg;
            dsg.dstates.clear();
            dsg.median=toDoubleofXML(dsNode,"Median");
            dsg.beta=toDoubleofXML(dsNode,"Beta");

            XMLElement * dssNode = dsNode->FirstChildElement( "DamageStates" );
            if(dssNode==NULL)   //This group only contains 1 ds. e.g.the 0th group in Seq(DS1,DS2,MutEx(DS3,DS4))
            {
                dsg.dsType=sequential;
                dsg.dstates.resize(1);
                dsg.dstates[0]=_SetDamageState(dsNode);
            }
            else    //This group contains >=2 ds. e.g.the 2th group in Seq(DS1,DS2,MutEx(DS3,DS4))
            {
                const char * grouptype = dssNode->FirstChildElement("DSGroupType")->GetText();
                if(!strncmp(grouptype,"MutuallyExclusive",17))
                    dsg.dsType=mutually_exclusive;
                else
                    dsg.dsType=simultaneous;
                XMLElement * leaf = dssNode->FirstChildElement( "DamageState" );
                while(leaf!=NULL)
                {
                    DamageState ds=_SetDamageState(leaf);
                    dsg.dstates.push_back(ds);
                    leaf=leaf->NextSiblingElement();
                }
            }
            dsGroups.push_back(dsg);
            dsNode=dsNode->NextSiblingElement();
        }

//    }
//    else    //mutually_exclusive or simultaneous. Only one group
//    {
//        dsGroups.resize(1);
//        if(grouptype=="MutuallyExclusive")
//            dsGroups[0].dsType=mutually_exclusive;
//        else
//            dsGroups[0].dsType=simultaneous;
//        dsGroups[0].dstates.clear();
//        XMLElement * dsNode = node->FirstChildElement( "DamageState" );
//        while(dsNode!=NULL)
//        {
//            DamageState ds=_SetDamageState(dsNode);
//            dsGroups[0].dstates.push_back(ds);
//            dsNode=dsNode->NextSiblingElement();
//        }
//    }

    component_type=JudgeComponentType();

}



FragilityCurve::ComponentType FragilityCurve::JudgeComponentType()
{
    ComponentType ct;
    //TODO
    return ct;
}

FragilityCurve::DamageState FragilityCurve::_SetDamageState(XMLElement * dsNode)
{
    const double inflation_rate=0.969;  //adjust the 2011 US$ into 2010 value. http://www.usinflationcalculator.com/
    DamageState ds;
    ds.percent=toDoubleofXML(dsNode,"Percent");
    const char *tag=dsNode->FirstChildElement( "ConsequenceGroup" )->FirstChildElement("TagState")->GetText();
    if(!strncmp(tag,"Red",3))
        ds.tagState=red;
    else
        ds.tagState=none;
    ds.redTagMedian=toDoubleofXML(dsNode->FirstChildElement( "ConsequenceGroup" ),"RedTagMedian");
    ds.redTagBeta=toDoubleofXML(dsNode->FirstChildElement( "ConsequenceGroup" ),"RedTagBeta");
    XMLElement * cost = dsNode->FirstChildElement( "ConsequenceGroup" )->FirstChildElement( "CostConsequence" );
    ds.cost.lowerQuantity=toDoubleofXML(cost,"LowerQuantity");
    ds.cost.maxAmount=toDoubleofXML(cost,"MaxAmount")*inflation_rate;
    ds.cost.upperQuantity=toDoubleofXML(cost,"UpperQuantity");
    ds.cost.minAmount=toDoubleofXML(cost,"MinAmount")*inflation_rate;
    ds.cost.uncertainty=toDoubleofXML(cost,"Uncertainty");
    const char *text=cost->FirstChildElement("CurveType")->GetText();
    if(!strncmp(text,"Normal",6))
        ds.cost.curve_type=normal;
    else
        ds.cost.curve_type=lognormal;

    XMLElement * time = dsNode->FirstChildElement( "ConsequenceGroup" )->FirstChildElement( "TimeConsequence" );
    ds.time.lowerQuantity=toDoubleofXML(time,"LowerQuantity");
    ds.time.maxAmount=toDoubleofXML(time,"MaxAmount");
    ds.time.upperQuantity=toDoubleofXML(time,"UpperQuantity");
    ds.time.minAmount=toDoubleofXML(time,"MinAmount");
    ds.time.uncertainty=toDoubleofXML(time,"Uncertainty");
    const char *text2=time->FirstChildElement("CurveType")->GetText();
    if(!strncmp(text2,"Normal",6))
        ds.time.curve_type=normal;
    else
        ds.time.curve_type=lognormal;

    return ds;
}

double FragilityCurve::toDoubleofXML(XMLElement * node,string s)
{
    XMLElement * child=node->FirstChildElement(s.c_str());
    if(child)
    {
        return atof(child->GetText());
    }
    return 0.0;
}
