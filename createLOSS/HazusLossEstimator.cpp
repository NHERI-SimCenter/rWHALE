#include "HazusLossEstimator.h"


HazusLossEstimator::~HazusLossEstimator()
{
    delete stat;
}

HazusLossEstimator::HazusLossEstimator()
{
    stat=new Stat();

    fragilityLib.clear();
    normativeQtyLib.clear();
    normativeQtyStrLib.clear();

    _Init();
    _LoadNormativeQty();
}

int HazusLossEstimator::determineLOSS(const char *filenameBIM,
				      const char *filenameEDP,
				      const char *filenameLOSS)
{
    Building * bldg= new Building();
    bldg->readBIM(filenameBIM);

    _GenRealizations(bldg);

    int numResponses=bldg->getNumResponses(filenameEDP);
    vector<double> vLoss,vRpTime,vTagProb;
    for(int i=0;i<numResponses;++i)
    {
        bldg->readEDP(filenameEDP,i);
        _CalcBldgConseqScenario(bldg);

        vLoss.insert(vLoss.end(),bldg->totalLoss.begin(),bldg->totalLoss.end());
        vRpTime.insert(vRpTime.end(),bldg->totalDowntime.begin(),bldg->totalDowntime.end());
        vTagProb.push_back(bldg->redTagProb);
    }


    double lossratio=stat->getMedian(vLoss)/bldg->replacementCost;
    json_t *root = json_object();
    json_t *dl = json_object();
    json_object_set(dl,"MedianLossRatio",json_real(lossratio));
    json_object_set(dl,"MedianRepairCost",json_real(stat->getMedian(vLoss)));
    json_object_set(dl,"MeanRepairCost",json_real(stat->getMean(vLoss)));
    json_object_set(dl,"StdRepairCost",json_real(stat->getStd(vLoss)));
    json_object_set(dl,"10PercentileLoss",json_real(stat->getPercentile(vLoss,10)));
    json_object_set(dl,"90PercentileLoss",json_real(stat->getPercentile(vLoss,90)));

    json_object_set(root,"EconomicLoss",dl);
    json_t *repairTime = json_object();
    json_object_set(repairTime,"MedianRepairTime",json_real(stat->getMedian(vRpTime)));
    json_object_set(root,"RepairTime",repairTime);
    json_t *tag = json_object();
    if(stat->getMean(vTagProb)>0.5)
        json_object_set(tag,"Tag",json_string("red"));
    else
        json_object_set(tag,"Tag",json_string("none"));
    json_object_set(tag,"RedTagProbability",json_real(stat->getMean(vTagProb)));
    json_object_set(root,"UnsafePlacards",tag);


    json_object_set(root,"Name",json_string(bldg->name.c_str()));
    json_object_set(root,"MaxPGA",json_real(bldg->edp.PFA[0]));

    json_dump_file(root,filenameLOSS,0);
    json_object_clear(root);

    delete bldg;
    return 0;
}

int HazusLossEstimator::_Init()
{
    inifile::IniFile ini;
    if(ini.load("data/settings.ini")!=0)
    {
        cout<<"Failed to open settings.ini file!\n";
        return -1;
    }
    int ret=0;  //return value of the ini methods. 0==OK, -1==error
    int value_i=0;
    value_i=ini.getIntValue("analysis","num_realizations",ret);
    if(ret==0)
        nor=value_i;
    int seed=ini.getIntValue("analysis","seed",ret);
    stat->SetSeed(seed);
    if(ini.getIntValue("analysis","calc_collapse",ret)!=0)
        calc_collapse=true;
    else
        calc_collapse=false;
    if(ini.getIntValue("analysis","calc_residual",ret)!=0)
        calc_residual=true;
    else
        calc_residual=false;

    double value_d=0.0;
    value_d=ini.getDoubleValue("parameters","modeling_uncertainty",ret);
    if(ret==0)
        beta_m=value_d;
    value_d=ini.getDoubleValue("parameters","ground_motion_uncertainty",ret);
    if(ret==0)
        beta_gm=value_d;

    value_d=ini.getDoubleValue("parameters","max_worker_per_square_meter",ret);
    if(ret==0 && value_d!=0)
        max_worker_per_square_meter=value_d;

    return 0;
}

int HazusLossEstimator::_LoadNormativeQty()
{
    //nonstructural and contents
    string path="";
    string pathStr="";

    path="data/NormativeQty.csv";
    pathStr="data/NormativeQtyStr.csv";

    try
    {
        io::CSVReader<18> in(path);
        in.next_line();
        in.read_header(io::ignore_extra_column, "fid", "unit", "q1", "beta1",
                       "q2", "beta2", "q3", "beta3", "q4", "beta4", "q5", "beta5",
                       "q6", "beta6", "q7", "beta7", "q8", "beta8");
        string fid;
        double unit,q1,beta1,q2,beta2,q3,beta3,q4,beta4,q5,beta5,q6,beta6;
        double q7,beta7,q8,beta8;
        while(in.read_row(fid, unit, q1,beta1,q2,beta2,q3,beta3,q4,beta4,
                          q5,beta5,q6,beta6,q7,beta7,q8,beta8))
        {
            NormativeQty nq;
            nq.fid=fid;
            nq.unit=unit;
            nq.median.insert(make_pair(Building::office,q1));
            nq.beta.insert(make_pair(Building::office,beta1));
            nq.median.insert(make_pair(Building::education,q2));
            nq.beta.insert(make_pair(Building::education,beta2));
            nq.median.insert(make_pair(Building::healthcare,q3));
            nq.beta.insert(make_pair(Building::healthcare,beta3));
            nq.median.insert(make_pair(Building::hospitality,q4));
            nq.beta.insert(make_pair(Building::hospitality,beta4));
            nq.median.insert(make_pair(Building::residence,q5));
            nq.beta.insert(make_pair(Building::residence,beta5));
            nq.median.insert(make_pair(Building::retail,q6));
            nq.beta.insert(make_pair(Building::retail,beta6));
            nq.median.insert(make_pair(Building::warehouse,q7));
            nq.beta.insert(make_pair(Building::warehouse,beta7));
            nq.median.insert(make_pair(Building::research,q8));
            nq.beta.insert(make_pair(Building::research,beta8));

            normativeQtyLib.insert(make_pair(fid,nq));

            if(fragilityLib.find(fid)==fragilityLib.end())
            {
                FragilityCurve fc;
                fc.ID=fid;
                fc.LoadFragility();
                fragilityLib.insert(make_pair(fid,fc));
            }
        }
    }
    catch (io::error::can_not_open_file e)
    {
        cout<<e.what();
        return -1;
    }

    //structural
    try
    {
        io::CSVReader<12> in(pathStr);
        in.next_line();
        in.read_header(io::ignore_extra_column, "fid", "unit", "q1", "beta1",
                       "q2", "beta2", "q3", "beta3", "q4", "beta4", "q5", "beta5");
        string fid;
        double unit,q1,beta1,q2,beta2,q3,beta3,q4,beta4,q5,beta5;
        while(in.read_row(fid, unit, q1,beta1,q2,beta2,q3,beta3,q4,beta4,
                          q5,beta5))
        {
            NormativeQtyStr nq;
            nq.fid=fid;
            nq.unit=unit;
            nq.median.insert(make_pair(Building::W1,q1));
            nq.beta.insert(make_pair(Building::W1,beta1));
            nq.median.insert(make_pair(Building::W2,q1));
            nq.beta.insert(make_pair(Building::W2,beta1));
            nq.median.insert(make_pair(Building::RM1,q2));
            nq.beta.insert(make_pair(Building::RM1,beta2));
            nq.median.insert(make_pair(Building::RM2,q2));
            nq.beta.insert(make_pair(Building::RM2,beta2));
            nq.median.insert(make_pair(Building::URM,q2));
            nq.beta.insert(make_pair(Building::URM,beta2));
            nq.median.insert(make_pair(Building::C1,q3));
            nq.beta.insert(make_pair(Building::C1,beta3));
            nq.median.insert(make_pair(Building::C2,q4));
            nq.beta.insert(make_pair(Building::C2,beta4));
            nq.median.insert(make_pair(Building::C3,q4));
            nq.beta.insert(make_pair(Building::C3,beta4));
            nq.median.insert(make_pair(Building::S1,q5));
            nq.beta.insert(make_pair(Building::S1,beta5));
            nq.median.insert(make_pair(Building::S2,q5));
            nq.beta.insert(make_pair(Building::S2,beta5));
            nq.median.insert(make_pair(Building::S3,q5));
            nq.beta.insert(make_pair(Building::S3,beta5));
            nq.median.insert(make_pair(Building::S4,q5));
            nq.beta.insert(make_pair(Building::S4,beta5));
            nq.median.insert(make_pair(Building::S5,q5));
            nq.beta.insert(make_pair(Building::S5,beta5));
            nq.median.insert(make_pair(Building::PC1,q4));
            nq.beta.insert(make_pair(Building::PC1,beta4));
            nq.median.insert(make_pair(Building::PC2,q4));
            nq.beta.insert(make_pair(Building::PC2,beta4));

            normativeQtyStrLib.insert(make_pair(fid,nq));

            if(fragilityLib.find(fid)==fragilityLib.end())
            {
                FragilityCurve fc;
                fc.ID=fid;
                fc.LoadFragility();
                fragilityLib.insert(make_pair(fid,fc));
            }
        }
    }
    catch (io::error::can_not_open_file e)
    {
        cout<<e.what();
        return -1;
    }

    return 0;
}

void HazusLossEstimator::_GenRealizations(Building *bldg)
{
    bldg->totalLoss.resize(nor);
    bldg->totalDowntime.resize(nor);
    bldg->redTag.resize(nor);
    _AutoGenComponents(bldg);

    for(int i=0;i<=bldg->nStory;++i)
    {
        for(unsigned int j=0;j<bldg->components[i].size();++j)
        {
            bldg->components[i][j].q.resize(nor);
            double median=bldg->components[i][j].qmedian;
            double beta=bldg->components[i][j].qbeta;
            for(int k=0;k<nor;k++)
            {
                if(beta==0.0||median==0.0)
                    bldg->components[i][j].q[k]=median;
                else
                    bldg->components[i][j].q[k]=exp(stat->gaussrand(log(median),beta));
            }
            bldg->components[i][j].loss.resize(nor);
            bldg->components[i][j].downtime.resize(nor);
        }
    }

    bldg->workers=(int)(bldg->area*this->max_worker_per_square_meter);
    if(bldg->occupancy==Building::residence && bldg->nStory<=2)    //single family house
        bldg->workers=(int)(bldg->area*0.003)+1;
    //if(bldg->replacementTime<=0)
    //_AutoCalcTotalValueAndDowntime(bldg);
    _AutoCalcTotalValueAndDowntime(bldg);
}

void HazusLossEstimator::_AutoCalcTotalValueAndDowntime(Building *bldg)
{
    bldg->replacementTime=0;
    bldg->replacementCost=0.0;
    for (int i=0;i<=bldg->nStory;++i)
    {
        double currFloorDowntime=0.0;
        for(unsigned int j=0;j<bldg->components[i].size();++j)
        {
            const FragilityCurve * fc=&fragilityLib[bldg->components[i][j].ID];
            double unitDowntime=fc->dsGroups.back().dstates.back().time.maxAmount;
            double currPGDowntime=unitDowntime*bldg->components[i][j].qmedian;
            double unitCost=fc->dsGroups.back().dstates.back().cost.maxAmount;
            bldg->replacementCost+=unitCost*bldg->components[i][j].qmedian;
            cout<<"\n"<<bldg->components[i][j].ID<<"\t"<<unitCost*bldg->components[i][j].qmedian;
            currPGDowntime=currPGDowntime/bldg->workers;
            currFloorDowntime+=currPGDowntime;
        }
        if(bldg->replacementTime<currFloorDowntime)
            bldg->replacementTime=currFloorDowntime;
    }
}

void HazusLossEstimator::_AutoGenComponents(Building *bldg)
{
    bldg->components.resize(bldg->nStory+1);
    //structural
    for(map<string,NormativeQtyStr>::iterator it=normativeQtyStrLib.begin();
        it!=normativeQtyStrLib.end();it++)
    {
        Component cmp;
        cmp.ID=it->first;
        double q=it->second.median[bldg->strutype];
        if(cmp.ID=="B1044.091"||cmp.ID=="B1052.011")    //qmedian = vertical area
        {
            cmp.qmedian=q*bldg->area*bldg->storyheight/it->second.unit;
        }
        else if(cmp.ID=="B1071.001")    //TODO lack of statistics
        {
            cmp.qmedian=q*sqrt(bldg->area)*2.0*bldg->storyheight/it->second.unit;
        }
        else
        {
            cmp.qmedian=q*bldg->area/it->second.unit;
        }
        cmp.qbeta=it->second.beta[bldg->strutype];
        for(int i=0;i<bldg->nStory;i++)
            bldg->components[i].push_back(cmp);
    }

    //non-structural
    for(map<string,NormativeQty>::iterator it=normativeQtyLib.begin();
        it!=normativeQtyLib.end();it++)
    {
        Component cmp;
        cmp.ID=it->first;
        double q=it->second.median[bldg->occupancy];
        cmp.qmedian=q*bldg->area/it->second.unit;
        cmp.qbeta=it->second.beta[bldg->occupancy];
        if(cmp.ID=="B3011.011") //roof
            bldg->components[bldg->nStory].push_back(cmp);
        else
        {
            for(int i=0;i<bldg->nStory;i++)
                bldg->components[i].push_back(cmp);
        }
    }

}


void HazusLossEstimator::_CalcBldgConseqScenario(Building *bldg)
{
    for(int currRealization=0;currRealization<nor;++currRealization)
    {
        //Judge whether the house is collapsed
        if(bldg->edp.IDR[0]<0)
        {
            bldg->totalLoss[currRealization]=bldg->replacementCost;
            bldg->totalDowntime[currRealization]=bldg->replacementTime;
            bldg->redTag[currRealization]=FragilityCurve::red;
            continue;
        }

        double randNum=stat->random();
        double maxResidual=bldg->edp.residual;
        if (maxResidual<0.000001)
        {
            maxResidual=0.000001;
        }
        double repairProb=stat->CDF_normal(log(maxResidual),log(bldg->residualMedian),bldg->residualDispersion);
        if (randNum<repairProb&&calc_residual) //simulation result: irreparable due to large residual deformation
        {
            bldg->totalLoss[currRealization]=bldg->replacementCost;
            bldg->totalDowntime[currRealization]=bldg->replacementTime;
            bldg->redTag[currRealization]=FragilityCurve::red;
            continue;
        }

        //compute economic loss and downtime component-by-compunent
        bldg->totalLoss[currRealization]=0.0;
        bldg->totalDowntime[currRealization]=0.0;
        bldg->redTag[currRealization]=FragilityCurve::none;
        for (int i=0;i<=bldg->nStory;++i)
        {
            for(unsigned int j=0;j<bldg->components[i].size();++j)
            {
                bldg->components[i][j].loss[currRealization]=0.0;
                bldg->components[i][j].downtime[currRealization]=0.0;
            }
        }
        _qRepair.clear();
        for (int i=0;i<=bldg->nStory;++i)
        {
            for(unsigned int j=0;j<bldg->components[i].size();++j)
            {
                FragilityCurve *fc=&fragilityLib[bldg->components[i][j].ID];
                double edp=0.0;
                int edpFloor=i+(int)fc->useEDPValueOfFloorAbove;
                switch(fc->edp_type)
                {
                case FragilityCurve::story_drift_ratio:
                    if (edpFloor<bldg->nStory)
                        edp=bldg->edp.IDR[edpFloor];
                    else
                        edp=bldg->edp.IDR[bldg->nStory-1];    //avoid overflow
                    break;
                case FragilityCurve::peak_floor_acceleration:
                    if(edpFloor>bldg->nStory)    //avoid overflow
                    {
                        edpFloor=bldg->nStory;
                    }
                    edp=bldg->edp.PFA[edpFloor];
                    edp=edp*1.2/10; //change unit to g; multiply 1.2 for nondirectionals based on FEMA P58
                    break;
                default:
                    break;
                }

                _CalcComponentDamage(&bldg->components[i][j],currRealization,edp);
            }
        }

        for (int i=0;i<=bldg->nStory;++i)
        {
            double currFloorDowntime=0.0;
            for(unsigned int j=0;j<bldg->components[i].size();++j)
            {
                _CalcComponentConseq(&bldg->components[i][j],currRealization);

                double currPGLoss=bldg->components[i][j].loss[currRealization];
                bldg->totalLoss[currRealization]+=currPGLoss;
                double currPGDowntime=bldg->components[i][j].downtime[currRealization];
                currPGDowntime=currPGDowntime/bldg->workers;
                currFloorDowntime+=currPGDowntime;
            }
            if(bldg->totalDowntime[currRealization]<currFloorDowntime)
                bldg->totalDowntime[currRealization]=currFloorDowntime;
        }

        //assumption: repair cost should not be higher than replacement value.
        if (bldg->totalLoss[currRealization]>bldg->replacementCost)
           bldg->totalLoss[currRealization]=bldg->replacementCost-10.0; //-10 to tell them apart
        if (bldg->totalDowntime[currRealization]>bldg->replacementTime)
           bldg->totalDowntime[currRealization]=bldg->replacementTime-1.0; //-1 to tell them apart

        bldg->redTag[currRealization]=_SimulateBldgTag();
    }   //end realization

    bldg->totalLossMedian=stat->getMedian(bldg->totalLoss);
    bldg->totalDowntimeMedian=stat->getMedian(bldg->totalDowntime);

    int nRedTags=0;
    for(int currRealization=0;currRealization<nor;currRealization++)
    {
        if(bldg->redTag[currRealization]==FragilityCurve::red)
            nRedTags++;
    }
    bldg->redTagProb=(double)nRedTags/(double)nor;
}


 void HazusLossEstimator::_CalcComponentDamage(Component *cpn,int currRealization, double edp)
 {
    FragilityCurve * fc=&fragilityLib[cpn->ID];
    cpn->loss[currRealization]=0;
    cpn->downtime[currRealization]=0;
    int nDS=0;    //number of damage states
    for(unsigned int i=0;i<fc->dsGroups.size();++i)
       nDS+=fc->dsGroups[i].dstates.size();

    cpn->q_ds.clear();
    cpn->q_ds.resize(nDS);

    //Calculate component quantities for each damage states
    if (fc->correlation)
    {
        _SimulateDS(cpn,edp);
        for (int i=0;i<nDS;++i)
            cpn->q_ds[i]*=cpn->q[currRealization];
    }
    else
    {
        int intQ=(int)cpn->q[currRealization];
        double doubleQ=cpn->q[currRealization]-intQ;
        if (doubleQ>0)
        {
            _SimulateDS(cpn,edp);
            for (int i=0;i<nDS;++i)
                cpn->q_ds[i]*=doubleQ;
        }

        for (int i=0;i<intQ;++i)
        {
            _SimulateDS(cpn,edp);
        }
    }

    if(_qRepair.find(cpn->ID)==_qRepair.end())
    {
        qrepair qr;
        qr.q=0.0;
        qr.q_ds.resize(cpn->q_ds.size());
        _qRepair.insert(make_pair(cpn->ID,qr));
    }
    _qRepair[cpn->ID].q+=cpn->q[currRealization];
    for(unsigned int i=0;i<cpn->q_ds.size();i++)
        _qRepair[cpn->ID].q_ds[i]+=cpn->q_ds[i];

 }

void HazusLossEstimator::_SimulateDS(Component *cpn, double edp)
{
    if(edp<0.00000001)
        edp=0.00000001;

    double randNum=0.0;
    int nDS=0;    //number of damage states
    FragilityCurve * fc=&fragilityLib[cpn->ID];
    for(unsigned int i=0;i<fc->dsGroups.size();++i)
       nDS+=fc->dsGroups[i].dstates.size();

    //Monte Carlo: simulate which group does the damage belong to
    int GP_flag=-1; //damage state group flag
    randNum=stat->random();
    for (int i=fc->dsGroups.size()-1;i>=0;i--)
    {
        double damageProbNum=stat->CDF_normal(log(edp),log(fc->dsGroups[i].median),fc->dsGroups[i].beta);
        if (randNum<=damageProbNum)
        {
            GP_flag=i;
            break;
        }
    }
    if (GP_flag!=-1)	//Simulate which damage state occurs among those in the group "GP_flag"
    {
        int DS_flag=-1;	//damage state flag
        int startIndex=0;
        for (int i=0;i<GP_flag;++i)
        {
            startIndex+=fc->dsGroups[i].dstates.size();
        }
        if(fc->dsGroups[GP_flag].dstates.size()==1)	//Damage state: sequential
        {
           DS_flag=startIndex;
           cpn->q_ds[DS_flag]++;
        }
        else if(fc->dsGroups[GP_flag].dsType==FragilityCurve::simultaneous)
        {
            bool isDamaged=false;	//simultaneous requires at least 1 damage state.
            while (!isDamaged)
            {
                for (unsigned int i=0; i<fc->dsGroups[GP_flag].dstates.size(); ++i)
                {
                    randNum=stat->random();
                    if (randNum<=fc->dsGroups[GP_flag].dstates[i].percent)
                    {
                        isDamaged=true;
                        DS_flag=startIndex+i;
                        cpn->q_ds[DS_flag]+=1;
                    }
                }
            }
        }
        else 	//Damage state: mutually_exclusive
        {
            double prob=0.0;
            randNum=stat->random();
            for (unsigned int i=0; i<fc->dsGroups[GP_flag].dstates.size(); ++i)
            {
                prob+=fc->dsGroups[GP_flag].dstates[i].percent;
                if (randNum<=prob)
                {
                    DS_flag=startIndex+i;
                    cpn->q_ds[DS_flag]+=1;
                    break;
                }
            }
        }
    }
}

void HazusLossEstimator::_CalcComponentConseq(Component *cpn,int currRealization)
{
    double q_total=0.0;
    for(unsigned int i=0;i<_qRepair[cpn->ID].q_ds.size();i++)
        q_total+=_qRepair[cpn->ID].q_ds[i];

    FragilityCurve * fc=&fragilityLib[cpn->ID];
    //Calculate component economic loss and downtime
    int dsFlag=0;
    for(unsigned int i=0;i<fc->dsGroups.size();++i)
    {
        for(unsigned int j=0;j<fc->dsGroups[i].dstates.size();++j)
        {
            if(cpn->q_ds[dsFlag]>0.0)
            {
                cpn->loss[currRealization]+=_SimulateConseq(cpn->q_ds[dsFlag],q_total,&fc->dsGroups[i].dstates[j].cost);
                //Here the unit of cpn->downtime is worker*day
                cpn->downtime[currRealization]+=_SimulateConseq(cpn->q_ds[dsFlag],q_total,&fc->dsGroups[i].dstates[j].time);
            }
            dsFlag++;
        }
    }
}

double HazusLossEstimator::_SimulateConseq(double q, double q_total,const FragilityCurve::ConsequenceCurve * curve)
{
    double conseq=0.0;

    double MinQty=curve->lowerQuantity;
    double MaxQty=curve->upperQuantity;
    double MinAverage=curve->maxAmount; //a smaller quantity corresponds to a higher repair cost/time.
    double MaxAverage=curve->minAmount;
    double MinDispersion=curve->uncertainty;
    double MaxDispersion=curve->uncertainty;

    double median=0.0,dispersion=0.0;
    if (q_total<=MinQty)
    {
        median=MinAverage;
        dispersion=MinDispersion;
    }
    else if (q_total>=MaxQty)
    {
        median=MaxAverage;
        dispersion=MaxDispersion;
    }
    else	//Linear interpolation
    {
        median=MinAverage+(MaxAverage-MinAverage)*(q_total-MinQty)/(MaxQty-MinQty);
        dispersion=MinDispersion+(MaxDispersion-MinDispersion)*(q_total-MinQty)/(MaxQty-MinQty);
    }


    //reducing standard deviation
    for(int i=0;i<q;i++){
        if(curve->curve_type==FragilityCurve::lognormal)
            conseq+=exp(stat->gaussrand(log(median),dispersion));
        else	//normal distribution
            conseq+=stat->gaussrand(median,dispersion*median);
    }
    if(curve->curve_type==FragilityCurve::lognormal)
        conseq+=exp(stat->gaussrand(log(median),dispersion))*(q-(int)q);
    else	//normal distribution
        conseq+=stat->gaussrand(median,dispersion*median)*(q-(int)q);

    if (conseq<0)
    {
        conseq=0.0;
    }

    return conseq;
}

FragilityCurve::Tag HazusLossEstimator::_SimulateBldgTag()
{
    //calc red tags
    for(map<string,qrepair>::iterator it=_qRepair.begin();it!=_qRepair.end();it++)
    {
        const FragilityCurve *fc =&fragilityLib[it->first];
        int thisDS=0;
        for(unsigned int i=0;i<fc->dsGroups.size();i++)
        {
            for(unsigned int j=0;j<fc->dsGroups[i].dstates.size();j++)
            {
                if(fc->dsGroups[i].dstates[j].tagState==FragilityCurve::red)
                {
                    double median=fc->dsGroups[i].dstates[j].redTagMedian;
                    double dispersion=fc->dsGroups[i].dstates[j].redTagBeta;
                    double fractionNeeded=exp(stat->gaussrand(log(median),dispersion));
                    double fractionDamaged=it->second.q_ds[thisDS]/it->second.q;
                    if(fractionNeeded<=fractionDamaged)
                        return FragilityCurve::red;
                }
                thisDS++;
            }
        }
    }
    return FragilityCurve::none;
}
