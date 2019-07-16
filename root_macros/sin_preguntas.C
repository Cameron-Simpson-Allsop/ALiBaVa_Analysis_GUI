#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <TCanvas.h>
#include <TROOT.h>
#include <TDirectory.h>
#include <TStyle.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TRegexp.h>
#include <TSystem.h>
#include <TGraphErrors.h>
#include "analysis.h"
#include "sin_preguntas.h"


/*
 * This the driving function to make analysis of an Alibava file.
 * It has an awfully long list of parameters:
 * A:          a pointer to a user supplied DataFileRoot (or descendant) object.
 *             Here one should pass a user defined "son/daughter" of DataFileRoot
 *             defining the virtual methods used to decode the special data blocks.
 * data_file:  The path of the data file
 * cal_file:   The path of a calibration file. It can be an Alibava data file
 *             produced during a calibration run or an ASCII text file with as many
 *             lines as channels with gain and offset in each line
 * ped_file:   The path of pedestal file. It can be an Alibava data file from where
 *             we can compute pedestals or an ascii text file with as many lines as
 *             channels and pedestal and noise for each channel. If no file is given,
 *             sin_pregunta will use de data file to compute pedestals.
 * polarity:   Polarity of bias voltage.
 * dofit:      Do a landau fit to the signal histogram
 * peak time:  Define a time window around the peak of the pulse shape
 *
 */
void sin_preguntas(DataFileRoot *A,
                   const char *data_file, const char *cal_file, const char *ped_file,
                   int polarity, bool dofit,
                   int peaktimesliceinitial, int peaktimeslicefinal)
{
    const char *ped_f = "/tmp/alibava_ped.ped";
    const char *cal_f = "/tmp/alibava_cal.cal";
//    int baselinetimesliceinitial=80;
//    int baselinetimeslicefinal=100;

    // Compute the pedestals
    // If none given, use the data file itself to compute pedestals
    if (!ped_file || !is_text(ped_file))
    {
        std::cout << "*** Computing pedestals\n"
                  << "***********************"
                  << std::endl;
        A->save();
        A->rewind();
        A->compute_pedestals_fast();
        A->save_pedestals(ped_f);
        {
            TCanvas *pcnvs = (TCanvas *)gROOT->FindObject("pcnvs");
            if (pcnvs)
                delete pcnvs;

            pcnvs = new TCanvas("pcnvs","Pedestals and Noise");
            pcnvs->Divide(1, 2);
            pcnvs->cd(1);
            A->show_pedestals()->DrawCopy();
            pcnvs->cd(2);
            A->show_noise()->DrawCopy();
            pcnvs->Update();
        }
        A->restore();
    }
    else
        ped_f = ped_file;

    // Get calibratioin file
    if (cal_file)
    {
        if (is_text(cal_file))
        {
            cal_f = cal_file;
        }
        else
        {
            std::cout << "*** Computing calibration factors" << std::endl
                      << "*********************************" << std::endl;
            DataFileRoot *B = DataFileRoot::OpenFile(cal_file, ped_f);
            analyze(*B, "hGain");
            save_text_file((TH1 *)gDirectory->Get("hGain"), cal_f);
            TCanvas *cnvs = (TCanvas *)gROOT->FindObject("Gain");
            if (cnvs)
                delete cnvs;

            cnvs = new TCanvas("Gain","Gain",700,800);
            cnvs->Divide(1, 2);
            cnvs->cd(1)->SetGrid(1, 1);
            gDirectory->Get("hGain")->Draw();
            cnvs->cd(2)->SetGrid(1, 1);
            gDirectory->Get("hGain_all")->Draw("surf3z");
            delete B;
        }
    }
    else
        cal_f = 0;

    // Analyze the data file
    std::cout << "*** Analyzing the data file" << std::endl
              << "***************************" << std::endl;

//    A = new AsciiRoot(data_file,ped_f, cal_f);
    if (data_file)
        A->open(data_file);

    if (cal_f)
        A->load_gain(cal_f);

    if (ped_f)
        A->load_pedestals(ped_f);

//    A->set_cuts(5., 3.); //First: Seedcut, Second Neighcut
    double t1, t2;
    t1 = 15; // set it to some arbitary value
    t2 = 25; //again
    if ( peaktimesliceinitial>0)
        t1 = peaktimesliceinitial;

    if ( peaktimeslicefinal>0 )
    {
        t2 = peaktimeslicefinal;
        if (t2<=t1)
            t2 = t1 + 20;
    }
    A->set_timecut(t1, t2);
    analyze(*A, "hSpec", polarity);
    if (A->type()>2)
    {
        if (A->type()==3) // Laser run
        {
            t1 = 0.;
            t2 = 1000.;
            dofit = false;
        }
        else // RadSource
        {
            //	  if (timeslice==0)
            //	    {
            //	      std::cout << "Need a type value, please insert one" << std::endl;
            //	      cin >> timeslice;
            //	    }
          }
        show_spectrum("hSpec", t1, t2, polarity, A, dofit);
    }
    else if (A->type()>0)
    {
        int ix, iy, iz;
        TVirtualPad *pad;
        TCanvas *cnvs = (TCanvas *)gROOT->FindObject("cnvs");
        if (cnvs)
            delete cnvs;

        gStyle->SetOptStat(0);
        cnvs = new TCanvas("cnvs", (A->type()==1 ? "Charge Scan" : "Laser Delay"), 700,800);
        cnvs->Divide(1, 2);
        pad = cnvs->cd(1);
        pad->SetGrid(1, 1);
        pad->SetObjectStat(false);
        gDirectory->Get("hSpec")->Draw();

        pad = cnvs->cd(2);
        pad->SetGrid(1, 1);
        pad->SetObjectStat(false);
        TH2 *h2 = (TH2 *)gDirectory->Get("hSpec_all");
        if ( A->type()==2 )
        {
            h2->GetMaximumBin(ix, iy, iz);
            h2->GetXaxis()->SetRange(ix-5, ix+5);
        }
        h2->Draw("surf4");
    }
}

double get_volt(const char *fnam)
{
    char *str =strdup(fnam);
    char *q, *p = strstr(str, "laser_");
    if (!p)
    {
        free(str);
        return 0.;
    }

    std::cout << "**** fname: p0 " << p;
    p+= strlen("laser_");
    q = strchr(p, '_');
    if (!q)
    {
        q = strchr(p, '.');
        if (!q)
            return 0.;
    }

    *q=0;
    std::cout << " p1 " << p << std::endl;
    free(str);
    return atof(p);
}

TGraphErrors *scan_laser(const char *detector, const char *cal_file,
                         const char *ped_file, int polarity, bool dofit,
                         int mxevts)
{
    const char *ped_f = "/tmp/alibava_ped.ped";
    const char *cal_f = "/tmp/alibava_cal.cal";

    /*
     * Compute pedestals.
     */
    if (!ped_file)
    {
        std::cout << "I need a pedestal file for laser data" << std::endl;
        return 0;
    }
    else
        ped_f = ped_file;


    DataFileRoot *A;

    /*
     * Get calibration file
     */
    if (cal_file)
    {
        if (is_text(cal_file))
            cal_f = cal_file;
        else
        {
            std::cout << "*** Computing calibration factors" << std::endl
                      << "*********************************" << std::endl;
            DataFileRoot *B = DataFileRoot::OpenFile(cal_file,ped_f);
            analyze(*B, "hGain");
            save_text_file((TH1 *)gDirectory->Get("hGain"), cal_f);
            delete B;
        }
    }
    else
        cal_f = 0;

    /*
     * Scan the laser files
     */
    // Analyze the data file
    std::cout << "*** Analyzing the data files for "
              << detector << std::endl
              << "***************************" << std::endl;

    TGraphErrors *gout = new TGraphErrors();
    gout->SetName(detector);
    int npts;
    char *str, path[256];
    char fnam[256];
    sprintf(path, "ls -1 %s/Analysis_Hamamatsu/%s/laser_*.dat", getenv("PWD"), detector);
    FILE *pipe = gSystem->OpenPipe(path, "r");
    for (npts=0, str=fgets(fnam, sizeof(fnam), pipe); str; str=fgets(fnam, sizeof(fnam),
                                                                     pipe))
    {
        *(strchr(fnam, '\n'))=0;
        TString *sf = new TString(fnam);
        std::cout << fnam << std::endl;
        double volt = get_volt(fnam);

        A = DataFileRoot::OpenFile(fnam,ped_f, cal_f);
        A->set_cuts(5., 3.0);
        analyze(*A, "hSpec", polarity, -1, 0, mxevts);
        show_spectrum("hSpec", 0., 1000., polarity, A, dofit);

        TH1 *hst = (TH1 *)gDirectory->Get("hSpec_timecut");
        TF1 *func = hst->GetFunction("gaus");
        if (func)
        {
            double mean = func->GetParameter(1);
            double sig = func->GetParError(1) ;
            std::cout << volt << " " << mean << ", " << sig << ", "
            << func->GetParameter(2) << std::endl;

            gout->SetPoint(npts, volt, mean);
            gout->SetPointError(npts, 0., sig);
            npts++;
        }

        delete sf;
    }
    gSystem->ClosePipe(pipe);
    return gout;
}

void graph2csv(TGraphErrors *gr, const char *ofilename)
{
    char fnam[256];
    if (!ofilename)
        sprintf(fnam, "%s.csv", gr->GetName());
    else
        strcpy(fnam, ofilename);

    std::ofstream ofile(fnam);

    ofile << "Volt\tQ\teQ" << std::endl;
    int ip, np=gr->GetN();
    for (ip=0; ip<np; ip++)
    {
        ofile << gr->GetX()[ip] << '\t' << gr->GetY()[ip] << '\t' << gr->GetEY()[ip] << std::endl;
    }

    ofile.close();
}

int printCanvas(TCanvas *cnvs, const char *out, int res)
{
    int rc = 0;
    char line[512];
    cnvs->SaveAs("/tmp/cnvs.eps");
    sprintf(line, "convert -units PixelsPerInch -density %d /tmp/cnvs.eps %s", res, out);
    rc = gSystem->Exec(line);
    gSystem->Unlink("/tmp/cnvs.eps");
    return rc;
}
void help()
{
    std::cout << "****************************************************************************************************************************************** " << std::endl;
    std::cout << " Function call is sin_preguntas(rs.dat, cali.dat, ped.dat, polarity, dofit, peak time1, peak time2, base time1, base time2) " << std::endl;
    std::cout << " Where the data files are enclosed by quotation marks, and the polarity is the polarity of your bias voltage (e.g. -1 or +1) " << std::endl;
    std::cout << " Do fit is the supplement for various fitting functions - suggested is to pass 'true' always " << std::endl;
    std::cout << " Peak time 1 and 2 should correspond to an arbitary time window (I use 10 nanoseconds, others use 5) around the peak of the pulse shape " << std::endl;
    std::cout << " Base line time 1 and 2 are non-important unless in specific debugging situations. You can leave it blank or not use it completely, e.g. " << std::endl;
    std::cout << " sin_preguntas(rs.dat, cali.dat, ped.dat, polarity, dofit, peak time1, peak time2) " << std::endl;
    std::cout << " and the program will still function fine. Current nominal values for these are peak 1 = 8, peak 2 = 18, base 1 = 80, base 2 = 100. " << std::endl;
    std::cout << "****************************************************************************************************************************************** " << std::endl;
    std::cout << " Other useful information : " << std::endl;
    std::cout << " To change the fitting parameters, open analysis.cc and go to line 760. The two numeric parameters correspond to the first and last ADC value (note, not bin) of the fit region." << std::endl;
}
