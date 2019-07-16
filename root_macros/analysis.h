#ifndef _ROOT_ANALYSIS_H_
#define _ROOT_ANALYSIS_H_

#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TProfile.h>
#include <DataFileRoot.h>

void spy_file(DataFileRoot &A, int mxevts=-1);
TH1 *analyze(DataFileRoot &A,
             const char *hname, 
             double polarity=0., 
             int chip=-1, 
             int odd=0,
             int mxevts=-1);

TH1 *draw_scan(DataFileRoot &A,
               const char *hname, 
               double factor=1.,
               bool flip=false,
               int chip=-1, 
               int odd=-1, 
               int mxevts=-1);

TH1 *spectrum(DataFileRoot &A,
              const char *hname, 
              double polarity=0., 
              int chip=-1,
              int mxevts=-1);

void show_spectrum(const char *hname, double t0, double t1, int polarity, DataFileRoot *A, bool dofit=true);

TF1 *fit_landau(TH1 *hst);
TF1 *funcScurve(const char *name="fScurve");
TF1 *funcScurveC(const char *name="fScurveC");
TGraph *pulse_shape(TH2 *hst, int polarity);
void set_debug(bool x);
bool get_debug();
void init_landau();
void save_text_file(TH1 *h1, const char *name);
#endif /*_ROOT_ANALYSIS_H_*/
