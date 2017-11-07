#include "Stat.h"

Stat::Stat()
{

}

void Stat::SetSeed(int seed)
{
    _seed=seed;
    if (_seed==0)
    {
        srand((int)time(0));
    }
    else
    {
        srand(_seed);
    }
}


double Stat::random(double min, double max)
{
    double r=(double)rand() / (double)RAND_MAX;
    r=(max-min)*r+min;
    return r;
}

int Stat::round(double r)
{
    return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

double Stat::CDF_normal(double x,double median,double std)
{
    if (std==0)
    {
        std=0.0000001;
    }
    x=(x-median)/std;	//convert into standard normal distribution
    int index=round(fabs(x)*100.0);
    double y=0.0;

    if (index>=390)
    {
        y=1.0000;
    }
    else if (index>299)	//Look up the other standard normal distribution table
    {
        index=round(index/10.0-30.0);
        y=StandardNormalDistributionTable2[index];
    }
    else	//Look up the standard normal distribution table
    {
        y=StandardNormalDistributionTable1[index];
    }

    if (x<0)
    {
        y=1.0-y;
    }
    return y;
}

double Stat::gaussrand(double median, double std)
{
    static double V1, V2, S;
    static int phase = 0;
    double X;

    if ( phase == 0 ) {
        do {
            double U1 = (double)rand() / RAND_MAX;
            double U2 = (double)rand() / RAND_MAX;

            V1 = 2 * U1 - 1;
            V2 = 2 * U2 - 1;
            S = V1 * V1 + V2 * V2;
        } while(S >= 1 || S == 0);

        X = V1 * sqrt(-2 * log(S) / S);
    } else
        X = V2 * sqrt(-2 * log(S) / S);

    phase = 1 - phase;

    X = X * std + median;

    return X;

}
