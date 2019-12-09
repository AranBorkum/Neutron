#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TProfile.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TLine.h"

#include "Helper.h"
#include "DataAnaInputManager.hh"

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

  std::map<int, DataAnaInputManager*> aaim;

  aaim[100] = new DataAnaInputManager(); aaim[100]->SetInputTree("dataAnarunsum100/DataTree");
  aaim[150] = new DataAnaInputManager(); aaim[150]->SetInputTree("dataAnarunsum150/DataTree");
  aaim[200] = new DataAnaInputManager(); aaim[200]->SetInputTree("dataAnarunsum200/DataTree");
  aaim[250] = new DataAnaInputManager(); aaim[250]->SetInputTree("dataAnarunsum250/DataTree");
  aaim[300] = new DataAnaInputManager(); aaim[300]->SetInputTree("dataAnarunsum300/DataTree");
 
  for (auto& it: aaim) {
    it.second->SetInputFile(InFileName);
    it.second->LoadTree();
  }

  std::map<int, TH1D*>     hitView;
  std::map<int, TH1D*>     hitRMS;
  std::map<int, TH1D*>     hitSADC;
  std::map<int, TProfile*> hitViewComp;

  for (auto const& it: aaim) {
    int fNEvent       = nEvent;
    int adc_threshold = it.first;
    DataAnaInputManager* im = it.second;
 
    if (fNEvent!=-1) fNEvent = std::min(fNEvent,(int)im->GetEntries());
    else             fNEvent = im->GetEntries();

    hitView    [adc_threshold] = new TH1D    (Form("hitView_Th%i"    , adc_threshold), ";Hit View;"      ,   3, 0,   3);
    hitRMS     [adc_threshold] = new TH1D    (Form("hitRMS_Th%i"     , adc_threshold), ";Hit RMS [tick];",  40, 0,  40);
    hitSADC    [adc_threshold] = new TH1D    (Form("hitSADC_Th%i"    , adc_threshold), ";Hit SADC [adc];", 200, 0, 200);
    hitViewComp[adc_threshold] = new TProfile(Form("hitViewComp_Th%i", adc_threshold), ";U-Plane hits;V-Plane hits", 
					      10, 0, 8000, 0, 8000);

    for (int CurrentEvent=0; CurrentEvent<fNEvent; ++CurrentEvent) {
      im->GetEntry(CurrentEvent);

      int kU = 0;
      int kV = 0;

      for (size_t it=0; it<im->Hit_RMS->size(); ++it) {
	if ((*im->Hit_View)[it] == 0) kU++;
	if ((*im->Hit_View)[it] == 1) kV++;
	hitView[adc_threshold]->Fill((*im->Hit_View)[it]);
	hitRMS [adc_threshold]->Fill((*im->Hit_RMS )[it]);
	hitSADC[adc_threshold]->Fill((*im->Hit_SADC)[it]);
      }
      hitViewComp[adc_threshold]->Fill(kU, kV);
    }
  }

  const std::vector<int> color = getColors();
  int i = 0;
  TLegend *legend = new TLegend(0.1, 0.1, 0.9, 0.9);
  for (auto const& it: hitView) {
    hitView    [it.first]->SetStats(0);
    hitRMS     [it.first]->SetStats(0);
    hitSADC    [it.first]->SetStats(0);
    hitViewComp[it.first]->SetStats(0);
    hitView    [it.first]->SetMarkerColor(color.at(i));
    hitRMS     [it.first]->SetMarkerColor(color.at(i));
    hitSADC    [it.first]->SetMarkerColor(color.at(i));
    hitViewComp[it.first]->SetMarkerColor(color.at(i));
    hitView    [it.first]->SetLineColor(color.at(i));
    hitRMS     [it.first]->SetLineColor(color.at(i));
    hitSADC    [it.first]->SetLineColor(color.at(i));
    hitViewComp[it.first]->SetLineColor(color.at(i));
    legend->AddEntry(hitView[it.first], Form("ADC Threshold = %i", it.first), "L");
    i++;
  }

  TCanvas *c = new TCanvas("c", "c");
  c->Print((outFileDir+OutFileName+".pdf[").c_str());
  PlotAll(hitView);
  c->Print((outFileDir+OutFileName+".pdf").c_str());

  PlotAll(hitRMS);
  c->Print((outFileDir+OutFileName+".pdf").c_str());

  PlotAll(hitSADC);
  c->Print((outFileDir+OutFileName+".pdf").c_str());

  PlotAll(hitViewComp);
  c->Print((outFileDir+OutFileName+".pdf").c_str());

  c->Clear();
  legend->Draw();
  c->Print((outFileDir+OutFileName+".pdf").c_str());
  c->Print((outFileDir+OutFileName+".pdf]").c_str());
}
