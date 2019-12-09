#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TProfile.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TLine.h"

#include "Helper.h"
#include "ArbitraryAnaInputManager.hh"

#include <iostream>
#include <cmath>

double pi = 3.1415926535797;
std::string outFileDir = "/dune/app/users/aborkum/Neutrons/output/";

int main(int argc, char** argv) {

  int opt;
  extern char *optarg;
  extern int   optopt;

  int nEvent              = -1;
  std::string InFileName  = "";
  std::string OutFileName = "";

  while( (opt = getopt(argc, argv, "i:o:n:")) != -1) {
    switch (opt) {
    case 'i':
      InFileName = optarg;
      break;
    case 'o':
      OutFileName = optarg;
      break;
    case 'n':
      nEvent = atoi(optarg);
      break;
    case '?':
      std::cerr << "Unknown option: '" << char(optopt) << "'!" << std::endl;
      break;
    }
  }

  std::map<int, ArbitraryAnaInputManager*> aaim;

  aaim[100] = new ArbitraryAnaInputManager(); aaim[100]->SetInputTree("arbitraryanarunsum100/SNSimTree");

  for (auto& it: aaim) {
    it.second->SetInputFile(InFileName);
    it.second->LoadTree();
  }

  std::map<int, TH1D*> hitView;
  std::map<int, TProfile*> hitRMSvsAngle_U;
  std::map<int, TProfile*> hitRMSvsAngle_V;
  std::map<int, TProfile*> hitSADCvsAngle_U;
  std::map<int, TProfile*> hitSADCvsAngle_V;
  std::map<int, TProfile*> hitViewComp;

  for (auto const& it: aaim) {
    int fNEvent       = nEvent;
    int adc_threshold = it.first;
    ArbitraryAnaInputManager* im = it.second;

    if (fNEvent!=-1) fNEvent = std::min(fNEvent,(int)im->GetEntries());
    else             fNEvent = im->GetEntries();

    hitView[adc_threshold] = new TH1D(Form("Hit_View_Th%i", adc_threshold), ";Hit View;", 3, 0, 3);
    hitRMSvsAngle_U [adc_threshold] = new TProfile(Form("HitRMSvsAngle_U_Th%i" , adc_threshold), 
						   "U-Plane;YZ-Plane Angle [rad];RMS [tick]" , 100, -pi/2, pi/2, 0,   20);
    hitRMSvsAngle_V [adc_threshold] = new TProfile(Form("HitRMSvsAngle_V_Th%i" , adc_threshold), 
						   "V-Plane;YZ-Plane Angle [rad];RMS [tick]" , 100, -pi/2, pi/2, 0,   20);
    hitSADCvsAngle_U[adc_threshold] = new TProfile(Form("HitSADCvsAngle_U_Th%i", adc_threshold), 
						   "U-Plane;YZ-Plane Angle [rad];SADC [tick]", 100, -pi/2, pi/2, 0, 2000);
    hitSADCvsAngle_V[adc_threshold] = new TProfile(Form("HitSADCvsAngle_V_Th%i", adc_threshold), 
						   "V-Plane;YZ-Plane Angle [rad];SADC [tick]", 100, -pi/2, pi/2, 0, 2000);
    hitViewComp     [adc_threshold] = new TProfile(Form("HitViewComp_Th%i"     , adc_threshold), 
						   ";U-Plane Hits;V-Plane Hits"              ,  10,     0, 1000, 0, 1000);

    for (int CurrentEvent=0; CurrentEvent<fNEvent; ++CurrentEvent) {
      im->GetEntry(CurrentEvent);
      
      int kU = 0; int kV = 0;

      double yComponent = (*im->True_Diry)[0];
      double zComponent = (*im->True_Dirz)[0];
      double angle      = atan( yComponent / zComponent );

      for (size_t i=0; i<im->Hit_View->size(); ++i) {
	hitView[adc_threshold]->Fill((*im->Hit_View)[i]);
	if ((*im->Hit_View)[i] == 0) {
	  kU++; 
	  hitRMSvsAngle_U [adc_threshold]->Fill(angle,     (*im->Hit_RMS) [i]); 
	  hitSADCvsAngle_U[adc_threshold]->Fill(angle, abs((*im->Hit_SADC)[i])); }
	if ((*im->Hit_View)[i] == 1) {
	  kV++; 
	  hitRMSvsAngle_V [adc_threshold]->Fill(angle,     (*im->Hit_RMS) [i]); 
	  hitSADCvsAngle_V[adc_threshold]->Fill(angle, abs((*im->Hit_SADC)[i])); }
      }
      hitViewComp[adc_threshold]->Fill(kU, kV);
    }
  }

  TCanvas *c = new TCanvas("c", "c");
  c->Print((outFileDir+OutFileName+".pdf[").c_str());
  PlotAll(hitView);
  c->Print((outFileDir+OutFileName+".pdf").c_str());

  PlotAll(hitRMSvsAngle_U);
  c->Print((outFileDir+OutFileName+".pdf").c_str());

  PlotAll(hitRMSvsAngle_V);
  c->Print((outFileDir+OutFileName+".pdf").c_str());

  PlotAll(hitSADCvsAngle_U);
  c->Print((outFileDir+OutFileName+".pdf").c_str());

  PlotAll(hitSADCvsAngle_V);
  c->Print((outFileDir+OutFileName+".pdf").c_str());

  PlotAll(hitViewComp);
  c->Print((outFileDir+OutFileName+".pdf").c_str());
  c->Print((outFileDir+OutFileName+".pdf]").c_str());

}
