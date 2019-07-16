/*
 * crrc.cc
 *
 *  Created on: Mar 21, 2014
 *      Author: lacasta
 */


#include <iostream>
#include <cmath>
#include <TROOT.h>
#include <TF1.h>
#include "crrc.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////
// CRRC functions
//
//   Created: Wed Dec 16 09:02:36 1998        Author: Carlos Lacasta
//   Purpose:
//
//////////////////////////////////////////////////////////////////////////
static double N=3,N2=N*N;
void   SetN(double n) { N=n;N2=n*n; }
double GetN() { return N; }

//---------------------------------------------------------------------
// Give formulas for CR-(RC)^n, n=1,2,3
// Parameters:
//             0 = charge
//             1 = peak time
//             2 = offset
//---------------------------------------------------------------------
double crrcn(double *x, double *par) {
//   static double e = exp(1);
   if ( N<1. ) { cout << "n<=1" << endl; return 0.; }
   if ( par[1]<=0. ) return 0.;

   double xx = *x;
   xx = N*xx/par[1];
   double func = par[0]*pow(xx/N,N)*exp(N-xx)+par[2];
   return func;
}

//---------------------------------------------------------------------
// Parameters:
//             0 = charge
//             1 = peak time
//             2 = delta t
//             3 = offset
//             4 = n [only for txcrrcn]
//---------------------------------------------------------------------

double tcrrcn(double *x, double *par) {
//   static double e = exp(1);
   if ( N<1. ) { return 0.; }
   if ( par[1]<=0. ) return 0.;

   double xx = (*x -par[2]);
   if ( xx<=0. ) return 0.;
   xx = N*xx/par[1];
   double func = par[0]*pow(xx/N,N)*exp(N-xx)+par[3];
   return func;
}

double txcrrcn(double *x, double *par) {
   double ret,n_old=GetN();
   SetN(par[4]);
   ret = tcrrcn(x,par);
   SetN(n_old);
   return ret;
}


/*
 * Has 4 parameters
 * 0. Q: amplitude (fC)
 * 1. tau_1: falling time (ns)
 * 2. tau_2: raising time (ns). Tr = 2.2*tau_2
 * 3. Cfb (fF)
 */
double beetle(double *x, double *par)
{
    double t = x[0];
    double Q = par[0];
    double tau_1 = par[1];
    double tau_2 = par[2];
    double Cfb = par[3];

    double Vout = (Q*tau_1/(Cfb*(tau_1-tau_2)))*(exp(-t/tau_1)-exp(-t/tau_2));
    return Vout;
}

void init_crrc()
{
    TF1 *func;
    if ( (func=(TF1 *)gROOT->FindObject("fCRRC"))==0 ) {
        func = new TF1("fCRRC",crrcn,0.,150.,3);
        func->SetParameters(1.,25.,0.);
        func->SetParName(0,"Charge");
        func->SetParName(1,"Pktime");
        func->SetParName(2,"Offset");
    }

    if ( (func=(TF1 *)gROOT->FindObject("ftCRRC"))==0 ) {
        func = new TF1("ftCRRC",tcrrcn,0.,150.,4);
        func->SetParameters(1.,25.,0.);
        func->SetParName(0,"Charge");
        func->SetParName(1,"Pktime");
        func->SetParName(2,"DeltaT");
        func->SetParName(3, "Offset");
    }

    if ( (func=(TF1 *)gROOT->FindObject("ftxCRRC"))==0 ) {
        func = new TF1("ftxCRRC",txcrrcn,0.,150.,5);
        func->SetParameters(1.,25.,0.,3.);
        func->SetParName(0,"Charge");
        func->SetParName(1,"Pktime");
        func->SetParName(2,"DeltaT");
        func->SetParName(3, "Offset");
        func->SetParName(4,"n ");
    }

    if ( (func==(TF1 *)gROOT->FindObject("fBeetle"))==0 ) {
        func = new TF1("fBeetle",beetle,0.,150.,4);
        func->SetParameters(1.,260.0, 7.8, 400.6);
        func->SetParName(0,"Charge");
        func->SetParName(1,"tau_1");
        func->SetParName(2,"tau_2");
        func->SetParName(3, "Cfb");
    }

}
