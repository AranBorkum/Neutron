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
  aaim[3 ] = new DataAnaInputManager(); aaim[3 ]->SetInputTree("dataAnarunsum3/DataTree");
  aaim[4 ] = new DataAnaInputManager(); aaim[4 ]->SetInputTree("dataAnarunsum4/DataTree");
  aaim[5 ] = new DataAnaInputManager(); aaim[5 ]->SetInputTree("dataAnarunsum5/DataTree");
  aaim[6 ] = new DataAnaInputManager(); aaim[6 ]->SetInputTree("dataAnarunsum6/DataTree");
  aaim[7 ] = new DataAnaInputManager(); aaim[7 ]->SetInputTree("dataAnarunsum7/DataTree");
  aaim[8 ] = new DataAnaInputManager(); aaim[8 ]->SetInputTree("dataAnarunsum8/DataTree");
  aaim[10] = new DataAnaInputManager(); aaim[10]->SetInputTree("dataAnarunsum10/DataTree");
  aaim[12] = new DataAnaInputManager(); aaim[12]->SetInputTree("dataAnarunsum12/DataTree");
  aaim[16] = new DataAnaInputManager(); aaim[16]->SetInputTree("dataAnarunsum16/DataTree");
  aaim[20] = new DataAnaInputManager(); aaim[20]->SetInputTree("dataAnarunsum20/DataTree");
  aaim[30] = new DataAnaInputManager(); aaim[30]->SetInputTree("dataAnarunsum30/DataTree");


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

    int nCHits = 0;
    int nIHits = 0;

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
      
      nCHits += cHits;
      nIHits += iHits;
      
      iHitsByThreshold[adc_threshold]->Fill(iHits);

    }

    std::cout << adc_threshold << "\t" << nCHits / 10 << "\t" << nIHits / 10 << std::endl;

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
  
  
  





