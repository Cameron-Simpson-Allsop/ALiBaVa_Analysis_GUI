#ifdef _WINDOWS
#define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TProfile2D.h>
#include <TSpectrum.h>
#include <TGraph.h>
#include <TMath.h>

#include <csignal>
#include "Data.h"
#include "Tracer.h"
#include "analysis.h"
#include "utils.h"


#include "sin_preguntas.C"


#ifdef __APPLE__
#define sighandler_t sig_t
#endif
#ifdef __MINGW32__
#define sighandler_t __p_sig_fn_t
#endif
#if _WINDOWS
typedef void (*sighandler_t)(int);
#endif

int timecutlowbinptr=0;
int timecuthighbinptr=0;

bool do_run = true;
bool debug_event=false;
void set_debug(bool x)
{
    debug_event = x;
}
bool get_debug()
{
    return debug_event;
}
void got_intr(int)
{
    do_run = false;
}

//////////////////////////////////////////////////////////////////////////
// DisLandau
//
//   Created: Mon Apr  6 07:56:29 1998        Author: Carlos Lacasta
//   Purpose:
//
//////////////////////////////////////////////////////////////////////////
static double DisLandau(double *x, double *)
{
    static double p1[5] =
    { .2514091491, -.06250580444, .0145838123, -.002108817737, 7.41124729e-4 };
    static double q5[4] =
    { 1., 50.09928881, 139.9819104, 420.0002909 };
    static double p6[4] =
    { 1.000000983, 132.9868456, 916.2149244, -960.5054274 };
    static double q6[4] =
    { 1., 133.9887843, 1055.990413, 553.2224619 };
    static double a1[3] =
    { -.4583333333, .6675347222, -1.641741416 };
    static double a2[3] =
    { 1., -.4227843351, -2.043403138 };
    static double q1[5] =
    { 1., -.005571175625, .06225310236, -.003137378427, .001931496439 };
    static double p2[4] =
    { .2868328584, .3564363231, .1523518695, .02251304883 };
    static double q2[4] =
    { 1., .6191136137, .1720721448, .02278594771 };
    static double p3[4] =
    { .2868329066, .3003828436, .09950951941, .008733827185 };
    static double q3[4] =
    { 1., .4237190502, .1095631512, .008693851567 };
    static double p4[4] =
    { 1.00035163, 4.503592498, 10.8588388, 7.536052269 };
    static double q4[4] =
    { 1., 5.539969678, 19.33581111, 27.21321508 };
    static double p5[4] =
    { 1.000006517, 49.09414111, 85.05544753, 153.2153455 };

    /* System generated locals */
    double ret_val;

    /* Local variables */
    static double u, v;

    v = *x;
    if (v < -5.5)
    {
        u = exp(v + 1);
        ret_val = exp(-1 / u) * .3989422803 * sqrt(u) * ((a1[0] + (a1[
                                                                      1] + a1[2] * u) * u) * u + 1);
    }
    else if (v < -1.)
    {
        u = exp(-v - 1);
        ret_val = exp(-u) / sqrt(u) * (p1[0] + (p1[1] + (p1[2] + (p1[3] + p1[
                                                                             4] * v) * v) * v) * v) / (q1[0] + (q1[1] + (q1[2] + (q1[3] + q1[4] * v) * v) * v)
                                                                                     * v);
    }
    else if (v < 1.)
    {
        ret_val = (p2[0] + (p2[1] + (p2[2] + p2[3] * v) * v) * v) / (q2[0] + (q2[1]
                                                                                 + (q2[2] + q2[3] * v) * v) * v);
    }
    else if (v < 4.)
    {
        ret_val = (p3[0] + (p3[1] + (p3[2] + p3[3] * v) * v) * v) / (q3[0] + (q3[1]
                                                                                 + (q3[2] + q3[3] * v) * v) * v);
    }
    else if (v < 12.)
    {
        u = 1 / v;
        ret_val = (p4[0] + (p4[1] + (p4[2] + p4[3] * u) * u) * u) / (q4[0] + (q4[1]
                                                                                 + (q4[2] + q4[3] * u) * u) * u);
    }
    else if (v < 50.)
    {
        u = 1 / v;
        ret_val = (p5[0] + (p5[1] + (p5[2] + p5[3] * u) * u) * u) / (q5[0] + (q5[1]
                                                                                 + (q5[2] + q5[3] * u) * u) * u);
    }
    else if (v < 300.)
    {
        u = 1 / v;
        ret_val = (p6[0] + (p6[1] + (p6[2] + p6[3] * u) * u) * u) / (q6[0] + (q6[1]
                                                                                 + (q6[2] + q6[3] * u) * u) * u);
    }
    else
    {
        u = 1 / (v - v * log(v) / (v + 1));
        ret_val = 1 - (a2[0] + (a2[1] + a2[2] * u) * u) * u;
    }
    return ret_val;
}

//////////////////////////////////////////////////////////////////////////
// DenLandau
//
//   Created: Mon Apr  6 07:48:35 1998        Author: Carlos Lacasta
//   Purpose:
//
//////////////////////////////////////////////////////////////////////////
static double DenLandau(double *x, double *)
{
    static double p1[5] =
    { .4259894875, -.124976255, .039842437, -.006298287635, .001511162253 };
    static double q5[5] =
    { 1., 156.9424537, 3745.310488, 9834.698876, 66924.28357 };
    static double p6[5] =
    { 1.000827619, 664.9143136, 62972.92665, 475554.6998, -5743609.109 };
    static double q6[5] =
    { 1., 651.4101098, 56974.73333, 165917.4725, -2815759.939 };
    static double a1[3] =
    { .04166666667, -.01996527778, .02709538966 };
    static double a2[2] =
    { -1.84556867, -4.284640743 };
    static double q1[5] =
    { 1., -.3388260629, .09594393323, -.01608042283, .003778942063 };
    static double p2[5] =
    { .1788541609, .1173957403, .01488850518, -.001394989411, 1.283617211e-4 };
    static double q2[5] =
    { 1., .7428795082, .3153932961, .06694219548, .008790609714 };
    static double p3[5] =
    { .1788544503, .09359161662, .006325387654, 6.611667319e-5, -2.031049101e-6 };
    static double q3[5] =
    { 1., .6097809921, .2560616665, .04746722384, .006957301675 };
    static double p4[5] =
    { .9874054407, 118.6723273, 849.279436, -743.7792444, 427.0262186 };
    static double q4[5] =
    { 1., 106.8615961, 337.6496214, 2016.712389, 1597.063511 };
    static double p5[5] =
    { 1.003675074, 167.5702434, 4789.711289, 21217.86767, -22324.9491 };

    /* System generated locals */
    double ret_val, d__1;

    /* Local variables */
    static double u, v;

    v = *x;
    if (v < -5.5)
    {
        u = exp(v + 1.);
        ret_val = exp(-1 / u) / sqrt(u) * .3989422803 * ((a1[0] + (a1[
                                                                      1] + a1[2] * u) * u) * u + 1);
    }
    else if (v < -1.)
    {
        u = exp(-v - 1);
        ret_val = exp(-u) * sqrt(u) * (p1[0] + (p1[1] + (p1[2] + (p1[3] + p1[
                                                                             4] * v) * v) * v) * v) / (q1[0] + (q1[1] + (q1[2] + (q1[3] + q1[4] * v) * v) * v)
                                                                                     * v);
    }
    else if (v < 1.)
    {
        ret_val = (p2[0] + (p2[1] + (p2[2] + (p2[3] + p2[4] * v) * v) * v) * v) / (q2[0]
                                                                                      + (q2[1] + (q2[2] + (q2[3] + q2[4] * v) * v) * v) * v);
    }
    else if (v < 5.)
    {
        ret_val = (p3[0] + (p3[1] + (p3[2] + (p3[3] + p3[4] * v) * v) * v) * v) / (q3[0]
                                                                                      + (q3[1] + (q3[2] + (q3[3] + q3[4] * v) * v) * v) * v);
    }
    else if (v < 12.)
    {
        u = 1 / v;
        /* Computing 2nd power */
        d__1 = u;
        ret_val = d__1 * d__1 * (p4[0] + (p4[1] + (p4[2] + (p4[3] + p4[4] * u) * u) * u)
                * u) / (q4[0] + (q4[1] + (q4[2] + (q4[3] + q4[4] * u) * u) * u) * u);
    }
    else if (v < 50.)
    {
        u = 1 / v;
        /* Computing 2nd power */
        d__1 = u;
        ret_val = d__1 * d__1 * (p5[0] + (p5[1] + (p5[2] + (p5[3] + p5[4] * u) * u) * u)
                * u) / (q5[0] + (q5[1] + (q5[2] + (q5[3] + q5[4] * u) * u) * u) * u);
    }
    else if (v < 300.)
    {
        u = 1 / v;
        /* Computing 2nd power */
        d__1 = u;
        ret_val = d__1 * d__1 * (p6[0] + (p6[1] + (p6[2] + (p6[3] + p6[4] * u) * u) * u)
                * u) / (q6[0] + (q6[1] + (q6[2] + (q6[3] + q6[4] * u) * u) * u) * u);
    }
    else
    {
        u = 1 / (v - v * log(v) / (v + 1));
        /* Computing 2nd power */
        d__1 = u;
        ret_val = d__1 * d__1 * ((a2[0] + a2[1] * u) * u + 1);
    }
    return ret_val;
}

//////////////////////////////////////////////////////////////////////////
// Gauss
//
//   Created: Thu Sep 10 06:02:06 1998        Author: K.S. Kolbig
//   Purpose: Integration by adaptive Gaussian quadrature
//
//////////////////////////////////////////////////////////////////////////
static double Gauss(double (*f)(double *,double *), double *param, double a, double b, double eps=0.00001)
{
    static double x[12] =
    { .96028985649753623, .79666647741362674, .52553240991632899, .1834346424956498,
      .98940093499164993, .94457502307323258, .86563120238783174,
      .75540440835500303, .61787624440264375, .45801677765722739,
      .28160355077925891, .09501250983763744 };
    static double w[12] =
    { .10122853629037626, .22238103445337447, .31370664587788729, .36268378337836198,
      .027152459411754095, .062253523938647893, .095158511682492785,
      .12462897125553387, .14959598881657673, .16915651939500254,
      .18260341504492359, .1894506104550685 };

    /* System generated locals */
    double d1, d2;

    /* Local variables */
    double h;
    int i;
    double u, c1, c2, cx, s8, aa, bb, s16;

    h = 0.;
    if (b == a)
        return h;

    cx = .005 / fabs(b-a);
    bb = a;
    CASE1: aa = bb;
    bb = b;
    CASE2: c1 = (bb + aa) * .5;
    c2 = (bb - aa) * .5;
    s8 = 0.;
    for (i = 0; i < 4; i++)
    {
        u = c2 * x[i];
        d1 = c1 + u;
        d2 = c1 - u;
        s8 += w[i] * ((*f)(&d1, param) + (*f)(&d2, param));
    }
    s16 = 0.;
    for (i = 4; i < 12; i++)
    {
        u = c2 * x[i];
        d1 = c1 + u;
        d2 = c1 - u;
        s16 += w[i] * ((*f)(&d1, param) + (*f)(&d2, param));
    }
    s16 = c2 * s16;
    if (fabs(s16 - c2 * s8) <= eps * (fabs(s16) + 1.))
    {
        h += s16;
        if (bb != b)
            goto CASE1;
    }
    else
    {
        bb = c1;
        if ( (cx * fabs(c2) + 1.) != 1.)
            goto CASE2;
        h = s8;
        std::cerr << "(GAUS:) too high accuracy required " << cx * fabs(c2) << " "
                << fabs(s16 - c2 * s8) << " " << eps * (fabs(s16) + 1.) << std::endl;
    }
    return h;

}

//////////////////////////////////////////////////////////////////////////
// LandGaus
//
//   Created: Mon Apr  6 07:39:03 1998        Author: Carlos Lacasta
//   Purpose: Fits to a Landau convoluted with a gaussian
//   Parameters:
//       0 - Scale parameter of Landau density
//       1 - Location parameter of Landau density
//       2 - Normalization constant
//       3 - Width (sigma) of Gaussian resolution function
//
//   Landau peak will be at par[1]+0.225*par[0]
//
//////////////////////////////////////////////////////////////////////////
static double LGconvl(double *x, double *par)
{
    double yy = ( *x - par[1] )/par[0];
    double lnd = (yy<-10. ? 0. : DenLandau(&yy, 0))/par[0];
    double r2 = (par[4]- *x)/par[3];
    double ex = exp(-0.5*r2*r2);
    double fun = lnd*ex;
    return fun;
}

static double Landau(double *x, double *par)
{
    double xm = par[1]+0.2225*par[0];
    double yy = ( *x - xm )/par[0];
    double lnd = (yy<-10. ? 0. : DenLandau(&yy, 0))/par[0];
    return par[2]*lnd;
}

static double FitLandGaus(double *x, double *par)
{
    double fun, xp[5];

    if (fabs(par[3])<=1.e-3)
    {
        double yy = ( *x - par[1] )/par[0];
        double lnd = (yy<-10. ? 0. : DenLandau(&yy, 0))/par[0];
        return lnd;
    }
    xp[0] = par[0];
    xp[1] = par[1]-0.2225*par[0];
    xp[2] = par[2];
    xp[3] = fabs(par[3]);
    xp[4] = *x;
    double l1 = (*x) -15.*xp[3];
    double l2 = (*x) +15.*xp[3];
    fun = Gauss(LGconvl, xp, l1, l2, 0.00001*fabs(xp[3]));

    return par[2]*fun/(par[3]*2.506628275);
}

static double FitLandSimple(double *x, double *par)
{
    int i, j;
    double fun;
    double xd;
    double step, xlow, xupp, a, b, c;
    double fland;
    double xx, sig, sum, r1;
    double r2, ee;
    static int np=50;

    a = par[0];
    b = par[1]+0.225*a;
    c = par[2];
    sig=par[3];
    if (sig==0.)
        sig=1.e-10;
    sum = 0.;
    xd = *x;
    /* Range of convolution integral */
    xlow = xd - sig * 5.;
    xupp = xd + sig * 5.;
    step = (xupp - xlow) / ((double)np);
    /* Convolution integral of Landau and Gaussian by sum */
    j=np/2;
    for (i=1; i<=j; i++)
    {
        xx=xlow+(i-.5)*step;
        r1 = (xd-xx);
        ee = exp(-r1*r1/(2*sig*sig));
        r2 = (xx-b)/a;
        fland = (r2<-5 ? 0. : DenLandau(&r2, 0)/a);
        sum=sum+fland*ee;

        xx=xupp-(i-.5)*step;
        r2 = (xx-b)/a;
        fland = (r2<-5 ? 0. : DenLandau(&r2, 0)/a );
        sum=sum+fland*ee;
    }

    fun = step*sum*c/sqrt(2.*M_PI)/sig;
    return (fun);
}

static bool __landau_init__ = false;
void init_landau()
{
    if (__landau_init__)
        return;

    __landau_init__ = true;

    TF1 *fFunc;
    fFunc = new TF1("fDisLandau",DisLandau,-50.,50.,0);
    fFunc = new TF1("fDenLandau",DenLandau,-10.,50.,0);

    fFunc = new TF1("fLandau",Landau,-10.,50.,3);
    fFunc ->SetParameters(0.25, 3.5, 1000.);
    fFunc ->SetParName(0, "Scale");
    fFunc ->SetParName(1, "Peak");
    fFunc ->SetParName(2, "Constant");

    fFunc = new TF1("fFitLandGaus",FitLandGaus,0.,25.,4);
    fFunc ->SetParameters(0.25, 3.5, 1000., 0.1);
    fFunc ->SetParName(0, "Scale");
    fFunc ->SetParName(1, "Peak");
    fFunc ->SetParName(2, "Norm");
    fFunc ->SetParName(3, "Sigma");

    fFunc = new TF1("fFitLandSimple",FitLandSimple,0.,25.,4);
    fFunc ->SetParameters(0.25, 3.5, 1000., 0.1);
    fFunc ->SetParName(0, "Scale");
    fFunc ->SetParName(1, "Peak");
    fFunc ->SetParName(2, "Norm");
    fFunc ->SetParName(3, "Sigma");
}

void spy_file(DataFileRoot &A, int mxevts)
{
    if (!A.valid())
        return;

    Tracer tr("hChan", "Channel signal", 500);
    Tracer temp("hTemp", "Temperature", 500);
    Tracer tim("hTim", "Time", 500);
    sighandler_t old_handler = signal(SIGINT, got_intr);
    do_run = true;

    /*
     * We read the first event to have an estimate of the
     * number of channels
     */
    A.read_event();
    A.rewind();

    TCanvas *cnvs = (TCanvas *)gROOT->FindObject("cnvs");
    if (cnvs)
    {
        cnvs->Clear();
        delete cnvs;
    }

    cnvs = new TCanvas("cnvs","cnvs",500, 800);
    cnvs->Divide(1, 4);
    TH1 *hst = (TH1 *)gROOT->FindObject("hst");
    if (hst)
        delete hst;

    hst = new TH1D("hst","Data",A.nchan(), -0.5, A.nchan()-0.5);
    hst->SetMinimum(512.-300);
    hst->SetMaximum(512.+300);

    cnvs->cd(1);
    hst->Draw();
    int ievt;
    TVirtualPad *gpad;

    if (mxevts<0)
        mxevts = 100000000;

    for (ievt=0; A.read_event()==0 && ievt<mxevts && do_run; ievt++)
    {
        A.process_event();
        std::ostringstream ostr;
        int i;
        for (i=1; i<=A.nchan(); i++)
        {
            hst->SetBinContent(i, A.signal(i-1));
        }
        tr.fill(A.data(60) );
        temp.fill(A.temp() );
        tim.fill(A.time());
        ostr << "\rEvent " << std::setw(10) << ievt;
        std::cout << ostr.str() << std::flush;
        gpad = cnvs->cd(1);
        hst->Draw();
        gpad = cnvs->cd(2);
        tr.Draw();
        gpad = cnvs->cd(3);
        temp.Draw();
        gpad = cnvs->cd(4);
        tim.Draw();
        cnvs->Update();

    }
    std::cout << std::endl;
    do_run= true;
    signal(SIGINT, old_handler);
    A.close();
}

void draw_event(DataFileRoot &A)
{
    TCanvas *cnvs = (TCanvas *)gROOT->FindObject("cnvs");
    if (!cnvs)
    {
        cnvs = new TCanvas("cnvs","Event",500,800);
        cnvs->Divide(1, 2);
    }

    TH1 *hraw = create_h1("hRaw", "Raw Data", A.nchan(), -0.5, A.nchan()-0.5);
    hraw->SetMinimum(512.-300);
    hraw->SetMaximum(512.+300);

    TH1 *hsig = create_h1("hsig", "Signal", 256, 0., 256./A.gain());
    hsig->SetMinimum(-300/A.gain());
    hsig->SetMaximum(+300/A.gain());

    int i;
    for (i=1; i<=A.nchan(); i++)
    {
        hsig->SetBinContent(i, A.signal(i-1));
        hraw->SetBinContent(i, A.data(i-1));
    }
    cnvs->cd(1);
    hraw->Draw();
    cnvs->cd(2);
    hsig->Draw();

    cnvs->Update();
}

/*
 * Analyzes data from where we expect a spectrum
 * It makes a common mode analysis and find clusters.
 * If the DataFileRoot object has a non empty channel_list collection,
 * it will use the first one to make the analysis
 * TODO: Use all the channels lists assuming they are disjoint
 */
TH1 *spectrum(DataFileRoot &A, const char *hname, double polarity, int chip, int mxevts)
{
    /*
     * This is for the names of the histograms
     */
    std::ostringstream ohtname, ohtprof, ochannel, ocmmd, oclust, osig, owidth;

    ohtname << hname << "_time";
    ohtprof << hname << "_time_prof";
    ochannel << hname << "_chan";
    ocmmd << hname << "_cmmd";
    oclust << hname << "_nclust";
    osig << hname << "_all";
    owidth << hname << "_cwidth";

    /*
     * Figure out if we just do a region defined in a ChanList
     * or if we just do the whole stuff
     */
    bool has_chanlist=false;
    ChanList C;
    if (A.n_channel_list()>0)
    {
        C = A.get_channel_list(0);
        has_chanlist = true;
    }
    else
    {
        int chan0=0;
        int chan1=A.nchan()-1;

        if (chip>=0 && chip<2)
        {
            chan0 = chip*128;
            chan1 = (chip+1)*128 -1;
        }
        std::ostringstream str;
        str << chan0 << '-' << chan1;
        C.Set(str.str().c_str());
    }

    /*
     * Create Histograms
     */
    double sgmax = 1024./A.gain();
    double qfact = 1.;

    /**
    if (sgmax>1000.)
    {
        qfact = 1e-3;
        sgmax *= qfact;
    }
    */

    TH2 *hcmmd = create_h2(ocmmd.str().c_str(), "CommonMode", 2, -0.5, 1.5, 50, -50., 50.);
    TH1 *hst = create_h1(hname, "spectrum", 128, -sgmax, sgmax);
    TH2 *htim = create_h2(ohtname.str().c_str(), "Time .vs. signal", 50, 0., 100., 512, -sgmax, sgmax);
    TProfile *hprof = create_profile(ohtprof.str().c_str(), "Time", 50, 0., 100., -sgmax, sgmax);
    TH2 *hchan = create_h2(ochannel.str().c_str(), "Channel .vs. signal", A.nchan(), -0.5, A.nchan()-0.5, 512, -sgmax, sgmax);
    TH1 *hclust = create_h1(oclust.str().c_str(), "Number of clusters", 10, -0.5, 9.5);
    TH2 *hall = create_h2(osig.str().c_str(), "Signal (no cuts)", A.nchan(), -0.5, A.nchan()-0.5, 300, -sgmax, sgmax);
    TH1 *hwidth = create_h1(owidth.str().c_str(), "Cluster width", 10, -0.5, 9.5);

    /*
     * Prepare to intercept a Ctrl-C
     */
    sighandler_t old_handler = signal(SIGINT, got_intr);
    do_run = true;

    int ievt;
    bool do_common_mode = !has_chanlist;
    std::cout << "Spectrum..." << std::endl;
    for (ievt=0; A.read_event()==0 && ievt<mxevts&& do_run; ievt++)
    {
        int i;
        bool ignore = false;
        C.clear_hits();

        A.process_event(do_common_mode);
        if (has_chanlist)
        {
            A.common_mode(C, C.Nch()>10);
        }
        if (!has_chanlist)
        {
            for (i=0; i<2; i++)
            {
                hcmmd->Fill(i, A.get_cmmd(i));
                if (fabs(A.get_cmmd(i))>10.)
                    ignore = true;
            }

        }
        else
        {
            hcmmd->Fill(0.0, C.CommonMode());
            if (fabs(C.CommonMode())>10.0)
                ignore = true;
        }
        //        if ( ignore )
        //            continue;

        A.polarity(polarity);
        if ( has_chanlist )
        {
            A.find_clusters(C);
            A.set_hit_list(C.hit_list());
        }
        else
            A.find_clusters( chip );

        for (i=0; i<C.Nch(); i++)
            hall->Fill(C[i], qfact*A.signal(C[i]));

        if (A.empty())
            continue;

        hclust->Fill(A.nhits());
        //        if (A.nhits()!=1)
        //            continue;

        HitList::iterator ip;
        for (ip=A.begin(); ip!=A.end(); ++ip)
        {
            Hit &h = *ip;

            // remove first and last in each chip
            if (h.center()%128 ==0)
                continue;

            double val = qfact * h.signal();
            double tval = A.time();
            int icenter = h.center();
            hst->Fill(val);
            htim->Fill(tval, val);
            hprof->Fill(tval, val);
            if (A.valid_time(tval))
            {
                hchan->Fill(icenter, val);
                hwidth->Fill(h.width());
            }
        }
        if (!(ievt%100))
        {
            std::cout << "\revent " << std::setw(10) << ievt << std::flush;
        }
    }

    do_run= true;
    signal(SIGINT, old_handler);
    return hst;

}

TF1 *fit_landau(TH1 *hx)
{
    TSpectrum sp;
    int ib, ibin;
    int maxbin = hx->GetMaximumBin();
    double yy, ymax, xpeak, xmin=hx->GetBinCenter(1), xmax=hx->GetBinCenter(hx->GetNbinsX()-1);
    TF1 *fland = (TF1 *)gROOT->FindObject("fFitLandSimple");
    if (!fland)
    {
        init_landau();
        fland = (TF1 *)gROOT->FindObject("fFitLandSimple");
    }
    if (fland)
    {
        sp.Search(hx,10,"",0.1);//,"goffnodraw");
        if (sp.GetNPeaks()>0)
        {
            double RMS = hx->GetRMS(1);
            double Mean = hx->GetMean(1);
            double Norm = hx->GetNormFactor();
            std::cout << "\nRMS: " << RMS << ", Mean: " << Mean << ", Norm: " << Norm << std::endl;

            int ipeak=-1;
            xpeak = -1e20;
            for (int ip=0; ip<sp.GetNPeaks(); ip++)
            {
                double xpos = sp.GetPositionX()[ip];
                if (xpos>xpeak)
                {
                    ipeak = ip;
                    xpeak = xpos;
                }
            }
            if (ipeak<0)
            {
                ibin = hx->GetMaximumBin();
                ymax = hx->GetMaximum();
                xpeak = hx->GetBinCenter(ibin);
            }
            else
            {
                ibin = hx->FindBin(xpeak);
                ymax = hx->GetBinContent(ibin);
            }
            std::cout << "*** Landau peak at bin " << ibin << "[" << xpeak << "] val " << ymax << std::endl;
            for (ib=ibin; ib>1; ib--)
            {
                yy = hx->GetBinContent(ib);
                if ( yy< 0.10*ymax)
                {
                    xmin = hx->GetBinCenter(ib);
                    break;
                }
            }
            for (ib=ibin; ib<maxbin; ib++)
            {
                yy = hx->GetBinContent(ib);
                if ( yy< 0.10*ymax)
                {
                    xmax = hx->GetBinCenter(ib);
                    break;
                }
            }

            fland->SetRange(xmin, xmax);
            std::cout << "*** Fit range is: " << xmin << " -> " << xmax << std::endl;
            fland->SetParameters(10, xpeak, hx->GetSumOfWeights(), 10.0);
            hx->Fit("fFitLandSimple", "wr");
        }
        else
            return 0;

        return fland;
    }
    return 0;
}

/**
 * This makes a landau fit in each of the proyections of the
 * XX_timecut histogram
 */
TGraph *pulse_shape(TH2 *hst, int polarity)
{
    int ibin, ipoint=0;
    TGraph *gr = new TGraph(0);

    for (ibin=1; ibin<=hst->GetNbinsX(); ibin++)
    {
        TH1 *hx;
        if (polarity>0)
            hx = create_h1("__h_pulse_shape__", "",  hst->GetNbinsY()/2, 0.0, hst->GetYaxis()->GetXmax());
        else
            hx = create_h1("__h_pulse_shape__", "",  hst->GetNbinsY()/2, 0.0, fabs(hst->GetYaxis()->GetXmin()));

        double sum = 0.0;
        for (int iy=1; iy<=hst->GetNbinsY(); iy++)
        {
            double val = fabs(hst->GetCellContent(ibin, iy));
            hx->Fill(fabs(hst->GetYaxis()->GetBinCenter(iy)), val);
            sum += val;
        }
        if (sum>10.0)
        {
            TF1 *func = fit_landau(hx);
            if (func)
            {
                gr->SetPoint(ipoint++, hst->GetXaxis()->GetBinCenter(ibin), func->GetParameter(1));
            }
            hx->Draw();
            gPad->Update();
        }
    }
    if (ipoint)
        return gr;
    else
    {
        delete gr;
        return 0;
    }
}

void show_spectrum(const char *hname, double t0, double t1, int polarity, DataFileRoot *A,
                   bool dofit)
{
    TH1 *hx;
    TH2 *h2;
    double noise;
    TVirtualPad *pad, *cell;
    std::ostringstream ostr;
    std::string ohtname, ohtprof, ochannel, ocmmd, ocut, oall;
    if (!gROOT->FindObject(hname))
        return;

    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1);
    ostr << "Spectrum with Time cut [" << t0 << " , " << t1 << "]";
    ohtname = hname + std::string("_time");
    ohtprof = hname + std::string("_time_prof");
    ochannel= hname + std::string("_chan");
    ocmmd = hname + std::string("_cmmd");
    ocut = hname + std::string("_timecut");
    oall = hname + std::string("_all");

    TCanvas *cnvs = (TCanvas *)gROOT->FindObject("cnvs_spec");
    if (cnvs)
        delete cnvs;
    else
        cnvs = new TCanvas("cnvs_spec","Analysis",700, 800);

    cnvs->Divide(2, 2);

    /*
     * Show histograms with "all signal", that is, signal and noise
     */
    pad = cnvs->cd(1);
    pad->SetGrid(1, 1);
    pad->Divide(1, 2);
    h2 = (TH2 *)gROOT->FindObject(oall.c_str());
    for (int ichip=1; ichip<=2; ichip++)
    {

        std::ostringstream oss;
        oss << "Chip " << ichip;
        cell = pad->cd(ichip);
        cell->SetLogy(1);
        cell->SetGrid(1, 1);
        hx = h2->ProjectionY(ichip == 1 ? "_hx_" : "_hy_", (ichip-1)*128+1, ichip*128);
        hx->SetTitle(oss.str().c_str());
        if (A->gain()==1)
        {
            hx->SetXTitle("Signal (all) ADCs");
        }
        else
        {
            hx->SetXTitle("Signal (all) x10^3 electrons");
        }
        hx->SetFillColor(kYellow+1);
        hx->Fit("gaus", "wq");
        TF1 *ff = hx->GetFunction("gaus");
        if (ff)
            noise = ff->GetParameter(1);
        else
        {
            noise = 0.0;
            hx->Draw();
        }
    }

    /*
     * Signal histogram
     */
    cnvs->cd(2);
    h2 = (TH2 *)gROOT->FindObject(ohtname.c_str());
    bool do_fill=true;
    if (polarity==0)
    {
        hx = h2->ProjectionY(ocut.c_str(), h2->GetXaxis()->FindBin(t0), h2->GetXaxis()->FindBin(t1) );
        hx->SetTitle(ostr.str().c_str());
        do_fill = false;
    }
    else if (polarity>0.)
    {
        hx = create_h1(ocut.c_str(), ostr.str().c_str(), h2->GetNbinsY()/2, 0, h2->GetYaxis()->GetXmax());
    }
    else
    {
        hx = create_h1(ocut.c_str(), ostr.str().c_str(), h2->GetNbinsY()/2, 0, fabs(h2->GetYaxis()->GetXmin()) );
    }

    if (do_fill)
    {
        int ib, iy;
        if ( A->gain()==1)
            hx->SetXTitle("Signal (all) ADCs");
        else
            hx->SetXTitle("|Charge| x 10^{3} electrons");


        for (ib=h2->GetXaxis()->FindBin(t0); ib<=h2->GetXaxis()->FindBin(t1); ib++)
        {
            for (iy=1; iy<=h2->GetNbinsY(); iy++)
            {
                hx->Fill(fabs(h2->GetYaxis()->GetBinCenter(iy)), h2->GetCellContent(ib, iy));
            }
        }
    }
    gPad->SetGrid(1,1);
    hx->SetFillColor(kYellow+1);
    hx->Draw();
    /////////////////////////////////////////////
    double RMS = hx->GetRMS(1);
    double Mean = hx->GetMean(1);
    double Norm = hx->GetNormFactor();

    if (A->type()==4 && dofit)
    {
        fit_landau(hx);
    }
    else if (A->type()==3 && dofit)
    {

        if (A!=0)
        {//FIT TO A GAUSSIAN
            hx->Fit("gaus", "w");
        }
        else
        {//FIT TO TWO GAUSSIANS
            /*
             hx->Fit("gaus","wQ0","",50,250);
             float mean_1 = hx->GetFunction("gaus")->GetParameter("Mean");
             float sigma_1 = hx->GetFunction("gaus")->GetParameter("Sigma");
             float norm_1 = hx->GetFunction("gaus")->GetParameter(0);
             hx->Fit("gaus","wQ0+","",20,200);
             float mean_2 = hx->GetFunction("gaus")->GetParameter("Mean");
             float sigma_2 = hx->GetFunction("gaus")->GetParameter("Sigma");
             float norm_2 = hx->GetFunction("gaus")->GetParameter(0);
             */
            TF1 *DoubleGauss = new TF1("DoubleGauss","gaus(0)+gaus(3)");
            DoubleGauss->SetParameter(0, Norm);
            DoubleGauss->SetParameter(1, Mean);
            DoubleGauss->SetParameter(2, RMS);
            DoubleGauss->SetParameter(3, Norm);
            DoubleGauss->SetParameter(4, Mean);
            DoubleGauss->SetParameter(5, RMS/2.);
            hx->Fit("DoubleGauss", "w");
            double par[6];
            DoubleGauss->GetParameters(par);
            float value1 = hx->GetFunction("DoubleGauss")->Eval(par[1]);
            float value2 = hx->GetFunction("DoubleGauss")->Eval(par[4]);
            std::cout << value1 << " " << value2 << std::endl;
            if (value1>value2)
            {
                std::cout << "You must write down p1" << std::endl;
            }
            else
            {
                std::cout << "You must write down p4" << std::endl;
            }
        }
    }

    /*
     * Time histogram
     */
    cnvs->cd(3);
    gPad->SetGrid(1,1);
    TProfile *hp = (TProfile *)gROOT->FindObject(ohtprof.c_str());
    if (A->gain()==1)
    {
        hp->SetXTitle("ns");
        hp->SetYTitle("ADCs");
    }
    else
    {
        hp->SetXTitle("ns");
        hp->SetYTitle("x10^{3} electrons");
    }
    hp->Draw();

    /*
     * Noise and common mode
     */
    pad=cnvs->cd(4);
    if (A)
    {
        pad->Divide(1, 2);
        pad->cd(1);
        TH1 *hnoise = A->show_noise();
        hnoise->Draw();
        pad->cd(2);
    }
    TH2 *hcmmd = (TH2*)gROOT->FindObject(ocmmd.c_str());
    hx = hcmmd->ProjectionY("hcmmd_1", 1, 1);
    hx->SetFillColor(kYellow+1);
    hx->SetXTitle("Common mode (ADC)");
    hx->Fit("gaus", "q");
    gROOT->FindObject("hcmmd_1")->Draw();
    cnvs->cd();
}

TH1 *draw_scan(DataFileRoot &A, const char *hname, double factor, bool flip, int chip,
               int odd, int mxevts)
{
    double s = A.step()/2.;
    if (mxevts<0)
        mxevts = 100000000;

    int chan0=0;
    int chan1=A.nchan();
    int step=1;
    if (chip>=0 && chip<2)
    {
        chan0 = chip*128;
        chan1 = (chip+1)*128;
    }
    if (odd>0)
    {
        step = 2;
        if (odd>1)
        {
            chan0 ++;
            chan1 ++;
        }
    }

    int nbin = (chan1-chan0)/step;

    std::string name1 = hname + std::string("_all");
    TH2 *hst = create_profile2d(name1.c_str(), "Scan", nbin, chan0-step/2.0, chan1-step/2.0, A.npts(), A.from()-s, A.to()-s);
    hst->SetXTitle("Channel number");
    hst->SetYTitle(A.type()==1 ? "x10^{3} electrons" : "ns");
    bool delay_scan = A.scan_type() == DataFileRoot::Time;
    if ( delay_scan )
        hst->SetYTitle("Strobe Delay (ns)");

    hst->SetZTitle("Signal (ADC units)");
    int ichan, ievt;
    for (ievt=0; A.read_event()==0 && ievt<mxevts; ievt++)
    {
        A.process_event();
        short delay = int(A.value()) >> 16;
        short charge = int(A.value()) & 0xffff;
        double val;
        if (delay_scan)
        {
            val = delay;
        }
        else
        {
            val = factor*charge;
        }
        for (ichan=chan0; ichan<chan1; ichan +=step)
        {
            double ff=1.0;
            double xxx = A.signal(ichan);
            if (delay_scan)
            {
                if (fabs(xxx)>200.)
                    continue;

                double ss = ichan % 2 ? -1.0 : 1.0 ;
                if (ss*xxx < 0.0 )
                    continue;
            }
            if (flip && xxx<0 )
            {    xxx = -xxx;
            }
//            if (ichan<20)
//            {
//                std::cout << ichan << " signal: " << xxx << " corr. " << ff*xxx << std::endl;
//            }

            hst->Fill(ichan, val, ff*xxx);
        }
    }

    TH1 *hout=0;
    if (A.type()==1)
    {
        hout = create_h1(hname, "Gain Scan", nbin, chan0-step/2.0, chan1-step/2.0);
        hout->SetXTitle("Channel number");
        hout->SetYTitle("ADC/electron");
        TF1 *gpol = (TF1 *)gROOT->GetFunction("pol1");
        if (A.type()==1)
        {
            for (ichan=chan0; ichan<chan1; ichan +=step)
            {
                TH1 * h1 = (TH1 *)gROOT->FindObject("__h__");
                if (h1)
                    delete h1;

                h1 = hst->ProjectionY("__h__", ichan+1, ichan+1);
                if (h1->GetSumOfWeights()>0)
                {
                    h1->Fit("pol1", debug_event ? "w" : "qw");
                    if (debug_event)
                        gPad->Update();

                    double gain = gpol->GetParameter(1);
                    if (gain<0)
                        gain = -gain;

                    hout->Fill(ichan, gain);
                }
                delete h1;
            }
        }
    }
    else
    {
        int ix, iy, iz;
        hst->GetMaximumBin(ix, iy, iz);
        hout = hst->ProjectionY(hname, ix-3, ix+3);
        hout->SetTitle("Laser delay scan");
        hout->SetXTitle("Delay (ns)");
        hout->SetYTitle("Charge (electrons)");
    }
    return hout;
}

TH1 *analyze(DataFileRoot &A, const char *hname, double polarity, int chip, int odd, int mxevts)
{
    TH1 *ret=0;
    if (!A.valid() )
    {
        return 0;
    }

    if (mxevts<0)
        mxevts = 100000000;

    switch (A.type())
    {
        case 1: // calibration
            ret = draw_scan(A, hname, 1024., true, chip, odd, mxevts);
            break;
        case 2: // laser sync
            ret = draw_scan(A, hname, 1., false, chip, odd, mxevts);
            break;
        default:
            ret = spectrum(A, hname, polarity, chip, mxevts);
            break;
    }
    return ret;
}

void save_text_file(TH1 *h1, const char *name)
{
    std::ofstream ofile(name);
    if (!ofile)
    {
        std::cout << "Could not open " << name << " for writing" << std::endl;
        return;
    }
    int ib, nb=h1->GetNbinsX();
    for (ib=1; ib<=nb; ib++)
    {
        ofile << h1->GetBinCenter(ib) << '\t' << h1->GetBinContent(ib) << std::endl;
    }
    ofile.close();
}

static double fitscurve(double *x, double *par) {
   double t,f;
   t = (x[0]-par[1])/par[2]/M_SQRT2;
   f = 0.5*par[0]*TMath::Erfc(t) + par[3];
  return f;
}

static double fitscurvec(double *x, double *par) {
   double t,f;
   t = (x[0]-par[1])/par[2]/M_SQRT2;
   f = 0.5*par[0]*(1+TMath::Erf(t)) + par[3];
  return f;
}


TF1 *funcScurve(const char *name)
{
    TF1 *fScurv = (TF1 *)gROOT->FindObject(name);
    if (fScurv)
        delete fScurv;

    fScurv = new TF1(name,fitscurve,0.,6.,4);
    fScurv->SetParameters(1.,3.,1.0,0.);
    fScurv->SetParName(0,"Plateau");
    fScurv->SetParName(1,"Mean");
    fScurv->SetParName(2,"Sigma");
    fScurv->SetParName(3,"OffSet");
    return fScurv;
}


TF1 *funcScurveC(const char *name)
{
    TF1 *fScurv = (TF1 *)gROOT->FindObject(name);
    if (fScurv)
        delete fScurv;

    fScurv = new TF1(name,fitscurvec,0.,6.,4);
    fScurv->SetParameters(1.,3.,1.0,0.);
    fScurv->SetParName(0,"Plateau");
    fScurv->SetParName(1,"Mean");
    fScurv->SetParName(2,"Sigma");
    fScurv->SetParName(3,"OffSet");
    return fScurv;
}
