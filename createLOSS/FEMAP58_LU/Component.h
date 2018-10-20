#ifndef COMPONENT_H
#define COMPONENT_H

#include <iostream>
#include <vector>
using namespace std;
class Component
{
public:
    Component();

    string ID;
    double qmedian=0.0;   //median quantity
    double qbeta=0.0;    //dispersion of the quantity (lognormal)
    vector<double> q;   //random quantity. size=number of realizations
    vector<double> loss;    //economic loss. size=number of realizations
    vector<double> downtime;    //downtime. size=number of realizations
    vector<double> q_ds;    //quantity for each damage state. size = number of damage states.

};

#endif // COMPONENT_H
