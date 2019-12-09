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
    ("threshold", po::value<std::string>(), "Threshold value");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  DataAnaInputManager *im = new DataAnaInputManager();
  im->SetInputFile(vm["input"].as<std::string>());
  im->SetInputTree(("dataAnarunsum"+vm["threshold"].as<std::string>()+"/DataTree").c_str());
  im->LoadTree();

  for (int CurrentEvent=0; CurrentEvent<im->GetEntries(); ++CurrentEvent) {
    im->GetEntry(CurrentEvent);
    
    int nTot = 0;
    int nCol = 0;
    int nInd = 0;

    std::set<int> TPCs;

    for (int i=0; i<im->Hit_View->size(); ++i) {
      TPCs.insert((*im->Hit_TPC)[i]);
      if ((*im->Hit_View)[i] == 0) nInd++;
      if ((*im->Hit_View)[i] == 1) nInd++;
      if ((*im->Hit_View)[i] == 0) nCol++;
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


  }

}
