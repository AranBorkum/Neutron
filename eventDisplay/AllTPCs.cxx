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
    ("Input,I"  , po::value<std::string>(), "Raw datafile ROOT input")
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


  std::map<int, TH2D*> UPlane;
  std::map<int, TH2D*> VPlane;
  std::map<int, TH2D*> ZPlane;
