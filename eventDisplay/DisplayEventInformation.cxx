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

#include "DataAnaInputManager.hh"

#include "boost/program_options.hpp"

namespace po = boost::program_options;

int main(int argc, char** argv) {

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h"   , "produce help message")
    ("input,i"  , po::value<std::string>(), "Input ROOT file")
    ("threshold", po::value<std::string>(), "Threshold value")
    ("nevent,n" , po::value<int        >()->default_value(-1), "Event number to display");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if(vm.count("help") || vm.empty()) {
    std::cout << desc << "\n"; return 1;
  }

  DataAnaInputManager *im = new DataAnaInputManager();
  im->SetInputFile(vm["input"].as<std::string>());
  im->SetInputTree(("dataAnarunsum"+vm["threshold"].as<std::string>()+"/DataTree").c_str());
  im->LoadTree();

  int nEvents = 0;
  int start   = 0;
  if (vm["nevent"].as<int>() == -1) nEvents = im->GetEntries();
  else { start = vm["nevent"].as<int>() ; nEvents = start+1; }

  for (int CurrentEvent=start; CurrentEvent<nEvents; ++CurrentEvent) {
    im->GetEntry(CurrentEvent);
    
    int nTot = 0;
    int nCol = 0;
    int nInd = 0;

    std::set<int> TPCs;

    for (int i=0; i<im->Hit_View->size(); ++i) {
      TPCs.insert((*im->Hit_TPC)[i]);
      if ((*im->Hit_View)[i] == 0) nInd++;
      if ((*im->Hit_View)[i] == 1) nInd++;
      if ((*im->Hit_View)[i] == 2) nCol++;
    }

    std::cout                                                << std::endl;
    std::cout << "-----------------------------------------" << std::endl;
    std::cout << "EVENT " << CurrentEvent+1                  << std::endl;
    std::cout << "-----------------------------------------" << std::endl;
    std::cout << "*  "                                       << std::endl;
    std::cout << "*  TPCs with events:      ";
      for (auto const& i: TPCs) std::cout << i << " ";
    std::cout                                                << std::endl;
    std::cout << "*  Total number of hits:  " << nCol + nInd << std::endl;
    std::cout << "*  Total induction hits:  " << nInd        << std::endl;
    std::cout << "*  Total collection hits: " << nCol        << std::endl;
    std::cout << "*"                                         << std::endl;

    for (auto const& it: TPCs) {
      std::cout << "*  Number of hits in TPC " << it << ": \t";
      int uHits = 0; int vHits = 0; int cHits = 0; 
      for (size_t i=0; i<im->Hit_View->size(); ++i) {
	if ((*im->Hit_TPC)[i]==it) {
	  if ((*im->Hit_View)[i] == 2) cHits++;
	  if ((*im->Hit_View)[i] == 1) vHits++;
	  if ((*im->Hit_View)[i] == 0) uHits++;
	}
      }
      std::cout << "U: " << uHits << "\t V: " << vHits << "\t C: " << cHits << std::endl;
    }




  }

}
