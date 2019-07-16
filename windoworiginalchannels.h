#ifndef WINDOW_H
#define WINDOW_H
//
#include <QWidget>
#include <QDesktopWidget>
#include <QLineEdit>
#include <QApplication>
#include <QProgressBar>
#include <QSlider>
#include <QComboBox>
#include <QStringList>
#include <QLabel>
#include <QPushButton>
#include <QDial>
#include <iostream>
#include <fstream>
//#include <QtGui/QApplication>
#include <QtWidgets/QApplication>



#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TTree.h>
#include <cmath>
#include <TProfile.h>
#include "TKey.h"
#include <TImage.h>
#include <TFile.h>

#include "DataFileRoot.h"
#include <boost/filesystem.hpp>
#include <iomanip>
#include <qcustomplot.h>



#include <sstream>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <memory>
#include <cmath>
#include <string>
#include <cstring>
#include <TStyle.h>
#include <TAttMarker.h>
//#include <direct.h>
#include <sys/stat.h>

#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooGaussian.h"
#include "RooLandau.h"
#include "RooFFTConvPdf.h"
#include "RooPlot.h"
#include "RooHistPdf.h"
#include "RooAddPdf.h"
#include "RooMsgService.h"

#include <cstdlib>
#include <iomanip>
#include <sstream>

#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TFile.h"
#include "TKey.h"
#include "TTree.h"
#include "TGraphErrors.h"
#include "TLatex.h"
#include <TROOT.h>
#include "AtlasStyle.h"
#include "TLine.h"
//#include <QCPPlotTitle>


using namespace RooFit ;






namespace Ui{

  class Window;
}

class Window : public QWidget
{
  Q_OBJECT


 public:
  explicit Window(QWidget *parent = 0);
    
    
  QGraphicsPixmapItem* ClusterTimePicture;
        
    
  QLineEdit* input_pathToFileList;
  QLineEdit* input_pathToAnalysisDirectory;
  QLineEdit* input_sensorName;

  QLineEdit* input_seedCut;
  QLineEdit* input_neighbourCut;
  QLineEdit* input_minimumClusterSize;
  QLineEdit* input_lowerTimeCut;
  QLineEdit* input_upperTimeCut;
  QLineEdit* input_Temperature;
  QLineEdit* input_plotTitle;

  QLineEdit* currentEvent;
  QLineEdit* currentFile;


  QLabel* label_pathToFileList;
  QLabel* label_pathToAnalysisDirectory;
  QLabel* label_statusOfSensor;
  QLabel* label_sensorName;
  QLabel* label_seedCut;
  QLabel* label_neighbourCut;
  QLabel* label_minimumClusterSize;
  QLabel* label_currentEvent;
  QLabel* label_currentFile;
  QLabel* label_lowerTimeCut;
  QLabel* label_upperTimeCut;
  QLabel* label_Temperature;
  QLabel* label_plotTitle;
  QLabel* label_SensorType;

  QCustomPlot* histogram;
  QCustomPlot* Qhtotal;

  QSlider* testingSlider;

  QSlider* slider;
  QComboBox* statusOfSensor;
  QComboBox* SensorType;

  QPushButton* confirmSettings;

  TCanvas *c = new TCanvas;

  enum ignore{
    lowercase
  };

  bool contains_substring(std::string string, std::string substring){
    bool containsSubstring{false};
    if (string.find(substring)==std::string::npos) containsSubstring = false;
    else containsSubstring = true;
    return containsSubstring;
  }

  bool contains_substring(std::string string, std::string substring, ignore h){
    if (h == ignore::lowercase){
      auto it = std::search(string.begin(), string.end(), substring.begin(), substring.end(), [](char ch1,char ch2) {return std::toupper(ch1) == std::toupper(ch2);});
      return (it!=string.end());
    }
    else throw 101;
  }


  void loopdir (TDirectory* dir,std::vector<int>& V_Bias, std::vector<TH1*>& hst_histsNew, std::vector<TH1*>& h_width_histsNew) {
    //loop on all keys of dir including possible subdirs
    //print a message for all keys with a class TH1
    TIter next (dir->GetListOfKeys());
    (dir->GetListOfKeys())->Print();
    TKey *key;
    while ((key = (TKey*)next())) {
      TString KeyName = key->GetName();

      if (strstr(key->GetClassName(),"TH1D")) {
	printf (" key : %s is a %s in %s\n",
		key->GetName(),key->GetClassName(),dir->GetPath());
      }
      if (strstr(key->GetClassName(),"TDirectory")) {
	dir->cd(key->GetName());
	TDirectory *subdir = gDirectory;
	loopdir(subdir,V_Bias,hst_histsNew,h_width_histsNew);
	dir->cd();
      }
      if (KeyName.EndsWith("_hst")){
	V_Bias.push_back(atoi((KeyName(0, KeyName.Last('V') - 1 )).Data()));
	TH1* hstNew = (TH1*) dir->Get(key->GetName());
	hst_histsNew.push_back(hstNew);
      }
      if (KeyName.EndsWith("_hwidth")){
	TH1* hwidthNew = (TH1*) dir->Get(key->GetName());
	h_width_histsNew.push_back(hwidthNew);
      }


    }
  }



  std::string stringify(double x, int nplace) {

    x*=pow(10.,nplace);
    double num = floor(x+0.5)/pow(10.,nplace);

    std::string tmp = "NULL";
    std::ostringstream o;

    if (!(o << std::fixed << std::setprecision(nplace) << num) ) return tmp;
    return o.str();
  }


  void landgausfit(TFile* SaveFile, TH1* histogram, double& maxCent, double& CentError, double& width, double& widtherr, TString Status,int V_Bias, TString RootFile, TString AnalysisDir)
  {
    TString bias_voltage;
    bias_voltage += V_Bias;
    bool isIrradiated;
    if(Status == "0") isIrradiated = 0;
    else isIrradiated = 1;

    /*
      for(int n=0; n <=300; n++)
      {
      //   if( n > histogram->GetNbinsX()) histogram->SetBinContent(n,0.0);
      std::cout << n << "    "  << histogram->GetBinContent(n) << std::endl;
      }
    */
    histogram->Rebin(2);



    //Construct observable
    RooRealVar signal("s","Signal [ADC]",0.0,500.0);
    /* if(isIrradiated && !RootFile.Contains("KAZU") ) signal.setRange("fitrange", 40,200.); */
    /* if(isIrradiated && RootFile.Contains("KAZU") ) signal.setRange("fitrange",0.6*maxCent,200.); */
    /* if(!isIrradiated && !RootFile.Contains("KAZU") ) signal.setRange("fitrange",0.7*maxCent,200.); */
    /* if(!isIrradiated && RootFile.Contains("KAZU") ) signal.setRange("fitrange",0.7*maxCent,200.); */
    /* if(isIrradiated && !RootFile.Contains("KAZU") ) signal.setRange("fitrange",10.,200.); */

    // SETTINGS: Next two lines change fit range for landgaus curves (determined on ATLAS12 sensor)
    // if(isIrradiated && !RootFile.Contains("KAZU") ) signal.setRange("fitrange",15.,175.); // appropriate for irradiated sample
    // if(isIrradiated && !RootFile.Contains("KAZU") ) signal.setRange("fitrange",0.5*maxCent,200.);
    //if(isIrradiated && !RootFile.Contains("KAZU") ) signal.setRange("fitrange",10,150.);
    if(isIrradiated && !RootFile.Contains("KAZU") ) signal.setRange("fitrange",0.1*maxCent,100.); // original code (comment for irradiated)
    if(isIrradiated && RootFile.Contains("KAZU") ) signal.setRange("fitrange",0.1*maxCent,100.);
    if(!isIrradiated && !RootFile.Contains("KAZU") ) signal.setRange("fitrange",0.1*maxCent,100.);
    if(!isIrradiated && RootFile.Contains("KAZU") ) signal.setRange("fitrange",0.1*maxCent,100.);

    /*
    //Create background out of noise data
    TFile *noiseFile = new TFile(RootFile+".root","READ");
    TH1 *noisehist;

    TString noisename = bias_voltage;
    noisename += "V_hst_pos";
    noisehist = (TH1*) noiseFile->Get(noisename);

    noisehist->Rebin(2);

    if (!noisehist)
    std::cout << "Doesn't point to anything - V = " << V_Bias << std::endl;

    RooDataHist noisedata("noisedata","noisedata",signal,noisehist);
    RooHistPdf backgroundModel("backgroundModel", "backgroundModel", signal, noisedata);
    backgroundModel.setInterpolationOrder(1);
    */

    // Construct landau(signal,ml,sl) ;
    RooRealVar ml("ml","mean landau",maxCent,maxCent - 20,maxCent + 10) ;
    RooRealVar sl("sl","sigma landau",width,1,150) ;
    RooLandau landau("lx","lx",signal,ml,sl) ;

    // Construct gauss(signal,mg,sg)
    RooRealVar mg("mg","mg",0.,-5.,5.);
    mg.setConstant(kTRUE);
    RooRealVar sg("sg","sg",1.,0,100) ;
    //RooFormulaVar sg("sg","0.035*sl",RooArgList(sl));
    // sg.setConstant(kTRUE);
    RooGaussian gauss("gauss","gauss",signal,mg,sg) ;

    RooRealVar mg1("mg1","mg1",maxCent,maxCent-20,maxCent+20);
    RooRealVar sg1("sg1","sg1",1.,1,100);
    RooGaussian gauss1("gauss1","gauss",signal,mg1,sg1) ;

    RooDataHist data("data","hist of signal", signal, histogram);

    // Set #bins to be used for FFT sampling to 10000
    signal.setBins(10000,"cache") ;


    // Construct convoluted pdf lxg(x) = landau(x) (*) gauss(x)
    RooFFTConvPdf lxg("lxg","landau (X) gauss",signal,landau,gauss);
    if(V_Bias >= 100 || !isIrradiated) lxg.fitTo(data,Range("fitrange"));

    //RooRealVar Nsignal("Ns","Number Signal",histogram->Integral(),0.,1e6);
    // RooRealVar Nbackground("Nb","Number background",noisehist->Integral(),0.,1e6);
    /*
      RooRealVar bkgfrac("bkgfrac","fraction of background",0.3,0.,0.5);
      RooAddPdf blxg("blxg"," Background + landau (X) gauss", RooArgList(backgroundModel,lxg), RooArgList(bkgfrac));
    */
    // int maxbin = noisehist->FindBin(249);

    // double BGNorm = noisehist->Integral(1,maxbin);
    // double SIGNorm = histogram->Integral(1,maxbin);

    // bkgfrac.setVal(BGNorm/(BGNorm+SIGNorm));
    // bkgfrac.setConstant(kTRUE);


    // lxg.fitTo(data,Range("fitrange"));

    if(V_Bias < 100 && isIrradiated)
      {
	signal.setRange("narrowrange",maxCent-15,maxCent+15);
	gauss1.fitTo(data,Range("narrowrange"));
      }

    // Plot data, landau pdf, landau (X) gauss pdf
    RooPlot* frame = signal.frame(Title(" ")/*,Bins(20)*/) ;
    data.plotOn(frame,MarkerSize(0.5)) ;


    frame->SetYTitle("Events/2 ADC");
    if((V_Bias >= 100 && isIrradiated) || !isIrradiated) lxg.plotOn(frame,Components(lxg),LineColor(kRed),LineStyle(kDashed));
    if(V_Bias < 100 && isIrradiated) gauss1.plotOn(frame,Components(gauss1),LineColor(kRed),LineStyle(kDashed));


    /* blxg.plotOn(frame) ;
       blxg.plotOn(frame,Components(backgroundModel),LineColor(kGreen),LineStyle(kDashed));
       blxg.plotOn(frame,Components(lxg),LineColor(kRed),LineStyle(kDashed));*/

    // Draw frame on canvas
    TCanvas *landgausfit = new TCanvas("rf208_convolution","rf208_convolution",600,600) ;
    gPad->SetLeftMargin(0.15) ; frame->GetYaxis()->SetTitleOffset(1.8) ; frame->Draw() ;

    std::cout<<"test"<<std::endl;
    
    if(V_Bias < 100 && isIrradiated)
      {
	ml.setVal(mg1.getVal());
	ml.setError(mg1.getError());
	sl.setVal(sg1.getVal());
	sl.setError(sg1.getError());
      }

    // SETTINGS: Next two lines change error in CCE plot
    CentError = fabs(maxCent-ml.getVal()); // Use 2 point systematic on landgaus-landau
    // CentError = ml.getError(); // Use statistical error from fit
    maxCent = ml.getVal();


    TString StringInc1 = "Peak = ";
    StringInc1 += stringify(ml.getVal(),1);
    StringInc1 += " +/- ";
    StringInc1 += stringify(ml.getError(),1);
    StringInc1 += " ADC ";
    TString StringInc2 = "Width = ";
    StringInc2 += stringify(sl.getVal(),1);
    StringInc2 += " +/- ";
    StringInc2 += stringify(sl.getError(),1);
    StringInc2 += " ADC ";
    TString Voltage = "Bias Voltage = ";
    Voltage += V_Bias;
    Voltage += "V";

    TString s_irradiated;
    if(isIrradiated) s_irradiated = "POST irradiation";
    if(!isIrradiated) s_irradiated = "PRE irradiation";

    TLatex* label1 = new TLatex(0.45,0.76,RootFile);
    label1->SetNDC();
    label1->SetTextSize(0.035);
    label1->SetTextFont(42);
    label1->Draw();
    TLatex* label2 = new TLatex(0.45,0.72,Voltage);
    label2->SetNDC();
    label2->SetTextSize(0.035);
    label2->SetTextFont(42);
    label2->Draw();
    TLatex* label3 = new TLatex(0.45,0.68,StringInc1);
    label3->SetNDC();
    label3->SetTextSize(0.035);
    label3->SetTextFont(42);
    label3->Draw();
    TLatex* label4 = new TLatex(0.45,0.64,StringInc2);
    label4->SetNDC();
    label4->SetTextSize(0.035);
    label4->SetTextFont(42);
    label4->Draw();
    TLatex* label5 = new TLatex(0.45,0.6,s_irradiated);
    label5->SetNDC();
    label5->SetTextSize(0.035);
    label5->SetTextFont(42);
    label5->Draw();

    frame->Write( bias_voltage + "V_landgausfit_frame");
    landgausfit->Write( bias_voltage + "V_landgausfit");
    landgausfit->SaveAs(AnalysisDir + "/" + RootFile + "_" + Status + "/" + bias_voltage + "V_landgausfit.png");
    landgausfit->Close();
    // noiseFile->Close();
  }




  void ClusterwidthPlots(TString ADir, TString Name, TString Status, TString StatusEntry)
  {
    TFile *WidthFile = new TFile(ADir +"/" + Name+ "_" + StatusEntry + "/ClusterSize.root","READ");

    TGraph *g_oneHitFrac = (TGraph*) WidthFile->Get("onehit");
    TGraph *g_twoHitFrac =(TGraph*) WidthFile->Get("twohit");
    TGraph *g_threeHitFrac =(TGraph*) WidthFile->Get("threehit");
    TGraph *g_fourHitFrac = (TGraph*) WidthFile->Get("fourhit");
    TGraph *g_fourOrMoreHitFrac = (TGraph*) WidthFile->Get("fourormorehit");
    /* TGraph *g_fiveHitFrac = (TGraph*) WidthFile->Get("fivehit"); */

    if (!g_oneHitFrac)
      std::cout << "Doesn't point to anything " << std::endl;

    TCanvas *widthcan = new TCanvas("widthcan","widthcan",800,600);

    g_oneHitFrac->GetXaxis()->SetTitle("Bias Voltage[V]");
    g_oneHitFrac->GetYaxis()->SetTitle("Relative cluster size");
    g_oneHitFrac->SetMarkerColor(1);
    g_oneHitFrac->SetLineColor(1);
    g_oneHitFrac->SetMaximum(1);
    g_oneHitFrac->SetMinimum(0);
    g_oneHitFrac->Draw("ALP");

    g_twoHitFrac->SetMarkerColor(2);
    g_twoHitFrac->SetLineColor(2);
    g_twoHitFrac->SetLineWidth(2);
    g_twoHitFrac->Draw("LP");

    g_threeHitFrac->SetMarkerColor(3);
    g_threeHitFrac->SetLineColor(3);
    g_threeHitFrac->SetLineWidth(2);
    g_threeHitFrac->Draw("LP");

    g_fourHitFrac->SetMarkerColor(4);
    g_fourHitFrac->SetLineColor(4);
    g_fourHitFrac->SetLineWidth(2);
    g_fourHitFrac->Draw("LP");

    g_fourOrMoreHitFrac->SetMarkerColor(6);
    g_fourOrMoreHitFrac->SetLineColor(6);
    g_fourOrMoreHitFrac->SetLineWidth(2);
    g_fourOrMoreHitFrac->Draw("LP");

    /* g_fiveHitFrac->SetMarkerColor(6); */
    /* g_fiveHitFrac->SetLineColor(6); */
    /* g_fiveHitFrac->SetLineWidth(2); */
    /* g_fiveHitFrac->Draw("LP"); */


    TLegend* leg = new TLegend(0.65,0.65,0.90,0.87);
    leg->AddEntry(g_oneHitFrac,"1 Hit","L P");
    leg->AddEntry(g_twoHitFrac,"2 Hits","L P");
    leg->AddEntry(g_threeHitFrac,"3 Hit","L P");
    leg->AddEntry(g_fourHitFrac,"4 Hits","L P");
    leg->AddEntry(g_fourOrMoreHitFrac,">4 Hit","L P");
    /* leg->AddEntry(g_fiveHitFrac,"5 Hit","L P"); */
    leg->SetBorderSize(0);
    leg->SetFillColor(0);
    leg->Draw();
    std::string outputDirectory3 = (ADir + "/Combined/"+ Name + "/").Data();
    boost::filesystem::create_directory(outputDirectory3);


    TFile *clustsizeFile = new TFile(ADir+"/Combined/relClust_Store.root", "UPDATE");
    widthcan->Write(Name + "_" + Status,TObject::kOverwrite);

    widthcan->SaveAs( ADir + "/Combined/"+ Name + "/" + Name+"_"+Status+"_relativeClusterSize.png");
    widthcan->Close();

    clustsizeFile->Close();

  }



  void Plotter(std::vector<TGraphErrors*> graph,std::vector<TString> label, std::vector<TString> sensor, TString Title, TString AnalysisDir, std::vector<long double> Temperature)
  {
    TCanvas *can = new TCanvas(Title,Title,800,600);

    double LegHeight = 0.028*graph.size();

    TAxis *axisX = graph[0]->GetXaxis();
    TAxis *axisY = graph[0]->GetYaxis();

    TLegend* leg = new TLegend(0.175,0.92-LegHeight,0.9,0.92);

    axisX->SetTitle("Bias Voltage [V]");
    axisX->CenterTitle();
    axisX->SetLimits(0,1000);
    axisY->SetTitle("Collected Charge [Ke]");
    axisY->CenterTitle();

    leg->SetFillStyle(0);
    leg->SetNColumns(1);

    can->SetGrid();

    bool justCurve = false;

    TLine * RedLine = new TLine(0.0,0.0,0.0,0.0);
    RedLine->SetLineColor(kRed);
    RedLine->SetLineWidth(2);

    if(Title.Contains("ASC_2") || Title.Contains("ASC_3") ) justCurve = true;

    // PRE_hist->SetMarkerStyle(1);
    //PRE_hist->SetMarkerSize(1);
    graph[0]->SetMaximum(40.);
    graph[0]->SetMinimum(0.);
    graph[0]->SetMarkerColor(1);
    graph[0]->SetLineColor(1);
    //axisX -> SetMaximum(1000);
    //graph[0]->SetMarkerStyle(21);
    if(!justCurve) leg->AddEntry(graph[0],label[0],"P");


    if(!justCurve) { graph[0]->Draw("ap"); }
    else {
      graph[0]->Draw("al");
      graph[0]->SetLineColor(kWhite);
    }
    //  leg->AddEntry(RedLine,"A12 Nominal Response","L");


    /*for(int j=0;j<graph[0]->GetN();j++) graph[0]->GetY()[j] *= 0.202*correction[0];//ADC to ke conversion of 1ADC = 202e
    //after first run at 32C, looks like needs to larger ny factor 21/17	

    for(size_t i = 1; i < graph.size();i++)
      {

	for(int j=0;j<graph[i]->GetN();j++) graph[i]->GetY()[j] *= 0.202*correction[i];

	if (i<14)
	  {


	    if(label[i].Contains("PostAnneal",TString::kExact) && sensor[i] == sensor[i+1])
	      {


		TString tmp_label = label[i];
		TGraphErrors* tmp_graph = graph[i];


		graph[i] = graph[i+1];
		graph[i+1] = tmp_graph;
		label[i] = label[i+1];
		label[i+1] = tmp_label;
	      }


	      if(!label[i].Contains("PostAnneal",TString::kExact)//i%2 != 0)

	      {
		int k;
		if(i==4) k=i+2;
		else k=i+1;
		std::cout<<" i & k = "<<i<<" , "<<k<<std::endl;
		graph[i]->SetMarkerColor(k);
		graph[i]->SetLineColor(k);

	      }



	    //if(!label[i].Contains("Unirradiated",TString::kExact)&& sensor[i] != sensor[i+1])//stops this applying to everything
	    //
	    //{
	    //
	    //graph[i]->SetMarkerColor(i);
	    //graph[i]->SetMarkerStyle(21);
	    //
	    //}


	    else if(sensor[i]==sensor[i-1])
	      {

		graph[i]->SetMarkerColor(graph[i-1]->GetMarkerColor());
		graph[i]->SetLineColor(graph[i-1]->GetLineColor());

		graph[i]->SetMarkerStyle(21);

	      }


	    else
	      {
		int k;
		if(i==4) k=i+63;
		else k=i+1;
		std::cout<<" i & k = "<<i<<" , "<<k<<std::endl;
		graph[i]->SetMarkerColor(k);
		graph[i]->SetLineColor(k);

		graph[i]->SetMarkerStyle(21);


	      }


	  }

	graph[i]->Draw("p");

	leg->AddEntry(graph[i],label[i],"P");

      }



    leg->SetTextSize(0.025);
    leg->SetBorderSize(0);
    leg->SetFillColor(0);
    leg->Draw();


    can->SaveAs(AnalysisDir+"/Combined/"+ Title+".png");
    can->SaveAs(AnalysisDir+"/Combined/"+ Title+".eps");
    can->SaveAs(AnalysisDir+"/Combined/"+ Title+".C");
    can->Close();
    std::cout<<"Plotter Function is doing something"<<std::endl;*/
  }






  std::vector<std::string> return_file_names(std::string path){


    path += "/";
    std::vector<std::string> fileNames;
    std::string currentFileName;
    std::ifstream fileList(path+"FileList");

    while(std::getline(fileList, currentFileName)){
      std::cout << currentFileName;
      if (contains_substring(currentFileName, "FileList")) continue;
      if (contains_substring(currentFileName, "RSRun", ignore::lowercase)){
	fileNames.push_back(path+currentFileName);
	std::cout << fileNames.back() << "\n";
      }
    }
    return fileNames;
  }

 signals:

  public slots:
   void alibava_reader(){
     std::ofstream debuggingOutput ("debuggingOutputALiBaVaReader.txt", std::ofstream::out);

     confirmSettings->setText("Settings Confirmed");
     std::string pathToFileList = input_pathToFileList->text().toStdString();
     std::string pathToAnalysisDirectory = input_pathToAnalysisDirectory->text().toStdString();
     std::string sensorName = input_sensorName->text().toStdString();
     int statusEntry = (statusOfSensor->currentIndex());
     double seedCut = std::stod(input_seedCut->text().toStdString());
     double neighbourCut = std::stod(input_neighbourCut->text().toStdString());
     int minimumClusterSize = std::stoi(input_minimumClusterSize->text().toStdString());
     double lowerTimeCut = std::stod(input_lowerTimeCut->text().toStdString());
     double upperTimeCut = std::stod(input_upperTimeCut->text().toStdString());
     std::string temperatureString = input_Temperature->text().toStdString();
     std::string PlotTitleString = input_plotTitle->text().toStdString();
     std::ofstream previousSettings ("previousSettings.txt", std::ofstream::out);
     int SensorTypeEntry = (SensorType->currentIndex());

     previousSettings << pathToFileList << "\n";
     previousSettings << pathToAnalysisDirectory << "\n";
     previousSettings << sensorName << "\n";
     previousSettings << statusEntry << "\n";
     previousSettings << seedCut << "\n";
     previousSettings << neighbourCut << "\n";
     previousSettings << minimumClusterSize << "\n";
     previousSettings << lowerTimeCut << "\n";
     previousSettings << upperTimeCut << "\n";
     previousSettings << temperatureString << "\n";
     previousSettings << PlotTitleString << "\n";
     previousSettings << SensorTypeEntry << "\n";

     previousSettings.close();



     std::vector<std::string> fileNames = return_file_names(pathToFileList);

     std::string outputDirectory1 = pathToAnalysisDirectory;
     boost::filesystem::create_directory(outputDirectory1);

     std::string outputDirectory = pathToAnalysisDirectory + "/" + sensorName+"_"+std::to_string(statusEntry);
     bool outputDirectoryAlreadyExists{false};

     if (boost::filesystem::exists(outputDirectory)) outputDirectoryAlreadyExists = true;

     boost::filesystem::create_directory(outputDirectory);
     std::ofstream Settings (outputDirectory+"/Settings.txt", std::ofstream::out);
     Settings << "Path To File List:" << pathToFileList << "\n";
     Settings << "Path To Analysis Directory:" << pathToAnalysisDirectory << "\n";
     Settings << "Sensor Name:"  << sensorName << "\n";
     Settings << "status Entry:"<< statusEntry << "\n";
     Settings << "Seed Cut:" << seedCut << "\n";
     Settings << "Neighbour Cut:" << neighbourCut << "\n";
     Settings << "Minimum Cluster Size:" << minimumClusterSize << "\n";
     Settings << "Lower Time Cut:" << lowerTimeCut << "\n";
     Settings << "Upper Time Cut:" << upperTimeCut << "\n";
     Settings << "Temperature:" << temperatureString << "\n";
     Settings << "Sensor Type:" << SensorTypeEntry << "\n";
     Settings.close();


     std::vector<long double> Temperature;
     std::stringstream ss(temperatureString);
     long double templongdouble;
     while (ss >> templongdouble)
       {
	 Temperature.push_back(templongdouble);
	 if (ss.peek() == ',') ss.ignore();
       }


     TFile *stripFile;
     TFile* outputFile;
     if (!outputDirectoryAlreadyExists){
       stripFile = new TFile((outputDirectory + "/strip.root").c_str(), "RECREATE");
       outputFile = new TFile( (outputDirectory+"/RAW_"+sensorName+".root").c_str(), "RECREATE");
     }
     std::vector<TH1*> hst_hists;
     std::vector<TH1*> h_width_hists;
     std::vector<int> V_Bias;
     for (auto it=fileNames.begin();it!=fileNames.end() && !outputDirectoryAlreadyExists;++it){

       double maxSignal = 500.0;
       std::string channels = "0-127";
       //std::string channels = "175-235";
       std::string ped_f = "alibava_ped.ped";

       std::string inputFile = *it;
       debuggingOutput << "Input File:" <<  inputFile << "\n";
       currentEvent->setText(QString::fromStdString("Initializing"));
       currentFile->setText(QString::fromStdString(inputFile));
       repaint();

       TString TString_inputFile = inputFile;
       TString biasVoltage = TString_inputFile(TString_inputFile.Last('_') + 1, TString_inputFile.Last('V') - TString_inputFile.Last('_') - 1);
       V_Bias.push_back(atoi(biasVoltage.Data()));
       std::cout.setstate(std::ios_base::failbit);
       DataFileRoot* A = DataFileRoot::OpenFile(inputFile.c_str());//gives annoying unnecessary cout
       std::cout.clear();

       TH2D *hRawADC = new TH2D((biasVoltage + "V_hRawADC"), "Raw ADC Data", 256, 0, 256,300,0,1000);
       hRawADC->GetXaxis()->SetTitle("Channel No.");
       hRawADC->GetYaxis()->SetTitle("ADC_Value[ADC]");

       TH2D *hPedSubADC = new TH2D((biasVoltage + "V_hPedSubADC"), "Ped Sub ADC Data", 256, 0, 256,300,-500,500);
       hPedSubADC->GetXaxis()->SetTitle("Channel No.");
       hPedSubADC->GetYaxis()->SetTitle("ADC_Value[ADC]");


       TH2D *hCMSADC = new TH2D((biasVoltage + "V_hCMSADC"), "Common Mode Subtraction ADC Data", 256, 0, 256,300,-500,500);
       hCMSADC->GetXaxis()->SetTitle("Channel No.");
       hCMSADC->GetYaxis()->SetTitle("ADC_Value[ADC]");



       TH1D *hRMSNoiseRawADC = new TH1D((biasVoltage + "V_hRMSNoiseRawADC"), "RMS Noise Vs Channel, Raw ADC", 256, 0, 256);
       hRMSNoiseRawADC->GetXaxis()->SetTitle("Channel No.");
       hRMSNoiseRawADC->GetYaxis()->SetTitle("Noise[ADC]");

       TH1D *hRMSNoiseCMSADC = new TH1D((biasVoltage + "V_hRMSNoiseCMSADC"), "RMS Noise Vs Channel, CMS ADC", 256, 0, 256);
       hRMSNoiseCMSADC->GetXaxis()->SetTitle("Channel No.");
       hRMSNoiseCMSADC->GetYaxis()->SetTitle("Noise[ADC]");



       TH1D *hTime = new TH1D((biasVoltage + "V_hTime"), "Time Profile", 100, 0, 100);
       hTime->GetXaxis()->SetTitle("Time [ns]");
       hTime->GetYaxis()->SetTitle("Frequency");


       TH1D *hClusterPos = new TH1D((biasVoltage + "V_hClusterPos"), "Cluster Position", 256, 0, 256);
       hClusterPos->GetXaxis()->SetTitle("Channel No.");
       hClusterPos->GetYaxis()->SetTitle("Frequency");


       TH1D *hClusterCharge = new TH1D((biasVoltage + "V_hClusterCharge"), "Cluster Charge", 800, 0, 800);
       hClusterCharge->GetXaxis()->SetTitle("Cluster Charge [ADC]");
       hClusterCharge->GetYaxis()->SetTitle("Frequency");

       TH2D *hClusterVsTime = new TH2D((biasVoltage + "V_hClusterVsTime"), "Cluster Charge vs Time", 100, 0, 100,400,0,400);
       hClusterVsTime->GetXaxis()->SetTitle("Time[ns]");
       hClusterVsTime->GetYaxis()->SetTitle("Cluster Charge[ADC]");

       TH1D *hClusterVsTimeProjection = new TH1D((biasVoltage + "V_hClusterChargeVsTimeProjection"), "Cluster Charge Vs Time Projection", 400, 0, 400);
       hClusterVsTimeProjection->GetXaxis()->SetTitle("Cluster Charge [ADC]");
       hClusterVsTimeProjection->GetYaxis()->SetTitle("Frequency");

       TH1D *hClusterSize = new TH1D((biasVoltage + "V_hClusterSize"), "Cluster Size", 5, 0, 5);
       hClusterSize->GetXaxis()->SetTitle("Cluster Size");
       hClusterSize->GetYaxis()->SetTitle("Frequency");

       TH1D *hclust = new TH1D((biasVoltage + "V_hclust"), "Number of clusters", 10, -0.5, 9.5);
       hclust->GetXaxis()->SetTitle("Number of Clusters");
       hclust->GetYaxis()->SetTitle("Counts");
       TH1D *hcmmd = new TH1D((biasVoltage + "V_hcmmd"), "Common Mode", 200, -100.0, 100.0);
       hcmmd->GetXaxis()->SetTitle("Common Mode");
       hcmmd->GetYaxis()->SetTitle("Counts");

       TH1D* hst = new TH1D((biasVoltage + "V_hst"), "spectrum", 250, 0., maxSignal);
       hst->GetXaxis()->SetTitle("ADC");
       hst->GetYaxis()->SetTitle("Counts");

       TH1D *hst_pos = new TH1D((biasVoltage + "V_hst_pos"), "spectrum Positive Signal", 250, 0., maxSignal);
       hst_pos->GetXaxis()->SetTitle("ADC");
       hst_pos->GetYaxis()->SetTitle("Counts");

       TH2D *htim = new TH2D((biasVoltage + "V_htim"), "Time .vs. signal", 50, 0., 100., 512, 0, maxSignal);
       htim->GetXaxis()->SetTitle("Time[ns]");
       htim->GetYaxis()->SetTitle("Cluster Charge [ADC]");

       TProfile *hprof = new TProfile((biasVoltage + "V_hprof"), "Time", 50, 0., 100., -maxSignal, maxSignal);
       hprof->GetXaxis()->SetTitle("Time[ns]");
       hprof->GetYaxis()->SetTitle("Cluster Charge [ADC]");

       TH2D *hchan = new TH2D((biasVoltage + "V_hchan"), "Channel .vs. signal", A->nchan(), -0.5, A->nchan()-0.5, 512, -0, maxSignal);
       hchan->GetXaxis()->SetTitle("CHANNEL Number");
       hchan->GetYaxis()->SetTitle("Cluster Charge [ADC]");

       TH2D *hall = new TH2D((biasVoltage + "V_hall"), "Signal (no cuts)", A->nchan(), -0.5, A->nchan()-0.5, 300, -maxSignal, maxSignal);
       hall->GetXaxis()->SetTitle("CHANNEL Number");
       hall->GetYaxis()->SetTitle("Cluster Charge [ADC]");

       TH2D *hchanwidth = new TH2D((biasVoltage + "V_hchanwidth"), "Channel Cluster Widths", A->nchan(), -0.5, A->nchan()-0.5, 15, 0., 15.);
       hchanwidth->GetXaxis()->SetTitle("Channel Number");
       hchanwidth->GetYaxis()->SetTitle("Cluster Width");

       TH1D *hwidth = new TH1D((biasVoltage + "V_hwidth"), "Cluster width", 10, -0.5, 9.5);
       hwidth->GetXaxis()->SetTitle("Cluster Width");
       hwidth->GetYaxis()->SetTitle("Counts");

       TH1D *hprecutwidth = new TH1D((biasVoltage + "V_hprecutwidth Before Cuts"), "Cluster width", 10, -0.5, 9.5);
       hprecutwidth->GetXaxis()->SetTitle("Cluster Width");
       hprecutwidth->GetYaxis()->SetTitle("Counts");

       TH1D *hstrips[A->nchan()];

       TH1D *htotal=new TH1D((biasVoltage + "V_" + "htotal"), "", 250, 0.,maxSignal);
       htotal->GetXaxis()->SetTitle("Cluster Charge [ADC]");
       htotal->GetYaxis()->SetTitle("Counts");

       for(int j=0; j<A->nchan(); j++)
	 {
	   TString ChanNum; ChanNum.Form("%d",j);
	   hstrips[j] = new TH1D((biasVoltage + "V_" + ChanNum), "",A->nchan(),-2*maxSignal, 0);
	 }
       debuggingOutput << "Histograms Initialized";

       ////////////////////////////CUTS//////////////////////////
       //seedCut is the threshold a single channel needs to be to start a cluster,
       //neighbourCut is the threshold a neighbouring channels needs to be to be added to a cluster,
       //these are both how much above the noise of the channel they must be.
       //timecut is the time between the signal being received by the scintillator+pmt trigger and the signal arriving at the sensors (possibly nanoseconds)

       A->set_cuts(seedCut, neighbourCut);
       double t1{lowerTimeCut}, t2{upperTimeCut};// set to some arbitary value
       A->set_timecut(t1, t2);
       ///////////////////////////////////////////////////////////
       A->save();
       A->rewind();

       std::cout.setstate(std::ios_base::failbit);
       A->compute_pedestals(-1,true); //gives annoying unnecessary cout
       std::cout.clear();



       A->save_pedestals(ped_f.c_str());

       TCanvas *pcnvs = (TCanvas *)gROOT->FindObject("pcnvs");
       if (pcnvs)  delete pcnvs;

       pcnvs = new TCanvas("pcnvs","Pedestals and Noise");
       pcnvs->Divide(1, 2);
       pcnvs->cd(1);
       A->show_pedestals()->DrawCopy();
       pcnvs->cd(2);
       A->show_noise()->DrawCopy();
       pcnvs->Update();
       A->restore();
       outputFile->cd();
       pcnvs->Write();
       A->load_pedestals(ped_f.c_str());
       
       ChanList channelList = ChanList(channels.c_str());
       A->common_mode(channelList,true);


       int currentEventNumber{0}, LowerProjectionTimeCut{0},UpperProjectionTimeCut{0};
       while(!A->read_event()){ //read_event returns 0 if event succesfully ran.
	 ++currentEventNumber;
	 UpperProjectionTimeCut = LowerProjectionTimeCut+2;
	 int j{0},k{0};

	 A->process_event(false); //argument is do_cmmd subtraction or not

	 for (int i=0;i<channelList.Nch();++i){
	   //the signal searched for is negative so in cases where it is compared to noise, multiply by -1 to make it positive like noise is by default.
	   //NOTE: do NOT Just use fabs(signal). The signal can be positive but we do not want positive signal.
	   double signal{A->signal(channelList[i])};

	   debuggingOutput << "Channel:" << i << ":" << channelList[i];

	   hall->Fill(channelList[i], -1.0*signal);
	   hRawADC->Fill(channelList[i],A->data(channelList[i]));
	   hPedSubADC->Fill(channelList[i],A->data(channelList[i])-A->ped(channelList[i]));
	   hCMSADC->Fill(channelList[i],A->data(channelList[i])-A->get_cmmd(channelList[i])-A->ped(channelList[i]));
	   hRMSNoiseRawADC->SetBinContent(channelList[i]+1,A->noise(channelList[i]));
	   hRMSNoiseCMSADC->SetBinContent(channelList[i]+1,A->noise(channelList[i])-A->get_cmmd(channelList[i]));
	   hTime->Fill(A->time());

	   hstrips[i]->Fill(-1.0*A->data(channelList[i]));

	   if(-1.0*signal < seedCut*A->noise(channelList[i])) continue;

	   if (true){
	     //The first for loop goes up from a channel that passes the seedcut, the second down.
	     //They continue until they reach a channel that does not pass the neighbourCut, creating a cluster.
	     int clusterSize{0};
	     for (j=1;j<128;++j){
	       if(i+j>127) break;
	       double neighsignal = A->signal(channelList[i+j]);
	       double neighnoise = A->noise(channelList[i+j]);
	       if(fabs(neighsignal/neighnoise) < neighbourCut || neighsignal >= 0. || neighnoise <= 0.) break;
	       signal += neighsignal;
	       ++clusterSize;
	     }
	     for (k=1;k<128;++k){
	       if(i-k < 0) break;
	       double neighsignal = A->signal(channelList[i-k]);
	       double neighnoise = A->noise(channelList[i-k]);
	       if(fabs(neighsignal/neighnoise) < neighbourCut || neighsignal >= 0. || neighnoise <=0.) break;
	       signal += neighsignal;
	       ++clusterSize;
	     }

	     if(clusterSize<minimumClusterSize) continue; //tiny clusters are just noise.
	     htotal->Fill(-1.0*signal);
	     i=i+j-1; //skip for loop past any channel in found cluster.
	   }
	 }

          /*  if (true){ */
	 /*     //The first for loop goes up from a channel that passes the seedcut, the second down. */
	 /*     //They continue until they reach a channel that does not pass the neighbourCut, creating a cluster. */
	 /*     int clusterSize{0}; */
	 /*     for (j=175;j<235;++j){ */
	 /*       if(i+j>234) break; */
	 /*       double neighsignal = A->signal(channelList[i+j]); */
	 /*       double neighnoise = A->noise(channelList[i+j]); */
	 /*       if(fabs(neighsignal/neighnoise) < neighbourCut || neighsignal >= 0. || neighnoise <= 0.) break; */
	 /*       signal += neighsignal; */
	 /*       ++clusterSize; */
	 /*     } */
	 /*     for (k=175;k<235;++k){ */
	 /*       if(i-k < 0) break; */
	 /*       double neighsignal = A->signal(channelList[i-k]); */
	 /*       double neighnoise = A->noise(channelList[i-k]); */
	 /*       if(fabs(neighsignal/neighnoise) < neighbourCut || neighsignal >= 0. || neighnoise <=0.) break; */
	 /*       signal += neighsignal; */
	 /*       ++clusterSize; */
	 /*     } */

	 /*     if(clusterSize<minimumClusterSize) continue; //tiny clusters are just noise. */
	 /*     htotal->Fill(-1.0*signal); */
	 /*     i=i+j-1; //skip for loop past any channel in found cluster. */
	 /*   } */
         /* } */

	 //====================================
	 //======== ALiBaVa Processing ========
	 //=================================std::vector<long double> Temperature===

	 A->polarity(-1);
	 A->find_clusters( SensorTypeEntry ); //0 for new sensors, 1 for old.
	 hclust->Fill(A->nhits());

	 bool GoodStrip{false};
	 for (HitList::iterator ip=A->begin(); ip!=A->end(); ++ip){
	   Hit &h =*ip;

	   //remove first and last in each chip
	   if (!h.center()%128) continue;
	   //if (!h.center()%235) continue;

	   hprecutwidth->Fill(h.width());

	   if ((A->time()<lowerTimeCut||A->time()>upperTimeCut) && contains_substring(sensorName,"KAZU")) continue;  //Set time cut 15 25 original and working
	   if ((A->time()<lowerTimeCut||A->time()>upperTimeCut)  && !contains_substring(sensorName,"KAZU")) continue;
	   hchanwidth->Fill(h.center(),h.width());
	   hClusterPos->Fill(h.center());
	   if (A->valid_time(A->time())) {hwidth->Fill(h.width()); hClusterSize->Fill(h.width());}
	   if (-1.0*h.signal()>hst_pos->GetBinCenter(hst_pos->GetNbinsX())) continue;
	   if (A->valid_time(A->time())) hchan->Fill(h.center(), -1.0*h.signal());
	   GoodStrip = true;
	   hst->Fill(-1.0*h.signal());
	   hClusterCharge->Fill(-1.0*h.signal());
	   hClusterVsTime->Fill(A->time(),-1.0*h.signal());
	   hClusterVsTimeProjection = hClusterVsTime->ProjectionY("V_hClusterChargeVsTimeProjection",LowerProjectionTimeCut,UpperProjectionTimeCut);

	   htim->Fill(A->time(), -1.0*h.signal());
	   hprof->Fill(A->time(), -1.0*h.signal());
	 }
	 if(GoodStrip) hcmmd->Fill(A->get_cmmd(1));
	 if (!(currentEventNumber%100)) {
	   currentEvent->setText(QString::fromStdString(std::to_string(currentEventNumber)));
	   // Plot hst in real time
	   QVector<double> x, y;
	   for (int i=0; i<hst->GetNbinsX(); ++i)
	     {
	       x.push_back(hst->GetBinCenter(i));
	       y.push_back(hst->GetBinContent(i));
	     }
	   QVector<double> x1, y1;
	   for (int i=0; i<hClusterVsTimeProjection->GetNbinsX(); ++i)
	     {
	       x1.push_back(hClusterVsTimeProjection->GetBinCenter(i));
	       y1.push_back(hClusterVsTimeProjection->GetBinContent(i));
	     }

	   histogram->graph(0)->setData(x,y);

                       
	   Qhtotal->graph(0)->setData(x1,y1);
	   QString QhtotalXTitle;
	   std::string tempTimeLimits;
	   int binmax = hClusterVsTimeProjection->GetMaximumBin();
	   double binMaxX = hClusterVsTimeProjection->GetXaxis()->GetBinCenter(binmax);

	   tempTimeLimits = "ADC, TimeCut: " + std::to_string(LowerProjectionTimeCut) + "_" + std::to_string(UpperProjectionTimeCut) + " ,Mean:" + std::to_string(hClusterVsTimeProjection->GetMean()) + ", Peak:" + std::to_string(binMaxX);
	   QhtotalXTitle = QString::fromStdString(tempTimeLimits);
	   Qhtotal->xAxis->setLabel(QhtotalXTitle);
                        
                        
	   hClusterVsTime->Draw();
	   //TImage *img = TImage::Create();
	   //img->FromPad(c);
	   //img->WriteImage("temp.png");
	   //QPixmap ClusterTimePictureImage;
	   //ClusterTimePictureImage.load("temp.png");
	   //ClusterTimePicture->setPixmap(ClusterTimePictureImage);
	   Qhtotal->replot();
	   histogram->replot();
	   repaint();
	   if (!(currentEventNumber%4000)) {
	     ++LowerProjectionTimeCut;
	   }
	 }
	 if (LowerProjectionTimeCut==25){
	   LowerProjectionTimeCut=0;
	 }
       }
       TDirectory *currentBiasDirectory = outputFile->mkdir(biasVoltage);
       currentBiasDirectory->cd();

       hRawADC->Write();
       hPedSubADC->Write();
       hCMSADC->Write();
       hRMSNoiseRawADC->Write();
       hRMSNoiseCMSADC->Write();
       hTime->Write();
       hClusterPos->Write();
       hClusterCharge->Write();
       hClusterVsTime->Write();
       hClusterVsTimeProjection->Write();
       hClusterSize->Write();



       hclust->Write();
       hclust->SetStats(0);
       hcmmd->Write();
       hcmmd->SetStats(0);
       hst->Write();
       hst->SetStats(0);
       hst_pos->Write();
       hst_pos->SetStats(0);
       htim->Write();
       htim->SetStats(0);
       hprof->Write();
       hprof->SetStats(0);
       hchan->Write();
       hchan->SetStats(0);
       hall->Write();
       hall->SetStats(0);
       hchanwidth->Write();
       hchanwidth->SetStats(0);
       hwidth->Write();
       hwidth->SetStats(0);
       hprecutwidth->Write();
       hprecutwidth->SetStats(0);
       htotal->SetStats(0);
       htotal->Write();

       int maxi = hwidth->GetNbinsX();

       double width, volts;

       std::ofstream f_widths;
       f_widths.open(outputDirectory+"/"+biasVoltage+"V_width_output_PF.txt");

       for(int ii=0; ii< maxi+2; ii++ ){
	 //CCE_hist->GetPoint(i, volts, charges);
	 f_widths << hwidth->GetBinCenter(ii);
	 f_widths <<"\t";
	 f_widths << hwidth->GetBinContent(ii);
	 f_widths<< "\n";
       }
		
       f_widths.close();

       hst_hists.push_back(hst);
       h_width_hists.push_back(hwidth);

       stripFile->cd();
       TDirectory *currentStripDirectory = stripFile->mkdir(biasVoltage);
       currentStripDirectory->cd();

        for(int j=0; j<128; j++){ 
          hstrips[j]->SetStats(0);
          hstrips[j]->Write(); 
        } 
       
       //for(int j=175; j<235; j++){
       // hstrips[j]->SetStats(0);
       // hstrips[j]->Write();
       //}       

       delete A;
     }
     if (!outputDirectoryAlreadyExists){
       stripFile->Close();
       delete stripFile;
       outputFile->Close();
       delete outputFile;
     }
     if (outputDirectoryAlreadyExists) currentFile->setText(QString::fromStdString("OUTPUT DIRECTORY ALREADY EXISTS"));
     currentEvent->setText(QString::fromStdString("FINISHED"));
     repaint();

     // /////////////////FITTER///////////////
     //  sensorName,statusEntry,outputDirectory

     std::cout << "\n" << (outputDirectory+"/RAW_"+sensorName+".root").c_str() << "\n";
     TFile* FileWithRawPlots = new TFile((outputDirectory+"/RAW_"+sensorName+".root").c_str(), "READ");
     std::vector<TH1*> hst_histsNew,h_width_histsNew;
     std::vector<int> V_BiasNew;
     loopdir(FileWithRawPlots,V_BiasNew,hst_histsNew,h_width_histsNew);
     /*
       TIter next(FileWithRawPlots->GetListOfKeys());
       TKey *key;
       (FileWithRawPlots->GetListOfKeys())->Print();
       std::vector<TH1*> hst_histsNew,h_width_histsNew;
     */



     /*
       while ((key=(TKey*)next())){
       TIter nextInner(FileWithRawPlots->GetListOfKeys());
       TKey *keyNext;
       (FileWithRawPlots->GetListOfKeys())->Print();




       TString KeyName = key->GetName();
       std::cout << "In Here!!!!" << "\n";
       if(KeyName.EndsWith("_hst")){
       TH1* hstNew = (TH1*) FileWithRawPlots->GetName();
       hst_histsNew.push_back(hstNew);
       }
       if(KeyName.EndsWith("_hwidth")){
       TH1* hwidthNew = (TH1*) FileWithRawPlots->GetName();
       h_width_histsNew.push_back(hwidthNew);
       }
       }
     */

     TFile* FittedPlots = new TFile((outputDirectory+"/PROCESSED_"+sensorName+".root").c_str(),"RECREATE");
     FittedPlots->cd();


     std::vector<double> VoltsVec;
     std::vector<double> PeakVec;
     std::vector<double> PeakErrVec;
     std::vector<double> WidthVec;

     std::vector<double> oneHitFrac,twoHitFrac,threeHitFrac,fourHitFrac,fourOrMoreHitFrac;
     /* std::vector<double> oneHitFrac,twoHitFrac,threeHitFrac,fourHitFrac,fiveHitFrac; */
     std::vector<double> voltsscan;
     double fitStart = 10;
     double fitEnd = 150;
     if (contains_substring(sensorName,"KAZU")) fitEnd=250;

     std::cout << "\n\n\nTEST" << h_width_histsNew.size() << "\n";
     //std::cout << hst_histsNew.at(0)->GetBin(1);

     for (unsigned int i=0; i<hst_histsNew.size();++i){
       TF1* FuncLandau = new TF1("FuncLandau","landau(0)",fitStart,fitEnd);
       hst_histsNew[i]->Fit("FuncLandau","MR");

       double peak = (FuncLandau->GetParameter(1));
       double peakerr = FuncLandau->GetParError(1);
       /* double peak = hst_histsNew[i]->GetXaxis()->GetBinCenter(hst_histsNew[i]->GetMaximumBin()); */
       /* double peakerr = hst_histsNew[i]->GetBinWidth(hst_histsNew[i]->GetMaximumBin()); */
       double width = FuncLandau->GetParameter(2);
       double widtherr = FuncLandau->GetParError(2);

       landgausfit(FittedPlots,hst_histsNew.at(i),peak,peakerr,width,widtherr,(std::to_string(statusEntry)).c_str(),V_BiasNew.at(i),sensorName.c_str(), pathToAnalysisDirectory.c_str());

       VoltsVec.push_back(V_BiasNew.at(i));
       PeakVec.push_back(peak);
       PeakErrVec.push_back(peakerr);
       WidthVec.push_back(width);

       oneHitFrac.push_back( h_width_histsNew.at(i)->GetBinContent(2)/h_width_histsNew.at(i)->Integral());
       twoHitFrac.push_back( h_width_histsNew.at(i)->GetBinContent(3)/h_width_histsNew.at(i)->Integral());
       threeHitFrac.push_back( h_width_histsNew.at(i)->GetBinContent(4)/h_width_histsNew.at(i)->Integral());
       fourHitFrac.push_back( h_width_histsNew.at(i)->GetBinContent(5)/h_width_histsNew.at(i)->Integral());
       fourOrMoreHitFrac.push_back( (h_width_histsNew.at(i)->Integral()-h_width_histsNew.at(i)->GetBinContent(2)-h_width_histsNew.at(i)->GetBinContent(3)-h_width_histsNew.at(i)->GetBinContent(4)-h_width_histsNew.at(i)->GetBinContent(5))/h_width_histsNew.at(i)->Integral());
       /* fiveHitFrac.push_back( h_width_histsNew.at(i)->GetBinContent(6)/h_width_histsNew.at(i)->Integral()); */
       voltsscan.push_back(V_BiasNew.at(i));
     }
     TGraphErrors *Peak_VScan = new TGraphErrors(VoltsVec.size(),&VoltsVec[0],&PeakVec[0],0,&PeakErrVec[0]);
     Peak_VScan->SetName("Peak_VScan");
     TGraphErrors *Width_VScan = new TGraphErrors(VoltsVec.size(),&WidthVec[0],&WidthVec[0],0,&PeakErrVec[0]);
     Width_VScan->SetName("Width_VScan");

     Peak_VScan->Write();
     Width_VScan->Write();
     FittedPlots->Close();


     TGraph *g_oneHitFrac = new TGraph(voltsscan.size(),&voltsscan[0],&oneHitFrac[0]);
     TGraph *g_twoHitFrac = new TGraph(voltsscan.size(),&voltsscan[0],&twoHitFrac[0]);
     TGraph *g_threeHitFrac = new TGraph(voltsscan.size(),&voltsscan[0],&threeHitFrac[0]);
     TGraph *g_fourHitFrac = new TGraph(voltsscan.size(),&voltsscan[0],&fourHitFrac[0]);
     TGraph *g_fourOrMoreHitFrac = new TGraph(voltsscan.size(),&voltsscan[0],&fourOrMoreHitFrac[0]);
     /* TGraph *g_fiveHitFrac = new TGraph(voltsscan.size(),&voltsscan[0],&fiveHitFrac[0]); */

     TFile* WidthSaveFile = new TFile((outputDirectory + "/ClusterSize.root").c_str(), "RECREATE");
     g_oneHitFrac->Write("onehit");
     g_twoHitFrac->Write("twohit");
     g_threeHitFrac->Write("threehit");
     g_fourHitFrac->Write("fourhit");
     g_fourOrMoreHitFrac->Write("fourormorehit");
     /* g_fiveHitFrac->Write("fivehit"); */
     WidthSaveFile->Close();


#ifdef __CINT__
     gROOT->LoadMacro("/home/aliuser/alibava/ALiBaVaReaderGUI/AtlasStyle.C");
#endif
     SetAtlasStyle();
     TString Status;
     if(statusEntry == 0) Status = "Unirradiated";
     else if(statusEntry == 1) Status = "IrradPreAnneal";
     else if(statusEntry == 2) Status = "IrradPostAnneal";


     TFile *SaveFile = new TFile((outputDirectory +"/" + "PROCESSED_"+ sensorName + ".root").c_str(), "READ");

     TGraphErrors* CCE_hist;
     TGraphErrors* WidthScan_hist;

     CCE_hist = (TGraphErrors*) SaveFile->Get("Peak_VScan");
     WidthScan_hist = (TGraphErrors*) SaveFile->Get("Width_Vscan");

     CCE_hist->SetMinimum(0);
                

     TFile *CCEFile = new TFile((pathToAnalysisDirectory+"/Combined/CCE_Store.root").c_str(), "UPDATE");
     CCE_hist->Write(sensorName+"_"+Status,TObject::kOverwrite);

     //gDirectory->Delete("Sensor1_12Jan16_KAZU_100nA_enclosed_foils_dummy_IrradPreAnneal;1"); //Delete incorrect histograms in rootfile here!
     //gDirectory->Delete("Sensor2_12Jan16_KAZU_100nA_enclosed_foils_dummy_IrradPostAnneal;1");
     CCEFile->Close();
     TCanvas *can = new TCanvas(sensorName.c_str(),sensorName.c_str(),900,700);
     //TAxis *axis = CCE_hist->GetXaxis();
     //Defining Axes Labels for CCE_Scan in Output Folder
     CCE_hist->GetXaxis()->SetTitle("Bias Voltage [V]");
     CCE_hist->GetYaxis()->SetTitle("Collected Charge [ADC]");

     CCE_hist->Draw("ap");

     
     Double_t voltages=0;
     Double_t charges=0;
     Double_t errors=0;

     std::ofstream myfile;
     myfile.open(outputDirectory+"/CCE_output_PF.txt");
     int i{0};

     for(i=0; i<CCE_hist->GetN(); i++) {
       CCE_hist->GetPoint(i, voltages, charges);
       errors = CCE_hist->GetErrorY(i);
       myfile << voltages;
       myfile <<"\t";
       myfile << charges;
       myfile <<"\t";
       myfile << errors;
       myfile<< "\n";
     }
     
     //Double_t *Voutput =   	CCE_hist->GetX();
     //Double_t *CCEoutput =   	CCE_hist->GetY();

				
     //myfile <<Voutput;
     //myfile<< CCEoutput;
     myfile.close();

     //begin Cameron's addition (ke plot)          
     std::ofstream kefile;
     kefile.open(outputDirectory+"/CCE_output_PF_ke.txt");
     for(int ic=0; ic<CCE_hist->GetN(); ic++) {

       Double_t voltagesc{0};
       Double_t chargesc{0};
       Double_t evoltagesc{0};
       Double_t echargesc{0};
       Double_t correctionc{0};
       Double_t ADC_Ke{0.202};
       Double_t ecorrectionc{0};
       
       CCE_hist->GetPoint(ic, voltagesc, chargesc);
       echargesc = CCE_hist->GetErrorY(ic);
       correctionc = 110.4/((-0.383*(Temperature[ic]))+101);
       ecorrectionc = abs((110.4*0.05)/(-0.383*pow(-0.383*(Temperature[ic])+101,2)));
       kefile << voltagesc/1000. << "\t" << chargesc*correctionc*ADC_Ke << "\t" << evoltagesc/1000. << "\t" << pow(pow(chargesc*ADC_Ke*ecorrectionc,2)+pow(correctionc*ADC_Ke*echargesc,2),0.5) << std::endl;

       
     }
     kefile.close();

     TString kefilepath = outputDirectory+"/CCE_output_PF_ke.txt";
     TGraphErrors *keplot = new TGraphErrors(kefilepath);
     keplot->GetXaxis()->SetTitle("Bias Voltage [-kV]");
     keplot->GetYaxis()->SetTitle("Collected Charge [ke]");
     TCanvas *ke = new TCanvas("ke","ke",600,600);
     keplot->Draw("AP");
     ke->SaveAs(outputDirectory + "/CCEScan_" + sensorName+"_"+ Status +"ke.png");
     ke->Close();
     //end Cameron's addition
           
     can->SaveAs((outputDirectory + "/CCEScan_" + sensorName+"_"+ Status +".png"));
     can->Close();

     ClusterwidthPlots(pathToAnalysisDirectory,sensorName.c_str(), Status,std::to_string(statusEntry).c_str());

     SaveFile->Close();

     TFile *CCEFile2 = new TFile((pathToAnalysisDirectory+"/Combined/CCE_Store.root").c_str(), "READ");
     std::vector<TGraphErrors*> CCE_overlay;
     std::vector<TString> CCE_labels;
     std::vector<TString> CCE_sensor;
     TString PlotTitle = PlotTitleString.c_str();




     std::ifstream overlayFile(pathToAnalysisDirectory+"/OverlayList.txt");
     std::vector<TString> GraphList;
     std::string line;

     while (std::getline(overlayFile, line))
       {
	 GraphList.push_back(line);
       }
     std::cout << "\n" << GraphList.at(0) << "\n";





     for(size_t i = 0; i<GraphList.size(); i++)
       {

	 TIter next(CCEFile2->GetListOfKeys());
	 TKey *key;

	 while ((key=(TKey*)next()))
	   {
	     TString KeyName = key->GetName();

	     //	  std::cout << i << std::endl;
	     //	  std::cout << GraphList[i].Data() << std::endl;
	     if(KeyName.Contains(GraphList[i],TString::kExact))
	       {
		 //std::cout << KeyName.Data() << std::endl;
		 TString graphLabel = "";
		 if(KeyName.Contains("UNKNOWNA12",TString::kExact)) graphLabel = "Unirradiated_Sensor";
		 else if(KeyName.Contains("2nd_Anneal",TString::kExact)) graphLabel = "Sensor1_12Jan16_PostAnneal" ;
		 //else if(KeyName.Contains("3rd_Anneal",TString::kExact)) graphLabel = GraphList[i];
		 else if(KeyName.Contains("IrradPreAnneal",TString::kExact)) graphLabel = GraphList[i] + "_PreAnneal";
		 else if(KeyName.Contains("IrradPostAnneal",TString::kExact)) graphLabel = GraphList[i] + "_PostAnneal";
		 else if(KeyName.Contains("Unirradiated",TString::kExact)) graphLabel = GraphList[i] + "_Unirrad";

		 std::cout << GraphList[i] << std::endl;
		 CCE_labels.push_back(graphLabel);
		 CCE_overlay.push_back((TGraphErrors*) CCEFile2->Get(KeyName.Data()));
		 CCE_sensor.push_back(GraphList[i]);
	       }
	   }

       }

     PlotTitle += "_CCEScan";

     Plotter(CCE_overlay, CCE_labels,CCE_sensor, PlotTitle, pathToAnalysisDirectory, Temperature );

     CCEFile2->Close();





   }

};




#endif // WINDOW_H
