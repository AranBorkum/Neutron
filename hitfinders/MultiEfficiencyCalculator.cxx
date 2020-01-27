#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>

#include "TFile.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TColor.h"
#include "TStyle.h"
#include "TPad.h"

#include "canvas/Utilities/InputTag.h"
#include "gallery/Event.h"

#include "lardataobj/Simulation/SimChannel.h"
#include "lardataobj/RawData/RawDigit.h"
#include "lardataobj/RawData/RDTimeStamp.h"
#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_vectors.h"
#include "lardataobj/RawData/raw.h"

#include "DataAnaInputManager.hh"
#include "Helper.h"

#include "boost/program_options.hpp"

namespace po = boost::program_options;

int main(int argc, char** argv) {

  // --- Command Line Flags -----------------------
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h"   , "produce help message")
    ("input,i"  , po::value<std::string>(), "Input ROOT file")
    ("tag,g"    , po::value<std::string>()->default_value("daq"), "raw::RawDigit tag");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if(vm.count("help") || vm.empty()) {
    std::cout << desc << "\n"; return 1;
  }

  // --- Read in the data file -------------------
  std::map<int, DataAnaInputManager*> aaim;
  aaim[20 ] = new DataAnaInputManager(); aaim[20 ]->SetInputTree("dataAnarunsum20/DataTree");
  aaim[40 ] = new DataAnaInputManager(); aaim[40 ]->SetInputTree("dataAnarunsum40/DataTree");
  aaim[60 ] = new DataAnaInputManager(); aaim[60 ]->SetInputTree("dataAnarunsum60/DataTree");
  aaim[80 ] = new DataAnaInputManager(); aaim[80 ]->SetInputTree("dataAnarunsum80/DataTree");
  aaim[100] = new DataAnaInputManager(); aaim[100]->SetInputTree("dataAnarunsum100/DataTree");
  aaim[200] = new DataAnaInputManager(); aaim[200]->SetInputTree("dataAnarunsum200/DataTree");
  aaim[500] = new DataAnaInputManager(); aaim[500]->SetInputTree("dataAnarunsum500/DataTree");

  for (auto& it: aaim) {
    it.second->SetInputFile(vm["input"].as<std::string>());
    it.second->LoadTree();
  }
  
  std::map<int, TH1D*> cHitsByThreshold;
  std::map<int, TH1D*> iHitsByThreshold;

  for (auto const& it: aaim) {
    int adc_threshold = it.first;
    DataAnaInputManager* im = it.second;

    iHitsByThreshold[adc_threshold] = new TH1D(Form("iHits_Th%i", adc_threshold), ";nHits;", 4000, 0, 4000);
    cHitsByThreshold[adc_threshold] = new TH1D(Form("cHits_Th%i", adc_threshold), ";nHits;", 4000, 0, 4000);

    for (int CurrentEvent=0; CurrentEvent<im->GetEntries(); ++CurrentEvent) {
      im->GetEntry(CurrentEvent);

      // std::cout << "ADC Threshold = " << adc_threshold << " ";
      // float prog = (float)CurrentEvent / (float)im->GetEntries();
      // progress(prog);

      int cHits = 0;
      int iHits = 0;

      for (size_t i=0; i<im->Hit_View->size(); ++i) {
	if ((*im->Hit_View)[i] == 2 ) cHits++;
	else                          iHits++;
      }
      
      std::cout << adc_threshold << ": "  << cHits << std::endl; 
      cHitsByThreshold[adc_threshold]->Fill(cHits);
      iHitsByThreshold[adc_threshold]->Fill(iHits);

    }

  }

  TCanvas *c1 = new TCanvas("c1", "c1");
  c1->Print("InductionHits.pdf[");
  for (auto const& it: iHitsByThreshold) {
    it.second->Draw();
    c1->Print("InductionHits.pdf");
  }
  c1->Print("InductionHits.pdf]");

  TCanvas *c2 = new TCanvas("c2", "c2");
  c2->Print("CollectionHits.pdf[");
  for (auto const& it: cHitsByThreshold) {
    it.second->Draw();
    c2->Print("CollectionHits.pdf");
  }
  c2->Print("CollectionHits.pdf]");

  
}
  
  
  





